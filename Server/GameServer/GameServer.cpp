// GameServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
using namespace fge;
using namespace ocl;

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

	CClockManager* clockMgr = INSTANCE(CClockManager);
	MoveManager* moveMgr = INSTANCE(MoveManager);

	ServerApp app;
	app.Start();
	app.WaitForCompletion();
	return 0;
}

