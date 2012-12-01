#include "stdAfx.h"
#include "ActiveObject.h"
#include <process.h>
#include <fgeException.h>
using namespace ocl;
ActiveObject::ActiveObject(void)
{
	m_handle = 0;
	m_id = 0;
}

ActiveObject::~ActiveObject(void)
{
}
unsigned int ActiveObject::_Run(LPVOID	lpParam)
{
	ActiveObject *pThread = reinterpret_cast<ActiveObject*>(lpParam);
	if(pThread)
	{
		try
		{
			pThread->Run( );
		}
		catch(std::exception&)
		{
		}
		pThread->ClearUp( );
		
	}
	return (0);
}
//等待线程结束
void	ActiveObject::Close( )
{
	::WaitForSingleObject(m_handle,INFINITE);	
}
//挂起
void	ActiveObject::Suspend()
{
	::SuspendThread(m_handle); 
}
//就绪
void	ActiveObject::Resume()
{
	::ResumeThread(m_handle); 
}
bool	ActiveObject::Start( )
{
	fge::Lock<fge::CCriticalSection> l(m_cs);

	if(m_handle == NULL)
	{
		unsigned int f = 0;
		m_handle = (HANDLE)_beginthreadex(NULL,0,_Run,reinterpret_cast<LPVOID>(this),f,&m_id);
		return true;
	}
	else
	{
		Resume( );
	}
	return false;
}
void	ActiveObject::Run()
{

}
void	ActiveObject::ClearUp( )
{
	fge::Lock<fge::CCriticalSection> l(m_cs);

	::CloseHandle(m_handle);
	m_id = 0;
	m_handle = NULL;
}