/*
*  ����  :	�˳�
*  QQ    :	23427470
*  Mail	 :  dengchao000@gmail.com
*  ʱ��  :	2007-11-6
*
*  Fly Game 2
*  Copyright (C) 2005-2008, Fantasy Games
*  Kernel functions
*
*  �汾	 :	1.0
*  ����  :	�˳�
*  ����  :  ʹ��һ������������м�ʱ��,ͳһ��ѯ.�ŵ���ʵ�ּ�, �ڼ�ʱ����������ʱ���Գ���.
*
*  �汾	 :	2.0
*  ����  :	�˳�
*  ����  :  
*			ʹ�ö����������ʱ��,����Ҫ��ѯ. ���������һ���������. ���һ����Ԫʱ��,
*		��������Ϊ100ms. ÿ�����鶼�Ǵ˵�Ԫ�ı���.������� [ 0 | 100 | 200 | 300 | ... | 60000 ]
*		��60������. ÿ�����Ԫʱ����ѯ��Ӧ�����м�¼��ʱ����. �ŵ��ǿ���,��ѯ�ļ�ʱ�������϶�����Ч��.
*		ȱ���ǲ�����ȷ.
*
*  �汾  :	3.0
*  ����  :	�˳�
*  ����  :	
*			�ı��ʱ����ʱ��ʽ�� ʹ�÷ּ���ʱ��������ʱ����Ϊ5���ȼ��� �ֱ�Ϊ��
*			1. 0 - 100 ms
*			2. 100 - 300 ms
*			3. 300 - 500 ms
*			4. 500 - 1000 ms
*			5. 1000ms ����
*
*			������ʱ��Խ�̵Ĳ�ѯԽƵ��, ʱ��Խ��Խ��ѯ����Ӧ��Խ��.ÿ����ʱ����������ѯ���Ʋ�ѯʱ�䡣 
*		ÿ����ѯһ��������ʱ�����ƣ�ÿ����ʱ�����ڸ�����̬������ �ȼ� 1 һֱ��ѯ��
*			�˷����ۺ�������������Ʒ���. ʱ�侫ȷ,����һ�������ϼ��ٲ�ѯ����.
*
*  �汾  :	4.0
*  ����  :	�˳�
*  ����  :	
*			ʹ�����ṹ�Լ�ʱ�����й���,��ModifyClcok��DestroyTimer�������������Ż�,ʹ֮������Update�ص��Ĺ����е���.
*  �汾	 :  5
*
*/


#pragma once
#include <fgeDelegate.h>
#include <fgeSingleton.h>
#include <fgeSharedObject.h>
#include <time.h>
#include <map>
#include <strstream>
#include "fgeMinHeap.h"
#include "fgeList.h"

namespace fge{
	//#include <mmsystem.h>
	/**
	* ��ʱ���¼��ӿ�
	*/
	class ITimeEvent
	{
	protected:
		ITimeEvent(){}
		virtual ~ITimeEvent(){}		
	public:
		virtual void	ReleaseTimeEvent( ) = 0;				//�ͷż�ʱ���¼�����
		virtual void	OnTimeOut( unsigned timerid ) = 0;		//��ʱʱ�ص�
		virtual void	OnCancel( unsigned timerid ) = 0;		//ȡ����ʱ
	};
	/**
	* ��ʱ���¼��ӿڵ�ģ��ʵ��
	*/
	template<typename T>
	class DTimeEvent: public ITimeEvent
	{
	public:
		typedef void (T::*Handle)( );
	private:
		fge::SharedPtr<T>	mThis;
		Handle mHandle;
		Handle mCancel;
	public:
		/**
		* ���캯��,ע���¼���Ӧ����.
		*/
		DTimeEvent(T* refThis, Handle handle)
		{ 
			mThis = refThis;
			mHandle = handle;
			mCancel = 0;
		}
		DTimeEvent(T* refThis, Handle handle, Handle cancel)
		{ 
			mThis = refThis;
			mHandle = handle;
			mCancel = cancel;
		}
		virtual void ReleaseTimeEvent( ){ delete this ;}

		/**
		* ��ʱ��ģ��ʵ��,ʹ�����Աָ��ص���ע��ĺ�����.
		*/
		virtual void OnTimeOut(unsigned timerid)
		{
			T* refThis = mThis.Lock();
			if(refThis && mHandle)
			{			
				INVOKE(refThis,mHandle)( );
			}
		}
		//ȡ����ʱ
		virtual void OnCancel( unsigned timerid )
		{
			T* refThis = mThis.Lock();
			if(refThis && mCancel)
			{
				INVOKE(refThis,mCancel)( );
			}
		}
	};
	class FTimeEvent: public ITimeEvent
	{
	public:
		typedef void (*Handle)( );
	private:
		Handle mHandle;
		Handle mCancel;
	public:
		/**
		* ���캯��,ע���¼���Ӧ����.
		*/
		FTimeEvent(Handle handle)
		{ 
			mHandle = handle;
			mCancel = 0;
		}
		FTimeEvent(Handle handle, Handle cancel)
		{ 
			mHandle = handle;
			mCancel = cancel;
		}
		virtual void ReleaseTimeEvent( ){ delete this ;}
		virtual void OnTimeOut(unsigned timerid)
		{
			if(mHandle)
				mHandle( );
		}
		virtual void OnCancel( unsigned timerid )
		{
			if(mCancel)
				mCancel( );
		}
	};

