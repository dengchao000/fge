#include "StdAfx.h"
#include "fgeMatch.h"

namespace fge
{

	//----------------------------------
	CByteMatch::CByteMatch( const BYTE* pmatchData, size_t size )
	{
		m_pMatchData = pmatchData;
		m_size = size;
		m_len = (BYTE)min(size,UCHAR_MAX);
		memset( m_off, m_len, sizeof(m_off) );
		--m_len;

		BYTE last = 0;
		for( BYTE offset = m_len; offset>0; --offset, ++last )
		{
			m_off[ *(pmatchData+last) ] = offset;
		}
	}
	const BYTE*	CByteMatch::Find( const BYTE* pData,int size ) const
	{
		if( size<=0 )
			return 0;
		size_t offset = m_size-1;
		const BYTE* _pData = pData;
		for( size_t idx = offset; idx < (size_t)size; idx += offset )
		{
			size_t  idx_pat = m_size-1;
			size_t  idx_tmp = idx;

			for( ; _pData[idx_tmp] == m_pMatchData[idx_pat]; --idx_pat, --idx_tmp )
			{
				if( !idx_pat )
				{
					return pData+idx_tmp;
				}
			}

			offset = m_off[ _pData[idx] ];
		}
		return 0;
	}

}