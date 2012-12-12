#include "StdAfx.h"
#include "NetMsg.h"

using namespace fge;
using namespace protocol;
using namespace serverframe;

TcpServer* Net::s_pNetGm=0;
TcpServer* Net::s_pNetClient=0;
TcpClient* Net::s_pNetLogin = 0;

ocl::FrameProtocol fp;
struct SendFrameProtocol{
	SendFrameProtocol(TcpServer* _net, ConnectionID _id ){
		id = _id;
		pNet = _net;
	}
	u_long	operator()( const void* body, u_long size )
	{
		assert(pNet);
		return pNet->Send((const char*)body,size,id);
	}
	ConnectionID id;
	TcpServer* pNet;
};
struct SendToUpdateServer{
	SendToUpdateServer( ){}
	u_long	operator()( const void* body, u_long size )
	{
		assert(Net::s_pNetUpdateServer);
		return Net::s_pNetUpdateServer->Send((const char*)body,size);
	}
};

u_long	Net::SendToClient( const void* body, u_long size, ConnectionID id )
{
	return fp.Send((const char*)body,size,SendFrameProtocol(Net::s_pNetClient,id));
	//return Net::s_pNetGameServer->Send((const char*)body,size,id);
}
u_long	Net::SendToClient( u_long msgid, const void* body, u_long size, ConnectionID id)
{
	protocol::NetMsg head;
	head.msg_id = msgid;
	head.msg_len = size;
	head.msg_num = 1;
	ocl::Packer	msg;
	msg.AddToBuf(&head,sizeof(head));
	msg.AddToBuf(body,size);
	return SendToClient(msg(),msg.GetBufferLen(),id);
}
u_long	Net::SendToUpdate( const void* body, u_long size )
{
	return fp.Send((const char*)body,size,SendToUpdateServer());
}

ClientListener::ClientListener(void)
{
}

ClientListener::~ClientListener(void)
{
}

void	ClientListener::OnAccept( const char* ip, int port, ConnectionID id )
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	NetConnected msg;
	msg.connID = id;
	sprintf(msg.name,"%s:%d",ip,port);
	ServerMsgHead head;
	head.cnid = id;
	head.msg_num = 1;
	pMsgQueue->Push(protocol_name::NET_CONNECTED,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)&msg,sizeof(NetConnected));
}
void	ClientListener::OnDisconnect( ConnectionID id )
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	NetDisconnected msg;
	msg.connID = id;
	ServerMsgHead head;
	head.cnid = id;
	head.msg_num = 1;
	pMsgQueue->Push(protocol_name::NET_DISCONNECTED,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)&msg,sizeof(NetConnected));
}
void	ClientListener::OnSend( const char* body, u_long size, ConnectionID id )
{

}
void	ClientListener::OnRecv( const char* body, u_long size, ConnectionID id )
{
	if( size<=0 )
		return;
	m_tempID = id;
	m_protocol[id.nID].Recv(body,size,fge::Delegate(this,&CNetMsgListener::OnRecvFrameData));
}
void	ClientListener::OnClose( )
{

}
//消息解析回调
void	ClientListener::OnRecvFrameData(const char* body, u_long size, byte flag)
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	protocol::NetMsg *pMsg = (NetMsg*)body;
	ServerMsgHead head;
	head.cnid = m_tempID;
	head.msg_num = pMsg->msg_num;

	if(pMsg->msg_len <= size)
		pMsgQueue->Push(pMsg->msg_id,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)(pMsg+1),size-sizeof(NetMsg));

}


//////////////////////////////////////////////////////////////////////////
void	LoginListener::OnRecv( const char* body, u_long size )
{
	if( size<=0 )
		return;
	m_protocol.Recv(body,size,fge::Delegate(this,&CUpdateMsgListener::OnRecvFrameData));
}
//消息解析回调
void	LoginListener::OnRecvFrameData(const char* body, u_long size, byte flag)
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	protocol::NetMsg *pMsg = (NetMsg*)body;
	ServerMsgHead head;
	head.cnid.nID = 0;
	head.msg_num = pMsg->msg_num;

	if(pMsg->msg_len <= size)
		pMsgQueue->Push(pMsg->msg_id,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)(pMsg+1),size-sizeof(NetMsg));

}
//////////////////////////////////////////////////////////////////////////
void	GmListener::OnAccept( const char* ip, int port, ocl::ConnectionID id )
{
	fge::log_debug("OnAccept gm[%d]: %s:%d", id.nID, ip, port);
};
void	GmListener::OnDisconnect( ocl::ConnectionID id )
{
	fge::log_debug("OnDisconnect gm[%d]", id.nID);
};
void	GmListener::OnSend( const char* body, u_long size, ocl::ConnectionID id )
{

};
void	GmListener::OnRecv( const char* body, u_long size, ocl::ConnectionID id )
{
}
void	GmListener::OnClose( )
{
};