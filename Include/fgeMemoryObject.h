/*
*
** Author:	邓超
** QQ:		23427470
** Mail:    dengchao000@gmail.com
** Time:	2007/12/30
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Vertion: 
** 描述：	内存对象,重载new 和 delete,从内存池中分配内存,所有从此继承的类不能声明为全局变量,因为将无法保证全局变量删除之前内存池还有命在.
*
*/
#pragma once
#include <fgeutil.h>
#include <fgeMemoryPool.h>
#include <fgeThread.h>
namespace fge
{
	class CFgeObject 
	{
	public:
		CFgeObject(){}
		virtual ~CFgeObject(){}
	};

	class CMemoryObject
	{
	public:
		CMemoryObject();
		virtual ~CMemoryObject();
		//public:
		//	static int Init();
	public:
		void * operator new(unsigned int size);
		void * operator new[](unsigned int size);
		void   operator delete(void * pFree, unsigned int size);
		void   operator delete[](void * pFree, unsigned int size);
	};

	//线程安全的内存池对象
	class CMemoryObjectThreadSafe
	{
	public:
		CMemoryObjectThreadSafe();
		virtual ~CMemoryObjectThreadSafe();
	public:
		void * operator new(unsigned int size);
		void * operator new[](unsigned int size);
		void   operator delete(void * pFree, unsigned int size);
		void   operator delete[](void * pFree, unsigned int size);
	};
} // end of namespace