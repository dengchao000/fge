#include "StdAfx.h"
#include "oclSocket.h"
#include <fgelog.h>
//#include <fgeDebug.h>

namespace ocl
{

SocketInitializtion::SocketInitializtion( )
{
	INT nRet = 0;
	WSADATA wsaData;
	nRet = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
}
SocketInitializtion::~SocketInitializtion( )
{
	::WSACleanup( );
}
//-------------------------------------------
Socket::Socket (void)
{
	m_handle = 0;
}

Socket::~Socket (void)
{
}

int Socket::SetBlock (int value) 
{
	SOCKET_HANDLE sock = m_handle;
	return ::ioctlsocket (sock, FIONBIO, reinterpret_cast<unsigned long *> (&value));
}

SOCKET_HANDLE Socket::GetHandle (void) const
{
	return m_handle;
}
void Socket::SetHandle (SOCKET_HANDLE handle)
{
	m_handle = handle;
}

/// Wrapper around the <setsockopt> system call.
int Socket::SetOption (int level,int option,const char *optval,int optlen) const
{
	return ::setsockopt (GetHandle(),
		level,
		option,
		optval,
		optlen);
}

/// Wrapper around the <getsockopt> system call.
int Socket::GetOption (int level,int option,char *optval,int *optlen) const
{
	return ::getsockopt (GetHandle(),
		level,
		option,
		optval,
		optlen);
}

/**
* Close the socket.
* This method also sets the object's handle value to ACE_INVALID_HANDLE.
*
* @return The result of closing the socket; 0 if the handle value
*         was already ACE_INVALID_HANDLE.
*/
int Socket::Close (void)
{
	int result = 0;
	if (GetSocketHandle() != INVALID_SOCKET)
	{
		result = ::closesocket (GetHandle());
		this->SetHandle( INVALID_SOCKET );
	}
	return result;
}

/// Wrapper around the <socket> system call (no QoS).
int Socket::Open (int type,	int protocol_family, int protocol, int reuse_addr)
{
	char one = '1';

	this->SetHandle (::socket(protocol_family,type,protocol));

	if (this->GetSocketHandle() == INVALID_SOCKET)
		return GetLastError();
	else if (protocol_family != PF_UNIX
		&& reuse_addr
		&& this->SetOption (SOL_SOCKET,
		SO_REUSEADDR,
		&one,
		sizeof one) == -1)
	{
		this->Close();
		return INVALID_SOCKET;
	}
	return 0;
}
//-------------------------------------------
AsyncSocket::AsyncSocket(void)
{
	m_sendnum = 0;
}
AsyncSocket::~AsyncSocket(void)
{
}
u_long AsyncSocket::Recv ( )
{
	OverlappeStruct *lpBuffer = new OverlappeStruct;
	lpBuffer->Reset( );

	DWORD	flag = 0; //common value	
	lpBuffer->opType = OCL_RECV;
	int ret = ::WSARecv( this->GetSocketHandle(),&lpBuffer->wsaBuf,1,(LPDWORD)&(lpBuffer->bufferLen),&flag,&lpBuffer->overlap,0 );
	if(ret == SOCKET_ERROR)
	{
		if( GetLastError()!=WSA_IO_PENDING )
		{		
			delete lpBuffer;
			lpBuffer = NULL;
			fge::log_error("WSARecv error%d",GetLastError());
			this->Close();
		}		
	}
	return ret;
}
u_long AsyncSocket::Recv( OverlappeStruct* pOverlappe )
{
	pOverlappe->Reset( );
	DWORD	flag = 0; //common value	
	pOverlappe->opType = OCL_RECV;
	int ret = ::WSARecv( this->GetSocketHandle(),&pOverlappe->wsaBuf,1,(LPDWORD)&(pOverlappe->bufferLen),&flag,&pOverlappe->overlap,0 );
	if(ret == SOCKET_ERROR)
	{
		if( GetLastError()!=WSA_IO_PENDING )
		{		
			delete pOverlappe;
			pOverlappe = NULL;
			fge::log_error("WSARecv error: %d",GetLastError());
			this->Close();
		}
		//else
		//	::Sleep(1);
	}	
	return ret;
}
void   AsyncSocket::OnSend()
{
	//m_sendnum--;
}
/// Send @a n bytes via Win32 <WriteFile> using overlapped I/O.
u_long AsyncSocket::Send (const char *buf,u_long n)
{
	//�ְ�����
	int idx = 0;
	int infolen = 0;
	DWORD rterr = 0;
	while( n>0 )
	{	
		OverlappeStruct* pOverlap = new OverlappeStruct();
		if(pOverlap)
			pOverlap->Reset();
		else
			return SOCKET_ERROR;

		//������Ϣ������
		infolen = n < OverlappeStruct::BUFF_LEN ? (int)n : OverlappeStruct::BUFF_LEN;
		//��䷢�ͻ�����
		memcpy(pOverlap->frameBuf,buf+idx,infolen);		
		idx+=infolen;
		n -= infolen;

		//����OVERLAP�ṹ
		pOverlap->bufferLen = infolen ;
		pOverlap->wsaBuf.len = infolen;
		pOverlap->opType = OCL_SEND;

		//����
		DWORD sendNum(0);
		int ret = ::WSASend( this->GetSocketHandle(),&(pOverlap->wsaBuf),1,
			&sendNum,0,&(pOverlap->overlap),NULL );
		
		// �������ʧ�ܣ���ͻ��Ͽ����ӣ�����׽����д��󣬻��ӳٷ���
		if( ret == SOCKET_ERROR )
		{
			DWORD err = ::GetLastError();
			switch( err )
			{
			case WSA_IO_PENDING:					// work later
				rterr = err;
				break;
			case WSAEFAULT:                         // ָ�����
				return err;	
			default:
				// �ͷ��ص��ṹ�Ŀռ�
				delete pOverlap;
				pOverlap = NULL;
				this->Close();
				return err;				
			}
		}
	}
	return rterr;
}
ListenerSocket::ListenerSocket (void)
{
	m_addrLen = sizeof(m_addr);
}


ListenerSocket::~ListenerSocket (void)
{
}

SOCKET_HANDLE ListenerSocket::Accept ( ) const
{
	SOCKET clientSocket = ::WSAAccept(GetSocketHandle(), (SOCKADDR *)&m_addr, (LPINT)&m_addrLen, NULL, 0);
	return clientSocket;
}
int ListenerSocket::Open (int type,	int protocol_family, int protocol, int reuse_addr)
{
	int hr = Socket::Open (type,	protocol_family, protocol, reuse_addr);
	if( hr == INVALID_SOCKET )
		return INVALID_SOCKET;
	return S_OK;
	//int nSet = 0;
	//// �ı��׽��ֵ����ݽ��ջ�������С
	//nSet = 1024 * 16;
	//this->SetOption(SOL_SOCKET,hSetSock, SO_RCVBUF, (char *)&nSet, sizeof(nSet));

	//// �ı��׽��ֵ����ݷ��ͻ�������С
	//nSet = 1024 * 16;
	//setsockopt(hSetSock, SOL_SOCKET, SO_SNDBUF, (char *)&nSet, sizeof(nSet));
}
int	ListenerSocket::Bind( const char* host, int port )
{
	// �SOCKET
	SOCKADDR_IN internetAddr;
	internetAddr.sin_family = AF_INET;
	internetAddr.sin_addr.s_addr = inet_addr(host);
	internetAddr.sin_port = htons(port);
	if (::bind(GetSocketHandle(), (PSOCKADDR)&internetAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR){	return WSAGetLastError(); }

	// ����SOCKETΪ����ģʽ
	if (::listen(GetSocketHandle(), 2000) == SOCKET_ERROR) { return WSAGetLastError(); }
	return S_OK;
}
}