/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  �汾	 :	1.1
*  ����  :  ����ģʽ�Ͷ��
**/

#pragma once
#include <assert.h>

namespace fge
{
	//����ģ��,ͨ���Ե�����ģ��ͺ���ʽ���װ,ͳһ�˵����ĵ��÷�ʽ,�����˴���ͱ���ʱ��.
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
	
	//���ģ��,֧��һ����ӵ�ж��ʵ�������.
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