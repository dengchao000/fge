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

#include "fgeClock.h"

#define nil 0
using namespace fge;

struct Test : public fge::SharedObject
{
	void OnTimer( )
	{
		printf("OnTimer");
	}
	void OnTimer1( )
	{
		static int n = 1;
		printf("OnTimer %d\n",n++);
		this->n = n;
	}
	int n;
};
int _tmain(int argc, _TCHAR* argv[])
{

#include <crtdbg.h>
	//����ڴ�й©
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//�����ڴ�й©���ٱ�־
#endif //WIN32 & _DEBUG

	CClockManager clockMgr;
	Test t;
	clockMgr.SetClock(0,1000*5,1,CTimeEvent(&t,&Test::OnTimer));
	clockMgr.SetClock(0,1000,1000,CTimeEvent(&t,&Test::OnTimer1));

	DWORD c = clock();
	while( t.n < 10 )
	{
		clockMgr.Update();
		Sleep(10);
	}
	system("pause");
	return 0;
}

