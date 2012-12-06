/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-8-1

** 版本	 :	1.0
** 描述  :  网络消息监听,将收到的消息发送到主服务器.
**/

#pragma once
#include "GameProtocol.h"
#include <net\oclFrameProtocol.h>

using namespace ocl;

namespace serverframe{
class CNetMsgListener: public TcpServer::Listener
{
public:
	CNetMsgListener(void);
	~CNetMsgListener(void);

private:
	virtual void	OnAccept( const char* ip, int port, ConnectionID id );
	virtual void	OnDisconnect( ConnectionID id );
	virtual void	OnSend( const char* body, u_long size, ConnectionID id );
	virtual void	OnRecv( const char* body, u_long size, ConnectionID id );
	virtual void	OnClose( );
public:
	void	OnRecvFrameData(const char* body, u_long size, byte flag);	//消息解析回调
private:
	//typedef std::map<unsigned int, std::string>	MsgBuf;
	//MsgBuf	m_msgBuf;
	ConnectionID		m_tempID;
	std::map<unsigned int,ocl::FrameProtocol>	m_protocol;
};
class CUpdateMsgListener: public TcpClient::Listener
{
public:
	CUpdateMsgListener(void){};
	~CUpdateMsgListener(void){};

private:
	virtual void	OnDisconnect(  ){};
	virtual void	OnSend( const char* body, u_long size ){};
	virtual void	OnRecv( const char* body, u_long size );
	virtual void	OnClose( ){};
public:
	void OnRecvFrameData(const char* body, u_long size, byte flag);	//消息解析回调
private:
	ocl::FrameProtocol	m_protocol;
};
struct Net
{
	static TcpServer* s_pNetGameServer;
	static TcpClient* s_pNetUpdateServer;
	
	static u_long	SendToClient( const void* body, u_long size, ConnectionID id);
	static u_long	SendToClient( u_long msgid, const void* body, u_long size, ConnectionID id);
	static u_long	SendToUpdate( const void* body, u_long size );
};

}