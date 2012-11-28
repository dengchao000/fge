/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.1
*  描述  :  单件模式和多件
**/

#pragma once
#include <assert.h>

namespace fge
{
	//单件模板,通过对单件的模板和函数式宏封装,统一了单件的调用方式,减少了代码和编码时间.
	template < typename T >
	class CSingleton
	{
	public:
		CSingleton( ){ }
		virtual ~CSingleton(){ }
	public:
		static T* Instance( )
		{
			static std::auto_ptr<T> ptrInstance;
			if(ptrInstance.get() == 0)
				ptrInstance.reset( new T() ); 	
			return ptrInstance.get();
		}
		static T* getSingleton( )
		{
			return Instance();
		}
	private:
	};
	
	//多件模板,支持一个类拥有多个实例的情况.
	template < typename T, int ID >
	class CSinglecall
	{
	public:
		CSinglecall( ){	m_instanceID = ID;	}
		virtual ~CSinglecall(){	}
		int	GetInstanceID( ){ return m_instanceID; }
	public:
		static T* Instance( )
		{
			static std::auto_ptr<T> ptrInstance;
			if(ptrInstance.get() == 0)
				ptrInstance.reset( new T() ); 	
			return ptrInstance.get();
		}
		static T* getInstance( )
		{
			return Instance();
		}
	private:

		int m_instanceID;
	};
}
#define INSTANCE(CLASS) fge::CSingleton< CLASS >::Instance( )
#define SINGLECALL(CLASS,ID) fge::CSinglecall< CLASS,ID >::Instance( )
#define SINGLETON(CLASS) friend fge::CSingleton< CLASS >; friend std::auto_ptr< CLASS >;