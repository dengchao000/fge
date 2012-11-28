/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  INI配置文件操作对象
**/

#include "stdafx.h"
#include <fgeini.h>
using namespace fge;
Ini::Ini(const char *filename)
{
	strcpy(szIniFile,filename);
}
void Ini::Set(const char *section, const char *name, int value)
{
	char buf[256];

	if(szIniFile[0]) {
		sprintf(buf,"%d",value);
		WritePrivateProfileString(section, name, buf, szIniFile);
	}
}


int Ini::Get(const char *section, const char *name, int def_val)
{
	char buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileString(section, name, "", buf, sizeof(buf), szIniFile))
		{ return atoi(buf); }
		else { return def_val; }
	}
	return def_val;
}


void Ini::Set(const char *section, const char *name, float value)
{
	char buf[256];

	if(szIniFile[0]) {
		sprintf(buf,"%f",value);
		WritePrivateProfileString(section, name, buf, szIniFile);
	}
}


float Ini::Get(const char *section, const char *name, float def_val)
{
	char buf[256];

	if(szIniFile[0]) {
		if(GetPrivateProfileString(section, name, "", buf, sizeof(buf), szIniFile))
		{ return (float)atof(buf); }
		else { return def_val; }
	}
	return def_val;
}


void Ini::Set(const char *section, const char *name, const char *value)
{
	if(szIniFile[0]) WritePrivateProfileString(section, name, value, szIniFile);
}


char* Ini::Get(const char *section, const char *name, const char *def_val)
{
	if(szIniFile[0]) GetPrivateProfileString(section, name, def_val, szIniString, sizeof(szIniString), szIniFile);
	else strcpy(szIniString, def_val);
	return szIniString;
}

