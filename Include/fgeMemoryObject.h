/*
*
** Author:	�˳�
** QQ:		23427470
** Mail:    dengchao000@gmail.com
** Time:	2007/12/30
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Vertion: 
** ������	�ڴ����,����new �� delete,���ڴ���з����ڴ�,���дӴ˼̳е��಻������Ϊȫ�ֱ���,��Ϊ���޷���֤ȫ�ֱ���ɾ��֮ǰ�ڴ�ػ�������.
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

	//�̰߳�ȫ���ڴ�ض���
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