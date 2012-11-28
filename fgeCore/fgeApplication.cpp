#include "StdAfx.h"
#include "fgeApplication.h"
#include <shellapi.h>

#define WINDOW_CLASS_NAME "CFgeWindow"

using namespace fge;
CApplication*	CApplication::m_pApp = NULL;
HWND			CApplication::m_hwnd = NULL;
HINSTANCE		CApplication::g_hInstance = NULL;


CApplication::CApplication(void)
{
	m_dwWindowStyle = WS_OVERLAPPEDWINDOW|WS_VISIBLE; 
					//= WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	m_dwFStyle = WS_VISIBLE|WS_POPUP;

	strcpy(m_szWindowTitle,"fge demo");	
	m_menu = NULL;
	m_icon = NULL;	
	m_bgColor = 0;
	m_bWindowed = true;

	m_wndSize.cx = 640;
	m_wndSize.cy = 480;
	m_x=0;
	m_y=0;

	m_LimitFPS=FPS_UNLIMITED;
	m_fTime=0;
	m_fDeltaTime=0.0f;
	m_nFPS=0;
	m_nFixedDelta=0;
	m_nDelayDelta = 0;
	m_runMode = 0;
}

CApplication::~CApplication(void)
{
	m_pApp = NULL;
}


//----------------------------------------------------------------------------------------------
//全局的消息

LRESULT CApplication::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(m_pApp == NULL)
		return 0;

	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		if(wParam==MK_LBUTTON)
		{
			long x=GET_X_LPARAM(lParam),y=GET_Y_LPARAM(lParam);

			if(x<0) x=0; else if(x>m_pApp->m_clientRect.right) x=m_pApp->m_clientRect.right;
			if(y<0) y=0; else if(y>m_pApp->m_clientRect.bottom) y=m_pApp->m_clientRect.bottom;

			lParam = (x&0xFFFF)|(y<<16);
		}
		break;
	case 0x020A:
		{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			::ScreenToClient(hWnd,&pt);
			lParam = (pt.x&0xFFFF)|(pt.y<<16);
		}
		break;
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		m_pApp->MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }
    return m_pApp->MsgProc( hWnd, uMsg, wParam, lParam );
}

//得到文件的完整路径
bool CApplication::GetFullPath(TCHAR* lBuffer,const TCHAR *File)
{
	GetCurrentDirectory(MAX_PATH,lBuffer);
	lstrcat(lBuffer,TEXT("\\"));
	lstrcat(lBuffer,File);
	return true;
}

