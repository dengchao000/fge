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
}

