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
	c->flag = DEAD;
	hclock.Reset();
}

//���¼�ʱ��
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

		//����Ϊ�״̬
		i->flag = ACTIVE;
		if(i->callback)
		{//�ص�����
			i->callback->OnTimeOut(i->id);
		}

		if( i->flag == DEAD )
		{//�����ڻص��Ĺ��̱�ɾ��
			delete i;			
			continue;
		}
		else if( i->flag == RELOAD )
		{//�����ڻص��Ĺ��̱�����
			continue;
		}

		if(i->count>0)
		{//��ʱ�Զ�����,�����Զ�����������1.
			i->flag = LIFE;
			i->count--;
		}

		if(i->count==0)
		{//��ʱ�Զ�ɾ��
			delete i;
		}
		else
		{//��ʱ�Զ�����
			i->timer.ReSet();
			m_clockList.push(i);
		}
		count++;
	}

	return count;
}
//�޸ļ�ʱ
void	CClock::ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count/* = -1*/)
{
	//SClock *c = hclock.Lock();
	//if( !c ) return;

	//if( RELOAD != c->flag &&	//����Ѿ�������״̬,��m_clockList��
	//	ACTIVE != c->flag		//�������Update�������޸�,�����޸�m_clockList
	//	)
	//{
	//	TimeQueue::iterator i = m_clockList.find( c->timer.OverTime() );
	//	while( i != c ) i++;
	//	if( i!=m_clockList.end() )
	//		m_clockList.erase(i);
	//}
	//
	////����Ѿ���m_reloadList����,�����ٴμ���
	//if( RELOAD != c->flag )
	//	m_reloadList.push_back(c);

	////���ò���,��������
	//c->count = count;
	//c->flag = RELOAD;
	//c->timer.SetCyc(dwMesl);
	//c->timer.Start();	
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