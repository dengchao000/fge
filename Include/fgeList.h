//作者: 邓超
//日期: 2006-7-1


#pragma once
#include <fgeMemoryObject.h>
namespace fge
{
	template< class T, class Mem = fge::CMemoryObjectThreadSafe >
	class List : public Mem
	{
	public:
		struct Node : public Mem
		{
			T			data;
			Node		*next;
			Node		*previous;
		};
	public:
		//迭代器
		class ListIterator/* :public Iterator<T>*/
		{
		public:
			ListIterator()					{ m_pdata = 0; }
			ListIterator(Node* node)		{ m_pdata = node; }
			void	Pointto(Node* node)		{ m_pdata = node; }
			~ListIterator(void)				{ m_pdata = 0; }

			T&		get()					{ if(!m_pdata) throw std::exception("迭代器空."); return m_pdata->data; }
			void	next()					{ m_pdata = m_pdata->next; }
			void	previous()				{ m_pdata = m_pdata->previous; }
			bool	isdone()				{ return m_pdata == 0; }
			Node*	getNodePtr()			{ return m_pdata; }
		private:
			Node* m_pdata;
		};

		//
	private:
		Node	*m_pHead;					// head pointer
		Node	*m_pTrail;					// trail pointer
		unsigned int m_count;
		ListIterator m_it;
	private:
		
		List( const List<T>& ){}
	public:
		List(void)
		{
			m_pHead = NULL;
			m_pTrail = NULL;
			m_count = 0;
		}
		virtual ~List(void)
		{
			Clear();			
		}
		List&	operator=( List<T>& l )
		{
			m_pHead = NULL;
			m_pTrail = NULL;
			m_count = 0;

			Iterator<T>* pi = l.iterator();
			while( !pi->isdone() )
			{
				this->PushBack(pi->get());
				pi->next();
			}
			return *this;
		}
		//在表头增加结点:
		Node*	PushFront(Node *p)
		{
			p->previous = NULL;
			p->next = m_pHead;
			
			if(m_pHead)
				m_pHead->previous = p;
			else
				m_pTrail = p;
			
			m_pHead = p;
			m_count++;
			return p;
		}
		//在表末增加结点:
		Node*	PushBack(Node *p)
		{
			p->next = NULL;
			p->previous = m_pTrail;

			if(m_pTrail)
				m_pTrail->next = p;
			else
				m_pHead = p;	

			m_pTrail = p;
			m_count++;

			return p;
		}

			//在表头增加结点:
		Node*	PushFront(const T &t)
		{
			Node *p = new Node;
			p->data = t;

			return PushFront(p);
		}
		//在表末增加结点:
		Node*	PushBack(const T &t)
		{
			Node *p = new Node;
			p->data = t;
			return PushBack(p);
		}

		//插入
		void	Insert(Node *where, const T &t)
		{
			Node *p = new Node;
			p->data = t;
			p->next = where->next;
			p->previous = where;
			
			if(where->next)
				where->next->previous = p;
			where->next = p;
			if(m_pTrail == where)
				m_pTrail = p;
		}
		//移出
		Node*	RemoveNode(Node* node)
		{
			if(node == m_pHead)
				m_pHead = node->next;
			if(node == m_pTrail)
				m_pTrail = node->previous;
			if(node->previous)
				node->previous->next = node->next;
			if(node->next)
				node->next->previous = node->previous;				

			node->previous = node->next = NULL;
			m_count--;
			return node;
		}
		Node*	RemoveFrontNode()
		{
			return RemoveNode(m_pHead);
		}
		Node*	RemoveBackNode()
		{
			return RemoveNode(m_pTrail);
		}

		const T&		Remove(const T &t)
		{
			Node *pTemp = m_pHead;

			//find:
			while(pTemp != NULL)
			{
				if(pTemp->data == t)
					break;
				else
					pTemp = pTemp->next;
			}
			if(!pTemp) return t;

			RemoveNode(pTemp);
			//return;			
			delete pTemp;
			return t;				
		}
		void	Remove(Node* node)
		{
			if(node != 0)
			{
				RemoveNode(node);
				//return;			
				delete node;		
			}
		}

		void	Remove(ListIterator *where)
		{
			Remove(where->getNodePtr());
		}

		//清出
		void	Clear()
		{
			Node *pTemp = NULL;	
			while(m_pHead)
			{
				pTemp = m_pHead;
				m_pHead = m_pHead->next;

				delete pTemp;
				pTemp = NULL;				
			}
			m_pTrail = m_pHead = NULL;
			m_count = 0;
		}
		//统计
		unsigned int Count() const
		{
			return m_count; 
		}

		//移动
		void	MoveToFront(const T &t)
		{
			Node *pTemp = m_pHead;

			//find:
			while(pTemp != NULL)
			{
				if(pTemp->data == t)
					break;
				else
					pTemp = pTemp->next;
			}
			if(!pTemp) return;			
			if(pTemp == m_pHead)
				return;

			//remove node;
			pTemp->previous->next = pTemp->next;
			if(pTemp->next)
				pTemp->next->previous = pTemp->previous;				
			else		
				m_pTrail = pTemp->previous;

			//push to front:
			pTemp->previous = NULL;
			pTemp->next = m_pHead;
			m_pHead->previous = pTemp
			m_pHead = pTemp;
		}
		void	MoveToBack(const T &t)
		{
			Node *pTemp = m_pHead;

			//find:
			while(pTemp != NULL)
			{
				if(pTemp->data == t)
					break;
				else
					pTemp = pTemp->next;
			}
			if(!pTemp) return;			
			if(pTemp == m_pTrail)
				return;

			//remove node;
			pTemp->next->previous = pTemp->previous;
			if(pTemp->previous)
				pTemp->previous->next = pTemp->next;				
			else		
				m_pHead = pTemp->next;

			//push to back:
			pTemp->previous = m_pTrail;
			pTemp->next = NULL;
			m_pTrail->next = pTemp;
			m_pTrail = pTemp;
		}

	
		Node*	GetHead()	{ return m_pHead; }
		Node*	GetTail()	{ return m_pTrail; }
	public:
		ListIterator& iterator()
		{
			m_it.Pointto(m_pHead);
			return m_it;
		};	
		ListIterator& begin()
		{
			m_it.Pointto(m_pHead);
			return m_it;
		};
		ListIterator& end()
		{
			m_it.Pointto(m_pTrail);
			return m_it;
		};
	};

};