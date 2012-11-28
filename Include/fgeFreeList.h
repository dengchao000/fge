/*
*
** Author:	�˳�
** QQ:		23427470
** Mail:    sen.night@hotmail.com
** Time:	2007/12/30
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Vertion:   0x01 (0.1)
** ������	���б�,���ڴ��ʹ��.
*
*/

#pragma once
#include <windows.h>
#include <fgeException.h>
namespace fge
{
	// ���б�ɷ��������ֽڵ��ڴ�ռ�
	// (16384)��СΪ(1024 * 16)
	// MALLOC_MAX_SIZE���Ը���ʵ�ʵ����ɷ�������ֽڿռ䣬���ڴ˿ռ䣬��ֱ���ڶ��Ϸ���ռ�
	const int	ALLOT_MAX_SIZE = 16384;		

	// ���б�Ĺ�ϣ���
	// ������ռ��ڴ˿���ڵ��ڴ��������һ���ڴ�����
	const int   HASH_WDITH = 8;			

	// ���ڴ���з����ȥ���ڴ�Ŀ���ͷ����(��С+�Ƿ��Ѿ�����)
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

		//�ͷ��ڴ�
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