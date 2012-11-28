/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  INI配置文件操作对象
**/

#pragma once
#include <fgeutil.h>
//#include <..\MemPool\MemoryObject.h>
namespace fge
{
#define NULL_ID  0
	/************************************************
	* 类功能 ：分配和回收ID号
	************************************************/
	class CIdAssigner/* : public CMemoryObjectThreadSafe*/
	{		
	public:
		CIdAssigner()
		{
			m_pIds = NULL;
			m_num = 0;
		}

		~CIdAssigner(void)
		{
			SAFE_DELETE_ARRAY(m_pIds);
		}

		CIdAssigner(const CIdAssigner& id) 
		{
			//因为我们从1开始算ID，所以数组总数+1
			m_num = id.m_num;
			m_pIds = new _ID[m_num+1];
			if(m_pIds)
			{
				::memcpy(m_pIds,id.m_pIds,(m_num+1)*sizeof(_ID));
			}
			m_available = id.m_available;
		}

		/************************************************
		功能:	初始化分配器,必须显示调用
		参数:	[IN] num:int ID号数量,分配时从1-num分配.
		返回值: bool, 成功返回true,失败返回false;
		异常:   
		************************************************/
		bool	Init(int num)
		{			
			SAFE_DELETE_ARRAY(m_pIds);
			//因为我们从1开始算ID，所以数组总数+1
			m_num = ++num;			
			m_pIds = new _ID[m_num+1];
			if(!m_pIds)
				return false;
			Clear();			
			return true;
		}

		/************************************************
		功能:	清空所有ID
		参数:	无
		返回值: void
		异常:   
		************************************************/
		void	Clear()
		{
			if(!m_pIds)
				return;
			m_available = 1;

			for(int i=1; i<=m_num; i++)
			{
				m_pIds[i].enable = true;
				m_pIds[i].next = i+1;
				m_pIds[i].last = i-1;
			}
			m_pIds[0].last = 0;
			m_pIds[0].next = 1;
			m_pIds[0].enable = false;
			m_pIds[m_num].next = m_num;
		}

		/************************************************
		功能:	弹出一个系统自动默认的未分配的ID
		参数:	
		返回值: int, 成功返回>0的ID,失败返回NULL_ID;
		异常:   
		************************************************/
		int		Pop()
		{
			//检查
			if( m_available >= m_num )
				return NULL_ID; 

			int id = m_available;
			m_pIds[id].enable = false;
			m_available = m_pIds[m_available].next;

			return id;
		}


		/************************************************
		功能:	分配一个指定ID号
		参数:	
		返回值: 如果id已经分配过了,则返回假,反之返回这为真.
		异常:   
		************************************************/		
		bool	Pop(int id)
		{
			if(id==m_available) return Pop()>0;
			if(!id) return false;
			if(id>=m_num) return false;
			if(!m_pIds[id].enable) return false;

			m_pIds[id].enable = false;

			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			m_pIds[id].next = m_available;
			m_pIds[id].last = m_pIds[m_available].last;		
			m_pIds[m_pIds[m_available].last].next = id;
			m_pIds[m_available].last = id;

			return true;
		}

		/************************************************
		功能:	放入一个已分配的ID
		参数:	
		返回值: 成功为true;失败为false;
		异常:   
		************************************************/		
		bool	Push(int id)
		{
			//check enable;
			if( id<=0 || id>=m_num ) return false;
			if( m_pIds[id].enable ) return false;
			//
			//if( id == m_currentItPos )
			//{
			//	m_currentItPos = m_pIds[m_currentItPos].last;
			//}
			//
			m_pIds[id].enable = true;

			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			m_pIds[id].next = m_num;
			m_pIds[id].last = m_pIds[m_num].last;
			m_pIds[m_pIds[m_num].last].next = id;
			m_pIds[m_num].last = id;	

			if(m_available==m_num)
				m_available = id;

			return true;
		}

		/************************************************
		功能:	将一个已经分配的ID移动到最前
		参数:	
		返回值: 成功为true;失败为false;
		异常:   
		************************************************/	
		bool	MoveToFront(int id)
		{
			if( id<1 || id>=m_num ) return false;
			if(m_pIds[id].enable ) return false;
			if(m_pIds[id].last == 0 ) return true;

			//从队列中删除
			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			//插入到前端
			m_pIds[id].next = m_pIds[0].next;
			m_pIds[id].last = 0;

			m_pIds[m_pIds[0].next].last = id;
			m_pIds[0].next = id;			
			return true;
		}
		/************************************************
		功能:	将一个已经分配的ID移动到已经分配表的最后
		参数:	
		返回值: 成功为true;失败为false;
		异常:   
		************************************************/	
		bool	MoveToBack(int id)
		{
			if( id<1 || id>=m_num ) return false;
			if(m_pIds[id].enable ) return false;
			if(m_pIds[id].next == m_available ) return true;

			//从队列中删除
			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			//插入到前端
			m_pIds[id].next = m_available;
			m_pIds[id].last = m_pIds[m_available].last;

			m_pIds[m_pIds[m_available].last].next = id;
			m_pIds[m_available].last = id;	
			return true;
		}

