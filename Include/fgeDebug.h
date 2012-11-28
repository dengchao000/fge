#pragma once 
#include <crtdbg.h>
#if defined _MEM_DEBUG
#if defined(WIN32)&&defined(_DEBUG)
static char THIS_FILE[]	= __FILE__;
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#endif

/*
#include <crtdbg.h>
	//检测内存泄漏
#if defined(WIN32) && defined(_DEBUG) 
	int tmpDbgFlag;

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);			//设置内存泄漏跟踪标志
#endif //WIN32 & _DEBUG
*/