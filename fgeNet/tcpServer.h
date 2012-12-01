/* 
 Copyright (C) 2007-2009, WangYou Technology

 ���� :	�˳�
 QQ   :	23427470
 Mail : aron_d@yeah.net 

 ���� :	����ͨ���������շ����ݵĽӿ�,��ӦTCP/IP�Ĵ����, ��һ���Э����TCP/UDP����

 */
#pragma once
#include "connectionManager.h"
#include <fgeSharedObject.h>
namespace ocl
{
	class TcpServer : public fge::SharedObject
	{
	public:
		class Listener
		{
		public:
			Listener( ){ }
			virtual ~Listener( ){ }
			virtual void   ReleaseListener( ) { delete this; }
			virtual void	OnAccept( const char* ip, int port, ConnectionID id ) = 0;
			virtual void	OnDisconnect( ConnectionID id ) = 0;
			virtual void	OnSend( const char* body, u_long size, ConnectionID id ) = 0;
			virtual void	OnRecv( const char* body, u_long size, ConnectionID id ) = 0;
			virtual void	OnClose( ) = 0;
		};
	public:
		TcpServer( );
		virtual ~TcpServer( );

		virtual int		Open( const char* host, int port, int workNum);
		virtual u_long	Send( const char* body, u_long size, ConnectionID id);
		virtual void	Close( );

		virtual void	AddListener( Listener* proc );
		void			Disconnect( ConnectionID id );	//�����Ͽ�����
		void			AddDataModule( DataProcess* pdm ){ m_dataProcessList.push_back(pdm); }
		const char*		GetErrorMsg( ){ return m_errMsg.c_str(); }
	private:
		//�����̴߳�����
		static unsigned int WINAPI S_WorkStart(LPVOID	lpParam);
		static unsigned int WINAPI S_ListenStart(LPVOID	lpParam);
		static unsigned int WINAPI S_SendStart(LPVOID	lpParam);
	protected:
		virtual void	SendWork( );
		virtual void	Work( );
		virtual void	Listen( );

		void			Accept( SOCKET_HANDLE, SOCKADDR_IN& addr );

		void			OnSend(const char * pData, u_long nSendLen, ConnectionID id);
		void			OnRecv(const char * pData, u_long nRecvLen, ConnectionID id);
		void			OnClose( );
		void			OnAccept( const char* ip, int port, ConnectionID id );
		void			ErrorMsg(const char* pMsg){ if(pMsg) m_errMsg = pMsg; else m_errMsg = ""; }
		void			OnDisconnect( ConnectionID id );		// �Ͽ�����	
		
		void			SendToBuff( const char * pData, u_long nSendLen, ConnectionID& id );
		u_long			SendToClient(const char* body, u_long size, ConnectionID& id);
	private:
		ocl::ListenerSocket			m_listener;
		std::list<Listener*>		m_processer;
		std::vector<HANDLE>			m_hWorkThread;		// �߳̾��		
		HANDLE						m_hListenTherad;
		HANDLE						m_hCompletionPort;	// ��ɶ˿ں��Ķ�����
		HANDLE						m_hSendTherad;
		fge::CCriticalSection		m_workLock;

		ConnectionManager			m_connManager;
		std::list<DataProcess*>		m_dataProcessList;
		fge::FreeMutex				m_csConn;
		std::string					m_errMsg;

		//Send�߳�
		enum{ SEND_BUFF_LEN=16*1024*1024,SEND_STATE_EXIT=1,SEND_STATE_EVENT=2,SEND_STATE_BUSY=4, };	//16M IO������
		fge::HEvent		m_sendEvent;
		fge::FreeMutex	m_csSend;		
		char		m_sendBuff[SEND_BUFF_LEN+16];
		u_long		m_sendhead;
		u_long		m_sendlast;
		u_long		m_sendState;
	};
	typedef fge::SharedPtr<ocl::TcpServer> TcpServerPtr;
}