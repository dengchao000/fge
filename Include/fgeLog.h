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
*  版本	 :	
*  描述  :  日志工具
**/

#pragma once

#include "fgeUtil.h"
#include "fgeThread.h"
#include "fgeFile.h"
//
namespace fge
{
	enum LOG_SEVERITY{
		LOG_DEBUG, LOG_WARN, LOG_ERR
	};
	class CLog
	{
	public:
		CLog( );
		CLog(const std::string& name);
		~CLog(void);

		void		Open(const std::string& name);
		void		Close(void);

		void		Error(const char *fmt, ...);
		void		Warn(const char *fmt, ...);
		void		Debug(const char *fmt, ...);

		void		Flush( );		//手动Flush可以把内存中的文件立即写入到磁盘文件中.
		void		SetWriteTime(bool bWriteTime){ m_bWriteTime = bWriteTime; }
		bool		GetWriteTime( ){ return m_bWriteTime; }
		void		SetDisplaySeverity( int severity ){ m_displaySeverity= severity; }
	private:
		void		Log(int severity, const char *fmt, va_list ap);
	public:		
		static void WirteBinFile(const char* file, void* bin, unsigned long binLen);					// 写二进制文件
		static void BinTrunToTxt(char *text,unsigned long textLen, void* bin, unsigned long binLen);	// 将二进制转化为文本文件便于查看

	private:
		std::string		m_strName;
		clock_t			m_lastLogTime;
		DataBuf			m_logbuf;
		bool			m_bWriteTime;	//是否记录时间,  默认为真
		int				m_displaySeverity;
		CCriticalSection	m_cs;
	};

	//以下函数将默认将日志输出到标准输出

	//多字节版本
	typedef void (*fge_log_cb)(int severity, const char *msg);
	void set_log_callback(fge_log_cb cb);

	void set_display_severity( int severity );
	void log_error(const char *fmt, ...);
	void log_warn(const char *fmt, ...);
	void log_debug(const char *fmt, ...);

	//宽字节版本
	typedef void (*fge_log_cbw)(int severity, const wchar *msg);
	void set_log_callback(fge_log_cbw cb);
	void log_error(const wchar *fmt, ...);
	void log_warn(const wchar *fmt, ...);
	void log_debug(const wchar *fmt, ...);
}