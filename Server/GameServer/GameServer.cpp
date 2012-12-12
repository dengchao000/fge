// GameServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
using namespace fge;
using namespace ocl;

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

	CClockManager* clockMgr = INSTANCE(CClockManager);
	MoveManager* moveMgr = INSTANCE(MoveManager);

	ServerApp app;
	app.Start();
	app.WaitForCompletion();
	return 0;
}

