
/*
Copyright (C) 2005-2007, Fantasy Games
Author:	邓超
QQ:		23427470
Time:	

描述：计时器插件。负责实现计时器。
*/


#pragma once
#include <fgePlugin.h>
#include <fgeClock.h>
namespace fge
{
	class CTimePlugin : public CPlugin
	{
	public:
		CTimePlugin(void);
		virtual ~CTimePlugin(void);

		//初始化
		virtual void		OnInstall();
		//关闭
		virtual void		OnUninstall();
		//更新
		virtual void		Update();
		//消息函数
		virtual UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam );

		CClock*				GetClock()	{ return m_clock; }

	public:
		void	OnTestTimer(unsigned int timerid);
	private:
		//DWORD		startTime;
		//CTimer	testTimer;
		CClock		*m_clock;
	};
}