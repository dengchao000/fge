#include "StdAfx.h"
#include "fgeMessageManager.h"
#include <assert.h>
#include <fgeutil.h>

using namespace fge;
CMessageManager::CMessageManager(void)
{
	m_maxMessageNum = 0;
	//memset(m_writeing,0,sizeof(m_writeing));
	m_msgQueue = 0;
	//m_writeing = 0;
	m_nLast = m_nHead = 0;
	m_memAllotor = 0;
}

CMessageManager::~CMessageManager(void)
{
	if(m_memAllotor)
	{
		for( int i=0; i<m_maxMessageNum; i++ )
		{
			if(m_msgQueue[i].wParam)
				m_memAllotor->Free( m_msgQueue[i].wParam );
			if(m_msgQueue[i].lParam)
				m_memAllotor->Free( m_msgQueue[i].lParam );
		}
		delete m_memAllotor;
		m_memAllotor = 0;
	}
	SAFE_DELETE( m_msgQueue );
	//SAFE_DELETE( m_writeing );
}
bool CMessageManager::Init( int size, MemAllotor *memAllotor)
{
	Lock<FreeLock> l(m_freeLock);

	if(m_msgQueue)
		return true;
	if( size<=0 )
		return false;

	m_maxMessageNum = size;
	m_msgQueue = new SMSG[m_maxMessageNum];
	//m_writeing = new bool[m_maxMessageNum];
	//memset(m_writeing,0,m_maxMessageNum*sizeof(bool));

	m_nLast = m_nHead = 0;
	m_memAllotor = memAllotor;
	if(!m_memAllotor)
		m_memAllotor = new SystemMemAllotor();
	return true;
}
bool	CMessageManager::PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize)
{
	Lock<FreeLock> l(m_freeLock);

	int newHead = m_nHead>0?m_nHead-1:m_maxMessageNum-1;

	if( newHead == m_nLast )//队满
		return false;

	if(Insert(m_nHead,msg,wParam,wParamSize,lParam,lParamSize))
		m_nHead = newHead;
	else
		return false;

	return true;
}
//向队列中插入条消息,copy pParam的数据.成功返回true,失败返回false.
bool	CMessageManager::Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize)
{
	//assert(wParamSize<WPARAMSIZE && lParamSize<LPARAMSIZE);
	Lock<FreeLock> l(m_freeLock);
	
	int newLast = (m_nLast+1)%m_maxMessageNum;
	if( newLast == m_nHead )//队满
		return false;
	if(Insert(newLast,msg,wParam,wParamSize,lParam,lParamSize))
		m_nLast = newLast;
	else
		return false;

	return true;
}
bool	CMessageManager::Insert(INT pos,DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize)
{
	if(!m_msgQueue)
		return false;
	bool ret = true;		
	m_msgQueue[pos].msg = msg;

	if(wParam && wParamSize){
		if(m_msgQueue[pos].w_size<wParamSize){
			m_memAllotor->Free(m_msgQueue[pos].wParam);
			m_msgQueue[pos].wParam = m_memAllotor->Allot(wParamSize);
			m_msgQueue[pos].w_size = wParamSize;
		}
		else if(m_msgQueue[pos].w_size>1024){
			m_memAllotor->Free(m_msgQueue[pos].wParam);
			m_msgQueue[pos].wParam = m_memAllotor->Allot(wParamSize);
			m_msgQueue[pos].w_size = wParamSize;
		}
		if(m_msgQueue[pos].wParam)
			memcpy(m_msgQueue[pos].wParam,wParam,wParamSize);			
		else
			ret = false;
	}

	if(lParam && lParamSize)
	{
		if(m_msgQueue[pos].l_size<lParamSize){
			m_memAllotor->Free(m_msgQueue[pos].lParam);
			m_msgQueue[pos].lParam = m_memAllotor->Allot(lParamSize);
			m_msgQueue[pos].l_size = lParamSize;
		}
		else if(m_msgQueue[pos].l_size>2048){
			m_memAllotor->Free(m_msgQueue[pos].lParam);
			m_msgQueue[pos].lParam = m_memAllotor->Allot(lParamSize);
			m_msgQueue[pos].l_size = lParamSize;
		}

		if(m_msgQueue[pos].lParam)
			memcpy(m_msgQueue[pos].lParam,lParam,lParamSize);
		else
			ret = false;
	}
	return ret;
}
//弹出一个消息队列中的一个消息.直到下一次调用时,才会删除当前消息.如果队列为空返回false,如果成功返回为true.
SMSG*	CMessageManager::Pop( )
{
	Lock<FreeLock> l(m_freeLock);

	if(m_nHead == m_nLast)//队空
		return NULL;

	int	head = (m_nHead+1)%m_maxMessageNum;
	m_msgQueue[m_nHead].msg = 0;
	m_nHead = head;

	return &m_msgQueue[m_nHead];
}

void	CMessageManager::Clear()
{
	//更新last
	m_nLast = m_nHead;
}
bool	CMessageManager::Push(DWORD msg, DWORD wParam,DWORD lParam )
{
	//assert(wParamSize<WPARAMSIZE && lParamSize<LPARAMSIZE);
	Lock<FreeLock> l(m_freeLock);
	int newLast = (m_nLast+1)%m_maxMessageNum;
	if( newLast == m_nHead )//队满
		return false;

	if(Insert(newLast,msg,(BYTE*)&wParam,sizeof(DWORD),(BYTE*)&lParam,sizeof(DWORD)))
		m_nLast = newLast;
	else
		return false;

	return true;
}
bool	CMessageManager::PushFront(DWORD msg, DWORD wParam,DWORD lParam )
{
	Lock<FreeLock> l(m_freeLock);

	int newHead = m_nHead>0?m_nHead-1:m_maxMessageNum-1;

	if( newHead == m_nLast )//队满
		return false;

	if(Insert(m_nHead,msg,(BYTE*)&wParam,sizeof(DWORD),(BYTE*)&lParam,sizeof(DWORD)))
		m_nHead = newHead;
	else
		return false;

	return true;
}