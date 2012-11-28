/*
** Author:	�˳�
** QQ:		23427470
** Time:	2007-10-19
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
���ƣ�		��Ϣ������
������		��Ϸ�������и��ַ�����Բ���ķ�ʽ�ṩ���񡣻����ж�������ͨ�ýӿڡ�
*/

#pragma once
#include <fgeSharedObject.h>
#include <fgeSingleton.h>
#include <fgeMemAllotor.h>
#include <fgeThread.h>
//#define WPARAMSIZE 64
//#define LPARAMSIZE 2048

namespace fge{
	// ��Ϣ�ṹ
	struct SMSG
	{
	public:
		DWORD		type;
		DWORD		msg;
		void*		wParam;//[WPARAMSIZE];
		void*		lParam;//[LPARAMSIZE];
		size_t		w_size,l_size;
		SMSG()	
		{ type=0; msg = 0; wParam = 0; lParam = 0; w_size = 0; l_size = 0;}
	};

	class CMessageManager : public SharedObject
	{
	public:
		CMessageManager(void);
		virtual ~CMessageManager(void);	

		//��ʼ�����г���
		bool	Init( int size,MemAllotor *memAllotor );		
		//������в�������Ϣ,copy pParam������.�ɹ�����true,ʧ�ܷ���false.
		bool	Push(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
		bool	Push(DWORD msg, DWORD wParam,DWORD lParam );
		bool	PushFront(DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
		bool	PushFront(DWORD msg, DWORD wParam,DWORD lParam );
		//����һ����Ϣ�����е�һ����Ϣ.ֱ����һ�ε���ʱ,�Ż�ɾ����ǰ��Ϣ.�������Ϊ�շ���false,����ɹ�����Ϊtrue.
		SMSG*	Pop( );
		void	Clear();
		
		//enum{ MAX_MESSAGE_NUMBER = 10000 };
	private:
		bool	Insert(INT pos,DWORD msg, BYTE* wParam,size_t wParamSize, BYTE* lParam,size_t lParamSize);
	private:
		MemAllotor	*m_memAllotor;				//�ڴ������
		SMSG		*m_msgQueue;				//��ΪMAX_MESSAGE_NUMBER��ѭ����Ϣ����
		int			m_maxMessageNum;
		int			m_nHead;					//����ָ��
		int			m_nLast;					//��βָ��

		//bool		*m_writeing;				//д��־
		FreeLock	m_freeLock;
	};
}