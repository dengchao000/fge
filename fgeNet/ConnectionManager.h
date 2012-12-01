/* 
 Copyright (C) 2007-2009, WangYou Technology

 ���� :	�˳�
 QQ   :	23427470
 Mail : aron_d@yeah.net 

 ���� :	����ͨ���������շ����ݵĽӿ�,��ӦTCP/IP�Ĵ����, ��һ���Э����TCP/UDP����

 */
#pragma once
#include <list>
#include <vector>
#include <fgeThread.h>
#include <fgeIndexArray.h>
#include "oclSocket.h"

namespace ocl
{	// ��ɽ�
	struct ConnectionInfo
	{
	public:
		ConnectionInfo()
		{
			nPort = 0; strIP[0]=0;
		}

	public:
		char	strIP[20];			// �ͻ�IP
		int		nPort;				// �ͻ��˿�		
		ConnectionID	connid;		// ���
		AsyncSocket		asyncSocket; // �ͻ����ӵ��׽���
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

	//���ݿ�
	struct DataBlock{
		const char* body;
		u_long size;
		DataBlock( const char* _body, u_long _size ){ body = _body; size = _size; }
	};

	//���ݴ���ӿ�
	class DataProcess{
	public:
		virtual ~DataProcess(){}
		virtual void	Process( DataBlock& inData )=0;
	};
}