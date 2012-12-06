/** 
- Author:	�˳�
- QQ:		23427470
- Time:	    2011-3-21
-
- ������	��������н�Ҫʹ�õĳ���
*/

#pragma once

namespace protocol
{
	const TCHAR NOTHING_CHAR[]=		TEXT("");			// ���ַ���
	const TCHAR	PASSWORD_CHAR[]=	TEXT("123");		// ���ַ���
	const int NAME_LEN				= 20;				// ���Ƴ���
	const int ACCOUNT_LEN			= 17;				// �ʺ����Ƴ���
	const int PASSWORD_LEN			= 17;				// ���볤��
	
	// IP��ַ����
	#ifndef IP_LEN
	#define IP_LEN	20
	#endif

	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };

	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };

	/** Э��ͷ */
	struct NetMsgHead
	{
		DWORD	msg_len:22;		//��Ϣ����
		DWORD	msg_num:10;		//��Ϣ����
		DWORD	msg_id;			//��Ϣ���
	};

	template<typename T2>
	struct PackMsg :public NetMsgHead, public T2{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};
	
	template<>
	struct PackMsg<NetMsgHead> :public NetMsgHead{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};

}