void CApplication::Log(const char* file, const char *format, ...)
{
	FILE *hf = NULL;
	char tmp[256];
	
	if(!file[0]) return;

	hf = fopen(file, "a");
	if(!hf) return;

	vsprintf(tmp, format, (char *)&format+sizeof(format));

	fprintf(hf, tmp);
	fprintf(hf, "\n");

	fclose(hf);
}
void CApplication::SystemLog(const char *format, ...)
{
	if(!m_pApp) return;

	FILE *hf = NULL;
	char tmp[256];
	char file[256];
	strcpy(file,m_pApp->GetWindowText());
	strcat(file,"_log.txt");

	hf = fopen(file, "a");
	if(!hf) return;

	vsprintf(tmp, format, (char *)&format+sizeof(format));

	fprintf(hf, tmp);
	fprintf(hf, "\n");

	fclose(hf);
}
//访问URL
bool CApplication::Launch(const char *url)
{
	if((DWORD)ShellExecute(m_hwnd, NULL, url, NULL, NULL, SW_SHOWMAXIMIZED)>32) 
		return true;
	else 
		return false;
}
//截屏
void CApplication::Snapshot(const char *filename)
{
}
//----------------------------------------------------------------------------------------------
int CApplication::Go()
{		
	MSG		msg;

	t0 = clock();
	if(!m_hwnd)
		throw ("CApplication::Go: Setup wasn't called");

	m_nDelayDelta = 0;
	//bool bActive=true;
	m_Deltatimer.Start( );
	m_bIdle = false;
	for(;;)
	{		
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			if (msg.message == WM_QUIT)	break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		
		_Update( );
	}
	
	this->Shutdown();
	return int( msg.wParam );
}
int CApplication::Run( ULONG Accel )
{
	MSG		msg;
	HACCEL hAccelTable;
	t0 = clock();
	if(!m_hwnd)
		throw ("CApplication::Go: Setup wasn't called");
	m_runMode = 1;
	m_nDelayDelta = 0;
	//bool bActive=true;
	m_Deltatimer.Start( );
	m_bIdle = false;
	hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(Accel));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	this->Shutdown();
	return int( msg.wParam );
}
void CApplication::_Update( )
{
	//do { dt=clock() - t0; } while(dt < 1);
	
	dt=clock() - t0;		
	m_fTime += dt;

	if(/*m_bIdle && */dt >= (m_nFixedDelta-m_nDelayDelta))
	{
		m_bIdle = false;

		if(m_nFixedDelta>0)
			m_nDelayDelta = dt-m_nFixedDelta+m_nDelayDelta;
		t0 += dt;
		//if(m_nFixedDelta>0)
		//	m_fDeltaTime=m_nFixedDelta/1000.0f;
		//else
		
		if(t0-t0fps < 1000) 
		{
			cfps++;
		}
		else
		{ 
			m_nFPS=cfps; 
			cfps=0;
			t0fps=t0; 
		}

		m_fDeltaTime = dt/1000.0f;
		m_Deltatimer.Start();
		Update();
		Show();

		if(m_bWindowed && m_LimitFPS==FPS_VSYNC)
			Sleep(1);

	}
	else {
		m_bIdle = true;
		Idle();
		if((!m_bActive && ((dt+3) < m_nFixedDelta)) || (m_nFixedDelta && ((dt+5) < m_nFixedDelta)))
			Sleep(1);
	}

	//if(!m_bActive)
	//	Sleep(1);
}
bool CApplication::Initiate()
{	
	t0 = clock();
	t0fps = t0;

	return true;
}

