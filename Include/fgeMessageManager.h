/*
** Author:	邓超
** QQ:		23427470
** Time:	2007-10-19
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
名称：		消息管理器
描述：		游戏服务器中各种服务均以插件的方式提供服务。基类中定义插件的通用接口。
*/

#pragma once
#include <fgeSharedObject.h>
#include <fgeSingleton.h>
#include <fgeMemAllotor.h>
#include <fgeThread.h>
//#define WPARAMSIZE 64
//#define LPARAMSIZE 2048

namespace fge{
	// 消息结构
	struct SMSG
	{
	public:
		DWORD		type;
		DWORD		msg;
		void*		wParam;//[WPARAMSIZE];
		void*		lParam;//[LPARAMSIZE];
		size_t		w_size,l_size;
		SMSG()	
		{ type=0; msg = 0; wParam = 0; lParam = 0; w_size = 0; l_size = 0;}
	};

	class CMessageManager : public SharedObject
	{
	public:
		CMessageManager(void);
		virtual ~CMessageManager(void);	

		//初始化队列长度
		bool	Init( int size,MemAllotor *memAllotor );		
		//向队列中插入条消息,copy pParam的数据.成功返回true,失败返回false.
		bool	Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
		bool	Push(DWORD msg, DWORD wParam,DWORD lParam );
		bool	PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
		bool	PushFront(DWORD msg, DWORD wParam,DWORD lParam );
		//弹出一个消息队列中的一个消息.直到下一次调用时,才会删除当前消息.如果队列为空返回false,如果成功返回为true.
		SMSG*	Pop( );
		void	Clear();
		
		//enum{ MAX_MESSAGE_NUMBER = 10000 };
	private:
		bool	Insert(INT pos,DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	private:
		MemAllotor	*m_memAllotor;				//内存分配器
		SMSG		*m_msgQueue;				//长为MAX_MESSAGE_NUMBER的循环消息队列
		int			m_maxMessageNum;
		int			m_nHead;					//队首指针
		int			m_nLast;					//队尾指针

		//bool		*m_writeing;				//写标志
		FreeLock	m_freeLock;
	};
}