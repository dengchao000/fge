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
*  版本	 :	2.0
*  描述  :  定义常用的宏和函数
**/

#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <tchar.h>
#include <windows.h>
// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <strstream>
//#include "fgeStr.h"
#ifdef TARGETVER
#include "targetver.h"
#endif


namespace fge {

// Define ogre version
#define FGE_VERSION_MAJOR 2
#define FGE_VERSION_MINOR 6
#define FGE_VERSION_PATCH 0

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
#ifdef _UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string		tstring;
#endif	

	typedef wchar_t wchar;
	typedef wchar *PWCHAR, *LPWCH, *PWCH;
	typedef const wchar    *LPCWSTR, *PCWSTR;

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
#ifndef mod2x
#define mod2x(d1,d2) ((d1)-(((d1)>>(d2))<<(d2)))
#endif
	//求d1%d2
#ifndef mod2n
#define mod2n(d1,d2) (d1&(d2-1))
#endif
	//求数组的长度
	template<size_t size, typename T>
	size_t arraySize(T (&arr)[size]) { return size; }


	inline int vformatstr(wchar *buf, size_t buflen, const wchar *format, va_list ap)
	{
		return vswprintf_s(buf,buflen,format,ap);
	}
	inline int vformatstr(char *buf, size_t buflen, const char *format, va_list ap)
	{
		return vsprintf_s(buf,buflen,format,ap);
	}

	template< size_t size, typename C >
	int formatstr(C (&buf)[size],const C *format, ...)
	{ 
		va_list ap;
		va_start(ap, format);
		int r = vformatstr(buf,size,format,ap);
		va_end(ap);
		return r;
	}
	template< typename C >
	int formatstr(C *buf, size_t buflen, const C *format, ...)
	{
		va_list ap;
		va_start(ap, format);
		int r = vformatstr(buf,buflen,format,ap);
		va_end(ap);
		return r;
	}

	//交换两块大小相同的内存
	template <class T>
	void swap(void *p1,void *p2, size_t size)
	{
		size_t num = size / sizeof(T);
		T *pt1 = (T *)p1;
		T *pt2 = (T *)p2;

		for(size_t i=0; i<num; i++)
		{
			*pt1 ^= *pt2;
			*pt2 ^= *pt1;
			*pt1 ^= *pt2;

			pt1++;pt2++;
		}
		num = size % sizeof(T);
		if(num)
			swap<uchar>(pt1,pt2,num);
	}
	template <class t>
	inline void swap(t	*p1, t *p2){ Swap<int64>((void*)p1,(void*)p2,sizeof(t)); }

	//交换两块大小相同的内存
	template <class T>
	void copy(void *pbuf, const void *psrc, size_t size)
	{
		size_t num = size / sizeof(T);
		T *pt1 = (T *)pbuf;
		const T *pt2 = (const T *)psrc;

		for(size_t i=0; i<num; i++)
			*pt1++ = *pt2++;

		num = size % sizeof(T);
		if(num) copy<char>(pt1,pt2,num);
	}

	inline void * safeMemcpy(void * pBuf,const void *pSrc, size_t bufSize, size_t srcSize)
	{
		copy<int64>(pBuf,pSrc,min(bufSize,srcSize));
		return pBuf;
	};
	template < size_t _Len > 
	inline char* safeStrcpy( char(&dst)[_Len], const char *src, size_t size )
	{
		if(src==NULL || _Len==0) 
			return dst;

		copy<int64>(dst,src,min(size,(_Len-1)));

		if(_Len>size)
			dst[size] = '\0';
		else	
			dst[_Len-1] = '\0';
		return dst;
	}
	//源字符串不安全时的字符串拷贝函数
	inline char* safeStrcpy(char* Des,size_t desSize, const char* Src, size_t szLength)
	{
		//assert(desSize != 0);
		if(desSize == 0)
			return NULL;	
		if(Src==NULL || Des==NULL) 
			return NULL;

		copy<int64>(Des,Src,min((desSize-1),szLength));
		if(desSize>szLength)
			Des[szLength] = '\0';
		else
			Des[desSize - 1] = '\0';
		return Des;
	};

