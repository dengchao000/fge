/* 
 Copyright (C) 2007-2009, WangYou Technology

 作者 :	邓超
 QQ   :	23427470
 Mail : aron_d@yeah.net 

 描述 :	网络通道，用于收发数据的接口,对应TCP/IP的传输层, 这一层的协议有TCP/UDP两种

 */
#pragma once
#include <list>
#include <vector>
#include <fgeThread.h>
#include <fgeIndexArray.h>
#include "oclSocket.h"

namespace ocl
{	// 完成健
	struct ConnectionInfo
	{
	public:
		ConnectionInfo()
		{
			nPort = 0; strIP[0]=0;
		}

	public:
		char	strIP[20];			// 客户IP
		int		nPort;				// 客户端口		
		ConnectionID	connid;		// 编号
		AsyncSocket		asyncSocket; // 客户连接的套接字
		fge::FreeMutex	socketLock;
	};

	class ConnectionManager
	{
	public:
		ConnectionManager( );
		~ConnectionManager( );

		bool	AddConnection( ConnectionInfo* pClient );
		void	DelConnection( const ConnectionID& connid );

		ConnectionInfo*		GetConnectionInfo( const ConnectionID& connid );
	private:
		fge::CIndexArray<ConnectionInfo*>	m_clientKeys;
		fge::CCriticalSection m_cs;
	};

	//数据块
	struct DataBlock{
		const char* body;
		u_long size;
		DataBlock( const char* _body, u_long _size ){ body = _body; size = _size; }
	};

	//数据处理接口
	class DataProcess{
	public:
		virtual ~DataProcess(){}
		virtual void	Process( DataBlock& inData )=0;
	};
}