//创建窗口
bool CApplication::Setup(HINSTANCE hInstance,TCHAR* title,
						  int x, int y, int w, int h, bool bWindowed )
{
	OSVERSIONINFO	os_ver;		//操作系统版本
	SYSTEMTIME		tm;
	MEMORYSTATUS	mem_st;

	//HWND			hwndTmp;

	m_pApp = this;
	g_hInstance = hInstance;
	strcpy(m_szWindowTitle,title);

	//SystemLog("FGE Started..\n");

	//SystemLog("FGE version: %X.%X", FGE_VERSION>>8, FGE_VERSION & 0xFF);
	GetLocalTime(&tm);
	//SystemLog("Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);

	//SystemLog("Application: %s",m_szWindowTitle);
	os_ver.dwOSVersionInfoSize=sizeof(os_ver);
	GetVersionEx(&os_ver);
	//SystemLog("OS: Windows %ld.%ld.%ld",os_ver.dwMajorVersion,os_ver.dwMinorVersion,os_ver.dwBuildNumber);

	GlobalMemoryStatus(&mem_st);
	//SystemLog("Memory: %ldK total, %ldK free\n",mem_st.dwTotalPhys/1024L,mem_st.dwAvailPhys/1024L);

	//FGE init here.
	srand((unsigned)time(NULL));

	m_pApp = this;
	g_hInstance = hInstance;

	m_x = x;
	m_y = y;
	m_wndSize.cx = w;
	m_wndSize.cy = h;
	if(bWindowed)
	{
		//m_wndSize.cx += GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		m_wndSize.cx += GetSystemMetrics(SM_CXSIZEFRAME)*2;
		//m_wndSize.cy += GetSystemMetrics(SM_CYFIXEDFRAME)*2;
		m_wndSize.cy += GetSystemMetrics(SM_CYSIZEFRAME)*2;
		m_wndSize.cy += GetSystemMetrics(SM_CYCAPTION);
	}

	//if(m_dwWindowStyle & WS_VSCROLL)
	//	m_wndSize.cx += GetSystemMetrics(SM_CXVSCROLL);
	//if(m_dwWindowStyle & WS_HSCROLL)
	//	m_wndSize.cy += GetSystemMetrics(SM_CYHSCROLL);
	if(m_menu && bWindowed)
		m_wndSize.cy += GetSystemMetrics(SM_CYMENU);
	if(m_x<0)
		m_x=(GetSystemMetrics(SM_CXSCREEN)-m_wndSize.cx)/2;
	if(m_y<0)
		m_y=(GetSystemMetrics(SM_CYSCREEN)-m_wndSize.cy)/2;
	if(m_y<0)
		m_y = 0;
	m_hwnd = NULL;
	m_bWindowed = bWindowed;
	m_bActive = false;

	//HBRUSH bgBrush = CreateSolidBrush(m_bgColor);

	if(this->m_hwnd == NULL)
	{
		WNDCLASS wndClass = { CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW, CApplication::WndProc, 0, 0, hInstance,
			m_icon, LoadCursor( NULL, IDC_ARROW ),
			(HBRUSH)GetStockObject(BLACK_BRUSH),m_menu,TEXT(WINDOW_CLASS_NAME)};
		RegisterClass( &wndClass );

		if(bWindowed)
			m_hwnd = CreateWindowEx(0, TEXT(WINDOW_CLASS_NAME), GetWindowText(), m_dwWindowStyle,					
			m_x,m_y,m_wndSize.cx,m_wndSize.cy,
			NULL, NULL, hInstance, NULL);
		else
			m_hwnd = CreateWindowEx(WS_EX_TOPMOST, TEXT(WINDOW_CLASS_NAME), GetWindowText(), m_dwFStyle,
			0, 0, m_wndSize.cx,m_wndSize.cy,
			NULL, NULL, hInstance, NULL);
	}
	if(!m_hwnd) 
	{
		DWORD error = ::GetLastError( );
		return false;
	}
	::GetClientRect(m_hwnd,&m_clientRect);		
	if(!Initiate())
	{
		//CLog::Write("初始化应用程序失败");
		return false;	
	}
	
	m_fTime=0;
	t0=t0fps=clock();
	dt=cfps=0;
	m_nFPS=0;

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);
	m_bActive = true;

	//DeleteObject(bgBrush);
	//SystemLog("Init done.\n");
	return true;
}
//更新帧动画
void CApplication::Update()
{
}
void CApplication::Show()
{	
}
void CApplication::OnClose()
{
	PostQuitMessage(0);
}
//发送WM_DESTROY消息以释放资源。然后关闭窗口
void CApplication::Shutdown()
{
}

//窗口消息处理方法。在这里处理了Esc键。当按下Esc键后会向窗口发送WM_DESTROY消息
//如果重载此方法，请处理WM_DESTROY消息以释放资源
LRESULT CApplication::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		DragAcceptFiles(hWnd,TRUE);
		break;
	case WM_CHAR:
		if(VK_ESCAPE == wParam)
			OnClose();
		break;
	case WM_SETFOCUS:
		m_bActive = true;
		::InvalidateRect( hWnd,NULL,false );
		break;
	case WM_KILLFOCUS:
		m_bActive = false;
		break;
	case WM_SIZE:
		m_clientRect.right = LOWORD(lParam);
		m_clientRect.bottom = HIWORD(lParam);
		break;
	case WM_PAINT:	
		//_Update();
		{
			//PAINTSTRUCT pt;
			//::BeginPaint(hWnd, &pt);
			if(m_runMode)
			{
				Update();
				Show();
			}
			//ValidateRect( hWnd,NULL );
			//::EndPaint(hWnd, &pt);
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		OnClose();
		break;
	//case WM_MOVE:
	//	OnMove((int)(short) LOWORD(lParam),(int)(short) HIWORD(lParam));
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//设置FPS
void CApplication::SetFPS(int nfps)
{
	m_LimitFPS = nfps;
	if(nfps>0) 
	{
		m_nFixedDelta = 1000/nfps;
	}
	else
	{
		m_nFixedDelta=0;
	}
}