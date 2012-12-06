#pragma once
#include "tcpServer.h"
#include "tcpClient.h"
class ClientListener: public ocl::TcpServer::Listener
{
	virtual void	OnAccept( const char* ip, int port, ocl::ConnectionID id ){
		log_debug("OnAccept client[%d]: %s:%d", id.nID, ip, port);
	};
	virtual void	OnDisconnect( ocl::ConnectionID id ){
		log_debug("OnDisconnect client[%d]", id.nID);
	};
	virtual void	OnSend( const char* body, u_long size, ocl::ConnectionID id ){};
	virtual void	OnRecv( const char* body, u_long size, ocl::ConnectionID id ){

		log_debug("OnRecv client[%d]: %d", id.nID,size);
	};
	virtual void	OnClose( ){};
};

class LoginListener: public ocl::TcpClient::Listener
{
	virtual void	OnSend( const char* body, u_long size ){

	};
	virtual void	OnRecv( const char* body, u_long size ){
		log_debug("OnRecv login listener: %d", size);
	};
	virtual void	OnClose( ){
		log_debug("OnClose login listener");
	};
	virtual void	OnDisconnect( ){
		log_debug("OnDisconnect login listener");
	};
};
class GmListener: public ocl::TcpServer::Listener
{
	virtual void	OnAccept( const char* ip, int port, ocl::ConnectionID id ){
		log_debug("OnAccept gm[%d]: %s:%d", id.nID, ip, port);
	};
	virtual void	OnDisconnect( ocl::ConnectionID id ){
		log_debug("OnDisconnect gm[%d]", id.nID);
	};
	virtual void	OnSend( const char* body, u_long size, ocl::ConnectionID id ){

	};
	virtual void	OnRecv( const char* body, u_long size, ocl::ConnectionID id );
	virtual void	OnClose( ){};
};
