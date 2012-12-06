/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-8-12

** �汾	 :	1.0
** ����  :  ��ϷЭ�����ļ�.
**/

#pragma once
namespace protocol_name
{
	//��ȡ��Ϣ����,���� MSG_TYPE 
#define GET_MSG_TYPE(l)		(((DWORD)(l)) & 0xffff0000)
#define GET_MSG_ID(l)		(l/*(WORD)(((DWORD)(l)) & 0x0000ffff)*/)

	//��Э��ֶ�
	enum MSG_TYPE{
		NET_MSG =		0x10000,	//�������Ϣ
		SYSTEM_MSG =	0x20000,	//ϵͳ��Ϣ
		SERVER_MSG =	0x30000,	//�������ڲ���Ϣ
		GM_MSG =		0x70000,	//GM��Ϣ
		SCRIPT_MSG =	0x80000,	//�ű���Ϣ
	};
	//Э����
	enum
	{//----------������Ϣ-------------		
		NET_MSG_BEGIN = NET_MSG,
		NET_CONNECTED,			//����
		NET_DISCONNECTED,		//�Ͽ�
		NET_PULSE,				//���߼��
	}
	enum
	{//----------��������Ϣ-------------
		SERVER_BEGIN = NET_MSG,
		SERVER_SETUP,
		SERVER_UPDATE,
		SERVER_SHUTDOWN,
		SERVER_GETINFO,
	}
	enum
	{//----------GM��Ϣ-------------
		GMMSG_BEGIN = NET_MSG,
		GMMSG_CLOSESERVER,		//�رշ�����
	}
	enum
	{//----------�ű���Ϣ-------------
		SCRIPT_BEGIN = SCRIPT_MSG,
	};
}