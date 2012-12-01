#include "StdAfx.h"
#include "tcpClient.h"
//#include <fgedebug.h>
#include <fgelog.h>
#include <process.h>

using namespace fge;
using namespace ocl;

TcpClient::TcpClient( )
{
	m_hCompletionPort = 0;
}
TcpClient::~TcpClient( ) 
{
	if(m_hCompletionPort)
		this->Close( );
	m_hCompletionPort = 0;
	std::list<Chunnel::Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->ReleaseListener();
		i++;
	}
	m_processer.clear( );
}
void TcpClient::AddListener( Chunnel::Listener* proc )
{
	fge::Lock<fge::CCriticalSection> l(m_procLock);
	m_processer.push_back( proc );
}

int TcpClient::Open( const char* host, int port )
{
	fge::Lock<fge::CCriticalSection> l(m_socketLock);
	if(m_hCompletionPort!=0)
		return 1;
	INSTANCE( ocl::SocketInitializtion );	
	if ( m_socket.Open(SOCK_STREAM,AF_INET, IPPROTO_TCP,0)<0 )
		return -1;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);    
	addr.sin_addr.s_addr = inet_addr(host);
	if (SOCKET_ERROR == ::connect( m_socket.GetSocketHandle(),(SOCKADDR *)&addr, sizeof(addr) ))
	{
		m_socket.Close();
		DWORD e = GetLastError();
		return -1;
	}

	// 创建一个空iocp结构:
	m_hCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );
	m_hWorkThread = (HANDLE)_beginthreadex(NULL,0,Run,reinterpret_cast<LPVOID>(this),0,NULL);
	//// 将hClientSocket和完成端口绑定
	::CreateIoCompletionPort((HANDLE)m_socket.GetSocketHandle(), m_hCompletionPort, (ULONG_PTR)&m_socket, 0);

	m_socket.Recv( );
	//::WSARecv(m_socket.GetSocketHandle(),buf,1024,0);
	return S_OK;
}
u_long TcpClient::Send( const char* body, u_long size )
{
	fge::Lock<fge::CCriticalSection> l(m_socketLock);
	u_long err = m_socket.Send(body,size);
	switch( err )
	{
	case NO_ERROR:
		break;
	case WSAEFAULT:			// 指针出错
		// 日志记录
		fge::log_error("TcpClient::Send 数据出错[%d] ", err);
		break;
	case WSA_IO_PENDING:	// work later
		::Sleep(1);
		break;
	default:
		return 0;
	}
	return size;
}

void TcpClient::Close( )
{
	if(NULL == m_hCompletionPort)
		return;
	//发送事件，通知退出	
	::PostQueuedCompletionStatus( m_hCompletionPort,0,0,0);
	WaitForSingleObject(m_hWorkThread, INFINITE);
}

unsigned int TcpClient::Run(LPVOID	lpParam)
{
	TcpClient *pThis = reinterpret_cast<TcpClient*>(lpParam);
	pThis->Work( );
	pThis->OnClose( );
	return (0);
}

