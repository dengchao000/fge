#include "StdAfx.h"
#include "fgePluginManager.h"
#include <algorithm>

using namespace fge;
CPluginManager::CPluginManager(void)
{
}

CPluginManager::~CPluginManager(void)
{
	UninstallPlugin();
}
void	CPluginManager::InstallPlugin(CPlugin* plugin)
{
	if(plugin)
	{
		plugin->OnInstall();
		m_Plugins.push_front(plugin);
	}
}

void	CPluginManager::UninstallPlugin(CPlugin* plugin)
{
	PluginInstanceList::iterator i = 
		std::find(m_Plugins.begin(), m_Plugins.end(), plugin);
	if (i != m_Plugins.end())
	{
		plugin->OnUninstall();
		m_Plugins.erase(i);
	}
	// delete plugin;
}
void	CPluginManager::UninstallPlugin()
{
	PluginInstanceList::iterator i = m_Plugins.begin();
	while(i!= m_Plugins.end())
	{
		(*i)->OnUninstall();
        //delete (*i);
		i++;
	}
	m_Plugins.clear();
}
UINT	CPluginManager::MsgProc(UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT hr = 0;
	PluginInstanceList::iterator i = m_Plugins.begin();
	while(i!= m_Plugins.end())
	{
		hr = (*i)->MsgProc(message,wParam,lParam);
		if(hr)
			return hr;
		i++;
	}
	return hr;
}

void	CPluginManager::Update()
{
	PluginInstanceList::iterator i = m_Plugins.begin();
	while(i!= m_Plugins.end())
	{
		(*i)->Update();
		i++;
	}
}