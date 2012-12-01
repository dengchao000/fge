#include "StdAfx.h"
#include "tcpServer.h"
#include <fgedebug.h>
#include <fgelog.h>
#include <process.h>
using namespace fge;
using namespace ocl;

TcpServer::TcpServer( )
{
	m_hCompletionPort = 0;
	m_hListenTherad = 0;
	m_hSendTherad = 0;
	m_sendhead = m_sendlast = 0;
	m_sendState = 0;
}
TcpServer::~TcpServer( ) 
{
	Close( );
	std::list<Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->ReleaseListener();
		i++;
	}
	m_processer.clear( );
}
void TcpServer::AddListener( Listener* proc )
{
	fge::Lock<fge::CCriticalSection> l(m_workLock);
	m_processer.push_back( proc );
}
//主动断开连接
void TcpServer::Disconnect( ConnectionID id )
{
	fge::Lock<fge::FreeMutex> l(m_csConn);		//锁定连接管理器
	ConnectionInfo* pClient = m_connManager.GetConnectionInfo(id);
	if(!pClient)
		return;
	pClient->asyncSocket.Close();
	//asyncSocket.Close后,会自动删除连接
	//m_connManager.DelConnection( id );
}
int TcpServer::Open( const char* host, int port, int workNum )
{
	if(m_hCompletionPort!=0)
		return S_OK;

	INSTANCE( ocl::SocketInitializtion );

	// 创建一个空iocp结构:
	m_hCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );
	int hr = m_listener.Open( SOCK_STREAM,AF_INET, IPPROTO_TCP,0 );
	if( hr!=S_OK )
		return hr;
	hr = m_listener.Bind( host, port );
	if( hr!=S_OK )
		return hr;

	//创建工作者线程
	if(workNum == 0)
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		workNum = sysInfo.dwNumberOfProcessors*2;
	}
	m_hWorkThread.resize( workNum );

	for( u_long i=0; i<m_hWorkThread.size(); i++ )
		m_hWorkThread[i] = (HANDLE)_beginthreadex(NULL,0,S_WorkStart,reinterpret_cast<LPVOID>(this),0,NULL);

	//创建连接监听线程
	m_hListenTherad = (HANDLE)_beginthreadex(NULL,0,S_ListenStart,reinterpret_cast<LPVOID>(this),0,NULL);
	if( m_hListenTherad==0 )
		return -1;
	m_hSendTherad = (HANDLE)_beginthreadex(NULL,0,S_SendStart,reinterpret_cast<LPVOID>(this),0,NULL);
	if( m_hSendTherad==0 )
		return -1;
	return S_OK;
}
u_long TcpServer::SendToClient(const char* body, u_long size, ConnectionID& id)
{
	fge::Lock<fge::FreeMutex> l(m_csConn);		//锁定连接管理器
	ConnectionInfo* pClient = m_connManager.GetConnectionInfo(id);
	if(!pClient)
		return 0;

	if(pClient)
	{
		fge::Lock<fge::FreeMutex> l(pClient->socketLock);
		u_long err = pClient->asyncSocket.Send(body,size);
		switch( err )
		{
		case NO_ERROR:
			m_csSend.Enter();
			m_sendState &= ~SEND_STATE_BUSY;	
			m_csSend.Leave();
			break;
		case WSAEFAULT:                         // 指针出错
			// 日志记录
			fge::log_error("TcpServer::Send 数据出错[%d] ", err);
			break;
		case WSA_IO_PENDING:					// work later
			m_csSend.Enter();
			m_sendState |= SEND_STATE_BUSY;		//::Sleep(1);
			m_csSend.Leave();
			break;
		default:
			return err;
		}
	}
	return size;
}
u_long TcpServer::Send( const char* body, u_long size, ConnectionID id )
{
	bool bSendToBuff = true;
	m_csSend.Enter();
	bSendToBuff = (m_sendlast != m_sendhead) || (m_sendState & SEND_STATE_BUSY);	
	m_csSend.Leave();

	if(bSendToBuff)
		SendToBuff( body,size,id );
	else
		return SendToClient(body,size,id );
	return size;
}
void TcpServer::SendToBuff( const char * pData, u_long nSendLen, ConnectionID& id )
{
	bool isEnough = false;

	//判断缓存空间是否足够
	while(!isEnough)
	{
		if(m_sendState & SEND_STATE_EXIT)
			return;

		m_csSend.Enter();
		u_long newLast = m_sendlast + sizeof(id) + sizeof(nSendLen) + nSendLen;		
		if(m_sendlast < m_sendhead)
			isEnough = newLast<m_sendhead;
		else
			isEnough = newLast<SEND_BUFF_LEN || nSendLen<m_sendhead;
		m_csSend.Leave();

		if(!isEnough)	// 如果空间不够,则等待.
			::Sleep(1);
		else
			break;
	}


	u_long newLast = m_sendlast;
	//保存客户端编号和发送的长度
	memcpy(&m_sendBuff[newLast],&id,sizeof(id));			 newLast+=sizeof(id);
	memcpy(&m_sendBuff[newLast],&nSendLen,sizeof(nSendLen)); newLast+=sizeof(nSendLen);
	//保存发送同容
	if(newLast + nSendLen > SEND_BUFF_LEN)
		newLast = 0;
	memcpy(&m_sendBuff[newLast],pData,nSendLen);		newLast+=nSendLen;

	m_csSend.Enter();
	m_sendlast = newLast;
	if(!(m_sendState&SEND_STATE_EVENT))
	{
		m_sendEvent.SetEvent();
		m_sendState |= SEND_STATE_EVENT;
	}
	m_csSend.Leave();
}
void TcpServer::Close( )
{
	if(NULL == m_hCompletionPort)
		return;

	//关闭监听socket
	m_listener.Close( );
	//等待监听线程结束
	WaitForSingleObject(m_hListenTherad, INFINITE);
	CloseHandle(m_hListenTherad);
	m_hListenTherad = 0;

	//关闭发送线程
	m_csSend.Enter();
	m_sendState |= SEND_STATE_EXIT;
	m_sendEvent.SetEvent();
	m_csSend.Leave();
	WaitForSingleObject(m_hSendTherad, INFINITE);
	CloseHandle(m_hSendTherad);
	m_hSendTherad = 0;

	//发送事件，通知工作者线程退出	
	for(u_long i=0; i<m_hWorkThread.size(); i++)
		::PostQueuedCompletionStatus( m_hCompletionPort,0,0,0);
	for(u_long i=0; i<m_hWorkThread.size(); i++)
		WaitForSingleObject(m_hWorkThread[i], INFINITE);
	for(u_long i=0; i<m_hWorkThread.size(); i++)
	{
		CloseHandle(m_hWorkThread[i]);
		m_hWorkThread[i] = 0;
	}
	m_hWorkThread.clear( );
	
	OnClose( );

	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = 0;
}

