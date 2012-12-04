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
	TcpServer* pNet = new TcpServer();
	pNet->Open("127.0.0.1",port,0);
	while( clockMgr->Update() )
	{
		Sleep(10);
	}
	

	return 0;
}

