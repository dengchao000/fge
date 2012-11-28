/**
** Author:	µË³¬
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2006-7-1
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  °æ±¾	 :	1.0
*  ÃèÊö  :  µü´úÆ÷
**/


#pragma once
#include <exception>

namespace fge
{
	template<class T>
	class Iterator
	{
	public:

		Iterator(void)			{}
		virtual ~Iterator(void)	{}

		virtual T& get() = 0;
		virtual void next() = 0;
		virtual bool isdone() = 0;
		virtual void previous(){};
	};

	template< typename E, typename T >
		Iterator<E>* beginof(T* t)
	{
		return t->begin();
	};

	template< typename E, typename T >
		Iterator<E>* endof(T* t)
	{
		return t->end();
	};

	template< typename E, typename T >
		Iterator<E>* beginof(T& t)
	{
		return t.begin();
	};

	template< typename E, typename T >
		Iterator<E>* endof(T& t)
	{
		return t.end();
	};
};