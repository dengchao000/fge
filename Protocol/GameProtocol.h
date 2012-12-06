/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-8-1

** �汾	 :	1.0
** ����  :  ��ϷЭ���ļ�.
**/

#pragma once
#include "./GameProtocolName.h"
namespace protocol
{
#pragma pack(push)	
#pragma pack(1)				//�ֽڶ���

	struct NetConnected{
		ocl::ConnectionID	connID;
		char	name[32];
	};
	struct NetDisconnected{
		ocl::ConnectionID	connID;
	};

	struct ServerMsgHead
	{
		WORD	msg_num;		//��Ϣ����
		ocl::ConnectionID cnid;	//�׽��ֱ��
	};
	//-----------------------------------
	struct GmCloseServer
	{
		DWORD	closeTime;
	};	
	//----------��������Ϣ---------------
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