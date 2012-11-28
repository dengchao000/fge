#include "StdAfx.h"
#include "fgeClock.h"
#include <fgedebug.h>
using namespace fge;

CClock::CClock(void)
{
}

CClock::~CClock(void)
{
	Destroy();
}
void CClock::Destroy( )
{
	TimeMap::iterator i = m_clockList.begin();
	while( i!=m_clockList.end() ) 
	{
		if(i->second->callback)
			i->second->callback->OnCancel(i->second->id);
		delete i->second;
		i++;
	}
	m_clockList.clear();

	std::list<SClock*>::iterator r = m_reloadList.begin();
	unsigned timerid=0;
	while( r != m_reloadList.end() )
	{
		if((*r)->callback)
			(*r)->callback->OnCancel((*r)->id);
		delete (*r);
		r++;
	}
	m_reloadList.clear();
}
//插入计时器，不限次数
HCLOCK CClock::SetClock(unsigned id, unsigned dwMesl, ITimeEvent* callback, int offset)
{
	SClock *c = new SClock;
	c->callback = callback;
	c->count = 0;
	c->timer.SetCyc(dwMesl);
	c->timer.Start(offset);
	c->flag = LIFE;
	c->id = id;
	
	unsigned timerid = c->timer.OverTime();
	m_clockList.insert(TimeMap::value_type(timerid,c));
	return HCLOCK(c);
}
//插入计时器，限定conunt次
HCLOCK CClock::SetClock(unsigned id, unsigned dwMesl, int count, ITimeEvent* callback,int offset)
{
	SClock *c = new SClock;
	c->callback = callback;
	c->count = count;
	c->timer.SetCyc(dwMesl);
	c->timer.Start(offset);
	c->flag = LIFE;
	c->id = id;

	unsigned timerid = c->timer.OverTime();
	m_clockList.insert(TimeMap::value_type(timerid,c));
	return HCLOCK(c);
}

//释放计时器
void	CClock::DestroyClock(HCLOCK& hclock)
{
	SClock *c = (SClock *)hclock.Lock();
	if( !c ) return;
	c->flag = DEAD;
	hclock.Reset();
}

//更新计时器
int		CClock::Update()
{
	std::list<SClock*>::iterator r = m_reloadList.begin();
	unsigned timerid=0;
	while( r != m_reloadList.end() )
	{
		(*r)->flag = LIFE;
		timerid = (*r)->timer.OverTime();
		m_clockList.insert(TimeMap::value_type(timerid,*r));
		r++;
	}
	m_reloadList.clear();

	int count = 0;
	TimeMap::iterator i=m_clockList.begin();
	while(i!=m_clockList.end())
	{
		if( i->second->flag == DEAD )
		{
			delete i->second;
			i = m_clockList.erase(i);
			continue;
		}
		if( !i->second->timer.IsTimeOut() )
			break;

		//设置为活动状态
		i->second->flag = ACTIVE;
		if(i->second->callback)
		{//回调处理
			i->second->callback->OnTimeOut(0);
		}

		if( i->second->flag == DEAD )
		{//可能在回调的过程被删除
			delete i->second;
			i = m_clockList.erase(i);
			continue;
		}
		else if( i->second->flag == RELOAD )
		{//可能在回调的过程被重启
			i = m_clockList.erase(i);
			continue;
		}

		if(i->second->count>0)
		{//超时自动重启,将可自动重启次数减1.
			i->second->flag = LIFE;
			i->second->count--;
		}

		if(i->second->count==0)
		{//超时自动删除
			delete i->second;
			i = m_clockList.erase(i);
			continue;
		}
		else if(i->second->count<0)
		{//超时自动重启
			i->second->flag = RELOAD;
			i->second->timer.ReSet();
			m_reloadList.push_back(i->second);
			i = m_clockList.erase(i);
			continue;
		}

		i++;
	}

	return count;
}
//修改计时
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	SClock *c = hclock.Lock();
	if( !c ) return;

	if( RELOAD != c->flag &&	//如果已经在重启状态,则不m_clockList中
		ACTIVE != c->flag		//如果是在Update过程中修改,则不能修改m_clockList
		)
	{
		TimeMap::iterator i = m_clockList.find( c->timer.OverTime() );
		while( i->second != c ) i++;
		if( i!=m_clockList.end() )
			m_clockList.erase(i);
	}
	
	//如果已经在m_reloadList表中,则不用再次加入
	if( RELOAD != c->flag )
		m_reloadList.push_back(c);

	//设置参数,重新启动
	c->count = count;
	c->flag = RELOAD;
	c->timer.SetCyc(dwMesl);
	c->timer.Start();	
}

//启动计时,返回当前时间.
unsigned	CClock::Start()
{
	return 0;
}

int		CClock::TestTime(HCLOCK& hclock)
{
	SClock *c = hclock.Lock();
	if( !c ) return 0;
	return c->timer.TestTime( );

}
int		CClock::PassTime(HCLOCK& hclock)
{
	SClock *c = hclock.Lock();
	if( !c ) return 0;
	return c->timer.PassTime( );

}