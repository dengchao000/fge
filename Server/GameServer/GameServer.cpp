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

#include "fgeClock.h"

#define nil 0
using namespace fge;

HCLOCK hc;
struct Test : public fge::SharedObject
{
	void OnTimer( )
	{
		printf("OnTimer\n");
		INSTANCE(CClockManager)->ModifyClcok( hc,500,10 );
	}

	int n;
};
void OnTimer1( )
{
	static int n = 1;
	printf("OnTimer %d\n",n++);

}
int _tmain(int argc, _TCHAR* argv[])
{

#include <crtdbg.h>
	//检测内存泄漏
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//设置内存泄漏跟踪标志
#endif //WIN32 & _DEBUG

	CClockManager* clockMgr = INSTANCE(CClockManager);
	Test t;
	clockMgr->SetClock(0,1000*5,1,CTimeEvent(&t,&Test::OnTimer));
	hc = clockMgr->SetClock(0,1000,1000,CTimeEvent(&OnTimer1));

	DWORD c = clock();
	while( clockMgr->Update() )
	{
		Sleep(10);
	}
	system("pause");
	return 0;
}

