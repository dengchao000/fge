/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2012-12-6
** ����  :  
**/

#pragma once
#include "fgeThread.h"
#include "fgePluginManager.h"
#include "fgeMessageManager.h"

class ServerApp : public fge::CThread
{
public:
	void Init( )
	{
		INSTANCE(CClockManager);
	}

	//��ʼ��
	virtual void	OnInstall(){ m_msgQueue.Init(10000,0); }	
	//�ر�
	virtual void	OnUninstall(){ m_plugMag.UninstallPlugin(); m_msgQueue.Clear(); }
	//����
	virtual void	Update()	{ m_plugMag.Update(); }
	//��Ϣ����
	virtual UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam )
	{
		return m_plugMag.MsgProc(message,wParam,lParam);
	}


	//��װ���
	void	InstallPlugin(CPlugin* plugin){ m_plugMag.InstallPlugin(plugin); }
	//ж�ز��
	void	UninstallPlugin(CPlugin* plugin){ m_plugMag.UninstallPlugin(plugin); }
	//������Ϣ
	bool	Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	//������Ϣ����ǰ(���ȴ���)
	bool	PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	//������Ϣ
	bool	Push(DWORD msg, DWORD wParam,DWORD lParam );
	//������Ϣ����ǰ(���ȴ���)
	bool	PushFront(DWORD msg, DWORD wParam,DWORD lParam );

	fge::CMessageManager*	GetMessageQueue(){ return &m_msgQueue; }

protected:		
	fge::CMessageManager	m_msgQueue;	// ��Ϣ����.
	fge::CPluginManager     m_plugMag;	// �������
protected:
	virtual void	ThreadProc()
	{//�̺߳���
		CClockManager* clockMgr = INSTANCE(CClockManager);

		while( m_bRun )
		{
			clockMgr->Update();
			Sleep(10);
		}
		clockMgr->Destroy();
	}
};