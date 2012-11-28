// MemPool.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����ڴ�й©
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//�����ڴ�й©���ٱ�־
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

