/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  �汾	 :	1.0
*  ����  :  INI�����ļ���������
**/

#pragma once
#include <fgeutil.h>
//#include <..\MemPool\MemoryObject.h>
namespace fge
{
#define NULL_ID  0
	/************************************************
	* �๦�� ������ͻ���ID��
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
			//��Ϊ���Ǵ�1��ʼ��ID��������������+1
			m_num = id.m_num;
			m_pIds = new _ID[m_num+1];
			if(m_pIds)
			{
				::memcpy(m_pIds,id.m_pIds,(m_num+1)*sizeof(_ID));
			}
			m_available = id.m_available;
		}

		/************************************************
		����:	��ʼ��������,������ʾ����
		����:	[IN] num:int ID������,����ʱ��1-num����.
		����ֵ: bool, �ɹ�����true,ʧ�ܷ���false;
		�쳣:   
		************************************************/
		bool	Init(int num)
		{			
			SAFE_DELETE_ARRAY(m_pIds);
			//��Ϊ���Ǵ�1��ʼ��ID��������������+1
			m_num = ++num;			
			m_pIds = new _ID[m_num+1];
			if(!m_pIds)
				return false;
			Clear();			
			return true;
		}

		/************************************************
		����:	�������ID
		����:	��
		����ֵ: void
		�쳣:   
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
		����:	����һ��ϵͳ�Զ�Ĭ�ϵ�δ�����ID
		����:	
		����ֵ: int, �ɹ�����>0��ID,ʧ�ܷ���NULL_ID;
		�쳣:   
		************************************************/
		int		Pop()
		{
			//���
			if( m_available >= m_num )
				return NULL_ID; 

			int id = m_available;
			m_pIds[id].enable = false;
			m_available = m_pIds[m_available].next;

			return id;
		}


		/************************************************
		����:	����һ��ָ��ID��
		����:	
		����ֵ: ���id�Ѿ��������,�򷵻ؼ�,��֮������Ϊ��.
		�쳣:   
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
		����:	����һ���ѷ����ID
		����:	
		����ֵ: �ɹ�Ϊtrue;ʧ��Ϊfalse;
		�쳣:   
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
		����:	��һ���Ѿ������ID�ƶ�����ǰ
		����:	
		����ֵ: �ɹ�Ϊtrue;ʧ��Ϊfalse;
		�쳣:   
		************************************************/	
		bool	MoveToFront(int id)
		{
			if( id<1 || id>=m_num ) return false;
			if(m_pIds[id].enable ) return false;
			if(m_pIds[id].last == 0 ) return true;

			//�Ӷ�����ɾ��
			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			//���뵽ǰ��
			m_pIds[id].next = m_pIds[0].next;
			m_pIds[id].last = 0;

			m_pIds[m_pIds[0].next].last = id;
			m_pIds[0].next = id;			
			return true;
		}
		/************************************************
		����:	��һ���Ѿ������ID�ƶ����Ѿ����������
		����:	
		����ֵ: �ɹ�Ϊtrue;ʧ��Ϊfalse;
		�쳣:   
		************************************************/	
		bool	MoveToBack(int id)
		{
			if( id<1 || id>=m_num ) return false;
			if(m_pIds[id].enable ) return false;
			if(m_pIds[id].next == m_available ) return true;

			//�Ӷ�����ɾ��
			m_pIds[m_pIds[id].last].next = m_pIds[id].next;
			m_pIds[m_pIds[id].next].last = m_pIds[id].last;

			//���뵽ǰ��
			m_pIds[id].next = m_available;
			m_pIds[id].last = m_pIds[m_available].last;

			m_pIds[m_pIds[m_available].last].next = id;
			m_pIds[m_available].last = id;	
			return true;
		}

		/************************************************
		����:	�ж�һ��ID�Ƿ�ʹ��
		����:	
		����ֵ: ���id�Ѿ��������,�򷵻ؼ�,��֮������Ϊ��.
		�쳣:   
		************************************************/	
		bool	IsAllot(int id)
		{
			if( id<=0 || id>=m_num ) return false;
			return !m_pIds[id].enable; 
		}

		/************************************************
		����:	�õ�������������
		����:	
		����ֵ: int, ���ط������ֵ�ID������
		�쳣:   
		************************************************/	
		int		Size()		
		{		
			return m_num-1; 
		}


		///************************************************
		//����:	��ʼ����������꣬����ǰ������ָ��ָ���ѷ���ID����ı�ͷ
		//����:	
		//����ֵ: int, ���ط������ֵ�ID������
		//�쳣:   
		//************************************************/	
		//void		InitCursor()
		//{
		//	m_currentItPos = m_pIds[0].next;
		//}

		///************************************************
		//����:	��õ�ǰ��꣬�õ���ǰ������ָ��ָ����ѷ����ID
		//����:	
		//����ֵ: int,��������굱ǰλ��
		//�쳣:   
		//************************************************/	
		//int			GetCursor()
		//{		
		//	return m_currentItPos;		
		//}

		///************************************************
		//����:	�Ƿ��꣬�жϵ�ǰ������ָ���Ƿ����ѷ���ID��������
		//����:	
		//����ֵ: bool,���ص�ǰ������ָ���Ƿ����ѷ���ID��������
		//�쳣:   
		//************************************************/			
		//bool		Isdone()
		//{		
		//	return m_pIds[m_currentItPos].enable;
		//}

		///************************************************
		//����:	����ǰ������ָ��ָ����һ���ѷ���ID�����Ԫ��
		//����:	
		//����ֵ: 
		//�쳣:   
		//************************************************/			
		//void		NextCursor()
		//{		
		//	m_currentItPos = m_pIds[m_currentItPos].next; 
		//}

		///************************************************
		//����:	����ǰ������ָ��ָ����һ���ѷ���ID�����Ԫ��
		//����:	
		//����ֵ: 
		//�쳣:   
		//************************************************/	
		//void		PreviousCursor()
		//{
		//	m_currentItPos = m_pIds[m_currentItPos].last; 
		//}

	public:
		/*************************************************************************************
		* �๦�� ��Ƕ���ࡣΪID������ʹ�õı�����.
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
		//��������ģ������Ľṹ��
		struct _ID/* : public CMemoryObjectThreadSafe*/
		{
			int64 last:30;
			int64 enable:4;
			int64 next:30;
		};

		_ID		*m_pIds;			//���õ�id����������
		int		m_available;		//ָ���һ��δ�����id����Ϊm_unavailable����ָ��0λ�ã�������������0λ�ö�Ϊ�ѷ���ָ��λ��
		int		m_num;				//id����
		//int	m_currentItPos;		//ָ��ǰ����λ��(�Ѿ������ȥ��id).��ǰ����λ��ָ��
	};
};