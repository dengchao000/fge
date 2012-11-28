/*
 Copyright (C) 2005-2007, Fantasy Games
 Author:	�˳�
 QQ:		23427470
 Time:		2007-10-19
 ���ƣ�		���������
 ������		�������������ǰϵͳ�����в������Ϸ�������и��ַ�����Բ���ķ�ʽ�ṩ����
*/


#pragma once
#include <fgeutil.h>
#include <fgeplugin.h>
namespace fge
{
class CPluginManager
{
	//SINGLETON(CPluginManager);
public:
	CPluginManager(void);
	virtual ~CPluginManager(void);

public:
	typedef std::list<CPlugin*>  PluginInstanceList;

public:

	//��װ���
	void	InstallPlugin(CPlugin* plugin);
	//ж�ز��
	void	UninstallPlugin(CPlugin* plugin);
	//ж������
	void	UninstallPlugin( );

	UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam );
	//����
	void	Update();

private:
	PluginInstanceList m_Plugins;
};
}