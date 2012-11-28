/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  �汾	 :	1.0
*  ����  :  INI�����ļ���������
**/

#pragma once
#include <fgeutil.h>
namespace fge{
class Ini
{
public:
	Ini(const char* filename);

	void Set(const char *section, const char *name, int value);	
	void Set(const char *section, const char *name, float value);
	void Set(const char *section, const char *name, const char *value);
	
	int	  Get(const char *section, const char *name, int def_val);
	float Get(const char *section, const char *name, float def_val);	
	char* Get(const char *section, const char *name, const char *def_val);
private:
	char				szIniFile[MAX_PATH];
	char				szIniString[256];
};
}