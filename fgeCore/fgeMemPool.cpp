// MemPool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fgesingleton.h>
#include <crtdbg.h>
#include "fgeMemoryObject.h"
using namespace sky;
struct A:public CMemoryObject
{
public:
	A(){};

	int a;
	int b;
};

struct B :public A
{
	B(){
		a=b=c=0;
	};
	virtual ~B() {
	}
	int c;
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

	int *p = (int *)INSTANCE(CMemoryPool)->Allot(sizeof(int));
	char *pc = (char *)INSTANCE(CMemoryPool)->Allot(sizeof(char));
	INSTANCE(CMemoryPool)->Free(p);
	INSTANCE(CMemoryPool)->Free(pc);

	int s = sizeof(B);
	A* pa = new B();

	delete pa;
	return 0;
}