void TcpClient::Work( )
{
	BOOL queueState;					//队列状态
	DWORD TransferredBytesNum;			//传送字节数
	WSAOVERLAPPED *tempOverlap = NULL;	//标准重叠结构
	ocl::AsyncSocket	 *pKey = NULL;	//完成键
	ocl::OverlappeStruct *pOverlap = NULL;	//扩展重叠结构
	int err(0);							//错误值

	while(true)
	{
		pOverlap = NULL;
		//在一个完成端口上等待,每当一个重叠操作完成此等待便完成.
		queueState = ::GetQueuedCompletionStatus( m_hCompletionPort,&TransferredBytesNum,(PULONG_PTR)&pKey,&tempOverlap,INFINITE );
		if(tempOverlap==0)
			break;

		//当得到了指向OVERLAPPED结构的指针以后，
		//可以用CONTAINING_RECORD宏取出其中指向扩展结构的指针
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );

		//如果收到了0字节数据，则表示远端连接已经关闭
		if(TransferredBytesNum ==0 && pOverlap->opType==OCL_RECV)
			pOverlap->opType = OCL_CLOSE;
		bool invalidSocket = false;

		pOverlap->bufferLen = TransferredBytesNum;
		switch( pOverlap->opType )
		{
			//收到一个数据包,处理后再投一个接收操作.
		case OCL_RECV:
			OnRecv(pOverlap->frameBuf,pOverlap->bufferLen);
			m_socketLock.Enter( );	
			pKey->Recv( pOverlap );	//投递接收操作,如果失败,将关闭Socket.
			invalidSocket = pKey->GetHandle() == INVALID_SOCKET;
			m_socketLock.Leave( );		
			pOverlap = NULL;
			tempOverlap=NULL;
			if(invalidSocket)
				OnDisconnect( );	//通知连接被断开了		
			break;

			//发送完成,回收投出的重叠结构
		case OCL_SEND:
			/*m_socketLock.Enter( );			
			pKey->OnSend();
			m_socketLock.Leave( );*/
			//OnSend(pOverlap->frameBuf,pOverlap->bufferLen);
			delete pOverlap;
			pOverlap = NULL;
			tempOverlap=NULL;
			break;

			//关闭连接，会引起所有在些完成端口上等待的
		case OCL_CLOSE:
			//关闭socket
			m_socketLock.Enter( );			
			pKey->Close();
			m_socketLock.Leave( );			
			//通知连接被断开了
			OnDisconnect( );
			//删除重叠结构 
			delete pOverlap;
			pOverlap = NULL;
			tempOverlap=NULL;
			invalidSocket = true;
			break;

		default:
			fge::log_debug("close socket,delete OverlappeStruct");
			delete pOverlap;
			pOverlap = NULL;
			tempOverlap=NULL;
			break;
		}

		if(invalidSocket)
			break;
	}
	if(tempOverlap)
	{
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );
	}
	if(pOverlap)
	{
		fge::log_debug("Work closed,delete OverlappeStruct");
		delete pOverlap;
		pOverlap = 0;
	}

	//SAFE_DELETE(lpBuffer);
	//fge::log_debug("work exit");
}
void TcpClient::OnRecv(char * pData, u_long nRecvLen)
{
	//将消息发出
	fge::Lock<fge::CCriticalSection> l(m_procLock);
	std::list<Chunnel::Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnRecv(pData,nRecvLen);
		i++;
	}
}
void TcpClient::OnSend(char * pData, u_long nSendLen)
{
	fge::Lock<fge::CCriticalSection> l(m_procLock);
	std::list<Chunnel::Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnSend(pData,nSendLen);
		i++;
	}
}
void TcpClient::OnDisconnect( )
{
	::PostQueuedCompletionStatus( m_hCompletionPort,0,0,0);

	fge::Lock<fge::CCriticalSection> l(m_procLock);
	std::list<Chunnel::Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnDisconnect( );
		i++;
	}
}
void TcpClient::OnClose( )
{
	//------------
	m_socketLock.Enter( );
	m_socket.Close( );

	// To avoid memory leaks we should delete all results from queue.
	BOOL  queueState;						//队列状态
	DWORD TransferredBytesNum;				//传送字节数
	WSAOVERLAPPED *tempOverlap = NULL;		//标准重叠结构
	ocl::AsyncSocket	 *pKey = NULL;		//完成键
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

	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = 0;
	m_socketLock.Leave( );

	//------------
	fge::Lock<fge::CCriticalSection> l(m_procLock);
	std::list<Chunnel::Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnClose( );
		(*i)->ReleaseListener();
		i++;
	}
	m_processer.clear( );

	//------------
	CloseHandle(m_hWorkThread);
	m_hWorkThread = 0;
}