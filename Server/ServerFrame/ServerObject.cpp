#include "stdafx.h"
#include "ServerObject.h"
#include "fgeTimeplugin.h"

CServiceObject::CServiceObject(void)
{
}

CServiceObject::~CServiceObject(void)
{
}
//插入消息
bool	CServiceObject::Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize)
{
	return m_msgQueue.Push(msg,wParam,wParamSize,lParam,lParamSize);
}
//插入消息到最前(优先处理)
bool	CServiceObject::PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize)
{
	return m_msgQueue.PushFront(msg,wParam,wParamSize,lParam,lParamSize);
}
//插入消息
bool	CServiceObject::Push(DWORD msg, DWORD wParam,DWORD lParam )
{
	return m_msgQueue.Push(msg,(BYTE*)&wParam,sizeof(DWORD),(BYTE*)&lParam,sizeof(DWORD));
}
//插入消息到最前(优先处理)
bool	CServiceObject::PushFront(DWORD msg, DWORD wParam,DWORD lParam )
{
	return m_msgQueue.PushFront(msg,(BYTE*)&wParam,sizeof(DWORD),(BYTE*)&lParam,sizeof(DWORD));
}
//////////////////////////////////////////////////////////////////////////
CBaseService::CBaseService(void)
{
	m_nFixedDelta = 10;
	m_fpsTimer.SetCyc( m_nFixedDelta );
}
CBaseService::~CBaseService(void)
{

}

void	CBaseService::SetPerformance(int fps)
{
	if(fps)
		m_nFixedDelta = 1000/fps;
	else
		m_nFixedDelta = 1000;
	m_fpsTimer.SetCyc( m_nFixedDelta );
	m_fpsTimer.Start();
}
void	CBaseService::OnInstall()
{
	m_msgQueue.Init(10000,new fge::SystemMemAllotor());
	this->InstallPlugin(INSTANCE(fge::CTimePlugin));
}
void	CBaseService::OnUninstall()
{
	CServiceObject::OnUninstall();
}

void	CBaseService::ThreadProc()
{
	OnInstall( );

	fge::SMSG* pMsg=0;
	fge::CTimer fpsTimer;
	fpsTimer.SetCyc(1000);
	fpsTimer.Start();

	m_fpsTimer.Start();
	int fps = 0;
	bool bsleep = false;

	while(true)
	{		
		while(pMsg = m_msgQueue.Pop())
		{
			if(0 == pMsg->msg) goto _EXIT;
			MsgProc(pMsg->msg,(WPARAM)pMsg->wParam,(LPARAM)pMsg->lParam);
			if(m_fpsTimer.TestTime()<=0)
				break;
		} 	
		m_plugMag.Update();
		Update();
		fps++;

		int stime = m_fpsTimer.TestTime();
		if(stime>0)
		{
			bsleep = false;
			if(stime>10)
				::Sleep(stime);
		}
		else
		{
			bsleep = true;			
		}
		m_fpsTimer.ReSet();

		if(fpsTimer.IsTimeOut())
		{
			fpsTimer.ReSet();
			m_nFPS = fps;
			fps=0;
			if(bsleep){
				char szBuf[64]; 
				sprintf_s(szBuf,"FPS=%d,CPU占用过多,强制Sleep(1).\n",m_nFPS);
				::OutputDebugString(szBuf);
				::Sleep(1);
			}
		}
	}
_EXIT:
	OnUninstall( );
}
void	CBaseService::ClearThreadRes( )
{	
}
bool	CBaseService::Start()
{
	StartThread();
	return true;
}
bool	CBaseService::IsStart()
{
	return GetID()!=0;
}
void	CBaseService::Close()
{
	m_msgQueue.PushFront(0,0,0,0,0);
	CThread::Close();
	WaitForCompletion();
}