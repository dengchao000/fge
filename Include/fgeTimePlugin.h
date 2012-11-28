
/*
Copyright (C) 2005-2007, Fantasy Games
Author:	�˳�
QQ:		23427470
Time:	

��������ʱ�����������ʵ�ּ�ʱ����
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

		//��ʼ��
		virtual void		OnInstall();
		//�ر�
		virtual void		OnUninstall();
		//����
		virtual void		Update();
		//��Ϣ����
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