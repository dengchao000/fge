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
#include "tcpServer.h"

#define nil 0
using namespace fge;

class ServerApp
{
public:
	void Init( )
	{
		INSTANCE(CClockManager);
	}
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
	TcpServer* pNet = new TcpServer();
	pNet->Open("127.0.0.1",port,0);
	while( clockMgr->Update() )
	{
		Sleep(10);
	}
	

	return 0;
}

