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
	m_clockList.clear();
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
	//unsigned timerid = c->timer.OverTime();
	m_clockList.push(c);
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

	m_clockList.push(c);
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
	int count = 0;
	while(!m_clockList.empty())
	{
		SClock* i = m_clockList.top();
		if( i->flag == DEAD )
		{
			delete i;
			m_clockList.pop();
			continue;
		}
		if( !i->timer.IsTimeOut() )
			break;
		m_clockList.pop();

		//设置为活动状态
		i->flag = ACTIVE;
		if(i->callback)
		{//回调处理
			i->callback->OnTimeOut(i->id);
		}

		if( i->flag == DEAD )
		{//可能在回调的过程被删除
			delete i;			
			continue;
		}
		else if( i->flag == RELOAD )
		{//可能在回调的过程被重启
			continue;
		}

		if(i->count>0)
		{//超时自动重启,将可自动重启次数减1.
			i->flag = LIFE;
			i->count--;
		}

		if(i->count==0)
		{//超时自动删除
			delete i;
		}
		else
		{//超时自动重启
			i->timer.ReSet();
			m_clockList.push(i);
		}
		count++;
	}

	return count;
}
//修改计时
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	//SClock *c = hclock.Lock();
	//if( !c ) return;

	//if( RELOAD != c->flag &&	//如果已经在重启状态,则不m_clockList中
	//	ACTIVE != c->flag		//如果是在Update过程中修改,则不能修改m_clockList
	//	)
	//{
	//	TimeQueue::iterator i = m_clockList.find( c->timer.OverTime() );
	//	while( i != c ) i++;
	//	if( i!=m_clockList.end() )
	//		m_clockList.erase(i);
	//}
	//
	////如果已经在m_reloadList表中,则不用再次加入
	//if( RELOAD != c->flag )
	//	m_reloadList.push_back(c);

	////设置参数,重新启动
	//c->count = count;
	//c->flag = RELOAD;
	//c->timer.SetCyc(dwMesl);
	//c->timer.Start();	
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