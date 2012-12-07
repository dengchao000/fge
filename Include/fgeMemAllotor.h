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
** 描述：	内存分配器接口
*/
#pragma once
namespace fge
{
	class MemAllotor
	{
	public:
		virtual ~MemAllotor(void){}

		virtual void*	Allot(size_t size) = 0;
		virtual bool	Free(void* pmem) = 0;
	};

	class SystemMemAllotor :public MemAllotor
	{
		virtual ~SystemMemAllotor(void){}
	public:		
		SystemMemAllotor( ) { }

		virtual void*	Allot(size_t size)
		{
			return new unsigned char[size];
		}
		virtual bool	Free(void* pmem)
		{
			if(pmem == 0)
				return true;
			delete[] pmem;
			return true;
		}
	};
}// end of namespace
