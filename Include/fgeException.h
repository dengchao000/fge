/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  �汾	 :	1.0
*  ����  :  fge�쳣
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
	DeclareX( NullPointer,    0x00000000, "�Ƿ�ʹ�ÿ�ָ��" );
	DeclareX( FullBuffer,     0x00000001, "����������" );
	DeclareX( InvalidConvert, 0x00000002, "��Ч��ת��" );
	DeclareX( NotInitialize,  0x00000003, "ʹ�õĶ���û�г�ʼ��" );
	DeclareX( CurrentIsEnd,   0x00000004, "��ǰ�Ѿ��ǽ�β��" );
	DeclareX( BadDataSize,    0x00000005, "���ݴ�С�쳣" );
	DeclareX( BadString,      0x00000006, "������ַ����쳣" );
	DeclareX( InvalidBuffer,  0x00000007, "��Ч�Ļ�����" );
	DeclareX( BadArrayType,   0x00000008, "���л�����ֻ֧�̶ֹ���������" );

	DeclareX( MemAllotException,  0x00000009, "�ڴ�����쳣" );
	DeclareX( MemOverException,   0x0000000a, "�ڴ����" );
	DeclareX( MemLeakException,   0x0000000b, "�ڴ�й©" );
}