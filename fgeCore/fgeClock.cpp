#include "StdAfx.h"
#include "fgeClock.h"
#include <fgedebug.h>
using namespace fge;

CClock::CClock(void)
{
	//m_nodeBuf = m_reloadClock = 0;
}

CClock::~CClock(void)
{
	Destroy();
}
//void CClock::ClockBuf::dctor(ClockBuf* node)
//{
//	if( !node ) return;
//	for( ClockBuf* p = node->next; node; node=p )
//	{
//		if(node->data) 
//			delete node->data;
//		delete node;
//	} 
//}
void CClock::Destroy( )
{
	m_clockList.clear();
	//ClockBuf::dctor(m_nodeBuf);
	//ClockBuf::dctor(m_reloadClock);
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
	if(c->flag == ACTIVE)
	{//在回调函数中删除自己,只修改状态
		c->flag = DEAD;
	}
	else
	{
		m_clockList.erase(c);
		delete c;
	}
	hclock.Reset();
}

//更新计时器
int		CClock::Update()
{
	//while(m_reloadClock)
	//{
	//	if(m_reloadClock->data)
	//	{
	//		if(m_reloadClock->data->flag == DEAD)
	//		{
	//			delete m_reloadClock->data;
	//		}
	//		else
	//		{
	//			m_reloadClock->data->flag = LIFE;
	//			m_clockList.push(m_reloadClock->data);
	//		}
	//	}
	//	ClockBuf* pnode = m_reloadClock;
	//	m_reloadClock = m_reloadClock->next;		
	//	
	//	pnode->data = 0;
	//	pnode->next = m_nodeBuf;
	//	m_nodeBuf = pnode;
	//}

	int count = 0;
	while(!m_clockList.empty())
	{
		SClock* i = m_clockList.top();
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
		}
		if( i->flag != ACTIVE )
		{//可能在回调的过程被重启
			continue;
		}

		if(i->count>0)
		{//超时自动重启,将可自动重启次数减1.
			
			i->count--;
		}

		if(i->count==0)
		{//超时自动删除
			delete i;
		}
		else
		{//超时自动重启
			i->timer.ReSet();
			i->flag = LIFE;
			m_clockList.push(i);

			//i->flag = RELOAD;

			//ClockBuf *pNode = m_nodeBuf;
			//if(m_nodeBuf)
			//	m_nodeBuf = m_nodeBuf->next;
			//else
			//	pNode = new ClockBuf;

			//pNode->data = i;
			//pNode->next = m_reloadClock;
			//m_reloadClock = pNode;
		}
		count++;
	}

	if(m_clockList.empty() /*&& !m_reloadClock*/) return 0;
	return 1;
}
//修改计时
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	SClock *c = hclock.Lock();
	if( !c ) return;
	m_clockList.erase(c);
	//
	////如果已经在m_reloadList表中,则不用再次加入
	//if( RELOAD != c->flag )
	//{
	//	ClockBuf *pNode = m_nodeBuf;
	//	if(m_nodeBuf)
	//		m_nodeBuf = m_nodeBuf->next;
	//	else
	//		pNode = new ClockBuf;

	//	pNode->data = c;
	//	pNode->next = m_reloadClock;
	//	m_reloadClock = pNode;
	//}

	//设置参数,重新启动
	c->count = count;
	c->flag = LIFE;
	c->timer.SetCyc(dwMesl);
	c->timer.Start();	
	m_clockList.push(c);
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