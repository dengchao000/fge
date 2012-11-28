/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2011.6.16
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-2011, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  数据缓存对象和读取对象
**/
#pragma once
namespace fge
{
	class DataBuf
	{
	public:
		DataBuf(void)
		{ 
			m_bLock = false;
			m_buffLen=1024; 
			m_szSendData = (char*)malloc(m_buffLen); 
			ReSet(); 
		}
		DataBuf(int len)
		{
			m_bLock = false;
			m_buffLen=len; 
			m_szSendData = (char*)malloc(m_buffLen); 
			ReSet(); 
		}
		~DataBuf(void)
		{ 
			if(m_szSendData)
				free(m_szSendData);
			m_szSendData = 0;
			m_buffLen = 0;
		}
		int Resize(int size)
		{
			if(m_buffLen>=size)
				return m_buffLen;
			void* pNewMem = realloc(m_szSendData,size);
			if(pNewMem) {
				m_szSendData = (char*)pNewMem;
				m_pSendData = m_szSendData+m_len;
				m_buffLen = size;
			}
			return m_buffLen;
		}

		//并重置缓冲.
		void	Clear()
		{
			if(m_szSendData)
				memset(m_szSendData,0,m_buffLen);
			m_pSendData = m_szSendData;
			m_len = 0;
		}
		void	ReSet()
		{
			m_pSendData = m_szSendData;
			m_len = 0;
		}
		//如果增加缓冲成功,返回当前缓冲区的长度,否则返回0;
		int		Write(const void* body,int len)
		{
			if((m_buffLen - m_len) <len)
			{// 分配内存空间
				int newsize = m_buffLen<<1;
				while((newsize-m_len)<len) newsize<<=1;
				void* pNewMem = realloc(m_szSendData,newsize);
				if(pNewMem) {
					m_szSendData = (char*)pNewMem;
					m_pSendData = m_szSendData+m_len;
					m_buffLen = newsize;
				}
			}
			if((m_buffLen - m_len) >=len)
			{
				memcpy(m_pSendData,body,len);
				m_pSendData+=len;
				m_len+=len;
				return m_len;
			}
			else
				return 0;
		}
		template <class T>
		int		Write(const T& x)
		{
			return Write((char*)&x,sizeof(T));
		}
		//返回当前缓冲区的长度
		int		GetBufferLen(){
			return m_len;
		}
		int		GetLen(){
			return m_len;
		}
		// 得到缓冲区最大长度
		int		GetBufferMaxLen(){
			return m_buffLen;
		}
		//返回缓冲区的头指针
		char*	operator()()
		{
			return m_szSendData;
		}
		char*	Lockptr( ){  m_bLock = true; return m_szSendData;}
		char*	LockBuf(int len)
		{
			if((m_buffLen - m_len) <len)
			{// 分配内存空间
				int newsize = m_buffLen<<1;
				while((newsize-m_len)<len) newsize<<=1;
				void* pNewMem = realloc(m_szSendData,newsize);
				if(pNewMem) {
					m_szSendData = (char*)pNewMem;
					m_pSendData = m_szSendData+m_len;
					m_buffLen = newsize;
				}
			}

			if((m_buffLen - m_len) >=len)
			{
				char* prt = m_pSendData;
				m_pSendData+=len;
				m_len+=len;
				return prt;
			}
			else
				return 0;
		}
		void	Unlock( ){  m_bLock = false; }
		DataBuf& operator<< ( const char* p){ Write(p,(int)strlen(p)); return *this; }
	private:
		char*		m_szSendData;		
		char*		m_pSendData;	//指向当前装数据的字符串的首位置
		int			m_len;			//当前长度
		int			m_buffLen;
		bool		m_bLock;
	};	
	class DataReader
	{
	public:
		DataReader( )
		{
			m_pCurrentStream = m_pStream =""; 
			m_nCurrentSize = m_nStreamLen=0;
		}
		DataReader( const char* pStream, int len )
		{
			m_pCurrentStream = m_pStream = pStream; 
			m_nCurrentSize = m_nStreamLen = len;
		}
		const char*	ReadString( )
		{
			if(!m_pCurrentStream || m_nCurrentSize<=0) return 0;

			const char* p = m_pCurrentStream;
			int size = (int)strlen(p)+1;
			m_pCurrentStream+=size;
			m_nCurrentSize-=size;
			return p;
		}
		int			Read( void* pbuf, int size )
		{
			if(m_nCurrentSize<=0)
				return 0;
			if(size > m_nCurrentSize)
				size = m_nCurrentSize;
			if(pbuf)
				memcpy(pbuf,m_pCurrentStream,size);
			m_pCurrentStream+=size;
			m_nCurrentSize-=size;

			return size;
		}
		const void* ReadSome( int size )
		{
			if(m_nCurrentSize<=0)
				return 0;
			if(size > m_nCurrentSize)
				size = m_nCurrentSize;

			const void* pbuf = m_pCurrentStream;
			m_pCurrentStream+=size;
			m_nCurrentSize-=size;

			return pbuf;
		}
		template <class T>
		int			Read(const T& x)
		{
			return Read((char*)&x,sizeof(T));
		}
		const char*	ReadLine(std::string& strout)
		{
			strout = "";
			if(!m_pCurrentStream || m_nCurrentSize<=0) return 0;

			const char* p = m_pCurrentStream;
			int size = 0;
			while(*p)
			{
				if(*p == '\r' ||*p == '\n')
					break;
				size++; p++;
			}
			if(!size)
				return 0;
			strout.append(m_pCurrentStream,size);
			while(*p){ if(*p == '\r' ||*p == '\n'){p++;size++;} else break; }
			m_pCurrentStream+=size;
			m_nCurrentSize-=size;
			return strout.c_str();
		}
		int			GetStreamSize( ){ return m_nStreamLen; }
		int			GetCurrentSize( ){ return m_nCurrentSize; }
		const char*	GetCurrentStream( ){ return m_pCurrentStream; }
		void		Reset(char* pStream, int len)
		{
			m_pCurrentStream = m_pStream = pStream; 
			m_nCurrentSize = m_nStreamLen = len;
		}
		void		Reset( )
		{
			m_pCurrentStream = m_pStream; 
			m_nCurrentSize = m_nStreamLen;
		}
	private:
		const char*	m_pStream;
		const char*	m_pCurrentStream;
		int			m_nStreamLen;
		int			m_nCurrentSize;
	};	
	typedef DataReader ByteStream;
}