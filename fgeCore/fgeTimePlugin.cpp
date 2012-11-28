#include "StdAfx.h"
#include "fgeTimePlugin.h"
//#include <mmsystem.h>
#include "fgeClock.h"
#include <fgeException.h>
#include <fgeutil.h>

using namespace fge;
CTimePlugin::CTimePlugin(void)
{
	m_clock = 0;
}

CTimePlugin::~CTimePlugin(void)
{
	SAFE_DELETE(m_clock);
}

void CTimePlugin::Update()
{
	if(m_clock)
		m_clock->Update();
}

void CTimePlugin::OnInstall()
{
	if(m_clock == 0)
		m_clock = new CClock();
	if(m_clock)
        m_clock->Start();	
	else
		throw fge::MemAllotException("TimerPlugin.cpp row 26. new CClock");

}

//消息函数
UINT CTimePlugin::MsgProc(UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}
//关闭
void CTimePlugin::OnUninstall()
{
	
}
//测试时间误差,每分钟修正一次
void CTimePlugin::OnTestTimer(unsigned int timerid)
{
	static DWORD time = ::clock();
	static DWORD dt=0;
	DWORD testtime = ::clock() - time;
	DWORD passTime = testtime - dt;

	dt = testtime;
	std::cout<<"已过："<<passTime<<"(ms)"<<std::endl;
}