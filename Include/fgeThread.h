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
*  描述  :  线程相关工具及辅助类
**/


#pragma once
//#include <fgeutil.h>
#include <process.h>
#include "fgeMutex.h"
namespace fge
{
	//线程状态
	enum  ThreadState
	{
		ThreadState_Create,		//新线程状态
		ThreadState_Resume,		//就绪
		ThreadState_Suspend,	//挂起
		ThreadState_Run,		//运行态
		ThreadState_End,		//结束
	};
	//线程优先级
	enum ThreadPriority
	{
		ThreadState_Idle,//空闲
		ThreadState_Lowest,//最低
		ThreadState_BelowNormal,//低于正常 
		ThreadState_Normal,//正常
		ThreadState_AboveNormal,//高于正常
		ThreadState_Highest,//最高
		ThreadState_TimeCritical,//时间片
	};

	//线程类
	class CThread
	{
	public:

	public:
		CThread(void);
		virtual ~CThread(void);

		bool			StartThread(bool bRunAtOnce = true);//启动线程 bRunAtOnce : 立即执行?	
		void			WaitForCompletion();				//等待线程结束		
		void			SetPriority(ThreadPriority priority);//设置优先级		
		void			Suspend();							//挂起		
		void			Resume();							//就绪

		virtual void	Close();							//
		unsigned int	GetID()		{ return m_id; }
	public:
		static HANDLE	StartThread(unsigned ( __stdcall *start_address )( void * ),void* param, bool bRunAtOnce = true);
	protected:		
		virtual void	ThreadProc();						//线程函数
		virtual void	ClearThreadRes();					//清理线程资源
	private:		
		static unsigned int WINAPI Run(LPVOID	lpParam);	//启动线程处理函数

	protected:
		HANDLE			m_hThread;		//线程句柄
		unsigned int	m_id;			//线程 ID 号
		bool			m_bRun;			//线程运行标志.可以在CThreadProc中测试这个标志,如果为false则可以退出线程
		CCriticalSection m_cs;			//临界区	
	};

}