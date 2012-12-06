// TestClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <strstream>
#include <algorithm>
#include <deque>
#include <functional>
#include <vector>
#include <crtdbg.h>
#include "tcpClient.h"
#include "fgeCmdInterface.h"

#define nil 0
using namespace fge;
using namespace ocl;

class NetListener: public ocl::TcpClient::Listener
{
	virtual void	OnSend( const char* body, u_long size ){

	};
	virtual void	OnRecv( const char* body, u_long size ){
		log_debug("OnRecv login listener: %d", size);
	};
	virtual void	OnClose( ){
		log_debug("OnClose login listener");
	};
	virtual void	OnDisconnect( ){
		log_debug("OnDisconnect login listener");
	};
};

unsigned WINAPI MainLoop( void* )
{
	CClockManager* clockMgr = INSTANCE(CClockManager);
	//clockMgr->SetClock(0,1000,-1,CTimeEvent(&Update));
	while( clockMgr->Update() )
	{
		Sleep(10);
	}
	return 0;
}
TcpClient *pNet = 0;
class Cmd :public SharedObject
{
public:
	void Reg(CCmdInterface& ui)
	{
		ui.AddCommand("conn","���ӵ���Ϸ������",CEvent(this,&Cmd::Conn));
		ui.AddCommand("clos","�ر���Ϸ������",CEvent(this,&Cmd::CloseServer));
	}
	void Conn(CmdArgs*)
	{
		if(pNet)
			pNet->Open("192.168.0.207",7118);
	}
	void CloseServer(CmdArgs*)
	{
		if(pNet)
			pNet->Send("closeservice 30",strlen("closeservice 30"));
	}
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

	pNet = new TcpClient();
	pNet->AddListener(new NetListener());
	
	HANDLE ht = fge::CThread::StartThread( &MainLoop,0 );
	
	CCmdInterface ui;
	Cmd cmd;
	cmd.Reg(ui);
	ui.SetName(">");	
	ui.Run(argc,argv);
	
	::WaitForSingleObject(ht,10000);
	pNet->Close();
	SAFE_DELETE(pNet);
	return 0;
}

