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
*  描述  :  静态类型系统。
**/

#pragma once
#include <winbase.h>
#pragma optimize("", off)
typedef int TYPEID;

namespace fge{
	class STTI_ID
	{
	public:
		static TYPEID	GetClassID( )
		{
			static int classId=1;
			return ::InterlockedExchangeAdd((PLONG)&classId,1);
		}
	};
	template< class T >
	struct CType
	{
		virtual ~CType( ){ }
		static TYPEID GetClassID(){ 
			//CType _id; 
			//return *(TYPEID*)&_id;
			static TYPEID id = STTI_ID::GetClassID();
			return id;
		}
	};

	template<class T>
	TYPEID typeof()
	{
		//if(T::S_TypeId( ) != fge::CType<T>::GetClassID())
		//return T::S_TypeId( );
		return fge::CType<T>::GetClassID();
	};
#pragma optimize("", on)

	template<class T>
	TYPEID typeof(T& t)
	{
		return t.TypeId();
	};
	template<class T>
	TYPEID typeof(T* t)
	{
		return t->TypeId();
	};
	//是一个T类型的值
	template<class T, class E>
	bool isa(E& e)
	{
		return e.Isa(typeof<T>());
	};
	//是一个T类型的值
	template<class T, class E>
	bool isa(E* e)
	{
		if(e == NULL) return false;
		return e->Isa(typeof<T>());
	};
	//是T类型
	template<class T, class E>
	bool is(E& e)
	{
		return e.TypeId() == typeof<T>();
	};
	//是指向T类型的指针
	template<class T, class E>
	bool is(E* e)
	{
		if(e==NULL) return false;
		return e->TypeId() == typeof<T>();
	};
	//定义
#define S_TYPEID(THIS) public: static TYPEID S_TypeId() {\
	return fge::CType<THIS>::GetClassID();\
	}
#define GET_TYPEID(THIS) public: virtual int TypeId( ){ return THIS::S_TypeId( ); }

	//基类
#define BASE(THIS) public: virtual bool Isa(TYPEID _typeid) \
	{\
	if(THIS::S_TypeId() == _typeid) return true;\
	return false;\
	};\
	S_TYPEID(THIS);\
	GET_TYPEID(THIS);
	//是一个
#define ISA(Parent,THIS) public: virtual bool Isa(TYPEID _typeid) \
	{\
	if(THIS::S_TypeId() == _typeid) return true;\
	return Parent::Isa(_typeid);\
	};\
	S_TYPEID(THIS);\
	GET_TYPEID(THIS);
	//转换到T类型
	template<class T, class E>
	T* turnto(E* e)
	{
		if (e==NULL) return NULL;

		if(isa<T>(e)) 
			return (T*)e;
		else
			return NULL;
	};

	struct Yes{};
	struct No{};
	template <typename T>
	struct IsPtr{ enum{ result = 0 }; typedef No Result; };
	template <typename T>
	struct IsPtr< T *> { enum{ result = 1 }; typedef Yes Result; };
	template <typename T>
	struct IsPtr< T *const> { enum{ result = 1}; typedef Yes Result; };

};