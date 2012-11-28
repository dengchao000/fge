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
*  °æ±¾	 :	1.0
*  ÃèÊö  :  ´®ËÑË÷Àà
**/

#pragma once
#include <fgeutil.h>
namespace fge
{
	class CByteMatch
	{
	public:
		CByteMatch( const BYTE* pmatchData, size_t size );
		const BYTE*		Find( const BYTE* pData,int size ) const;
		size_t			GetSize( )	{ return m_size; }
	private:
		enum { OFFSET_SIZE = 257 };
		const BYTE			*m_pMatchData;
		size_t				m_size;
		BYTE				m_len;
		BYTE				m_off[ OFFSET_SIZE ];
	};
}