/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2006年4月19日
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  循环队列
**/

#pragma once

#include <fgeutil.h>
namespace fge
{
    //循环队列
    template <typename type>
    class CCycQueue
    {
    public:
        CCycQueue()
        {
            m_size = 0;
            m_data = NULL;
            m_head = m_last = 0;
        }		
        virtual ~CCycQueue()
        {
            SAFE_DELETE_ARRAY(m_data);
            m_data = NULL;
        }
    public:
        bool	Init(uint size)
        {
            SAFE_DELETE_ARRAY(m_data);
            m_data = new type[m_size = size];
            m_head = m_last = 0;
            return m_data!=NULL;
        }
        //is null?
        bool	IsNull() { return m_head == m_last; }
        bool	IsFull() { return (m_last+1)%m_size == m_head;}

        uint	GetHead(){ return (m_head+1)%m_size; }
        uint	GetLast(){ return m_last; }
        uint	GetSize(){ return m_size; }

        void	Clear()	 { m_head = m_last = 0;}
        //push from last of queue;
        void	Push(type& data)
        {
            if(IsFull())
                m_head = (m_head+1)%m_size;//throw exception("队列已满!");
            m_last = (m_last+1)%m_size;
            m_data[m_last] = data;			
        }
        //pop data from head of queue;
        type&	Pop()
        {
            if(IsNull())
                throw exception("队列为空!");
            m_head = (m_head+1)%m_size;
            return m_data[m_head];			
        }
		type&	Top()
		{
			if(IsNull())
				throw exception("队列为空!");
			uint head = (m_head+1)%m_size;
			return m_data[head];			
		}
        type& operator[](uint pos)
        {
            if(pos<m_size)
                return m_data[pos];
            else
                throw exception("[]越界!");
        }
    private:	
        type			*m_data;
        //the queue is null when m_head == m_last, 
        //and thie queue is full when m_last+1 == m_head;
        uint			m_head;
        uint			m_last;
        uint			m_size;
    };
}