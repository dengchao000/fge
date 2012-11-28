/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  �汾	 :	
*  ����  :  �������
**/


#pragma once
namespace fge
{
	//�ٽ���
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
		//�����ٽ���
		void	Enter()
		{
			::EnterCriticalSection(&m_pcs);
		}
		//�˳��ٽ���
		void	Leave()
		{
			LeaveCriticalSection(&m_pcs);	
		}
	private:
		CRITICAL_SECTION	m_pcs;
	};
	//���߳��µ��ٽ���
	class NoCriticalSection
	{
	public:
		//�����ٽ���
		void	Enter(){}
		//�˳��ٽ���
		void	Leave(){}
	};

	typedef  NoCriticalSection	SCRITICAL_SECTION;
	typedef  CCriticalSection	MCRITICAL_SECTION;
	//ѭ����
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
				// ��ĵط� lock ס��, �������߳̽�����,nLock��ֵ����0,���Ե�InterlockedExchange������0ʱ,��ʾ�����߳��Ѿ�����.
				while (InterlockedExchange(&nLock,id)) {
					// �ȴ���ĵط� Unlock
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
				::OutputDebugString(TEXT("FreeMutex::Level �����߳�������߳�ID��һ��!\n"));
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
	typedef FreeMutex FreeLock;		//Ϊ�˼��ݾ�ϵͳ
	
	//�Զ���
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