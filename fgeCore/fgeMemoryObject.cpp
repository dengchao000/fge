#include "StdAfx.h"
#include "fgeMemoryObject.h"
#include <fgeSingleton.h>

namespace fge
{
	static CMemoryPool memPool;
	CMemoryObject::CMemoryObject(void)
	{
	}

	CMemoryObject::~CMemoryObject(void)
	{
	}
	/*************************************************************************************
	* 函数功能：从（基于可利用空闲表）的内存池中分配对应字节大小的内存空间
	* 参    数：[IN] size_t size： 需要动态分配内存的大小
	* 返 回 值： 如果分配成功，则返回内存空间的首地址；否则返回NULL
	*备     注：如果分配空间大于MALLOC_MAX_SIZE，则将从系统堆上直接分配内存
	***************************************************************************************/
	void * CMemoryObject::operator new(unsigned int size)
	{
		return memPool.Allot(size);
	}

	/*************************************************************************************
	* 函数功能：从（基于可利用空闲表）的内存池中分配对应字节大小的内存空间
	* 参    数：[IN] size_t size： 需要动态分配内存的大小
	* 返 回 值：如果分配成功，则返回内存空间的首地址；否则返回NULL
	* 备    注：如果分配空间大于MALLOC_MAX_SIZE，则将从系统堆上直接分配内存
	***************************************************************************************/
	void * CMemoryObject::operator new[](unsigned int size)
	{
		return memPool.Allot(size);
	}

	/*************************************************************************************
	* 函数功能：将内存空间暂存到内存池中,
	* 注    意：因为重载delete会检查参数是否为空，如果为空，则
	*			自动返回，不执行函数的代码。所以我们不做指针检查
	* 参    数：[IN] void * pFree： 释放内存的首地址
	***************************************************************************************/
	void CMemoryObject::operator delete(void * pFree, unsigned int size)
	{
		memPool.Free(pFree);
	}

	/*************************************************************************************
	* 函数功能：将内存空间暂存到内存池中
	* 参    数：[IN] void * pFree： 释放内存的首地址
	***************************************************************************************/
	void CMemoryObject::operator delete[](void * pFree, unsigned int size)
	{
		memPool.Free(pFree);
	}

	//==============================

	CMemoryObjectThreadSafe::CMemoryObjectThreadSafe(void){}

	CMemoryObjectThreadSafe::~CMemoryObjectThreadSafe(void){}
	
	void * CMemoryObjectThreadSafe::operator new(unsigned int size)
	{
		return Safe_Allot(size);
	}
	void * CMemoryObjectThreadSafe::operator new[](unsigned int size)
	{
		return Safe_Allot(size);
	}

	void CMemoryObjectThreadSafe::operator delete(void * pFree, unsigned int size)
	{
		Safe_Free(pFree);
	}
	void CMemoryObjectThreadSafe::operator delete[](void * pFree, unsigned int size)
	{
		Safe_Free(pFree);
	}

}