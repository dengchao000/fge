// GameServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fgeUtil.h"
#include "fgeLog.h"
#include "fgeLzCompress.h"

#include <strstream>
#include <algorithm>
#include <deque>
#include <functional>
#include <vector>
#include <crtdbg.h>
#include "fgeClock.h"
#include "Net/NetMsg.h"
#include "GameObject.h"

#define nil 0
using namespace fge;
using namespace ocl;
class ServerApp : public fge::CThread
{
public:
	void Init( )
	{
		INSTANCE(CClockManager);
	}

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
protected:
	virtual void	ThreadProc()
	{//线程函数
		CClockManager* clockMgr = INSTANCE(CClockManager);
		
		while( m_bRun )
		{
			clockMgr->Update();
			Sleep(10);
		}
		clockMgr->Destroy();
	}
};
void GmListener::OnRecv( const char* body, u_long size, ocl::ConnectionID id )
{
	log_debug("OnRecv gm[%d]: %s %d", id.nID,body,size);
};
int _tmain(int argc, _TCHAR* argv[])
{
	//检测内存泄漏
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//设置内存泄漏跟踪标志
#endif //WIN32 & _DEBUG

	//
	CClockManager* clockMgr = INSTANCE(CClockManager);
	MoveManager* moveMgr = INSTANCE(MoveManager);

	TcpServer* pNetClient = new TcpServer();
	pNetClient->AddListener(new ClientListener());
	pNetClient->Open("192.168.0.207",7117,0);

	TcpServer* pNetGm = new TcpServer();
	pNetGm->AddListener(new GmListener());
	pNetGm->Open("192.168.0.207",7118,0);

	TcpClient *pNetLogin = new TcpClient();
	pNetLogin->AddListener(new LoginListener());
	pNetLogin->Open("192.168.0.207",7116);

	ServerApp app;
	app.StartThread();

	pNetClient->Close();
	pNetLogin->Close();
	
	SAFE_DELETE( pNetClient );
	SAFE_DELETE( pNetLogin );

	return 0;
}

