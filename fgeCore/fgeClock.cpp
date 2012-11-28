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
	
	unsigned timerid = c->timer.OverTime();
	m_clockList.insert(TimeMap::value_type(timerid,c));
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

	unsigned timerid = c->timer.OverTime();
	m_clockList.insert(TimeMap::value_type(timerid,c));
	return HCLOCK(c);
}

//�ͷż�ʱ��
void	CClock::DestroyClock(HCLOCK& hclock)
{
	SClock *c = (SClock *)hclock.Lock();
	if( !c ) return;
	c->flag = DEAD;
	hclock.Reset();
}

//���¼�ʱ��
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

		//����Ϊ�״̬
		i->second->flag = ACTIVE;
		if(i->second->callback)
		{//�ص�����
			i->second->callback->OnTimeOut(0);
		}

		if( i->second->flag == DEAD )
		{//�����ڻص��Ĺ��̱�ɾ��
			delete i->second;
			i = m_clockList.erase(i);
			continue;
		}
		else if( i->second->flag == RELOAD )
		{//�����ڻص��Ĺ��̱�����
			i = m_clockList.erase(i);
			continue;
		}

		if(i->second->count>0)
		{//��ʱ�Զ�����,�����Զ�����������1.
			i->second->flag = LIFE;
			i->second->count--;
		}

		if(i->second->count==0)
		{//��ʱ�Զ�ɾ��
			delete i->second;
			i = m_clockList.erase(i);
			continue;
		}
		else if(i->second->count<0)
		{//��ʱ�Զ�����
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
//�޸ļ�ʱ
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	SClock *c = hclock.Lock();
	if( !c ) return;

	if( RELOAD != c->flag &&	//����Ѿ�������״̬,��m_clockList��
		ACTIVE != c->flag		//�������Update�������޸�,�����޸�m_clockList
		)
	{
		TimeMap::iterator i = m_clockList.find( c->timer.OverTime() );
		while( i->second != c ) i++;
		if( i!=m_clockList.end() )
			m_clockList.erase(i);
	}
	
	//����Ѿ���m_reloadList����,�����ٴμ���
	if( RELOAD != c->flag )
		m_reloadList.push_back(c);

	//���ò���,��������
	c->count = count;
	c->flag = RELOAD;
	c->timer.SetCyc(dwMesl);
	c->timer.Start();	
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