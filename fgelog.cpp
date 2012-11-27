#include "stdafx.h"
#include "./fgeutil.h"
#include "./fgelog.h"
namespace fge
{
	static fge_log_cb log_fn = NULL;
	static fge_log_cbw log_fnw = NULL;
	static int log_severity = 0;
	void set_log_callback(fge_log_cb cb)
	{
		log_fn = cb;
	}
	void set_log_callback(fge_log_cbw cb)
	{
		log_fnw = cb;
	}

	void set_display_severity( int severity )
	{
		log_severity = severity;
	}
	void fge_log(int severity, const char *msg)
	{
		if(log_severity>severity)
			return;

		if (log_fn)
			log_fn(severity, msg);
		else 
		{
			const char *severity_str;
			switch (severity)
			{
			case _FGE_LOG_DEBUG:
				severity_str = "debug";
				break;
			case _FGE_LOG_WARN:
				severity_str = "warn";
				break;
			case _FGE_LOG_ERR:
				severity_str = "err";
				break;
			default:
				severity_str = "???";
				break;
			}
			fprintf(stderr, "[%s] %s\n", severity_str, msg);
		}
	}
	void fge_log(int severity, const wchar *msg)
	{
		if(log_severity>severity)
			return;

		if (log_fnw)
			log_fnw(severity, msg);
		else 
		{
			const wchar *severity_str;
			switch (severity)
			{
			case _FGE_LOG_DEBUG:
				severity_str = L"debug";
				break;
			case _FGE_LOG_WARN:
				severity_str = L"warn";
				break;
			case _FGE_LOG_ERR:
				severity_str = L"err";
				break;
			default:
				severity_str = L"???";
				break;
			}
			fwprintf(stderr, L"[%s] %s\n", severity_str, msg);
		}
	}
	
	void _warn_helper(int severity, int log_errno, const char *fmt, va_list ap)
	{
		char buf[1024];
		size_t len;

		if (fmt != NULL)
			vformatstr(buf,sizeof(buf), fmt, ap);
		else
			buf[0] = '\0';

		if (log_errno >= 0) 
		{
			len = strlen(buf);
			if (len < sizeof(buf) - 3) 
			{
				char bufno[4]; 
				strerror_s(bufno,log_errno);
				sprintf_s(buf + len, sizeof(buf) - len, ": %s",bufno);
			}
		}

		fge_log(severity, buf);
	}
	void _warn_helper(int severity, int log_errno, const wchar *fmt, va_list ap)
	{
		wchar buf[1024];
		size_t len;

		if (fmt != NULL)
			vformatstr(buf,arraysize(buf), fmt, ap);
		else
			buf[0] = '\0';

		if (log_errno >= 0) 
		{
			len = wcslen(buf);
			if (len < arraysize(buf) - 3) 
			{
				wchar bufno[4]; 
				_wcserror_s(bufno,log_errno);
				formatstr(buf + len, arraysize(buf) - len, L": %s",bufno);
			}
		}

		fge_log(severity, buf);
	}
	
	void log_error(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_ERR, errno, fmt, ap);
		va_end(ap);
	}

	void log_warn(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_WARN, errno, fmt, ap);
		va_end(ap);
	}
	void log_debug(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_DEBUG, -1, fmt, ap);
		va_end(ap);
	}

	
	void log_error(const wchar *fmt, ...)
	{//´íÎó
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_ERR, errno, fmt, ap);
		va_end(ap);
	}

	void log_warn(const wchar *fmt, ...)
	{//¾¯¸æ
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_WARN, errno, fmt, ap);
		va_end(ap);
	}
	void log_debug(const wchar *fmt, ...)
	{//µ÷ÊÔ
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(_FGE_LOG_DEBUG, -1, fmt, ap);
		va_end(ap);
	}
}