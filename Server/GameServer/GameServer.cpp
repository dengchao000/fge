// GameServer.cpp : �������̨Ӧ�ó������ڵ㡣
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
void GmListener::OnRecv( const char* body, u_long size, ocl::ConnectionID id )
{
	log_debug("OnRecv gm[%d]: %s %d", id.nID,body,size);
};
int _tmain(int argc, _TCHAR* argv[])
{
	//����ڴ�й©
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//�����ڴ�й©���ٱ�־
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

