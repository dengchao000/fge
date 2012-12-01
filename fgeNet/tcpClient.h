/* 
 Copyright (C) 2007-2009, WangYou Technology

 作者 :	邓超
 QQ   :	23427470
 Mail : aron_d@yeah.net 

 描述 :	网络通道，用于收发数据的接口,对应TCP/IP的传输层, 这一层的协议有TCP/UDP两种

 */
#pragma once
#include "oclSocket.h"
#include <fgemutex.h>

namespace ocl
{
	//通道,负责维护数据链路,并在上收发数据
	class Chunnel
	{
	public:
		class Listener
		{
		public:
			Listener( ){ }
			virtual ~Listener( ){ }
			virtual void	ReleaseListener( ) { delete this; }
			virtual void	OnSend( const char* body, u_long size ) = 0;
			virtual void	OnRecv( const char* body, u_long size ) = 0;
			virtual void	OnClose( ) = 0;
			virtual void	OnDisconnect( )=0;
		};
	public:
		Chunnel(void){}
		virtual ~Chunnel(void){}

		virtual void	AddListener( Chunnel::Listener* proc ) = 0;
		virtual int		Open( const char* host, int port ) = 0;
		virtual u_long	Send( const char* body, u_long size ) = 0;
		virtual void	Close( ) = 0;
	};

	//负责建立维护TCP链接,与之对应的还有UDP.
	class TcpClient : public Chunnel
	{
	public:
		TcpClient( );
		virtual ~TcpClient( );

		virtual int		Open( const char* host, int port );
		virtual u_long	Send( const char* body, u_long size );
		virtual void	Close( );

		virtual void	AddListener( Chunnel::Listener* proc );
	private:
		//启动线程处理函数
		static unsigned int WINAPI Run(LPVOID	lpParam);

	protected:
		virtual void	Work( );
		void			OnSend(char * pData, u_long nSendLen);
		void			OnRecv(char * pData, u_long nRecvLen);
		void			OnClose( );
		void			OnDisconnect( );
	private:
		std::list<Chunnel::Listener*>	m_processer;
		ocl::AsyncSocket			m_socket;
		HANDLE						m_hWorkThread;		// 线程句柄		
		HANDLE						m_hCompletionPort;	// 完成端口核心对象句柄
		fge::CCriticalSection		m_procLock;
		fge::CCriticalSection		m_socketLock;
	};

	class UdpClient : public Chunnel
	{
	};
}