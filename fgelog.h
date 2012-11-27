/**
日志文件
Dengchao. 2012.11.24
*/
#pragma once
namespace fge{
	enum LOG_SEVERITY{
		_FGE_LOG_DEBUG,_FGE_LOG_WARN,_FGE_LOG_ERR
	};

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
};
