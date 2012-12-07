/*
*  作者  :	邓超
*  QQ    :	23427470
*  Mail	 :  dengchao000@gmail.com
*  时间  :	2007-11-6
*
*  Fly Game 2
*  Copyright (C) 2005-2008, Fantasy Games
*  Kernel functions
*
*  版本	 :	1.0
*  作者  :	邓超
*  描述  :  使用一个链表管理所有计时器,统一轮询.优点是实现简单, 在计时器数量较少时可以承受.
*
*  版本	 :	2.0
*  作者  :	邓超
*  描述  :  
*			使用多个数组管理计时器,不需要轮询. 多个数组由一个链表管理. 设计一个单元时间,
*		初步设置为100ms. 每个数组都是此单元的倍数.如表中有 [ 0 | 100 | 200 | 300 | ... | 60000 ]
*		共60个数组. 每间隔单元时间便查询相应数组中记录的时间器. 优点是快速,查询的计时器基本上都是有效的.
*		缺点是不够精确.
*
*  版本  :	3.0
*  作者  :	邓超
*  描述  :	
*			改变计时器计时方式， 使用分级计时器。将计时器分为5个等级， 分别为：
*			1. 0 - 100 ms
*			2. 100 - 300 ms
*			3. 300 - 500 ms
*			4. 500 - 1000 ms
*			5. 1000ms 以上
*
*			理论是时间越短的查询越频繁, 时间越长越查询次数应该越少.每级计时器均采用轮询机制查询时间。 
*		每次轮询一级。随着时间推移，每个计时器会在各级动态调整。 等级 1 一直轮询。
*			此方案综合了以上两种设计方案. 时间精确,可在一定程序上减少查询数量.
*
*  版本  :	4.0
*  作者  :	邓超
*  描述  :	
*			使用树结构对计时器进行管理,对ModifyClcok和DestroyTimer两个函数做了优化,使之可以在Update回调的过程中调用.
*  版本	 :  5
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
	* 计时器事件接口
	*/
	class ITimeEvent
	{
	protected:
		ITimeEvent(){}
		virtual ~ITimeEvent(){}		
	public:
		virtual void	ReleaseTimeEvent( ) = 0;				//释放计时器事件对象
		virtual void	OnTimeOut( unsigned timerid ) = 0;		//超时时回调
		virtual void	OnCancel( unsigned timerid ) = 0;		//取消计时
	};
	/**
	* 计时器事件接口的模板实现
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
		* 构造函数,注册事件响应函数.
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
		* 超时的模板实现,使用类成员指针回调到注册的函数中.
		*/
		virtual void OnTimeOut(unsigned timerid)
		{
			T* refThis = mThis.Lock();
			if(refThis && mHandle)
			{			
				INVOKE(refThis,mHandle)( );
			}
		}
		//取消计时
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
		* 构造函数,注册事件响应函数.
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
	* 一个简单的计时器
	*/
	class CTimer
	{
		unsigned	m_lStartingTime;	//开始时间
		unsigned 	m_lClockCycle;		//计时周期

	public:
		CTimer()  { m_lStartingTime = m_lClockCycle = 0; }
		~CTimer() { }

		//开始计时
		void	Start( void ){ m_lStartingTime = clock();	}
		void	Start( int offset ){ m_lStartingTime = clock() + offset;	}
		void	Stop( )		{ m_lStartingTime = 0; }

		//设置计时周期
		void	SetCyc( unsigned lClockCycle )		{ m_lClockCycle = lClockCycle;	}

		//是否超时
		bool	IsTimeOut( )const { return m_lStartingTime && clock() - m_lStartingTime >= m_lClockCycle ? true : false;	}
		//是否已经启动
		bool	IsStart( )	const { return m_lStartingTime!=0; }
		//重置
		void	ReSet( )	{ m_lStartingTime+=m_lClockCycle; }//{ m_lStartingTime = clock(); }

		//测试计时器的剩余时间.如果返回为0,则时间已到. 如果返回<0,则已经超时.
		int		TestTime( )	const { return m_lStartingTime + (0x0fffffff&m_lClockCycle) - clock(); }
		//计算当前过去了多少时间
		unsigned	PassTime( )	const { return clock() - m_lStartingTime; }
		//计算结束时间
		unsigned	OverTime( )	const { return m_lStartingTime+m_lClockCycle; }
	};
	struct SClock : public SharedObject
	{
		unsigned	id;
		unsigned	min_heap_idx;		//在堆中的索引
		int			count;				//计数
		CTimer		timer;				//计时器对象
		ITimeEvent	*callback;			//回调函数
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
		//设置计时器
		HCLOCK	SetClock(unsigned id, unsigned dwMesl, ITimeEvent* callback, int offset=0);

		//设置计时器
		HCLOCK	SetClock(unsigned id, unsigned dwMesl, int count, ITimeEvent* callback,int offset=0);

		//释放计时器,可在回调事件中调用
		void	DestroyClock(HCLOCK& hclock);
		void	Destroy( );

		//更新计时器
		int		Update();

		//修改计时,可在回调事件中调用
		void	ModifyClcok(HCLOCK& hclock,unsigned dwMesl,int count = -1);

		//启动计时,返回当前时间.
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