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
*  ����  :  ѹ��/��ѹ����
**/

#pragma once
//#include <fgeutil.h>
//#include <fgeLzStream.h>
#include <ostream>
#include <istream>

namespace fge
{		
	const int WND_SIZE = 4096;	/* size of ring buffer */
	const int DIC_SIZE = 16384;	/* size of dictionary */
	const int F = 18;			/* upper limit for match_length */
	const int THRESHOLD = 3;   	/* encode string into position and length	��Сƥ�䳤�� */
	
	/** �ʵ����� */
	struct lzdWord
	{
		int		id;		
		int		offset;		
		lzdWord( ){ offset = -1; id = -1;}
		lzdWord( DWORD _id, int _offset ){ id = _id; offset = _offset; }
	private:
		lzdWord( const lzdWord& word ){ }
		lzdWord& operator= (const lzdWord& word) { return *this; }
	};
	/** lzd���� */
	struct lzdCode
	{
		DWORD	isCompressed:1;	//���ݿ��Ƿ�ѹ��
		DWORD	len:12;			//���ݳ���
		DWORD	offset:19;		//��ָ����
	};
	class lzdIStream
	{
	public:
		virtual ~lzdIStream(){};
		virtual int Read(void *buf, int count) = 0;
	};
	class lzdOStream
	{
	public:
		virtual ~lzdOStream(){};
		virtual int Write(const void *buf, int count) = 0;
		virtual void Flush( )=0;
	};
	/** ѹ���� */
	class lzdCompress
	{
	public:
		lzdCompress(void);
		virtual ~lzdCompress(void);

		int		Encode( lzdIStream *in, lzdOStream *out );
		void	ClearWnd( );
	private:
		void	OutCode(BYTE* buf,int len,int offset);
		void	DeleteNode( int p );
		void	InsertNode( int p,bool match=true );
	private:
		lzdIStream  *m_istream;
		lzdOStream  *m_ostream;
	private:
		BYTE	m_iobuf[256];
		BYTE*	m_pEncodeBuf;
		int		m_bufsize;
		int		m_wordip,m_ip;//m_ipΪ����ָ��,m_wordip����ָ��
		lzdWord m_dic[DIC_SIZE];
		int		m_match_position,m_match_length;
	};
	/** ��ѹ�� */
	class lzdDecompression
	{
	public:
		lzdDecompression(void);
		virtual ~lzdDecompression(void);
		int		Decode( lzdIStream *in, lzdOStream *out );
		void	ClearWnd( );
	private:
		BYTE	m_iobuf[256];
		BYTE*	m_pDecodeBuf;
		int		m_bufsize;
		int		m_ip;			//m_ipΪ����ָ��
	};

	class lzdStream :public lzdIStream, public lzdOStream
	{
	public:
		lzdStream( void* pbuf, size_t len );
		lzdStream( const void* pbuf, size_t len );
		virtual int Read(void *buf, int count);
		virtual int Write(const void *buf, int count);
		virtual void Flush( ){};

		size_t	Len( ){ return m_curpos; }

	private:
		char* m_writBuf;
		const char* m_readBuf;
		size_t m_buflen;
		size_t m_curpos;
	};
	DWORD lzdEncode(const void* pdata,size_t len, void* encodeBuf,size_t bufLen);
	DWORD lzdDecode(const void* pdata,size_t len, void* decodeBuf,size_t bufLen);
}