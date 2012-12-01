/*
Copyright (C) 2007-2009, Dc.

作者 :	邓超
QQ   :	23427470
Mail : aron_d@yeah.net 

描述 : 主动对象
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
	void			Suspend( );	//挂起		
	void			Resume( );	//就绪

	virtual void	Close();
	unsigned int	GetID(){ return m_id; }
protected:		
	virtual void	Run();
	virtual void	ClearUp( );
private:		
	static unsigned int WINAPI _Run(LPVOID	lpParam);	//启动线程处理函数

	HANDLE			m_handle;  //句柄
	unsigned int	m_id;	   //

	fge::CCriticalSection	m_cs;
};

}