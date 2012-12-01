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
//�����ʱ�������޴���
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
//�����ʱ�����޶�conunt��
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

//�ͷż�ʱ��
void	CClock::DestroyClock(HCLOCK& hclock)
{
	SClock *c = (SClock *)hclock.Lock();
	if( !c ) return;
	if(c->flag == ACTIVE)
	{//�ڻص�������ɾ���Լ�,ֻ�޸�״̬
		c->flag = DEAD;
	}
	else
	{
		m_clockList.erase(c);
		delete c;
	}
	hclock.Reset();
}

//���¼�ʱ��
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

		//����Ϊ�״̬
		i->flag = ACTIVE;
		if(i->callback)
		{//�ص�����
			i->callback->OnTimeOut(i->id);
		}

		if( i->flag == DEAD )
		{//�����ڻص��Ĺ��̱�ɾ��
			delete i;
		}
		if( i->flag != ACTIVE )
		{//�����ڻص��Ĺ��̱�����
			continue;
		}

		if(i->count>0)
		{//��ʱ�Զ�����,�����Զ�����������1.
			
			i->count--;
		}

		if(i->count==0)
		{//��ʱ�Զ�ɾ��
			delete i;
		}
		else
		{//��ʱ�Զ�����
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
//�޸ļ�ʱ
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	SClock *c = hclock.Lock();
	if( !c ) return;
	m_clockList.erase(c);
	//
	////����Ѿ���m_reloadList����,�����ٴμ���
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

	//���ò���,��������
	c->count = count;
	c->flag = LIFE;
	c->timer.SetCyc(dwMesl);
	c->timer.Start();	
	m_clockList.push(c);
}

//������ʱ,���ص�ǰʱ��.
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