//************************************************************
// 文件名称：HashTable.h
// 摘    要：Hash表
// 参    考：
// 当前版本：1.0
// 作    者：邓超
// 完成日期：2006年8月16日
//************************************************************
#pragma once
#include <vector>
#include <list>
#include <assert.h>
#include <fgestti.h>

namespace fge
{
	//template< typename T >
	//int GetHash( T& data ){
	//	return data.GetHash( );
	//}
	inline int GetHash( const int data ){
		return data;
	}
	inline int GetHash( const unsigned data ){
		return data;
	}
	inline int GetHash( const char* data ){
		return StrHash(String(data));
	}
	inline int GetHash( const std::string& data ){
		return StrHash(String(data));
	}

	template<typename K,typename T>
	class HashTable
	{
	public:
		struct Node{ 
			K key;
			T data;
		};
		typedef std::list<Node> NodeList;
		typedef std::vector< NodeList > HashMap;
	private:
		int					m_size;
		HashMap				m_pdata;
		int					m_count;
	private:
		HashTable&	operator=( const HashTable<K,T>& ){return *this;}
		HashTable( const HashTable<K,T>& ){}
	public:
		HashTable(){
			m_size		= 0;			//表大小
			m_count		= 0;
		}	
		~HashTable(){
			if(m_size > 0)
				Clear();
		}

		bool Init( int size ){
			if(m_size > 0)	Clear();
			m_size = size;
			m_pdata.resize(size);			
			return	true;
		}
		template< typename Ktype >
		bool Insert(const T &data,Ktype key)
		{
			if(m_size==0)	return false;
			int pos = GetHash(key) % m_size;

			Node node;
			node.data = data;
			node.key = key;
			m_pdata[pos].push_back(node);
			m_count++;
			return true;
		}
		template< typename Ktype >
		T* Remove(Ktype key)
		{
			if(m_size==0)	return false;
			int pos = GetHash(key) % m_size;

			NodeList::iterator i = m_pdata[pos].begin();
			while(i!=m_pdata[pos].end()){
				if(key == i->key){
					T * pRt = &i->data;
					m_pdata[pos].erase(i);
					m_count--;
					return pRt;
				}
			}
			return NULL;
		}
		template< typename Ktype >
		T*	 Search(Ktype key)
		{
			if(m_size==0)
				return 0;
			int pos = GetHash(key) % m_size;

			NodeList::iterator i = m_pdata[pos].begin();
			while(i!=m_pdata[pos].end()){
				if(key == i->key){
					return &i->data;
				}
			}
			return NULL;
		}

		void Clear()
		{
			if(m_size > 0)
			{
				cleanUp( typename IsPtr<T>::Result() );
			}
			m_count = 0;
		}
		int  GetSize( ) 
		{
			return m_size;
		}
		int	 Count( )
		{
			return m_count;
		}
		template< class S >
		void	Foreach(S& s){
			HashMap::iterator i=m_pdata.begin();
			while(i!=m_pdata.end())
			{
				NodeList::iterator j=i->begin();
				while( j!=i->end() ){
					s(j->data);
					j++;					
				}
				i++;
			}
		}
	protected:
		void cleanUp( const Yes& y){
			HashMap::iterator i=m_pdata.begin();
			while(i!=m_pdata.end())
			{
				NodeList::iterator j=i->begin();
				while( j!=i->end() ){
					delete j->data;
					j++;					
				}
				i++;
			}
			m_pdata.clear();
		}
		void cleanUp( const No& n){
			m_pdata.clear();
		}
	};
};