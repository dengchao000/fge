/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-8-1

** 版本	 :	1.0
** 描述  :  游戏协议文件.
**/

#pragma once
#include "./GameProtocolName.h"
namespace protocol
{
#pragma pack(push)	
#pragma pack(1)				//字节对齐

	struct NetConnected{
		ocl::ConnectionID	connID;
		char	name[32];
	};
	struct NetDisconnected{
		ocl::ConnectionID	connID;
	};

	struct ServerMsgHead
	{
		WORD	msg_num;		//消息数量
		ocl::ConnectionID cnid;	//套接字编号
	};
	//-----------------------------------
	struct GmCloseServer
	{
		DWORD	closeTime;
	};	
	//----------服务器消息---------------
	struct ServerSetup
	{
		char ip[IP_LEN];
		DWORD port;
		DWORD maxClientNum;
	};
	struct ServerUpdate
	{
		DWORD clientNum;
	};
	struct ServerShutdown
	{
	};
	struct ServerInfo
	{
		char ip[IP_LEN];
		DWORD port;
	};
#pragma pack()
#pragma pack(pop)
}