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

	// ����һ����iocp�ṹ:
	m_hCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );
	m_hWorkThread = (HANDLE)_beginthreadex(NULL,0,Run,reinterpret_cast<LPVOID>(this),0,NULL);
	//// ��hClientSocket����ɶ˿ڰ�
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
	case WSAEFAULT:			// ָ�����
		// ��־��¼
		fge::log_error("TcpClient::Send ���ݳ���[%d] ", err);
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
	//�����¼���֪ͨ�˳�	
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
	BOOL queueState;					//����״̬
	DWORD TransferredBytesNum;			//�����ֽ���
	WSAOVERLAPPED *tempOverlap = NULL;	//��׼�ص��ṹ
	ocl::AsyncSocket	 *pKey = NULL;	//��ɼ�
	ocl::OverlappeStruct *pOverlap = NULL;	//��չ�ص��ṹ
	int err(0);							//����ֵ

	while(true)
	{
		pOverlap = NULL;
		//��һ����ɶ˿��ϵȴ�,ÿ��һ���ص�������ɴ˵ȴ������.
		queueState = ::GetQueuedCompletionStatus( m_hCompletionPort,&TransferredBytesNum,(PULONG_PTR)&pKey,&tempOverlap,INFINITE );
		if(tempOverlap==0)
			break;

		//���õ���ָ��OVERLAPPED�ṹ��ָ���Ժ�
		//������CONTAINING_RECORD��ȡ������ָ����չ�ṹ��ָ��
		pOverlap = CONTAINING_RECORD( tempOverlap,OverlappeStruct,overlap );

		//����յ���0�ֽ����ݣ����ʾԶ�������Ѿ��ر�
		if(TransferredBytesNum ==0 && pOverlap->opType==OCL_RECV)
			pOverlap->opType = OCL_CLOSE;
		bool invalidSocket = false;

		pOverlap->bufferLen = TransferredBytesNum;
		switch( pOverlap->opType )
		{
			//�յ�һ�����ݰ�,�������Ͷһ�����ղ���.
		case OCL_RECV:
			OnRecv(pOverlap->frameBuf,pOverlap->bufferLen);
			m_socketLock.Enter( );	
			pKey->Recv( pOverlap );	//Ͷ�ݽ��ղ���,���ʧ��,���ر�Socket.
			invalidSocket = pKey->GetHandle() == INVALID_SOCKET;
			m_socketLock.Leave( );		
			pOverlap = NULL;
			tempOverlap=NULL;
			if(invalidSocket)
				OnDisconnect( );	//֪ͨ���ӱ��Ͽ���		
			break;

			//�������,����Ͷ�����ص��ṹ
		case OCL_SEND:
			/*m_socketLock.Enter( );			
			pKey->OnSend();
			m_socketLock.Leave( );*/
			//OnSend(pOverlap->frameBuf,pOverlap->bufferLen);
			delete pOverlap;
			pOverlap = NULL;
			tempOverlap=NULL;
			break;

			//�ر����ӣ�������������Щ��ɶ˿��ϵȴ���
		case OCL_CLOSE:
			//�ر�socket
			m_socketLock.Enter( );			
			pKey->Close();
			m_socketLock.Leave( );			
			//֪ͨ���ӱ��Ͽ���
			OnDisconnect( );
			//ɾ���ص��ṹ 
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
	//����Ϣ����
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
	BOOL  queueState;						//����״̬
	DWORD TransferredBytesNum;				//�����ֽ���
	WSAOVERLAPPED *tempOverlap = NULL;		//��׼�ص��ṹ
	ocl::AsyncSocket	 *pKey = NULL;		//��ɼ�
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