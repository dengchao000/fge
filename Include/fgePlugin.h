/*
 Copyright (C) 2005-2007, Fantasy Games
 Author:	�˳�
 QQ:		23427470
 Time:		2007-10-19
 ���ƣ�		�������
 ������		��Ϸ�������и��ַ�����Բ���ķ�ʽ�ṩ���񡣻����ж�������ͨ�ýӿڡ�
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
	//��ʼ��
	virtual void		OnInstall() = 0;
	//�ر�
	virtual void		OnUninstall() = 0;
	//����
	virtual void		Update() = 0;
	//��Ϣ����
	virtual UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam ) = 0;	
};
}