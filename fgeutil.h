/**
常用工具函数
Dengchao. 2012.11.24
*/
#pragma once

namespace fge
{
	// 常用宏
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p); (p)=NULL; } }
#endif

	template<size_t size, typename c>
	size_t arraysize( c (&str)[size] ) { return size; }

	int vformatstr(wchar *buf, size_t buflen, const wchar *format, va_list ap);
	int vformatstr(char *buf, size_t buflen, const char *format, va_list ap);

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
}