/*
 Copyright (C) 2005-2007, Fantasy Games
 Author:	邓超
 QQ:		23427470
 Time:		2007-10-19
 名称：		插件基类
 描述：		游戏服务器中各种服务均以插件的方式提供服务。基类中定义插件的通用接口。
*/

#pragma once
#include <fgeUtil.h>

namespace fge
{
class CPlugin
{
public:
	CPlugin(void){}
	virtual ~CPlugin(void){}

public:
	//初始化
	virtual void		OnInstall() = 0;
	//关闭
	virtual void		OnUninstall() = 0;
	//更新
	virtual void		Update() = 0;
	//消息函数
	virtual UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam ) = 0;	
};
}