/**
** Author:	µË³¬
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  °æ±¾	 :	
*  ÃèÊö  :  Êý×é
**/

#pragma once
namespace fge
{
	template<class T>
	class CArray
	{
	private:
		T*	m_data;
		size_t m_size;
		T	m_invalidValue;
	private:
		CArray&	operator=( const CArray<T>& ){ return *this; }
		CArray( const CArray<T>& ){}
	public:
		CArray(void){	
			m_size = 0;
			m_data = NULL;
			m_invalidValue = T( );
		}
		CArray(size_t size){		
			m_size = size;
			m_data = new T[size];			
		}

		virtual ~CArray(void){
			//Clear();
			if(m_data != NULL)
				delete[] m_data;
			m_data = NULL;
			m_size = 0;
		}

		void	SetInvalidValue(const T& v)
		{
			m_invalidValue = v;
		}

		void	SetSize(size_t size){
			if(m_data != NULL)
				delete[] m_data;
			m_size = size;		
			m_data = new T[size];			
		}
		//void	Clear()
		//{			
		//	if(m_data != NULL)
		//		delete[] m_data;
		//	m_data = NULL;
		//	m_size = 0;
		//}	
		T& operator[](int pos){
			if(pos < (int)m_size && pos >=0)
				return m_data[pos];
			else
				return m_invalidValue;
		}

		size_t	Size() {
			return m_size;
		}
		//public:
		//	//µü´úÆ÷
		//	class Iterator
		//	{
		//	public:
		//		Iterator(CArray<T>* p)		{ m_pos = 0; m_pArray = p; }
		//		~Iterator(void)				{ m_pos = 0; m_pArray = NULL; }

		//		 T& get()					{ return m_pArray->Read(m_pos); }
		//		 void next()				{ m_pos++; }
		//		 bool isdone()				{ return m_pos >= m_pArray->Size() || m_pos<0; }
		//		 void previous()			{ m_pos--;}
		//	private:
		//		int m_pos;
		//		CArray<T> *m_pArray;
		//	};
		//	Iterator iterator()
		//	{			
		//		return Iterator(this);
		//	};	
	};
};