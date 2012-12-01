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
*  ����  :  �߳���ع��߼�������
**/


#pragma once
//#include <fgeutil.h>
#include <process.h>
#include "fgeMutex.h"
namespace fge
{
	//�߳�״̬
	enum  ThreadState
	{
		ThreadState_Create,		//���߳�״̬
		ThreadState_Resume,		//����
		ThreadState_Suspend,	//����
		ThreadState_Run,		//����̬
		ThreadState_End,		//����
	};
	//�߳����ȼ�
	enum ThreadPriority
	{
		ThreadState_Idle,//����
		ThreadState_Lowest,//���
		ThreadState_BelowNormal,//�������� 
		ThreadState_Normal,//����
		ThreadState_AboveNormal,//��������
		ThreadState_Highest,//���
		ThreadState_TimeCritical,//ʱ��Ƭ
	};

	//�߳���
	class CThread
	{
	public:

	public:
		CThread(void);
		virtual ~CThread(void);

		bool			StartThread(bool bRunAtOnce = true);//�����߳� bRunAtOnce : ����ִ��?	
		void			WaitForCompletion();				//�ȴ��߳̽���		
		void			SetPriority(ThreadPriority priority);//�������ȼ�		
		void			Suspend();							//����		
		void			Resume();							//����

		virtual void	Close();							//
		unsigned int	GetID()		{ return m_id; }
	public:
		static HANDLE	StartThread(unsigned ( __stdcall *start_address )( void * ),void* param, bool bRunAtOnce = true);
	protected:		
		virtual void	ThreadProc();						//�̺߳���
		virtual void	ClearThreadRes();					//�����߳���Դ
	private:		
		static unsigned int WINAPI Run(LPVOID	lpParam);	//�����̴߳�����

	protected:
		HANDLE			m_hThread;		//�߳̾��
		unsigned int	m_id;			//�߳� ID ��
		bool			m_bRun;			//�߳����б�־.������CThreadProc�в��������־,���Ϊfalse������˳��߳�
		CCriticalSection m_cs;			//�ٽ���	
	};

}