unsigned int TcpServer::S_WorkStart(LPVOID	lpParam)
{
	TcpServer *pThis = reinterpret_cast<TcpServer*>(lpParam);
	pThis->Work( );
	return (0);
}
unsigned int TcpServer::S_ListenStart(LPVOID	lpParam)
{
	TcpServer *pThis = reinterpret_cast<TcpServer*>(lpParam);
	pThis->Listen( );
	return (0);
}
unsigned int TcpServer::S_SendStart(LPVOID	lpParam)
{
	TcpServer *pThis = reinterpret_cast<TcpServer*>(lpParam);
	pThis->SendWork( );
	return (0);
}
void TcpServer::Listen( )
{
	bool bRun = true;
	while (bRun)
	{
		// 接受一个连接
		SOCKET_HANDLE hClientSocket = m_listener.Accept( );

		// 如果得到有效的句柄，则为其服务
		if (INVALID_SOCKET != hClientSocket)
		{
			Accept(hClientSocket, m_listener.GetAddr());
		}
		// 错误记录和处理
		else
		{
			int nError = ::WSAGetLastError();
			ErrorMsg("接收连接时创建套接字失败");
			switch (nError)
			{
			case WSAEINTR:
				bRun = false;
				break;
			case WSAENETDOWN:	// 网络连接错误
				ErrorMsg("网络连接错误.");
				break;
			case WSAEFAULT:		// 指针参数无效
				ErrorMsg("指针参数无效.");
				break;
			case WSAENOBUFS:	// 指针无效，内存空间不足
				ErrorMsg("内存空间不足.");
				break;
			case WSAENOTSOCK:	// 不是一个Socket句柄
				ErrorMsg("不是一个Socket句柄.");
				break;
			case WSAEACCES:		// 访问被拒绝
				ErrorMsg("访问被拒绝.");
				break;
			case WSAEWOULDBLOCK://The socket is marked as nonblocking and no connections are present to be accepted.
				ErrorMsg("The socket is marked as nonblocking and no connections are present to be accepted.");
				break;
			default:
				{      
					ErrorMsg("未知原因");
					break;
				}
			}
		}
	}
}
void TcpServer::SendWork( )
{
	while (true)
	{
		m_sendEvent.WaitObject();
		m_csSend.Enter();
		
		while( !(m_sendState&SEND_STATE_EXIT) && m_sendhead != m_sendlast )
		{//如果发送状态不为0,并发送缓冲区不为空
			u_long newHead = m_sendhead;
			m_csSend.Leave();

			ConnectionID* pcnid = (ConnectionID*)&m_sendBuff[newHead];
			newHead+=sizeof(ConnectionID);
			u_long* pnSendLen = (u_long*)&m_sendBuff[newHead];
			newHead+=sizeof(u_long);

			if(newHead + *pnSendLen > SEND_BUFF_LEN)
				newHead = 0;
			const char * pData = &m_sendBuff[newHead];			
			SendToClient(pData,*pnSendLen,*pcnid);
			newHead += *pnSendLen;

			m_csSend.Enter();
			m_sendhead = newHead;
		}

		if(m_sendState & SEND_STATE_EXIT){
			m_csSend.Leave();
			break;
		}	
		//如果发送队列为空,则
		if(m_sendhead == m_sendlast)
		{			
			m_sendState &= ~SEND_STATE_EVENT;
			m_sendEvent.ResetEvent();
		}
		m_csSend.Leave();
	}
}
void TcpServer::Accept( SOCKET_HANDLE hClientSocket, SOCKADDR_IN& addr )
{
	ConnectionInfo * key = new ConnectionInfo;
	if( NULL == key )
	{
		::closesocket( hClientSocket );
		return;
	}

	key->asyncSocket.SetHandle( hClientSocket );
	key->nPort = htons( addr.sin_port ); 
	strcpy_s(key->strIP,inet_ntoa( addr.sin_addr ) );
	if(!m_connManager.AddConnection( key ))
	{//加入到连接管理器失败:
		delete key;
		key = 0;
		::closesocket(hClientSocket);
		return;
	}

	// 关联iocp和client socket:
	::CreateIoCompletionPort((HANDLE)hClientSocket, m_hCompletionPort, (ULONG_PTR)key, 0);
	this->OnAccept( key->strIP,key->nPort,key->connid );	
	key->asyncSocket.Recv( );
}
void TcpServer::Work( )
{
	BOOL queueState;					//队列状态
	DWORD TransferredBytesNum;			//传送字节数
	WSAOVERLAPPED *tempOverlap = NULL;	//标准重叠结构
	ocl::ConnectionInfo	 *pKey = NULL;		//完成键
	ocl::OverlappeStruct *pOverlap = NULL;	//扩展重叠结构
	int err(0);							//错误值

	while(true)
	{
		pOverlap = 0;
		//在一个完成端口上等待,每当一个重叠操作完成此等待便完成.
		queueState = ::GetQueuedCompletionStatus( m_hCompletionPort,&TransferredBytesNum,(PULONG_PTR)&pKey,&tempOverlap,INFINITE );
		if(tempOverlap==0)
		{
			/*
			If *tempOverlap is NULL, the function did not dequeue a completion packet from the completion port. In this case,
			the function does not store information in the variables pointed to by the TransferredBytesNum and pKey parameters,
			and their values are indeterminate. 
			*/
			break;
		}

		//当得到了指向OVERLAPPED结构的指针以后，
		//可以用CONTAINING_RECORD宏取出其中指向扩展结构的指针
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );
		//如果收到了0字节数据，则表示远端连接已经关闭
		if(TransferredBytesNum ==0 && pOverlap->opType==OCL_RECV)
			pOverlap->opType = OCL_CLOSE;

		pOverlap->bufferLen = TransferredBytesNum;
		bool invalidSocket = false;
		switch( pOverlap->opType )
		{			
		case OCL_RECV:	//收到一个数据包,处理后再投一个接收操作.
			OnRecv(pOverlap->frameBuf,pOverlap->bufferLen,pKey->connid);
			pKey->socketLock.Enter( );			
			pKey->asyncSocket.Recv( pOverlap );
			invalidSocket = pKey->asyncSocket.GetHandle() == INVALID_SOCKET;
			pKey->socketLock.Leave( );
			if(invalidSocket) 
				OnDisconnect( pKey->connid );
			pOverlap = NULL;
			break;
			
		case OCL_SEND:	//发送完成,回收投出的重叠结构
			OnSend(pOverlap->frameBuf,pOverlap->bufferLen,pKey->connid);
			delete pOverlap;
			pOverlap = NULL;
			break;
			
		case OCL_CLOSE:	//关闭连接，会引起所有在些完成端口上等待的
			pKey->socketLock.Enter( );
			pKey->asyncSocket.Close();
			pKey->socketLock.Leave( );
			OnDisconnect( pKey->connid );
			delete pOverlap;
			pOverlap = NULL;
			break;

		default:
			fge::log_debug("close socket,delete OverlappeStruct");
			delete pOverlap;
			pOverlap = NULL;
			break;
		}
	}
	if(tempOverlap)
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );
	if(pOverlap)
	{
		fge::log_debug("Close work,delete OverlappeStruct");
		delete pOverlap;
		pOverlap = 0;
	}
	//SAFE_DELETE(lpBuffer);
	fge::log_debug("work exit");
}
void TcpServer::OnAccept( const char* ip, int port, ConnectionID id )
{
	fge::Lock<fge::CCriticalSection> l(m_workLock);
	std::list<Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnAccept( ip, port, id );
		i++;
	}
}
void TcpServer::OnRecv(const char * pData, u_long nRecvLen, ConnectionID id)
{
	fge::Lock<fge::FreeMutex> l(m_csConn);		//锁定连接管理器
	ConnectionInfo* pclient = m_connManager.GetConnectionInfo( id );
	if(NULL == pclient || !nRecvLen)
		return;
	fge::Lock<fge::CCriticalSection> lproc(m_workLock);			
	std::list<Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnRecv(pData,nRecvLen,id);
		i++;
	}	
}
void TcpServer::OnSend(const char * pData, u_long nSendLen, ConnectionID id)
{
	//fge::Lock<fge::CCriticalSection> l(m_workLock);
	//std::list<Listener*>::iterator i = m_processer.begin( );
	//while( i!=m_processer.end() )
	//{
	//	(*i)->OnSend(pData,nSendLen,id);
	//	i++;
	//}
	m_csSend.Enter();
	m_sendState &= ~SEND_STATE_BUSY;	
	m_csSend.Leave();
}
void TcpServer::OnClose( )
{
	// To avoid memory leaks we should delete all results from queue.
	BOOL queueState;						//队列状态
	DWORD TransferredBytesNum;				//传送字节数
	WSAOVERLAPPED *tempOverlap = NULL;		//标准重叠结构
	ocl::ConnectionInfo	 *pKey = NULL;		//完成键
	ocl::OverlappeStruct *pOverlap = NULL;	//扩展重叠结构
	for (;;)
	{
		// Get the next asynchronous operation that completes
		queueState = ::GetQueuedCompletionStatus( m_hCompletionPort,&TransferredBytesNum,(PULONG_PTR)&pKey,
			&tempOverlap,0 );			
		if (tempOverlap == 0)
			break;
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );
		delete pOverlap;
	}

	//fge::Lock<fge::CCriticalSection> l(m_workLock);//到此,其它线程都已关闭.

	std::list<Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnClose( );
		i++;
	}
}
void TcpServer::OnDisconnect( ConnectionID id )
{
	{//通知Disconnect事件
		fge::Lock<fge::CCriticalSection> l(m_workLock);	
		std::list<Listener*>::iterator i = m_processer.begin( );
		while( i!=m_processer.end() )
		{
			(*i)->OnDisconnect( id );
			i++;
		}
	}

	fge::Lock<fge::FreeMutex> l(m_csConn);		//锁定连接管理器
	m_connManager.DelConnection( id );
}