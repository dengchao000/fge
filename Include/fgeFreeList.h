/*
*
** Author:	邓超
** QQ:		23427470
** Mail:    sen.night@hotmail.com
** Time:	2007/12/30
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Vertion:   0x01 (0.1)
** 描述：	空闲表,供内存池使用.
*
*/

#pragma once
#include <windows.h>
#include <fgeException.h>
namespace fge
{
	// 空闲表可分配的最大字节的内存空间
	// (16384)大小为(1024 * 16)
	// MALLOC_MAX_SIZE可以根据实际调整可分配最大字节空间，大于此空间，则直接在堆上分配空间
	const int	ALLOT_MAX_SIZE = 16384;		

	// 空闲表的哈希宽度
	// 在申请空间在此宽度内的内存均保存在一个内存链中
	const int   HASH_WDITH = 8;			

	// 从内存池中分配出去的内存的控制头长度(大小+是否已经分配)
	const int   MEM_HEAD_LENGTH	= (4+4);  

	struct FreeNode
	{
		void* pMemory;
		FreeNode* pPrevious;
		FreeNode* pNext;
	};

	class FreeList
	{
	public:
		FreeList(size_t	  _size);
		~FreeList(void);

	public:
		void*		Allot( );
		bool		Free(void *peme);

		//释放内存
		void		Release() {}
	private:
		FreeNode	*Available;
		//FreeNode *UnAvailable;
		FreeNode	NodeList;
		FreeNode	BlockList;
		int			Size;
		int			nBlock;
	};
}// end of namespace