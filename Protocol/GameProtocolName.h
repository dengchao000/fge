/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-8-12

** 版本	 :	1.0
** 描述  :  游戏协议名文件.
**/

#pragma once
namespace protocol_name
{
	//获取消息类型,返回 MSG_TYPE 
#define GET_MSG_TYPE(l)		(((DWORD)(l)) & 0xffff0000)
#define GET_MSG_ID(l)		(l/*(WORD)(((DWORD)(l)) & 0x0000ffff)*/)

	//将协议分段
	enum MSG_TYPE{
		NET_MSG =		0x10000,	//网络层消息
		SYSTEM_MSG =	0x20000,	//系统消息
		SERVER_MSG =	0x30000,	//服务器内部消息
		GM_MSG =		0x70000,	//GM消息
		SCRIPT_MSG =	0x80000,	//脚本消息
	};
	//协议编号
	enum
	{//----------网络消息-------------		
		NET_MSG_BEGIN = NET_MSG,
		NET_CONNECTED,			//连接
		NET_DISCONNECTED,		//断开
		NET_PULSE,				//断线检测
	}
	enum
	{//----------服务器消息-------------
		SERVER_BEGIN = NET_MSG,
		SERVER_SETUP,
		SERVER_UPDATE,
		SERVER_SHUTDOWN,
		SERVER_GETINFO,
	}
	enum
	{//----------GM消息-------------
		GMMSG_BEGIN = NET_MSG,
		GMMSG_CLOSESERVER,		//关闭服务器
	}
	enum
	{//----------脚本消息-------------
		SCRIPT_BEGIN = SCRIPT_MSG,
	};
}