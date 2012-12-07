/**
*
** Author:	邓超
** QQ:		23427470
** Mail:    dengchao000@gmail.com
** Time:	2007-11-21
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Kernel functions
** 
** Description: 应用程序框架类。封装了基本的Win32消息处理机制，使用适合于游戏的消息循环。
*
*/

#pragma once

#include <fgeutil.h>
#include <fgeclock.h>
#include <windows.h>
#include <windowsx.h>
namespace fge
{
class CApplication : public IConsign
{
public:
	CApplication(void);
	virtual ~CApplication(void);
public:

	//初始化
	virtual bool		Setup(HINSTANCE hInstance,TCHAR* title,
		int x, int y, int w, int h, bool bWindowed = true);	

	int			Go();
	int			Run( ULONG Accel=0 );
	//timer
	float				GetTime()						{ return m_fTime/1000.0f; }
	virtual float		GetDelta()						{ return m_fDeltaTime; }
	virtual float		SetDelta(float ftime)			{ return m_fDeltaTime = ftime; }
	int					GetFPS()						{ return m_nFPS; }
	void				SetDefaultFPS(int fps)			{ m_nFPS = fps; }
	int					GetLimitFPS()					{ return m_LimitFPS; }
	void				Close()							{ OnClose(); }
	//访问函数
	const	TCHAR*		GetWindowText()					{ return m_szWindowTitle;}
	DWORD				GetBgColor()					{ return m_bgColor; }
	bool				GetWindowed()					{ return m_bWindowed;}
	virtual RECT		GetClientRect()					{ return m_clientRect;}	
	virtual WORD		GetCanvasWidth( )				{ return (WORD)m_clientRect.right; }
	virtual WORD		GetCanvasHeight( )				{ return (WORD)m_clientRect.bottom; }

	void				SetMenu(TCHAR*	menu)			{ m_menu = menu; }
	void				SetIcon(HICON	icon)			{ m_icon = icon;}
	void				SetIcon(TCHAR*  lpStrIcon)		{ m_icon = (HICON) LoadImage(NULL,lpStrIcon,IMAGE_ICON,0,0,LR_LOADFROMFILE);}
	void				SetBackColor(DWORD bgColor)		{ m_bgColor = bgColor; }	
	void				SetCaption(const TCHAR* szTitle)	{ lstrcpy(m_szWindowTitle,szTitle); if(m_hwnd) SetWindowText(m_hwnd,szTitle);}

	void				SetFPS(int nfps);
	
public:
	//消息函数  
    virtual  LRESULT	MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);		
    //显示窗口	
    virtual  void		Show();			
    //帧循环				
    virtual  void		Update();
	//空闲时调用
	virtual  void		Idle(){}	
	//卸载 
	virtual void		Shutdown();		
protected:
    //初始化应用程序运行环境,在创建窗口后,显示窗口前被调用.	
    virtual  bool		Initiate();		
	//关闭
	virtual void		OnClose();		
public:
	static bool			GetFullPath(TCHAR* lBuffer,const TCHAR *File);		

	//访问URL
	static	bool		Launch(const char *url);
	//截屏
	static  void		Snapshot(const char *filename=0);

private:
    //写日志
	static	void		Log(const char* file, const char *format, ...);
	static	void		SystemLog(const char *format, ...);

protected:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
	void				_Update( );
protected:
	DWORD       m_dwWindowStyle;					// 窗口风格
	DWORD		m_dwFStyle;							// 全屏窗口风格

	TCHAR		m_szWindowTitle[256];				// 标题栏文本
	DWORD		m_bgColor;							// 背景色	
	bool		m_bWindowed;						// 初始为全屏？
	int			m_x,m_y;
	SIZE		m_wndSize;							// 窗口大小
	RECT		m_clientRect;
	TCHAR*		m_menu;								// 菜单
	HICON		m_icon;								// 图标
	int			m_LimitFPS;							// fps限制
	int			m_nFixedDelta;
	int			m_nDelayDelta;
	int			m_nFPS;
private:	
	clock_t		t0, t0fps;		
	int			dt;				//当前帧花了多少时间(ms)
	int			cfps;			//统计1秒内的fps

	DWORD		m_fTime;
	float		m_fDeltaTime;
	CTimer		m_Deltatimer;
	int			m_bIdle;
	int			m_runMode;
public:
	bool		m_bActive;					// 窗口是否为活动
	static		HWND		m_hwnd;			// 窗口句柄	
	static		HINSTANCE	g_hInstance;

	static CApplication *m_pApp;
};
}