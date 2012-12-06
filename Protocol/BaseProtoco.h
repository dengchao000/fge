/** 
- Author:	邓超
- QQ:		23427470
- Time:	    2011-3-21
-
- 描述：	定义程序中将要使用的常量
*/

#pragma once

namespace protocol
{
	const TCHAR NOTHING_CHAR[]=		TEXT("");			// 空字符串
	const TCHAR	PASSWORD_CHAR[]=	TEXT("123");		// 空字符串
	const int NAME_LEN				= 20;				// 名称长度
	const int ACCOUNT_LEN			= 17;				// 帐号名称长度
	const int PASSWORD_LEN			= 17;				// 密码长度
	
	// IP地址长度
	#ifndef IP_LEN
	#define IP_LEN	20
	#endif

	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };

	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };

	/** 协议头 */
	struct NetMsgHead
	{
		DWORD	msg_len:22;		//消息长度
		DWORD	msg_num:10;		//消息数量
		DWORD	msg_id;			//消息编号
	};

	template<typename T2>
	struct PackMsg :public NetMsgHead, public T2{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};
	
	template<>
	struct PackMsg<NetMsgHead> :public NetMsgHead{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};

}

