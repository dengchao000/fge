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
** 描述：	内存池,是一个单件,只能使用单件接口访问.INSTANCE(CMemoryPool)
*
*/
#pragma once
#include <stdlib.h>
#include <fgeFreeList.h>
#include <fgeSingleton.h>
#include <fgeThread.h>
#include <fgeMemAllotor.h>
namespace fge
{
	class CMemoryPool :public MemAllotor
	{
	public:
		//SINGLETON(CMemoryPool);
		CMemoryPool(void);
		virtual ~CMemoryPool(void);
	public:
		virtual void*	Allot(unsigned int size);
		virtual bool	Free(void* pmem);

	private:
		FreeList*	m_pFreeTable[ALLOT_MAX_SIZE / HASH_WDITH];  //空闲表数组指针
	};

	void * Allot(unsigned int size);
	void   Free(void * p);
	template <class T>
	T* Allot(unsigned int size){ return (T*) Allot(size*sizeof(T)); }

	//线程安全的释放与分配
	void * Safe_Allot(unsigned int size);
	void   Safe_Free(void * p);

}// end of namespace

