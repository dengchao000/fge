//------------------------------------------
// ����:�˳�
// ʱ��:2006.4.1
// ����:Э��ͷ�ļ�
//------------------------------------------

#pragma once
#ifndef byte
typedef unsigned char byte;
#endif
namespace ocl{

	const int FLAG_OVER	= 0;	//��ʼ��־
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
		OCL_OK = 0,			//�޴���
		OCL_NOTCONN,		//������
		OCL_NOSOCKET,		//���׽���
		OCL_NETBUSY,		//����æ
		OCL_IO_PENDING,		//IOδ����
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