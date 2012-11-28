/*
 Copyright (C) 2005-2007, Fantasy Games
 Author:	邓超
 QQ:		23427470
 Time:		2007-10-19
 名称：		插件管理器
 描述：		插件管理器管理当前系统的所有插件，游戏服务器中各种服务均以插件的方式提供服务。
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

	//安装插件
	void	InstallPlugin(CPlugin* plugin);
	//卸载插件
	void	UninstallPlugin(CPlugin* plugin);
	//卸载所有
	void	UninstallPlugin( );

	UINT	MsgProc(UINT message, WPARAM wParam, LPARAM lParam );
	//更新
	void	Update();

private:
	PluginInstanceList m_Plugins;
};
}