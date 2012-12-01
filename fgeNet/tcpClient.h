/* 
 Copyright (C) 2007-2009, WangYou Technology

 ���� :	�˳�
 QQ   :	23427470
 Mail : aron_d@yeah.net 

 ���� :	����ͨ���������շ����ݵĽӿ�,��ӦTCP/IP�Ĵ����, ��һ���Э����TCP/UDP����

 */
#pragma once
#include "oclSocket.h"
#include <fgemutex.h>

namespace ocl
{
	//ͨ��,����ά��������·,�������շ�����
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

	//������ά��TCP����,��֮��Ӧ�Ļ���UDP.
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
		//�����̴߳�����
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
		HANDLE						m_hWorkThread;		// �߳̾��		
		HANDLE						m_hCompletionPort;	// ��ɶ˿ں��Ķ�����
		fge::CCriticalSection		m_procLock;
		fge::CCriticalSection		m_socketLock;
	};

	class UdpClient : public Chunnel
	{
	};
}