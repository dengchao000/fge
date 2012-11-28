/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	
*  描述  :  互斥对象
**/


#pragma once
namespace fge
{
	//临界区
	class CCriticalSection
	{
	private:
		CCriticalSection( const CCriticalSection& cs ) {}
	public:
		CCriticalSection()
		{			
			::InitializeCriticalSection(&m_pcs);
		}
		~CCriticalSection(void)
		{
			::DeleteCriticalSection(&m_pcs);	
		}
		//进入临界区
		void	Enter()
		{
			::EnterCriticalSection(&m_pcs);
		}
		//退出临界区
		void	Leave()
		{
			LeaveCriticalSection(&m_pcs);	
		}
	private:
		CRITICAL_SECTION	m_pcs;
	};
	//单线程下的临界区
	class NoCriticalSection
	{
	public:
		//进入临界区
		void	Enter(){}
		//退出临界区
		void	Leave(){}
	};

	typedef  NoCriticalSection	SCRITICAL_SECTION;
	typedef  CCriticalSection	MCRITICAL_SECTION;
	//循环锁
	class FreeMutex{
	public:
		FreeMutex( )
		{
			nLock = 0; lastThreadId = 0; count = 0;
		}

		void    Enter( )
		{
			long id = (long)::GetCurrentThreadId( );
			long atom = InterlockedExchange(&nLock,id);
			if (atom && atom!=id) {
				// 别的地方 lock 住了, 当其它线程解锁后,nLock的值会变回0,所以当InterlockedExchange交换出0时,表示其它线程已经解锁.
				while (InterlockedExchange(&nLock,id)) {
					// 等待别的地方 Unlock
					Sleep(1);
				}
			}
			::InterlockedExchangeAdd(&count,1);
			lastThreadId = id;
		}
		void    Leave( )
		{
			long id = (long)::GetCurrentThreadId( );
			if(id != lastThreadId)
			{
				::OutputDebugString(TEXT("FreeMutex::Level 解锁线程与加锁线程ID不一致!\n"));
				return;
			}
			if(!count)
			{
				::OutputDebugString(TEXT("FreeMutex::Level count=-1"));
				return;
			}
			::InterlockedExchangeAdd(&count,-1);
			if(!count)
				::InterlockedExchange(&nLock,0);
		}
	private:
		long	nLock;
		long	count;
		long	lastThreadId;
	};
	typedef FreeMutex FreeLock;		//为了兼容旧系统
	
	//自动锁
	template< typename CS >
	class Lock
	{
	public:
		Lock(CS &cs)
		{
			m_pCs = &cs;
			m_pCs->Enter();
		}
		virtual ~Lock()
		{
			m_pCs->Leave(); m_pCs = NULL;
		}
	private:
		CS *m_pCs;
	};
	typedef Lock<CCriticalSection> MLOCK;
	typedef Lock<FreeMutex> FLOCK;
	struct HEvent
	{
		HANDLE hEvent;
		HEvent(){ hEvent = ::CreateEvent(NULL,TRUE,FALSE,0); }
		~HEvent(){ if(hEvent) ::CloseHandle( hEvent ); hEvent = 0; }
		void	SetEvent(){ ::SetEvent( hEvent ); }
		void	ResetEvent(){ ::ResetEvent( hEvent ); }
		DWORD	WaitObject(DWORD dt = INFINITE) { return ::WaitForSingleObject(hEvent,dt); }
	};
}