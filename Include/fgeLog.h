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
*  �汾	 :	
*  ����  :  ��־����
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

		void		Flush( );		//�ֶ�Flush���԰��ڴ��е��ļ�����д�뵽�����ļ���.
		void		SetWriteTime(bool bWriteTime){ m_bWriteTime = bWriteTime; }
		bool		GetWriteTime( ){ return m_bWriteTime; }
		void		SetDisplaySeverity( int severity ){ m_displaySeverity= severity; }
	private:
		void		Log(int severity, const char *fmt, va_list ap);
	public:		
		static void WirteBinFile(const char* file, void* bin, unsigned long binLen);					// д�������ļ�
		static void BinTrunToTxt(char *text,unsigned long textLen, void* bin, unsigned long binLen);	// ��������ת��Ϊ�ı��ļ����ڲ鿴

	private:
		std::string		m_strName;
		clock_t			m_lastLogTime;
		DataBuf			m_logbuf;
		bool			m_bWriteTime;	//�Ƿ��¼ʱ��,  Ĭ��Ϊ��
		int				m_displaySeverity;
		CCriticalSection	m_cs;
	};

	//���º�����Ĭ�Ͻ���־�������׼���

	//���ֽڰ汾
	typedef void (*fge_log_cb)(int severity, const char *msg);
	void set_log_callback(fge_log_cb cb);

	void set_display_severity( int severity );
	void log_error(const char *fmt, ...);
	void log_warn(const char *fmt, ...);
	void log_debug(const char *fmt, ...);

	//���ֽڰ汾
	typedef void (*fge_log_cbw)(int severity, const wchar *msg);
	void set_log_callback(fge_log_cbw cb);
	void log_error(const wchar *fmt, ...);
	void log_warn(const wchar *fmt, ...);
	void log_debug(const wchar *fmt, ...);
}