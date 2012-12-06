/****************************************************************
//  FileName:  Event.h
//  Author:		邓超
//  Create:		2010.5.13
//  Company:    DreamWork
****************************************************************/
#pragma once
#include <list>
#include <functional>
namespace fge{
#ifndef INVOKE
#define INVOKE(THIS,FUN) (THIS->*FUN)
#endif
/************************************************************************/
/*事件接口                                                              */
/************************************************************************/
class IEvent
{
protected:
	IEvent(){}
	virtual ~IEvent(){}
public:
	/**
	* 释放事件对象接口
	*/
	virtual void	ReleaseEvent( ) = 0;		// 释放事件
	virtual bool	IsOnceEvent( ) = 0;			// 是否是单次事件
};
class OnceEvent 
{
public:
	bool	IsOnceEvent( ){ return true; }
};
class DEvent : public IEvent
{
public:
	DEvent( ){ m_bIsOnceEvent = false; }
	/**
	* 释放事件对象接口,删除对象
	*/
	virtual void	ReleaseEvent( ){ delete this ;}					// 释放事件
	virtual bool	IsOnceEvent( ){ return m_bIsOnceEvent; }		// 是否是单次事件

	void			SetOnceEvent( bool bOnce ) { m_bIsOnceEvent = bOnce; }
protected:
	bool			m_bIsOnceEvent; // 是否是单次事件
};
template<typename TR>
class IEvent0 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( ) = 0;
};
template<typename TR,typename P1>
class IEvent1 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( P1 p1 )=0;
};
template<typename TR,typename P1,typename P2>
class IEvent2 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( P1 p1, P2 p2 )=0;
};
template<typename TR,typename P1,typename P2,typename P3>
class IEvent3 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( P1 p1, P2 p2, P3 p3 )=0;
};
template<typename TR,typename P1,typename P2,typename P3,typename P4>
class IEvent4 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4 )=0;
};
template<typename TR,typename P1,typename P2,typename P3,typename P4,typename P5>
class IEvent5 : public DEvent
{
public:
	typedef TR return_type;	
	virtual TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )=0;
};
template<typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class IEvent6 : public DEvent
{
public:
	typedef TR return_type;
	virtual TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )=0;
};
template<typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
class IEvent7 : public DEvent
{
public:
	typedef TR  return_type;
	virtual TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )=0;
};
//-----------------------------------------------
template<typename TR, typename TD>
class DEvent0 : public IEvent0<TR>
{
public:
	typedef TR (TD::*Handle)( );
	typedef TR return_type;
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent0(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( )
	{
		return INVOKE(mThis,mHandle)( );		
	}
};
template<typename TR,typename TD,typename P1,typename _P1=P1>
class DEvent1 : public IEvent1<TR,P1>
{
public:
	typedef TR (TD::*Handle)( _P1 );
	typedef TR return_type;
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent1(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1 )
	{
		return INVOKE(mThis,mHandle)( p1 );		
	}
};
template< typename TR,typename TD,typename P1,typename P2,
typename _P1=P1,typename _P2=P2>
class DEvent2 : public IEvent2<TR,_P1,_P2>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent2(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2 )
	{
		return INVOKE(mThis,mHandle)( p1,p2 );		
	}
};
template<typename TR,typename TD,typename P1,typename P2,typename P3,
typename _P1=P1,typename _P2=P2,typename _P3=P3>
class DEvent3 : public IEvent3<TR,P1,P2,P3>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2,_P3 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent3(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2, P3 p3 )
	{
		return INVOKE(mThis,mHandle)( p1,p2,p3 );		
	}
};
template<typename TR,typename TD,typename P1,typename P2,typename P3,typename P4,
typename _P1=P1,typename _P2=P2,typename _P3=P3,typename _P4=P4>
class DEvent4 : public IEvent4<TR,P1,P2,P3,P4>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2,_P3,_P4 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent4(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4 )
	{
		return INVOKE(mThis,mHandle)( p1,p2,p3,p4 );		
	}
};
template<typename TR,typename TD,typename P1,typename P2,typename P3,typename P4,typename P5,
typename _P1=P1,typename _P2=P2,typename _P3=P3,typename _P4=P4,typename _P5=P5>
class DEvent5 : public IEvent5<TR,P1,P2,P3,P4,P5>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2,_P3,_P4,_P5 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent5(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
	{
		return INVOKE(mThis,mHandle)( p1,p2,p3,p4,p5 );		
	}
};
template<typename TR,typename TD,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,
typename _P1=P1,typename _P2=P2,typename _P3=P3,typename _P4=P4,typename _P5=P5,typename _P6=P6>
class DEvent6 : public IEvent6<TR,P1,P2,P3,P4,P5,P6>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2,_P3,_P4,_P5,_P6 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent6(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
	{
		return INVOKE(mThis,mHandle)( p1,p2,p3,p4,p5,p6 );		
	}
};
template<typename TR,typename TD,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,
typename _P1=P1,typename _P2=P2,typename _P3=P3,typename _P4=P4,typename _P5=P5,typename _P6=P6,typename _P7=P7>
class DEvent7 : public IEvent7<TR,P1,P2,P3,P4,P5,P6,P7>
{
public:
	typedef TR (TD::*Handle)( _P1,_P2,_P3,_P4,_P5,_P6,_P7 );
private:
	TD*	mThis;
	Handle mHandle;
public:
	DEvent7(TD* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}

