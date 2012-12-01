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
//�����Ͽ�����
void TcpServer::Disconnect( ConnectionID id )
{
	fge::Lock<fge::FreeMutex> l(m_csConn);		//�������ӹ�����
	ConnectionInfo* pClient = m_connManager.GetConnectionInfo(id);
	if(!pClient)
		return;
	pClient->asyncSocket.Close();
	//asyncSocket.Close��,���Զ�ɾ������
	//m_connManager.DelConnection( id );
}
int TcpServer::Open( const char* host, int port, int workNum )
{
	if(m_hCompletionPort!=0)
		return S_OK;

	INSTANCE( ocl::SocketInitializtion );

	// ����һ����iocp�ṹ:
	m_hCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );
	int hr = m_listener.Open( SOCK_STREAM,AF_INET, IPPROTO_TCP,0 );
	if( hr!=S_OK )
		return hr;
	hr = m_listener.Bind( host, port );
	if( hr!=S_OK )
		return hr;

	//�����������߳�
	if(workNum == 0)
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		workNum = sysInfo.dwNumberOfProcessors*2;
	}
	m_hWorkThread.resize( workNum );

	for( u_long i=0; i<m_hWorkThread.size(); i++ )
		m_hWorkThread[i] = (HANDLE)_beginthreadex(NULL,0,S_WorkStart,reinterpret_cast<LPVOID>(this),0,NULL);

	//�������Ӽ����߳�
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
	fge::Lock<fge::FreeMutex> l(m_csConn);		//�������ӹ�����
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
		case WSAEFAULT:                         // ָ�����
			// ��־��¼
			fge::log_error("TcpServer::Send ���ݳ���[%d] ", err);
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

	//�жϻ���ռ��Ƿ��㹻
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

		if(!isEnough)	// ����ռ䲻��,��ȴ�.
			::Sleep(1);
		else
			break;
	}


	u_long newLast = m_sendlast;
	//����ͻ��˱�źͷ��͵ĳ���
	memcpy(&m_sendBuff[newLast],&id,sizeof(id));			 newLast+=sizeof(id);
	memcpy(&m_sendBuff[newLast],&nSendLen,sizeof(nSendLen)); newLast+=sizeof(nSendLen);
	//���淢��ͬ��
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

	//�رռ���socket
	m_listener.Close( );
	//�ȴ������߳̽���
	WaitForSingleObject(m_hListenTherad, INFINITE);
	CloseHandle(m_hListenTherad);
	m_hListenTherad = 0;

	//�رշ����߳�
	m_csSend.Enter();
	m_sendState |= SEND_STATE_EXIT;
	m_sendEvent.SetEvent();
	m_csSend.Leave();
	WaitForSingleObject(m_hSendTherad, INFINITE);
	CloseHandle(m_hSendTherad);
	m_hSendTherad = 0;

	//�����¼���֪ͨ�������߳��˳�	
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
		// ����һ������
		SOCKET_HANDLE hClientSocket = m_listener.Accept( );

		// ����õ���Ч�ľ������Ϊ�����
		if (INVALID_SOCKET != hClientSocket)
		{
			Accept(hClientSocket, m_listener.GetAddr());
		}
		// �����¼�ʹ���
		else
		{
			int nError = ::WSAGetLastError();
			ErrorMsg("��������ʱ�����׽���ʧ��");
			switch (nError)
			{
			case WSAEINTR:
				bRun = false;
				break;
			case WSAENETDOWN:	// �������Ӵ���
				ErrorMsg("�������Ӵ���.");
				break;
			case WSAEFAULT:		// ָ�������Ч
				ErrorMsg("ָ�������Ч.");
				break;
			case WSAENOBUFS:	// ָ����Ч���ڴ�ռ䲻��
				ErrorMsg("�ڴ�ռ䲻��.");
				break;
			case WSAENOTSOCK:	// ����һ��Socket���
				ErrorMsg("����һ��Socket���.");
				break;
			case WSAEACCES:		// ���ʱ��ܾ�
				ErrorMsg("���ʱ��ܾ�.");
				break;
			case WSAEWOULDBLOCK://The socket is marked as nonblocking and no connections are present to be accepted.
				ErrorMsg("The socket is marked as nonblocking and no connections are present to be accepted.");
				break;
			default:
				{      
					ErrorMsg("δ֪ԭ��");
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
		{//�������״̬��Ϊ0,�����ͻ�������Ϊ��
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
		//������Ͷ���Ϊ��,��
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
	{//���뵽���ӹ�����ʧ��:
		delete key;
		key = 0;
		::closesocket(hClientSocket);
		return;
	}

	// ����iocp��client socket:
	::CreateIoCompletionPort((HANDLE)hClientSocket, m_hCompletionPort, (ULONG_PTR)key, 0);
	this->OnAccept( key->strIP,key->nPort,key->connid );	
	key->asyncSocket.Recv( );
}
void TcpServer::Work( )
{
	BOOL queueState;					//����״̬
	DWORD TransferredBytesNum;			//�����ֽ���
	WSAOVERLAPPED *tempOverlap = NULL;	//��׼�ص��ṹ
	ocl::ConnectionInfo	 *pKey = NULL;		//��ɼ�
	ocl::OverlappeStruct *pOverlap = NULL;	//��չ�ص��ṹ
	int err(0);							//����ֵ

	while(true)
	{
		pOverlap = 0;
		//��һ����ɶ˿��ϵȴ�,ÿ��һ���ص�������ɴ˵ȴ������.
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

		//���õ���ָ��OVERLAPPED�ṹ��ָ���Ժ�
		//������CONTAINING_RECORD��ȡ������ָ����չ�ṹ��ָ��
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );
		//����յ���0�ֽ����ݣ����ʾԶ�������Ѿ��ر�
		if(TransferredBytesNum ==0 && pOverlap->opType==OCL_RECV)
			pOverlap->opType = OCL_CLOSE;

		pOverlap->bufferLen = TransferredBytesNum;
		bool invalidSocket = false;
		switch( pOverlap->opType )
		{			
		case OCL_RECV:	//�յ�һ�����ݰ�,�������Ͷһ�����ղ���.
			OnRecv(pOverlap->frameBuf,pOverlap->bufferLen,pKey->connid);
			pKey->socketLock.Enter( );			
			pKey->asyncSocket.Recv( pOverlap );
			invalidSocket = pKey->asyncSocket.GetHandle() == INVALID_SOCKET;
			pKey->socketLock.Leave( );
			if(invalidSocket) 
				OnDisconnect( pKey->connid );
			pOverlap = NULL;
			break;
			
		case OCL_SEND:	//�������,����Ͷ�����ص��ṹ
			OnSend(pOverlap->frameBuf,pOverlap->bufferLen,pKey->connid);
			delete pOverlap;
			pOverlap = NULL;
			break;
			
		case OCL_CLOSE:	//�ر����ӣ�������������Щ��ɶ˿��ϵȴ���
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
	fge::Lock<fge::FreeMutex> l(m_csConn);		//�������ӹ�����
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
	BOOL queueState;						//����״̬
	DWORD TransferredBytesNum;				//�����ֽ���
	WSAOVERLAPPED *tempOverlap = NULL;		//��׼�ص��ṹ
	ocl::ConnectionInfo	 *pKey = NULL;		//��ɼ�
	ocl::OverlappeStruct *pOverlap = NULL;	//��չ�ص��ṹ
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

	//fge::Lock<fge::CCriticalSection> l(m_workLock);//����,�����̶߳��ѹر�.

	std::list<Listener*>::iterator i = m_processer.begin( );
	while( i!=m_processer.end() )
	{
		(*i)->OnClose( );
		i++;
	}
}
void TcpServer::OnDisconnect( ConnectionID id )
{
	{//֪ͨDisconnect�¼�
		fge::Lock<fge::CCriticalSection> l(m_workLock);	
		std::list<Listener*>::iterator i = m_processer.begin( );
		while( i!=m_processer.end() )
		{
			(*i)->OnDisconnect( id );
			i++;
		}
	}

	fge::Lock<fge::FreeMutex> l(m_csConn);		//�������ӹ�����
	m_connManager.DelConnection( id );
}