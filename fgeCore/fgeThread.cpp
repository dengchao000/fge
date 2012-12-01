#include "StdAfx.h"
#include "fgeThread.h"
#include <process.h>
#include <fgeException.h>
using namespace fge;
CThread::CThread(void)
{
	m_hThread = NULL;
	m_id = 0;
	m_bRun = false;
}

CThread::~CThread(void)
{

}
//�����̴߳�����
unsigned int CThread::Run(LPVOID	lpParam)
{
	CThread *pThread = reinterpret_cast<CThread*>(lpParam);
	if(pThread)
	{
		try{
			pThread->m_bRun = true;
			pThread->ThreadProc();
		}
		catch(std::exception e)
		{
			
		}
		pThread->ClearThreadRes();
	}
	return (0);
}
//�����߳���Դ
void CThread::ClearThreadRes()
{
	::CloseHandle(m_hThread);
	m_id = 0;
	m_hThread = NULL;
}
//�������ȼ�
void CThread::SetPriority(ThreadPriority priority)
{
	::SetThreadPriority(m_hThread,(int)priority);
}
//�ȴ��߳̽���
void	CThread::WaitForCompletion()
{
	::WaitForSingleObject(m_hThread,INFINITE);	
}
//����
void	CThread::Suspend()
{
	::SuspendThread(m_hThread); 
}
//����
void	CThread::Resume()
{
	::ResumeThread(m_hThread); 
}
bool	CThread::StartThread(bool bRunAtOnce)
{
	if(m_hThread == NULL)
	{
		unsigned int f;
		bRunAtOnce? f=0: f = CREATE_SUSPENDED;		
		m_hThread = (HANDLE)_beginthreadex(NULL,0,Run,reinterpret_cast<LPVOID>(this),f,&m_id);
		return true;
	}
	return false;
}
HANDLE	CThread::StartThread(unsigned ( __stdcall *start_address )( void * ), void* param, bool bRunAtOnce/* = true*/)
{
	HANDLE hThread = NULL;
	unsigned int threadid;
	unsigned int f;
	bRunAtOnce? f=0: f = CREATE_SUSPENDED;		
	hThread = (HANDLE)_beginthreadex(NULL,0,start_address,reinterpret_cast<LPVOID>(param),f,&threadid);
	return hThread;
}
void	CThread::ThreadProc()
{
	//::_endthreadex(0);
}
void	CThread::Close()
{
	m_bRun = false;
}