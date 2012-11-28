/*
** Author:	邓超
** QQ:		23427470
** Time:	

** Fly Game 2
** Copyright (C) 2005-2007, Fantasy Games
** Kernel functions
*/

#pragma once
#include <fgeutil.h>
#include <fgeIdAssigner.h>

#include <assert.h>

namespace fge
{
	template<typename T>
	class CIndexArray
	{
	public:
		//构造函数
		CIndexArray()
		{	
			m_pData = NULL;
			m_pId = NULL;
			m_count = 0;
			m_size = 0;
		}
		virtual ~CIndexArray(void)
		{
			SAFE_DELETE(m_pId);
			SAFE_DELETE_ARRAY(m_pData);
			
			m_size = 0;
			m_count = 0;
		}

		/*************************************************************************************
		* 函数功能 ：索引数组初始化
		* 参    数 ：const T& DefaultValue	in 传入一个错误默认值，如果出现错误等，返回这个默认的值
		*			 int size				in 开辟数组大小，值等于欲分配的最大ID号
		* 返 回 值 ：成功返回真，失败返回假
		***************************************************************************************/
		bool	Init( const T& DefaultValue, int size)
		{
			if(size<=0)
				return false;

			SAFE_DELETE(m_pId);
			SAFE_DELETE_ARRAY(m_pData);			

			m_size = size;	
			m_count = 0;
			m_pId = new CIdAssigner();					
			m_pData = new T[m_size+1];
			if(!m_pId || !m_pData)
				return false;

			m_pData[0] = DefaultValue;	

			return m_pId->Init(m_size);;
		}

		/*************************************************************************************
		* 函数功能 ：重载下标运算，能通过[]直接偏移到相应的元素位置。
		* 参    数 ：index	in 欲查找的元素下标
		* 返 回 值 ：T& 
		* 异常	   ：无。
		* 注意	   ：此操作不保证多线程安全。
		***************************************************************************************/
		T& operator[](int index)
		{			
			assert(index>0&&index<=m_size);
			return m_pData[index];
		}

		/*************************************************************************************
		* 函数功能 ：将数组全部置为可用，（对上层调用者也即将数组内数据全部清空）
		***************************************************************************************/
		void	Clear()
		{			
			if(m_pId)
				m_pId->Clear();
			m_count = 0;
		}

		/*************************************************************************************
		* 函数功能 ：判断是否满
		* 参    数 ：
		* 返 回 值 ：bool
		***************************************************************************************/
		bool	IsFull()
		{
			return m_count >= m_size;
		}
		/*************************************************************************************
		* 函数功能 ：添加元素,返回位置.
		* 参    数 ：
		* 返 回 值 ：失败返回一个无效索引－NULL_ID
		***************************************************************************************/
		int		Add(const T& t)
		{
			int index = m_pId->Pop();
			if(index == NULL_ID)
				return NULL_ID;
			m_pData[index] = t;	
			m_count++;
			return index;
		}

		/*************************************************************************************
		* 函数功能 ：将元素添加到索引中的指定位置（重载add函数）
		* 参    数 ：
		* 返 回 值 ：失败返回一个无效索引－NULL_ID
		***************************************************************************************/
		int		Add(const T& t,int index)
		{
			if(index<=0)
				return NULL_ID;
			if( m_pId->Pop(index) )
				m_pData[index] = t;	
			else
				return NULL_ID;
			m_count++;
			return index;
		}

		/*************************************************************************************
		* 函数功能 ：将指定下标位置的对象从索引数组中删除，将ID分配号回收回去，将元素置为默认值
		* 参    数 ：
		* 返 回 值 ：失败返回一个无效索引－NULL_ID
		***************************************************************************************/
		int		Del(int index)
		{
			if(m_pId->Push(index))
			{		
				m_count--;
				m_pData[index] = m_pData[0];
				return index;
			}
			return 0;
		}
		/*************************************************************************************
		* 函数功能 ：返回已使用元素的总共个数
		**************************************************************************************/
		int		GetUesedSize()
		{
			return m_count;
		}

		/*************************************************************************************
		* 函数功能 ：返回整个数组的大小
		**************************************************************************************/
		int		GetArraySize()
		{		
			return m_size;
		}		

		/*************************************************************************************
		* 函数功能 ：判断指定的下标数组是否使用
		**************************************************************************************/
		bool	IsAllot(int index)
		{
			return m_pId->IsAllot(index);
		}


		/*************************************************************************************
		* 函数功能 ：获得一个ID,成功返回大于0的整数,失败返回0
		**************************************************************************************/
		int		GetID( )
		{
			return m_pId->Pop();
		}

		void	MoveToFront( int index )
		{
			m_pId->MoveToFront(index);
		}
		void	MoveToBack( int index )
		{
			m_pId->MoveToBack(index);
		}
	public:

		/*************************************************************************************
		* 类功能 ：嵌套类。为索引数组使用的遍历器.（基本上引用索引分配器中遍历器中功能）
		***************************************************************************************/
		class Iterator
		{
		private:
			CIndexArray<T>* pIndex;
			CIdAssigner::Iterator it;	
		public:
			Iterator( )
			{
			}

			Iterator(CIndexArray<T> *index)
			{
				assert(index!=NULL);
				pIndex = index;
				it = pIndex->m_pId->GetIterator();
			}

			const T&	get()			{ return (*pIndex)[it.get()]; }				
			bool		isdone()		{ return it.isdone(); }
			void		next()			{ it.next(); }
			void		previous()		{ it.previous(); }
			int			pos()			{ return it.get(); }
		};
		
		Iterator GetIterator()
		{
			return Iterator(this);
		};

	private:
		int			m_size;				//整个数组大小
		T			*m_pData;			//数组数据

		CIdAssigner *m_pId;				//ID分配器
		int			m_count;			//总共已使用节点的总数		
	};

};