	//源字符串不安全时的字符串拷贝函数
	inline char* safeStrcpy(char* Des,size_t szDesLength,const char* Src)
	{
		return safeStrcpy(Des,szDesLength,Src,::strlen(Src));
	};
	//判断坐标是否在矩形内
	inline bool posInRect(int x,int y,::RECT* rc)
	{
		if((x >= rc->left) && (x<= rc->right) && (y>= rc->top) && (y<= rc->bottom))
			return true;
		return false;
	}




	
	inline TCHAR* toString(TCHAR* buf, int value)
	{
		wsprintf(buf,_T("%d"),value);
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, unsigned int value, int x=0)
	{	//x:16进制？
		if(x)
			wsprintf(buf,_T("%x"),value);
		else
			wsprintf(buf,_T("%u"),value);
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, unsigned long value)
	{
		wsprintf(buf,_T("%u"),value);
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, long value)
	{
		wsprintf(buf,_T("%d"),value);
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, bool value)
	{
		wsprintf(buf,_T("%d"),value);
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, float value, int precision)
	{		
		if(precision<0)
			wsprintf(buf,_T("%f"),value);
		else
		{
			TCHAR szformat[64];
			wsprintf(szformat,_T("%s%df"),_T("%."),precision);
			wsprintf(buf,szformat,value);
		}
		int len = (int)::_tcslen(buf);
		for( len--; len>0; len-- )
		{
			if( buf[len] == '0' )				
			{
				buf[len] = '\0';
			}
			else if( buf[len] == '.' )
			{
				buf[len] = '\0';
				break;
			}
			else
				break;
		}
		return buf;
	}
	inline TCHAR* toString(TCHAR* buf, float value)
	{
		return toString( buf,value,0 );
	}
	
	template<class T>
	inline std::string toString(const T& t)
	{//效率低
		std::stringstream ss;
		ss<<t;
		return ss.str();
	}
	
	template<class Iterator>
	bool nulIterator(const Iterator& l)
	{
		return l == Iterator( );
	}

	
	//KeyState
	inline bool getKeyState(int key)
	{
		return ((GetAsyncKeyState(key) & 0x8000) != 0);
	}

	
	//str hash
	inline uint strHash(const tstring &str)
	{
		register unsigned int h;
		register const tstring::traits_type::_Elem *p;

		for(h=0, p = str.c_str( ); *p ; p++)			
			h = (h<<5) - h + *p; //h = 31 * h + *p;

		return h/*&0x7FFFFFFF*/;
	}
	inline uint strHash(const char* str)
	{
		register unsigned int h;
		const char *p=str;

		for(h=0; *p ; p++)			
			h = (h<<5) - h + *p; //h = 31 * h + *p;

		return h/*&0x7FFFFFFF*/;
	}
	template< class T >
	inline T strHash(const char* str)
	{
		T h;
		const char *p=str;
		for(h=0; *p ; p++)			
			h = (h<<5) - h + *p; //h = 31 * h + *p;
		return h;
	}
	
	//counter
	class  CCounter
	{
	public:
		CCounter( ){ gInstanceCount = 0; }
		long		GetCount( ){ return gInstanceCount; }
		void		AddReference( ){ ::InterlockedExchangeAdd((PLONG)&gInstanceCount,1); }
		void		Release( ){ ::InterlockedExchangeAdd((PLONG)&gInstanceCount,-1); }
	private:
		volatile long gInstanceCount;
	};

	//自动删除对象
	template<typename T>
	class CAutoDel
	{
	public:
		CAutoDel( )	{ m_pObj = 0; }
		CAutoDel( T* pObj )	{ m_pObj = pObj; }
		~CAutoDel( ) { SAFE_DELETE( m_pObj ); }

		void SetObject( T* pObj ){ m_pObj = pObj; }
	private:
		T* m_pObj;
	};

	
	//静态构造器:构造对象的静态构造方法.
	template <typename T>
	class SConstructor
	{
	public:
		SConstructor( )
		{	
			T::S_Constructor();
		}
		~SConstructor( )
		{	
			T::S_Destructor();
		}
	};
	template <typename T>
	class StaticConstructor
	{
	public:
		StaticConstructor( ){  static SConstructor<T> constructor; }
		virtual ~StaticConstructor( ){ }
	};
}