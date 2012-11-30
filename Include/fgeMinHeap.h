#pragma once
#include <fgedebug.h>
namespace fge
{
	template< typename T >
	class min_heap_t
	{
	private:
		typedef T event_type;
		event_type** p;
		unsigned n, a;	//数量,内存分配量
	public:
		min_heap_t( ){ p = 0; n = 0; a = 0; }
		~min_heap_t( ){ if(p) free(p); }

		int		elem_greater( event_type *a,  event_type *b)	{ return *a > *b; }
		void	elem_init( event_type* e) { e->min_heap_idx = -1; }
		int		empty() { return 0u == n; }
		unsigned	size() { return n; }
		event_type*	top() { return n ? *p : 0; }

		int		push(event_type* e)
		{
			if(reserve(n + 1))
				return -1;
			shift_up_(n++, e);
			return 0;
		}

		event_type*	pop()
		{
			if(n)
			{
				event_type* e = *p;
				shift_down_(0u, p[--n]);
				e->min_heap_idx = -1;
				return e;
			}
			return 0;
		}

		int		erase(event_type* e)
		{
			if(((unsigned int)-1) != e->min_heap_idx)
			{
				event_type *last = p[--n];
				unsigned parent = (e->min_heap_idx - 1) / 2;
				/* we replace e with the last element in the heap.  We might need to
				shift it upward if it is less than its parent, or downward if it is
				greater than one or both its children. Since the children are known
				to be less than the parent, it can't need to shift both up and
				down. */
				if (e->min_heap_idx > 0 && elem_greater(p[parent], last))
					shift_up_(s, e->min_heap_idx, last);
				else
					shift_down_(s, e->min_heap_idx, last);
				e->min_heap_idx = -1;
				return 0;
			}
			return -1;
		}

		int		reserve(unsigned n)
		{
			if(this->a < n)
			{
				event_type** _p=0;
				unsigned _a = a ? a * 2 : 8;
				if(_a < n)
					_a = n;
				if(!(_p = ( event_type**)realloc(this->p, _a * sizeof (*p))))
					return -1;
				this->p = _p;
				this->a = _a;
			}
			return 0;
		}

		void	shift_up_(unsigned hole_index,  event_type* e)
		{
			unsigned parent = (hole_index - 1) / 2;
			while(hole_index && elem_greater(p[parent], e))
			{
				(p[hole_index] = p[parent])->min_heap_idx = hole_index;
				hole_index = parent;
				parent = (hole_index - 1) / 2;
			}
			(p[hole_index] = e)->min_heap_idx = hole_index;
		}

		void	shift_down_(unsigned hole_index,  event_type* e)
		{
			unsigned min_child = 2 * (hole_index + 1);
			while(min_child <= n)
			{
				min_child -= min_child == n || elem_greater(p[min_child], p[min_child - 1]);
				if(!(elem_greater(e, p[min_child])))
					break;
				(p[hole_index] = p[min_child])->min_heap_idx = hole_index;
				hole_index = min_child;
				min_child = 2 * (hole_index + 1);
			}
			shift_up_(hole_index,  e);
		}
		void	clear()
		{
			for( unsigned i=0; i<n; i++ )
			{
				delete p[i];
			}
			n = 0;
		}
	};
}