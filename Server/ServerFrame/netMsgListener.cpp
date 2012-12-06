#include "StdAfx.h"
#include "netMsgListener.h"

#include <fgeMessageManager.h>

using namespace fge;
using namespace protocol;
using namespace serverframe;

TcpServer* Net::s_pNetGameServer=0;
TcpClient* Net::s_pNetUpdateServer = 0;
ocl::FrameProtocol fp;
struct SendFrameProtocol{
	SendFrameProtocol( ConnectionID _id ){
		id = _id;
	}
	u_long	operator()( const void* body, u_long size )
	{
		assert(Net::s_pNetGameServer);
		return Net::s_pNetGameServer->Send((const char*)body,size,id);
	}
	ConnectionID id;
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
	return fp.Send((const char*)body,size,SendFrameProtocol(id));
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

CNetMsgListener::CNetMsgListener(void)
{
}

CNetMsgListener::~CNetMsgListener(void)
{
}

void	CNetMsgListener::OnAccept( const char* ip, int port, ConnectionID id )
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
void	CNetMsgListener::OnDisconnect( ConnectionID id )
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	NetDisconnected msg;
	msg.connID = id;
	ServerMsgHead head;
	head.cnid = id;
	head.msg_num = 1;
	pMsgQueue->Push(protocol_name::NET_DISCONNECTED,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)&msg,sizeof(NetConnected));
}
void	CNetMsgListener::OnSend( const char* body, u_long size, ConnectionID id )
{
	
}
void	CNetMsgListener::OnRecv( const char* body, u_long size, ConnectionID id )
{
	if( size<=0 )
		return;
	m_tempID = id;
	m_protocol[id.nID].Recv(body,size,fge::Delegate(this,&CNetMsgListener::OnRecvFrameData));
}
void	CNetMsgListener::OnClose( )
{

}
//消息解析回调
void	CNetMsgListener::OnRecvFrameData(const char* body, u_long size, byte flag)
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
void	CUpdateMsgListener::OnRecv( const char* body, u_long size )
{
	if( size<=0 )
		return;
	m_protocol.Recv(body,size,fge::Delegate(this,&CUpdateMsgListener::OnRecvFrameData));
}
//消息解析回调
void	CUpdateMsgListener::OnRecvFrameData(const char* body, u_long size, byte flag)
{
	CMessageManager* pMsgQueue = INSTANCE(CServerFrame)->GetMessageQueue();
	protocol::NetMsg *pMsg = (NetMsg*)body;
	ServerMsgHead head;
	head.cnid.nID = 0;
	head.msg_num = pMsg->msg_num;

	if(pMsg->msg_len <= size)
		pMsgQueue->Push(pMsg->msg_id,(BYTE*)&head,sizeof(ServerMsgHead),(BYTE*)(pMsg+1),size-sizeof(NetMsg));

}