		/************************************************
		功能:	判断一个ID是否使用
		参数:	
		返回值: 如果id已经分配过了,则返回假,反之返回这为真.
		异常:   
		************************************************/	
		bool	IsAllot(int id)
		{
			if( id<=0 || id>=m_num ) return false;
			return !m_pIds[id].enable; 
		}

		/************************************************
		功能:	得到分配器的总数
		参数:	
		返回值: int, 返回分配器分的ID的总数
		异常:   
		************************************************/	
		int		Size()		
		{		
			return m_num-1; 
		}


		///************************************************
		//功能:	初始化迭代器光标，将当前迭代器指针指向已分配ID链表的表头
		//参数:	
		//返回值: int, 返回分配器分的ID的总数
		//异常:   
		//************************************************/	
		//void		InitCursor()
		//{
		//	m_currentItPos = m_pIds[0].next;
		//}

		///************************************************
		//功能:	获得当前光标，得到当前迭代器指针指向的已分配得ID
		//参数:	
		//返回值: int,迭代器光标当前位置
		//异常:   
		//************************************************/	
		//int			GetCursor()
		//{		
		//	return m_currentItPos;		
		//}

		///************************************************
		//功能:	是否完，判断当前迭代器指针是否到了已分配ID链表的最后
		//参数:	
		//返回值: bool,返回当前迭代器指针是否到了已分配ID链表的最后
		//异常:   
		//************************************************/			
		//bool		Isdone()
		//{		
		//	return m_pIds[m_currentItPos].enable;
		//}

		///************************************************
		//功能:	将当前迭代器指针指向下一个已分配ID链表的元素
		//参数:	
		//返回值: 
		//异常:   
		//************************************************/			
		//void		NextCursor()
		//{		
		//	m_currentItPos = m_pIds[m_currentItPos].next; 
		//}

		///************************************************
		//功能:	将当前迭代器指针指向上一个已分配ID链表的元素
		//参数:	
		//返回值: 
		//异常:   
		//************************************************/	
		//void		PreviousCursor()
		//{
		//	m_currentItPos = m_pIds[m_currentItPos].last; 
		//}

	public:
		/*************************************************************************************
		* 类功能 ：嵌套类。为ID分配器使用的遍历器.
		***************************************************************************************/
		class	Iterator
		{
		private:
			CIdAssigner* _padapter;		
			int			 _pos,_next,_last;
		public:
			Iterator()
			{
				_padapter = NULL;
			}
			Iterator(CIdAssigner *adapter)
			{
				_padapter = adapter; 
				if(_padapter->m_pIds)
				{
					_pos = _padapter->m_pIds[0].next;
					_next = _padapter->m_pIds[_pos].next;
					_last = _padapter->m_pIds[_pos].last;
				}
				else
				{
					_pos = _next = _last = 0;
				}
			}

			int		get()			{ return _pos; }
			bool	isdone()		{ return _pos == _next || _padapter->m_pIds[_pos].enable;}
			void	next()
			{ 
				_pos = _next; 
				_next = _padapter->m_pIds[_pos].next;
				_last = _padapter->m_pIds[_pos].last;
			}
			void	previous()
			{
				_pos = _last; 
				_next = _padapter->m_pIds[_pos].next;
				_last = _padapter->m_pIds[_pos].last;
			}
		};
	public:
		Iterator	GetIterator()
		{
			return Iterator(this);
		};
	private:
		//利用数组模拟链表的结构体
		struct _ID/* : public CMemoryObjectThreadSafe*/
		{
			int64 last:30;
			int64 enable:4;
			int64 next:30;
		};

		_ID		*m_pIds;			//可用的id号数组数据
		int		m_available;		//指向第一个未分配的id，因为m_unavailable总是指向0位置，所以我们总以0位置定为已分配指针位置
		int		m_num;				//id总数
		//int	m_currentItPos;		//指向当前遍历位置(已经分配出去的id).当前迭代位置指针
	};
};