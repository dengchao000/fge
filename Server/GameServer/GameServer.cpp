// GameServer.cpp : �������̨Ӧ�ó������ڵ㡣
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

