#pragma once
#include "tcpServer.h"
#include "tcpClient.h"
class ClientListener: public ocl::TcpServer::Listener
{
public:
	virtual void	OnAccept( const char* ip, int port, ocl::ConnectionID id );
	virtual void	OnDisconnect( ocl::ConnectionID id );
	virtual void	OnSend( const char* body, u_long size, ocl::ConnectionID id );
	virtual void	OnRecv( const char* body, u_long size, ocl::ConnectionID id );
	virtual void	OnClose( );

	void OnRecvFrameData(const char* body, u_long size, byte flag);
private:
	ocl::ConnectionID m_tempID;
	std::map<unsigned int,ocl::FrameProtocol>	m_protocol;
};

class LoginListener: public ocl::TcpClient::Listener
{
public:
	virtual void	OnSend( const char* body, u_long size );
	virtual void	OnRecv( const char* body, u_long size );
	virtual void	OnClose( );
	virtual void	OnDisconnect( );
private:
	ocl::FrameProtocol	m_protocol;
};

class GmListener: public ocl::TcpServer::Listener
{
public:
	virtual void	OnAccept( const char* ip, int port, ocl::ConnectionID id );
	virtual void	OnDisconnect( ocl::ConnectionID id );
	virtual void	OnSend( const char* body, u_long size, ocl::ConnectionID id );
	virtual void	OnRecv( const char* body, u_long size, ocl::ConnectionID id );
	virtual void	OnClose( ){};
};

struct Net
{
	static TcpServer* s_pNetGm;
	static TcpServer* s_pNetClient;
	static TcpClient* s_pNetLogin;	

	static u_long	SendToClient( const void* body, u_long size, ConnectionID id);
	static u_long	SendToClient( u_long msgid, const void* body, u_long size, ConnectionID id);
	static u_long	SendToLogin( const void* body, u_long size );
	static u_long	SendToGm( const void* body, u_long size );
};
