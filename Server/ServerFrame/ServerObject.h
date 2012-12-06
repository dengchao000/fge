/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011-6-1
** ����  :  
**/

#pragma once
#include <fgeThread.h>
#include <fgePluginManager.h>
#include <fgeMessageManager.h>
#include <fgeClock.h>
#include <fgefactory.h>
class CServiceObject : public fge::CPlugin
{
public:
	CServiceObject(void);
	virtual ~CServiceObject(void);

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
};
class CBaseService : public CServiceObject, fge::CThread
{
public:
	CBaseService(void);
	virtual ~CBaseService(void);

	virtual void	SetPerformance(int fps);
	virtual void	OnInstall();
	virtual void	OnUninstall();

	bool	Start();
	void	Close();
	bool	IsStart();
	int		GetFPS(){ return m_nFPS; }
protected:
	virtual void	ThreadProc();
	virtual void	ClearThreadRes( );
	virtual void	Update(){};
protected:
	int			m_nFixedDelta;	// fps����
	int			m_nFPS;			// ��ǰfps
	fge::CTimer m_fpsTimer;		// fps��ʱ
};

typedef fge::CVisualFactory<fge::CPlugin> CPluginFactory;