/*
Copyright (C) 2007-2009, WangYou Technology

作者 :	邓超
QQ   :	23427470
Mail : aron_d@yeah.net 

描述 : 网络IO接口 
*/
#pragma once
#include <winsock2.h>
#include <windows.h>
#include <fgesingleton.h>
#include <fgedelegate.h>
#include <fgeMemoryObject.h>

#include "oclProtocol.h"
#pragma comment(lib, "Ws2_32.lib")
namespace ocl
{
	typedef ::SOCKET SOCKET_HANDLE;
	typedef long pid_t;
	//内部使用的类.用于初始化Socket
	class SocketInitializtion
	{
	public:
		SINGLETON(SocketInitializtion);
	private:
		SocketInitializtion( );
		~SocketInitializtion( );
	};

	class Socket
	{
	protected:
		Socket (void);
		/**
		* Not a virtual destructor.  Protected destructor to prevent
		* operator delete() from being called through a base class Socket
		* pointer/reference.
		*/
		~Socket (void);
	public:

		/// Wrapper around the <setsockopt> system call.
		int				SetOption (int level,int option,const char *optval,int optlen) const;
		/// Wrapper around the <getsockopt> system call.
		int				GetOption (int level,int option,char *optval,int *optlen) const;
		/**
		* Close the socket.
		* This method also sets the object's handle value to ACE_INVALID_HANDLE.
		*
		* @return The result of closing the socket; 0 if the handle value
		*         was already ACE_INVALID_HANDLE.
		*/
		virtual int		Close (void);
		
		/// Wrapper around the <socket> system call (no QoS).
		virtual int		Open (int type,	int protocol_family, int protocol, int reuse_addr);
		int				SetBlock (int value);
		void				SetHandle (SOCKET_HANDLE handle);

		SOCKET_HANDLE	GetSocketHandle( ) const { return GetHandle(); }
		SOCKET_HANDLE	GetHandle (void) const;
	private:
		SOCKET_HANDLE m_handle;		// Underlying I/O handle.
		static pid_t s_pid;			// Cache the process ID
	};


	//重叠IO数据结构
	struct OverlappeStruct : fge::CMemoryObjectThreadSafe
	{
		enum{ BUFF_LEN = 2048, };
		OVERLAPPED	overlap;				//标准重叠数据结构
		WSABUF		wsaBuf;					//重叠IO数据传输类型，包括char*和长度lenth. 
		char		frameBuf[BUFF_LEN];		//数据区，实际上代表wsaBuf.buffer
		int			bufferLen;				//包体长度 
		int			opType;					//操作码

		//复位
		void Reset()
		{
			wsaBuf.buf = frameBuf;
			wsaBuf.len = BUFF_LEN;
			memset( &overlap,0,sizeof(OVERLAPPED) );
			memset( frameBuf,0,BUFF_LEN );
			opType = 0;	
			bufferLen = 0;
		}
	};
	//异步Socket
	class AsyncSocket : public Socket
	{
	public:
		// = Initialization and termination methods.

		/// Constructor.
		AsyncSocket (void);
		/// Destructor.
		~AsyncSocket (void);

		/// Recv @a n bytes via Win32 <ReadFile> using overlapped I/O.
		u_long Recv ( );
		u_long Recv ( OverlappeStruct* pOverlappe );

		/// Send @a n bytes via Win32 <WriteFile> using overlapped I/O.
		u_long Send (const char *buf,u_long n);
		void   OnSend( );
	private:

		WORD	m_sendnum;
	};
	//监听Socket,负责监听连接
	class ListenerSocket : public Socket
	{
	public:
		/// Constructor.
		ListenerSocket (void);
		/// Destructor.
		~ListenerSocket (void);

		virtual int Open (int type,	int protocol_family, int protocol, int reuse_addr);
		
		int			Bind( const char* host, int port );
		SOCKET_HANDLE Accept ( ) const;
		SOCKADDR_IN&  GetAddr( ) { return m_addr; }
	private:
		SOCKADDR_IN		m_addr;
		int				m_addrLen;
	};


	////控制台
	//typedef void (fge::IConsign::*InputHandle)(char* pstr);
	//typedef fge::CDelegate< fge::IConsign,InputHandle > IOEvent;

	//struct CONSOLE_MSG
	//{
	//	int			id;
	//	UINT_PTR	WPARAM;
	//	LONG_PTR	LPARAM;
	//};

	//class IConsole
	//{
	//public:
	//	virtual ~IConsole( ){ }
	//	virtual void	Out( const char* pstr ) = 0;
	//	virtual void	Outln( const char* pstr ) = 0;
	//	virtual void	Input( IOEvent& e ) = 0;
	//	virtual void	CancleInput( ) = 0;

	//	virtual void	PostMsg( int id, UINT_PTR wParam, LONG_PTR lParam) = 0;
	//};
	//-----------------------
	struct ConnectionID
	{
	public:
		unsigned int nID;			// 套接字编号
		unsigned int nSyncID;			// 同步编号
	public:
		ConnectionID()
		{
			nID = 0;
			nSyncID = 0;
		}
		ConnectionID(unsigned int id, unsigned int sync)
		{
			nID = id;
			nSyncID = sync;
		}
		bool operator == (const ConnectionID & other) const 
		{
			return nID == other.nID && nSyncID == other.nSyncID;
		}
		bool operator != (const ConnectionID & other) const 
		{
			return nID != other.nID || nSyncID != other.nSyncID;
		}
	};

}