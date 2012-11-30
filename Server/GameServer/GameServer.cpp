// GameServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "fgeUtil.h"
#include "fgeLog.h"
#include "fgeLzCompress.h"

#include <strstream>

#define nil 0
using namespace fge;

int _tmain(int argc, _TCHAR* argv[])
{
	fge::lzdCompress lzd;
	std::string str = "aaaaaaaaaaaaaaaaaadddddddfasdfkj;askdjfkasdklkgdfgfgjjjjjjjjjjjjjjjjasdkfalsdl";
	char ostr[200]; 
	char ostr2[200]; 
	memset(ostr,0,sizeof(ostr));
	memset(ostr2,0,sizeof(ostr2));

	size_t codeLen = lzdEncode(str.c_str(),str.length(),ostr,arraySize(ostr));
	if(!codeLen)
	{
		fge::log_warn("Encode 失败");
	}
	else
	{
		lzdDecode(ostr,codeLen,ostr2,arraySize(ostr2));
		fge::CLog log("logtest.txt");
		log.Debug(ostr2);
		log.WirteBinFile("test.lzd",ostr,codeLen);
	}
	return 0;
}