	/**
	* 事件回调接口,在这里调用真正的事件处理函数
	*/
	TR	InvokeEvent( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
	{
		return INVOKE(mThis,mHandle)( p1,p2,p3,p4,p5,p6,p7 );		
	}
};

//CEvent 函数负责创建一个代码事件对象.
template< typename TR,typename TD >
IEvent0<TR>* CEvent( TD* pObj, TR (TD::*handle)( void ) )
{
	return new DEvent0<TR,TD>(pObj,handle);
}
//--
template< typename TR,typename P1,typename TD >
IEvent1<TR,P1>* CEvent( TD* pObj, TR (TD::*handle)( P1 ) )
{
	return new DEvent1<TR,TD,P1>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename TD>
IEvent2<TR,P1,P2>* CEvent( TD* pObj, TR (TD::*handle)( P1,P2 ) )
{
	return new DEvent2<TR,TD,P1,P2>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename TD>
IEvent3<TR,P1,P2,P3>* CEvent( TD* pObj, TR (TD::*handle)( P1,P2,P3 ) )
{
	return new DEvent3<TR,TD,P1,P2,P3>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename TD>
IEvent4<TR,P1,P2,P3,P4>* CEvent( TD* pObj, TR (TD::*handle)( P1,P2,P3,P4 ) )
{
	return new DEvent4<TR,TD,P1,P2,P3,P4>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename TD>
IEvent5<TR,P1,P2,P3,P4,P5>*CEvent( TD* pObj, TR (TD::*handle)( P1,P2,P3,P4,P5 ) )
{
	return new DEvent5<TR,TD,P1,P2,P3,P4,P5>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename TD>
IEvent6<TR,P1,P2,P3,P4,P5,P6>* CEvent( TD* pObj, TR (TD::*handle)( P1,P2,P3,P4,P5,P6 ) )
{
	return new DEvent6<TR,TD,P1,P2,P3,P4,P5,P6>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename TD>
IEvent7<TR,P1,P2,P3,P4,P5,P6,P7>* CEvent( TD* pObj, TR (TD::*handle)( P1,P2,P3,P4,P5,P6,P7 ) )
{
	return new DEvent7<TR,TD,P1,P2,P3,P4,P5,P6,P7>(pObj,handle);
}
//--
template< typename TR,typename P1,typename TD, typename _P1 >
IEvent1<TR,P1>* CEvent( TD* pObj, TR (TD::*handle)( _P1 ) )
{
	return new DEvent1<TR,TD,P1,_P1>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename TD, 
typename _P1,typename _P2 >
IEvent2<TR,P1,P2>* CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2 ) )
{
	return new DEvent2<TR,TD,P1,P2,_P1,_P2>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename TD, 
typename _P1,typename _P2,typename _P3 >
IEvent3<TR,P1,P2,P3>* CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2,_P3 ) )
{
	return new DEvent3<TR,TD,P1,P2,P3,_P1,_P2,_P3>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename TD,
typename _P1,typename _P2,typename _P3,typename _P4 >
IEvent4<TR,P1,P2,P3,P4>* CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2,_P3,_P4 ) )
{
	return new DEvent4<TR,TD,P1,P2,P3,P4,_P1,_P2,_P3,_P4>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename TD,
typename _P1,typename _P2,typename _P3,typename _P4,typename _P5 >
IEvent5<TR,P1,P2,P3,P4,P5>*CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2,_P3,_P4,_P5 ) )
{
	return new DEvent5<TR,TD,P1,P2,P3,P4,P5,_P1,_P2,_P3,_P4,_P5>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename TD,
typename _P1,typename _P2,typename _P3,typename _P4,typename _P5,typename _P6 >
IEvent6<TR,P1,P2,P3,P4,P5,P6>* CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2,_P3,_P4,_P5,_P6 ) )
{
	return new DEvent6<TR,TD,P1,P2,P3,P4,P5,P6,_P1,_P2,_P3,_P4,_P5,_P6>(pObj,handle);
}
//--
template< typename TR,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7,typename TD,
typename _P1,typename _P2,typename _P3,typename _P4,typename _P5,typename _P6,typename _P7 >
IEvent7<TR,P1,P2,P3,P4,P5,P6,P7>* CEvent( TD* pObj, TR (TD::*handle)( _P1,_P2,_P3,_P4,_P5,_P6,_P7 ) )
{
	return new DEvent7<TR,TD,P1,P2,P3,P4,P5,P6,P7,_P1,_P2,_P3,_P4,_P5,_P6,_P7>(pObj,handle);
}

/************************************************************************/
/*               事件集合,用来对多个回调的支持                          */
/************************************************************************/

//template<class _Ty>
//struct equal_to
//	: public binary_function<_Ty, _Ty, bool>
//{	// functor for operator==
//	bool operator()(const _Ty& _Left, const _Ty& _Right) const
//	{	// apply operator== to operands
//		return (_Left == _Right);
//	}
//};

template< typename TEvent >
class EventSet
{
//protected:
//	typedef typename TEvent::return_type TR;
public:
	EventSet(){};
	~EventSet()
	{ 
		Clear();
	}
	//清空事件
	void	Clear( )
	{ 
		std::list<TEvent*>::iterator i=mEventList.begin();
		for( i; i!=mEventList.end(); i++ )
		{
			(*i)->ReleaseEvent(); *i=0;
		}
		mEventList.clear();
	}
	//添加事件
	void	operator += ( TEvent* pEvent ){ if(pEvent)  mEventList.push_back(pEvent); }
	TEvent*	PushOnceEvent( TEvent* pEvent ){ if(pEvent){ pEvent->SetOnceEvent(true); mEventList.push_back(pEvent); } return pEvent; }
	//移除事件
	void	RemoveEvent( TEvent* pEvent )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			if( (*i) == pEvent )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
				break;
			}
		}
	}
	//移除临时事件
	void	RemoveOnceEvent( )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	//回调事件
	void	Invoke( )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}	
	template<typename P1>
		void	Invoke( P1 p1 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( p1 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2>
		void	Invoke( P1 p1, P2 p2 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( p1,p2 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2,typename P3>
		void	Invoke( P1 p1, P2 p2, P3 p3 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( p1,p2,p3 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2,typename P3,typename P4>
		void	Invoke( P1 p1, P2 p2, P3 p3,P4 p4 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			rt = (*i)->InvokeEvent( p1,p2,p3,p4 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2,typename P3,typename P4,typename P5>
		void	Invoke( P1 p1, P2 p2, P3 p3,P4 p4, P5 p5 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			rt = (*i)->InvokeEvent( p1,p2,p3,p4,p5 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
		void	Invoke( P1 p1, P2 p2, P3 p3,P4 p4, P5 p5, P6 p6 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( p1,p2,p3,p4,p5,p6);
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
	template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
		void	Invoke( P1 p1, P2 p2, P3 p3,P4 p4, P5 p5, P6 p6,P7 p7 )
	{
		std::list<TEvent*>::iterator i=mEventList.begin();
		while(i!=mEventList.end())
		{
			(*i)->InvokeEvent( p1,p2,p3,p4,p5,p6,p7 );
			if( (*i)->IsOnceEvent( ) )
			{
				(*i)->ReleaseEvent();
				i = mEventList.erase(i);
			}
			else
				i++;
		}
	}
private:
	std::list<TEvent*> mEventList;
};
}