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
	std::istringstream si(str.c_str(),str.length());
	std::ostringstream so(ostr,fge::arraySize(ostr));
	if(S_OK != lzd.Encode(&si,&so))
	{
		fge::log_warn("Encode 失败");
	}
	else
	{
		fge::lzdDecompression lz;
		std::ostringstream sdo(ostr2,fge::arraySize(ostr2));
		std::stringbuf* sbuf = so.rdbuf();
		std::istringstream sdi(sbuf->str());
		lz.Decode(&sdi,&sdo);

		fge::log_debug("aaaaaaaaaaaaaaaaaaa");
		fge::log_debug("ccccccccccccccccccc");
		fge::CLog log("logtest.txt");
		log.SetDisplaySeverity(1);
		log.Debug("bbbbbbbbbbbbbbbbbbbbbbb");
		log.Error("eeeeeeeeeeeeeeeeeeeeeeeeee");
		log.WirteBinFile("test.lzd",sdo.str().c_str(),sdo.str().length());
	}
	return 0;
}

