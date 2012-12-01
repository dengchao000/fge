//------------------------------------------
// 作者:邓超
// 时间:2006.4.1
// 描述:协议头文件
//------------------------------------------

#pragma once
#ifndef byte
typedef unsigned char byte;
#endif
namespace ocl{

	const int FLAG_OVER	= 0;	//开始标志
	const int FLAG_KEEP	= 1;

	const int MAX_ACCEPT_NUM	= 10000;
	const int MAX_CONNECT_NUM	= 10000;
	const int SEND_BUFNUM		= 10000;

	enum OCL_OPERATOR
	{
		OCL_CLOSE,
		OCL_RECV,
		OCL_SEND,
		OCL_ACCEPT,
	};
	enum OCL_ERROR
	{
		OCL_OK = 0,			//无错误
		OCL_NOTCONN,		//无连接
		OCL_NOSOCKET,		//无套接字
		OCL_NETBUSY,		//网络忙
		OCL_IO_PENDING,		//IO未决定
	};
	class Protocol
	{
	public:
		Protocol( ){ }
		virtual ~Protocol( ){ }
		virtual const char*	Send( const char* body, u_long& size )	{ return body; }
		virtual const char*	Recv( const char* body, u_long& size ){ return body; }
	};
};