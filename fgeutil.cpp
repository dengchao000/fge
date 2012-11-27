#include "stdafx.h"
#include "fgeutil.h"
#include <Windows.h>
namespace fge
{

	int vformatstr(wchar *buf, size_t buflen, const wchar *format, va_list ap)
	{
		return vswprintf_s(buf,buflen,format,ap);
	}
	int vformatstr(char *buf, size_t buflen, const char *format, va_list ap)
	{
		return vsprintf_s(buf,buflen,format,ap);
	}
}