	template<typename T>
	ITimeEvent* CTimeEvent( T* pObj, void (T::*handle)( ) )
	{
		return new DTimeEvent<T>(pObj,handle);
	}
	template<typename T>
	ITimeEvent* CTimeEvent( T* pObj, void (T::*handle)( ), void (T::*cancel)( ) )
	{
		return new DTimeEvent<T>(pObj,handle,cancel);
	}
	
	inline ITimeEvent* CTimeEvent( void (*handle)( ) )
	{
		return new FTimeEvent(handle);
	}
	inline ITimeEvent* CTimeEvent( void (*handle)( ), void (*cancel)( ) )
	{
		return new FTimeEvent(handle,cancel);
	}
	/**
	* һ���򵥵ļ�ʱ��
	*/
	class CTimer
	{
		unsigned	m_lStartingTime;	//��ʼʱ��
		unsigned 	m_lClockCycle;		//��ʱ����

	public:
		CTimer()  { m_lStartingTime = m_lClockCycle = 0; }
		~CTimer() { }

		//��ʼ��ʱ
		void	Start( void ){ m_lStartingTime = clock();	}
		void	Start( int offset ){ m_lStartingTime = clock() + offset;	}
		void	Stop( )		{ m_lStartingTime = 0; }

		//���ü�ʱ����
		void	SetCyc( unsigned lClockCycle )		{ m_lClockCycle = lClockCycle;	}

		//�Ƿ�ʱ
		bool	IsTimeOut( )const { return m_lStartingTime && clock() - m_lStartingTime >= m_lClockCycle ? true : false;	}
		//�Ƿ��Ѿ�����
		bool	IsStart( )	const { return m_lStartingTime!=0; }
		//����
		void	ReSet( )	{ m_lStartingTime+=m_lClockCycle; }//{ m_lStartingTime = clock(); }

		//���Լ�ʱ����ʣ��ʱ��.�������Ϊ0,��ʱ���ѵ�. �������<0,���Ѿ���ʱ.
		int		TestTime( )	const { return m_lStartingTime + (0x0fffffff&m_lClockCycle) - clock(); }
		//���㵱ǰ��ȥ�˶���ʱ��
		unsigned	PassTime( )	const { return clock() - m_lStartingTime; }
		//�������ʱ��
		unsigned	OverTime( )	const { return m_lStartingTime+m_lClockCycle; }
	};
	struct SClock : public SharedObject
	{
		unsigned	id;
		unsigned	min_heap_idx;		//�ڶ��е�����
		int			count;				//����
		CTimer		timer;				//��ʱ������
		ITimeEvent	*callback;			//�ص�����
		int 		flag;
		
		bool operator> (SClock& r) { return timer.OverTime()>r.timer.OverTime(); }
		SClock( ){callback = 0;}
		~SClock(){if(callback) callback->ReleaseTimeEvent( ); callback = 0;}
	};
	typedef fge::SharedPtr<SClock> HCLOCK;
	class CClock
	{
	private:
		enum { LIFE,ACTIVE,DEAD,RELOAD, };

	public:
		CClock(void);
		virtual ~CClock(void);
	public:
		//���ü�ʱ��
		HCLOCK	SetClock(unsigned id, unsigned dwMesl, ITimeEvent* callback, int offset=0);

		//���ü�ʱ��
		HCLOCK	SetClock(unsigned id, unsigned dwMesl, int count, ITimeEvent* callback,int offset=0);

		//�ͷż�ʱ��,���ڻص��¼��е���
		void	DestroyClock(HCLOCK& hclock);
		void	Destroy( );

		//���¼�ʱ��
		int		Update();

		//�޸ļ�ʱ,���ڻص��¼��е���
		void	ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count = -1);

		//������ʱ,���ص�ǰʱ��.
		unsigned	Start();

		int		TestTime(HCLOCK& hclock);
		int		PassTime(HCLOCK& hclock);
	
	private:		
		typedef min_heap_t<SClock> TimeQueue;
		//struct ClockBuf
		//{
		//	SClock*		data;
		//	ClockBuf*	next;
		//	void init( ){ data=0; next=0; }
		//	static void dctor(ClockBuf* node);
		//};
		TimeQueue m_clockList;
		//ClockBuf* m_reloadClock;
		//ClockBuf* m_nodeBuf;
	};
	typedef CClock CClockManager;
}