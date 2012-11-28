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

		// �������ռ�ĳ��ȴ���ALLOT_MAX_SIZE�� ��ֱ�ӵ�ϵͳ�ö��Ͻ��з���ռ�
		if( size > ALLOT_MAX_SIZE )
		{
			// ����ռ䲢����Զ��������Ϣ
			void * pMemory = ::malloc(size + MEM_HEAD_LENGTH);
			if (NULL != pMemory)
			{
				::memcpy(pMemory, &size, sizeof(unsigned int));
				*((int*)pMemory+1) = 1;

				pMemory = (BYTE*)pMemory + MEM_HEAD_LENGTH;
			}

			return pMemory;
		}

		// ���򣬵���Ӧ�Ŀ��б��з���ռ�
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

		//�õ��������ڴ�ռ俪ͷ�Ŀռ��С����
		int *pBegin = (int *)((BYTE *)pmem - MEM_HEAD_LENGTH);	

		assert(pBegin>0);
		// �������ռ�ĳ��ȴ���ALLOT_MAX_SIZE�� ��ֱ�ӽ��ռ��ͷŵ�����
		if(*pBegin > ALLOT_MAX_SIZE )
		{
			//pSize�Ѿ�ָ���ڴ����ʼ��ַ
			::free(pBegin);
			return true;
		}
		int index = (*pBegin - 1)>>3;
		// ���򣬽��ڴ���յ���Ӧ�Ŀ��б�
		if( NULL != m_pFreeTable[index] )
		{
			return m_pFreeTable[index]->Free(pmem);
		}
		// ��������
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