/**
*
** Author:	�˳�
** QQ:		23427470
** Mail:    dengchao000@gmail.com
** Time:	2007-11-21
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Kernel functions
** 
** Description: Ӧ�ó������ࡣ��װ�˻�����Win32��Ϣ������ƣ�ʹ���ʺ�����Ϸ����Ϣѭ����
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

	//��ʼ��
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
	//���ʺ���
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
	//��Ϣ����  
    virtual  LRESULT	MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);		
    //��ʾ����	
    virtual  void		Show();			
    //֡ѭ��				
    virtual  void		Update();
	//����ʱ����
	virtual  void		Idle(){}	
	//ж�� 
	virtual void		Shutdown();		
protected:
    //��ʼ��Ӧ�ó������л���,�ڴ������ں�,��ʾ����ǰ������.	
    virtual  bool		Initiate();		
	//�ر�
	virtual void		OnClose();		
public:
	static bool			GetFullPath(TCHAR* lBuffer,const TCHAR *File);		

	//����URL
	static	bool		Launch(const char *url);
	//����
	static  void		Snapshot(const char *filename=0);

private:
    //д��־
	static	void		Log(const char* file, const char *format, ...);
	static	void		SystemLog(const char *format, ...);

protected:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
	void				_Update( );
protected:
	DWORD       m_dwWindowStyle;					// ���ڷ��
	DWORD		m_dwFStyle;							// ȫ�����ڷ��

	TCHAR		m_szWindowTitle[256];				// �������ı�
	DWORD		m_bgColor;							// ����ɫ	
	bool		m_bWindowed;						// ��ʼΪȫ����
	int			m_x,m_y;
	SIZE		m_wndSize;							// ���ڴ�С
	RECT		m_clientRect;
	TCHAR*		m_menu;								// �˵�
	HICON		m_icon;								// ͼ��
	int			m_LimitFPS;							// fps����
	int			m_nFixedDelta;
	int			m_nDelayDelta;
	int			m_nFPS;
private:	
	clock_t		t0, t0fps;		
	int			dt;				//��ǰ֡���˶���ʱ��(ms)
	int			cfps;			//ͳ��1���ڵ�fps

	DWORD		m_fTime;
	float		m_fDeltaTime;
	CTimer		m_Deltatimer;
	int			m_bIdle;
	int			m_runMode;
public:
	bool		m_bActive;					// �����Ƿ�Ϊ�
	static		HWND		m_hwnd;			// ���ھ��	
	static		HINSTANCE	g_hInstance;

	static CApplication *m_pApp;
};
}