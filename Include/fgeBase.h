/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011-10-25
*
*  版本	 :	1.0
*  描述  :  定义
**/

#pragma once
namespace fge {
#if FGE_DOUBLE_PRECISION == 1
	typedef double real;
#else
	typedef float real;
#endif

	/** In order to avoid finger-aches
	*/
	typedef unsigned char 		uchar;
	typedef unsigned short 		ushort;
	typedef unsigned int 		uint;
	typedef unsigned long 		ulong;
	typedef __int64				int64;

	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned __int64	uint64;

	typedef signed char         int8;
	typedef signed short        int16;
	typedef signed int          int32;
	typedef signed __int64      int64;

	/*
	** Common data types
	*/
#ifndef WIN32
	typedef unsigned long       DWORD;
	typedef unsigned short      WORD;
	typedef unsigned char       BYTE;
#endif

	/*
	** Common math constants
	*/
#ifndef M_PI
#define M_PI2	6.283185307179586476925f
#define M_PI	3.14159265358979323846f
#define M_PI_2	1.57079632679489661923f
#define M_PI_4	0.785398163397448309616f
#define M_1_PI	0.318309886183790671538f
#define M_2_PI	0.636619772367581343076f
#endif

	/*
	** FPS system state special constants
	*/

#ifdef UNICODE
	typedef std::wstring _StringBase;
#else
	typedef std::string _StringBase;
#endif
	typedef _StringBase String;


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if(p) { delete (p);	(p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete [](p);	(p)=NULL; } }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)			{ if(p) { free(p); (p)=NULL; } }
#endif

	/*
	** virtual 
	*/
#ifndef INTERFACE
#define INTERFACE virtual
#endif

#ifndef OVERWRITE
#define OVERWRITE virtual
#endif

#ifndef ENUM_STR
#define ENUM_STR(CLASS) #CLASS
#endif

#define FUNC_STATEMENT_0( rt, func_name ) virtual rt func_name( ) = 0;
#define FUNC_STATEMENT_1( rt, func_name, param1 ) virtual rt func_name(param1 _p) = 0;
#define FUNC_STATEMENT_2( rt, func_name, param1,param2 ) virtual rt func_name(param1 _p1, param2 _p2) = 0;
#define FUNC_STATEMENT_3( rt, func_name, param1,param2,param3 ) virtual rt func_name(param1 _p1, param2 _p2, param3 _p3) = 0;

#define FUNC_COMMUTATOR_0( rt, func_name ) virtual rt func_name( ){ m_pListener->func_name( ); }
#define FUNC_COMMUTATOR_1( rt, func_name, param1 ) virtual rt func_name(param1 _p){ m_pListener->func_name( _p ); }
#define FUNC_COMMUTATOR_2( rt, func_name, param1,param2 ) virtual rt func_name(param1 _p1, param2 _p2){ m_pListener->func_name( _p1,_p2 ); }
#define FUNC_COMMUTATOR_3( rt, func_name, param1,param2,param3 ) virtual rt func_name(param1 _p1, param2 _p2, param3 _p3){ m_pListener->func_name( _p1,_p2,_p3 ); }

	//求d1%(d2x次方)
#ifndef Mod2X
#define Mod2X(d1,d2) ((d1)-(((d1)>>(d2))<<(d2)))
#endif
	//求d1%d2
#ifndef Mod2N
#define Mod2N(d1,d2) (d1&(d2-1))
#endif
}