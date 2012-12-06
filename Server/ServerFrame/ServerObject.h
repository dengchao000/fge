/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011-6-1
** 描述  :  
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

	//初始化
	virtual void	OnInstall(){ m_msgQueue.Init(10000,0); }	
	//关闭
	virtual void	OnUninstall(){ m_plugMag.UninstallPlugin(); m_msgQueue.Clear(); }
	//更新
	virtual void	Update()	{ m_plugMag.Update(); }
	//消息处理
	virtual UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam )
	{
		return m_plugMag.MsgProc(message,wParam,lParam);
	}


	//安装插件
	void	InstallPlugin(CPlugin* plugin){ m_plugMag.InstallPlugin(plugin); }
	//卸载插件
	void	UninstallPlugin(CPlugin* plugin){ m_plugMag.UninstallPlugin(plugin); }
	//插入消息
	bool	Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	//插入消息到最前(优先处理)
	bool	PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	//插入消息
	bool	Push(DWORD msg, DWORD wParam,DWORD lParam );
	//插入消息到最前(优先处理)
	bool	PushFront(DWORD msg, DWORD wParam,DWORD lParam );
	
	fge::CMessageManager*	GetMessageQueue(){ return &m_msgQueue; }

protected:		
	fge::CMessageManager	m_msgQueue;	// 消息队列.
	fge::CPluginManager     m_plugMag;	// 插件管理
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
	int			m_nFixedDelta;	// fps限制
	int			m_nFPS;			// 当前fps
	fge::CTimer m_fpsTimer;		// fps计时
};

typedef fge::CVisualFactory<fge::CPlugin> CPluginFactory;