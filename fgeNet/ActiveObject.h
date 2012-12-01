/*
Copyright (C) 2007-2009, Dc.

���� :	�˳�
QQ   :	23427470
Mail : aron_d@yeah.net 

���� : ��������
*/

#pragma once
#include <fgethread.h>
namespace ocl
{

class ActiveObject
{
public:
	ActiveObject(void);
	virtual ~ActiveObject(void);

	bool			Start( );
	void			Suspend( );	//����		
	void			Resume( );	//����

	virtual void	Close();
	unsigned int	GetID(){ return m_id; }
protected:		
	virtual void	Run();
	virtual void	ClearUp( );
private:		
	static unsigned int WINAPI _Run(LPVOID	lpParam);	//�����̴߳�����

	HANDLE			m_handle;  //���
	unsigned int	m_id;	   //

	fge::CCriticalSection	m_cs;
};

}