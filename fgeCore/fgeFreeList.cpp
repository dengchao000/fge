#include "StdAfx.h"
#include "fgeFreeList.h"
#include "fgelog.h"
namespace fge
{

	FreeList::FreeList(size_t _size)
		:Size((int)_size)
	{
		memset(&NodeList,0,sizeof(NodeList));
		memset(&BlockList,0,sizeof(BlockList));

		Available = &NodeList;
		nBlock = ALLOT_MAX_SIZE / Size;
		if(!nBlock)
			nBlock=1;
	}

	FreeList::~FreeList(void)
	{
		FreeNode* pblock = BlockList.pNext;

		pblock = BlockList.pNext;
		while(pblock)
		{
			BlockList.pNext = pblock->pNext;
			delete[] pblock->pMemory;
			delete pblock;
			pblock = BlockList.pNext;
		}
		BlockList.pNext = 0;

		Available = 0;
		pblock = NodeList.pNext;
		while(pblock)
		{
			NodeList.pNext = pblock->pNext;
			if(!pblock->pMemory)
				//throw MemLeakException("内存池中的内存未完全回收");
				CLog::Write("内存池中的内存块未完全回收 Size=%d",Size);
			pblock->pMemory = 0;
			delete pblock;
			pblock = NodeList.pNext;
		}
		NodeList.pNext = 0;
	}
	void* FreeList::Allot( )
	{
		FreeNode* pnd=Available->pNext; 
		if(pnd)
		{
			Available=Available->pNext; 
		}
		else
		{
			////如果分配的内存在128Byte以下,
			//每次分配一大块连续的内存,以免过小的内存分配带来太多损失(操作系统占用一定空间)
			BYTE *pData = new BYTE[nBlock*(Size+MEM_HEAD_LENGTH)];
			if(!pData)
				return 0;

			FreeNode* pBlock = new FreeNode;
			if(!pBlock){
				delete[] pData;
				return 0;
			}
			//初始化大小及分配标志
			int n = 0;
			for(int i=0; i<nBlock; i++)
			{
				n = i*(Size+MEM_HEAD_LENGTH);
				*((int*)(pData+n)) = Size;
				*((int*)(pData+n)+1) = 0;
			}
			//将新开的内存块加入到块表中
			pBlock->pMemory = pData;
			pBlock->pNext = BlockList.pNext;
			pBlock->pPrevious = &BlockList;
			if(BlockList.pNext)
				BlockList.pNext->pPrevious = pBlock;
			BlockList.pNext = pBlock;

			//初始化可分配表
			FreeNode* pAvailable = Available;		
			for(int i=0; i<nBlock; i++)
			{			
				pnd = new FreeNode;
				if(!pnd) return 0;

				n = i*(Size+MEM_HEAD_LENGTH);
				pnd->pMemory = pData+n+MEM_HEAD_LENGTH;

				//将pnewnd插入到分配表中
				pnd->pNext = 0;
				pnd->pPrevious = pAvailable;

				pAvailable->pNext = pnd;
				pAvailable=pnd;
			}

			pnd=Available->pNext; 
			Available=Available->pNext; 
		}

		void* pret = pnd->pMemory;
		*((int*)(pret)-1) = 1;
		pnd->pMemory = 0;
		return pret;
	}
	bool FreeList::Free(void *peme)
	{
		if(!peme)
			return true;
		if(*((int*)peme-1) == 1)
		{			
			FreeNode* pnd=Available->pPrevious; 
			if(pnd == 0)
			{
				//throw MemOverException("可能误将内存以外的回收到池中了");
				CLog::Write("可能误将内存以外的回收到池中了");
				return false;
			}	
			else
			{
				*((int*)peme-1) = 0;
				Available->pMemory = (BYTE*)peme;
				Available = pnd;
			}		
		}
		else
		{
			return false;
		}
		return true;
	}
}