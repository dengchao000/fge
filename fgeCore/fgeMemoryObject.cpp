#include "StdAfx.h"
#include "fgeMemoryObject.h"
#include <fgeSingleton.h>

namespace fge
{
	static CMemoryPool memPool;
	CMemoryObject::CMemoryObject(void)
	{
	}

	CMemoryObject::~CMemoryObject(void)
	{
	}
	/*************************************************************************************
	* �������ܣ��ӣ����ڿ����ÿ��б����ڴ���з����Ӧ�ֽڴ�С���ڴ�ռ�
	* ��    ����[IN] size_t size�� ��Ҫ��̬�����ڴ�Ĵ�С
	* �� �� ֵ�� �������ɹ����򷵻��ڴ�ռ���׵�ַ�����򷵻�NULL
	*��     ע���������ռ����MALLOC_MAX_SIZE���򽫴�ϵͳ����ֱ�ӷ����ڴ�
	***************************************************************************************/
	void * CMemoryObject::operator new(unsigned int size)
	{
		return memPool.Allot(size);
	}

	/*************************************************************************************
	* �������ܣ��ӣ����ڿ����ÿ��б����ڴ���з����Ӧ�ֽڴ�С���ڴ�ռ�
	* ��    ����[IN] size_t size�� ��Ҫ��̬�����ڴ�Ĵ�С
	* �� �� ֵ���������ɹ����򷵻��ڴ�ռ���׵�ַ�����򷵻�NULL
	* ��    ע���������ռ����MALLOC_MAX_SIZE���򽫴�ϵͳ����ֱ�ӷ����ڴ�
	***************************************************************************************/
	void * CMemoryObject::operator new[](unsigned int size)
	{
		return memPool.Allot(size);
	}

	/*************************************************************************************
	* �������ܣ����ڴ�ռ��ݴ浽�ڴ����,
	* ע    �⣺��Ϊ����delete��������Ƿ�Ϊ�գ����Ϊ�գ���
	*			�Զ����أ���ִ�к����Ĵ��롣�������ǲ���ָ����
	* ��    ����[IN] void * pFree�� �ͷ��ڴ���׵�ַ
	***************************************************************************************/
	void CMemoryObject::operator delete(void * pFree, unsigned int size)
	{
		memPool.Free(pFree);
	}

	/*************************************************************************************
	* �������ܣ����ڴ�ռ��ݴ浽�ڴ����
	* ��    ����[IN] void * pFree�� �ͷ��ڴ���׵�ַ
	***************************************************************************************/
	void CMemoryObject::operator delete[](void * pFree, unsigned int size)
	{
		memPool.Free(pFree);
	}

	//==============================

	CMemoryObjectThreadSafe::CMemoryObjectThreadSafe(void){}

	CMemoryObjectThreadSafe::~CMemoryObjectThreadSafe(void){}
	
	void * CMemoryObjectThreadSafe::operator new(unsigned int size)
	{
		return Safe_Allot(size);
	}
	void * CMemoryObjectThreadSafe::operator new[](unsigned int size)
	{
		return Safe_Allot(size);
	}

	void CMemoryObjectThreadSafe::operator delete(void * pFree, unsigned int size)
	{
		Safe_Free(pFree);
	}
	void CMemoryObjectThreadSafe::operator delete[](void * pFree, unsigned int size)
	{
		Safe_Free(pFree);
	}

}