/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2012-12-6
** ����  :  
**/

#pragma once
#include "fgeThread.h"
#include "fgePluginManager.h"
#include "fgeMessageManager.h"

class ServerApp : public CBaseService
{
public:
	ServerApp( );
	virtual ~ServerApp( );
protected:
	virtual void	OnInstall();
	virtual void	OnUninstall();

	virtual void	Update();		//�̺߳���
	virtual UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam );
private:
	ocl::TcpServer	m_serverConn;
	ocl::TcpClient	m_updateConn;
};