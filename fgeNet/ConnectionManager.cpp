#include "stdafx.h"
#include "connectionManager.h"
namespace ocl
{

	ConnectionManager::ConnectionManager( )
	{
		m_clientKeys.Init( 0,MAX_CONNECT_NUM ); 
	}
	ConnectionManager::~ConnectionManager( )
	{
		fge::Lock<fge::CCriticalSection> lock(m_cs);
		fge::CIndexArray<ConnectionInfo*>::Iterator i = m_clientKeys.GetIterator( );
		for(; !i.isdone(); i.next() )
		{
			delete i.get();
		}
		m_clientKeys.Clear();
	}

	bool	ConnectionManager::AddConnection( ConnectionInfo* pClient )
	{
		fge::Lock<fge::CCriticalSection> lock(m_cs);
		static unsigned int s_nSynchronizationID = 0;
		if(m_clientKeys.IsFull())
			return false;
		pClient->connid.nID = m_clientKeys.Add(pClient);
		pClient->connid.nSyncID = ++s_nSynchronizationID;
		return true;
	}
	void	ConnectionManager::DelConnection( const ConnectionID& connid )
	{
		fge::Lock<fge::CCriticalSection> lock(m_cs);
		if(m_clientKeys[connid.nID] && m_clientKeys[connid.nID]->connid == connid)
		{
			delete m_clientKeys[connid.nID];
			m_clientKeys.Del(connid.nID);
		}
	}
	ConnectionInfo*		ConnectionManager::GetConnectionInfo( const ConnectionID& connid )
	{
		fge::Lock<fge::CCriticalSection> lock(m_cs);
		if(m_clientKeys[connid.nID] && m_clientKeys[connid.nID]->connid == connid)
			return m_clientKeys[connid.nID];
		else
			return 0;
	}
}