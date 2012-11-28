// GameServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fgeLog.h"
#define nil 0


int _tmain(int argc, _TCHAR* argv[])
{
	fge::log_debug("aaaaaaaaaaaaaaaaaaa");
	fge::log_debug("ccccccccccccccccccc");
	fge::CLog log("logtest.txt");
	log.SetDisplaySeverity(1);
	log.Debug("bbbbbbbbbbbbbbbbbbbbbbb");
	log.Error("eeeeeeeeeeeeeeeeeeeeeeeeee");
	return 0;
}

