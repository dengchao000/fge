#include "StdAfx.h"
#include "fgeMemorypool.h"
#include <fgeutil.h>
#include <assert.h>
namespace fge
{
	static CMemoryPool memPool;
	CMemoryPool::CMemoryPool(void)
	{
		for( int i=0; i<ALLOT_MAX_SIZE / HASH_WDITH; i++)
			m_pFreeTable[i] = 0;
	}

	CMemoryPool::~CMemoryPool(void)
	{
		for( int i=0; i<ALLOT_MAX_SIZE / HASH_WDITH; i++)
		{
			SAFE_DELETE( m_pFreeTable[i] );
		}
	}

	void*	CMemoryPool::Allot(unsigned int size)
	{
		if(size == 0)
			return 0;

		// 如果分配空间的长度大于ALLOT_MAX_SIZE， 则直接到系统得堆上进行分配空间
		if( size > ALLOT_MAX_SIZE )
		{
			// 分配空间并添加自定义控制信息
			void * pMemory = ::malloc(size + MEM_HEAD_LENGTH);
			if (NULL != pMemory)
			{
				::memcpy(pMemory, &size, sizeof(unsigned int));
				*((int*)pMemory+1) = 1;

				pMemory = (BYTE*)pMemory + MEM_HEAD_LENGTH;
			}

			return pMemory;
		}

		// 否则，到对应的空闲表中分配空间
		//if( NULL == m_pFreeTable[size/HASH_WDITH] )
		int index = (size-1)>>3;
		if( NULL == m_pFreeTable[index] )
		{
			m_pFreeTable[index] = new FreeList((index+1)<<3);
			if (!m_pFreeTable[index])
				return NULL;
		}

		return m_pFreeTable[index]->Allot();
	}
	bool	CMemoryPool::Free(void* pmem)
	{
		if(!pmem) 
			return true;

		//得到保存在内存空间开头的空间大小数字
		int *pBegin = (int *)((BYTE *)pmem - MEM_HEAD_LENGTH);	

		assert(pBegin>0);
		// 如果分配空间的长度大于ALLOT_MAX_SIZE， 则直接将空间释放到堆上
		if(*pBegin > ALLOT_MAX_SIZE )
		{
			//pSize已经指向内存的起始地址
			::free(pBegin);
			return true;
		}
		int index = (*pBegin - 1)>>3;
		// 否则，将内存回收到对应的空闲表
		if( NULL != m_pFreeTable[index] )
		{
			return m_pFreeTable[index]->Free(pmem);
		}
		// 做出错处理
		else
		{
			throw MemOverException();
		}
		return true;
	}

	/*----------------------------------*/
	void*	Allot(unsigned int size)
	{
		return memPool.Allot(size);
	}
	void	Free(void * p)
	{
		if (NULL != p)
		{
			memPool.Free(p);
		}
	}

	CCriticalSection g_memorypool_cs;
	static CMemoryPool safeMemPool;
	void*	Safe_Allot(unsigned int size)
	{
		MLOCK lock(g_memorypool_cs);
		return safeMemPool.Allot(size);
	}
	void	Safe_Free(void * p)
	{
		if (NULL != p)
		{
			MLOCK lock(g_memorypool_cs);
			safeMemPool.Free(p);
		}
	}	

}// end of namespace