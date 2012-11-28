#include "StdAfx.h"
#include "fgelog.h"
#include <iostream>
//#include <fgeLzCompress.h>

using namespace std;
namespace fge
{
	static CCriticalSection	s_cs;
	static CCriticalSection	s_csConsole;
	static fge_log_cb log_fn = NULL;
	static fge_log_cbw log_fnw = NULL;
	static int log_severity = 0;

	static CLog sLog("fge_error_log.txt");

	CLog::CLog( )
	{
		m_strName = "log.txt";
		m_lastLogTime = 0;
		m_bWriteTime = true;
		m_displaySeverity = LOG_DEBUG;
	}
	CLog::CLog(const std::string& name)
	{
		m_strName = name;
		m_lastLogTime = 0;
	}

	CLog::~CLog(void)
	{
		Close();
	}
	void CLog::Close(void)
	{
		MLOCK lk(m_cs);	// 多线程安全
		Flush();
		m_strName.clear();
	}

	void CLog::Open(const std::string& name)
	{
		MLOCK lk(m_cs);	// 多线程安全
		Close( );
		m_strName = name;
	}

	void CLog::Warn(const char *fmt, ...)
	{
		va_list header;
		va_start(header, fmt);
		Log(LOG_WARN,fmt,header);
		va_end(header);		
	}
	void CLog::Error(const char *fmt, ...)
	{
		va_list header;
		va_start(header, fmt);
		Log(LOG_ERR,fmt,header);
		va_end(header);		
	}
	void CLog::Debug(const char *fmt, ...)
	{
		va_list header;
		va_start(header, fmt);
		Log(LOG_DEBUG,fmt,header);
		va_end(header);		
	}

	void CLog::Log(int severity, const char *fmt, va_list ap)
	{
		if(severity<m_displaySeverity)
			return;
		static char tmp[4096];
		char* pbuf = tmp;

		size_t buflen = arraySize(tmp);
		const char* severity_str="";
		switch (severity)
		{
		case LOG_DEBUG:
			severity_str = "[debug] ";
			break;
		case LOG_WARN:
			severity_str = "[warn] ";
			break;
		case LOG_ERR:
			severity_str = "[err] ";
			break;
		default:
			severity_str = "";
			break;
		}
		formatstr(pbuf,buflen,severity_str);
		size_t _len = strlen(pbuf);
		buflen -= _len;
		pbuf += _len;

		if(m_bWriteTime)
		{
			SYSTEMTIME t;
			::GetLocalTime( &t );
			formatstr(pbuf,buflen,"[%d-%d-%d %d:%d:%d] ",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond);
			_len = strlen(pbuf);
			buflen -= _len;
			pbuf += _len;
		}

		if(!m_bWriteTime)
			vformatstr(pbuf,buflen, fmt, ap);
		else
			vformatstr(pbuf,buflen, fmt, ap);
		strcat(pbuf,"\n");
		m_cs.Enter();
		m_logbuf.Write(tmp,strlen(tmp));
		m_cs.Leave();

		if( (clock() - m_lastLogTime)>2 || m_logbuf.GetLen() >= 16*1024 )
			Flush();
		m_lastLogTime = clock();
	}
	void CLog::Flush( )
	{
		MLOCK lk(m_cs);	// 多线程安全
		fge::File outfile;
		if(outfile.Open(m_strName.c_str(),fge::File::MODE_APPEND|fge::File::MODE_WRITE))
			outfile.Write(m_logbuf(),m_logbuf.GetLen());
		m_logbuf.ReSet();
	}

	void CLog::WirteBinFile( const char* file, void* bin, unsigned long binLen )
	{
		MLOCK lk(s_cs);	// 多线程安全
		HANDLE hFile = CreateFileA( file, FILE_WRITE_DATA | FILE_READ_DATA, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL );
		if(!hFile) return;

		DWORD dwWrite;
		if( false == ::WriteFile( hFile, bin, binLen, &dwWrite, NULL ) )
		{
			sLog.SetWriteTime(true);
			sLog.Warn("CLog::WirteBinFile::Write Bin Log File Error!");
			return;
		}

	}

	void CLog::BinTrunToTxt(char *text, unsigned long textLen, void* bin, unsigned long binLen)
	{
		MLOCK lk(s_cs);	// 多线程安全

		char* p_sz = text;
		char ca[3];
		BYTE* p_b = (BYTE*)bin;

		for (DWORD i = 0 ; i < binLen || textLen > 3; i++)
		{
			formatstr(ca,"%x",*p_b);
			if(strlen(ca) == 1)
			{
				ca[1]=ca[0];
				memcpy(ca,"0",1);
				ca[2]=0;
			}
			memcpy(p_sz,ca,2);
			p_sz +=2;
			memcpy(p_sz," ",1);
			p_sz ++;
			//p_sz+=strlen(ca)+1;
			p_b++;
			textLen -= 3;
		}
		ZeroMemory(p_sz,1);
	}

	//----------------------------------------
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
			case LOG_DEBUG:
				severity_str = "debug";
				break;
			case LOG_WARN:
				severity_str = "warn";
				break;
			case LOG_ERR:
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
			case LOG_DEBUG:
				severity_str = L"debug";
				break;
			case LOG_WARN:
				severity_str = L"warn";
				break;
			case LOG_ERR:
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
			vformatstr(buf,arraySize(buf), fmt, ap);
		else
			buf[0] = '\0';

		if (log_errno >= 0) 
		{
			len = wcslen(buf);
			if (len < arraySize(buf) - 3) 
			{
				wchar bufno[4]; 
				_wcserror_s(bufno,log_errno);
				formatstr(buf + len, arraySize(buf) - len, L": %s",bufno);
			}
		}

		fge_log(severity, buf);
	}

	void log_error(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_ERR, errno, fmt, ap);
		va_end(ap);
	}

	void log_warn(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_WARN, errno, fmt, ap);
		va_end(ap);
	}
	void log_debug(const char *fmt, ...)
	{
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_DEBUG, -1, fmt, ap);
		va_end(ap);
	}


	void log_error(const wchar *fmt, ...)
	{//错误
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_ERR, errno, fmt, ap);
		va_end(ap);
	}

	void log_warn(const wchar *fmt, ...)
	{//警告
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_WARN, errno, fmt, ap);
		va_end(ap);
	}
	void log_debug(const wchar *fmt, ...)
	{//调试
		va_list ap;

		va_start(ap, fmt);
		_warn_helper(LOG_DEBUG, -1, fmt, ap);
		va_end(ap);
	}
}