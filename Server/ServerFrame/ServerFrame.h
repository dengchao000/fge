/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011-6-8
** ����  :  
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

	int  Setup( );					//����������
	void Stop( );					//ֹͣ����;
	void Pause( );					//��ͣ����;

	void Reload( );					
protected:
	virtual void	OnInstall();
	virtual void	OnUninstall();

	virtual void	Update();		//�̺߳���
	virtual UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam );
private:
	ocl::TcpServer	m_serverConn;
	ocl::TcpClient	m_updateConn;
};
