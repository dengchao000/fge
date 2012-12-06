/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011-6-8
** 描述  :  
**/

#pragma once
#include "TcpServer.h"
#include "TcpClient.h"
#include "ServerObject.h"

class CServerFrame : public CBaseService
{
public:
	CServerFrame(void);
	virtual ~CServerFrame(void);

	int  Setup( );					//启动服务器
	void Stop( );					//停止服务;
	void Pause( );					//暂停服务;

	void Reload( );					
protected:
	virtual void	OnInstall();
	virtual void	OnUninstall();

	virtual void	Update();		//线程函数
	virtual UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam );
private:
	ocl::TcpServer	m_serverConn;
	ocl::TcpClient	m_updateConn;
};
