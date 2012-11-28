/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  fge异常
**/

#pragma once
#include <exception>
namespace fge
{
#define DeclareX(CName, ErrCode, ErrInfo) class CName : public CException { \
public : CName() : CException( ErrCode ) {str=0;}\
	CName(LPCSTR _str) : CException( ErrCode ) {str=_str;}\
	LPCSTR GetErrInfo() { return str?str:ErrInfo; } LPCSTR str;};

	struct CException
	{
		CException( DWORD errCode ) : errCode( errCode ) {}
		virtual LPCSTR GetErrInfo() = 0;
		LPCSTR what(){ return GetErrInfo(); }
		DWORD errCode;
	};
	DeclareX( NullPointer,    0x00000000, "非法使用空指针" );
	DeclareX( FullBuffer,     0x00000001, "缓冲区已满" );
	DeclareX( InvalidConvert, 0x00000002, "无效的转换" );
	DeclareX( NotInitialize,  0x00000003, "使用的对象没有初始化" );
	DeclareX( CurrentIsEnd,   0x00000004, "当前已经是结尾了" );
	DeclareX( BadDataSize,    0x00000005, "数据大小异常" );
	DeclareX( BadString,      0x00000006, "传入的字符串异常" );
	DeclareX( InvalidBuffer,  0x00000007, "无效的缓冲区" );
	DeclareX( BadArrayType,   0x00000008, "序列化数组只支持固定长度类型" );

	DeclareX( MemAllotException,  0x00000009, "内存分配异常" );
	DeclareX( MemOverException,   0x0000000a, "内存溢出" );
	DeclareX( MemLeakException,   0x0000000b, "内存泄漏" );
}