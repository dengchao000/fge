#include "StdAfx.h"
#include "fgeLzCompress.h"
#include "fgeUtil.h"
#include "fgeLog.h"

namespace fge
{
	lzdCompress::lzdCompress(void)
	{
		m_istream = 0;
		m_ostream = 0;
		m_wordip = 0;
		m_ip = 0;
		m_bufsize = WND_SIZE+F;	  //F部分是扩展出来方便计算的. 窗口的实际大小还是WND_SIZE.
		if(m_bufsize>0)
			m_pEncodeBuf = new BYTE[m_bufsize];
		else
			m_pEncodeBuf = 0;	
		if(m_pEncodeBuf)
			ZeroMemory(m_pEncodeBuf,m_bufsize);
	}

	lzdCompress::~lzdCompress(void)
	{
		SAFE_DELETE_ARRAY(m_pEncodeBuf);
		m_istream = 0;
		m_ostream = 0;
	}
	void lzdCompress::ClearWnd( )
	{
		if(m_pEncodeBuf)
			ZeroMemory(m_pEncodeBuf,m_bufsize);
		m_istream = 0;
		m_ostream = 0;
		m_wordip = 0;
		m_ip = 0;
		for( int i=0; i<DIC_SIZE; i++ )
		{
			m_dic[i].id = -1;
			m_dic[i].offset = -1;
		}
	}
	void lzdCompress::InsertNode( int p,bool match )
	{
		m_match_length = 0;
		m_match_position = -1;

		int _offset = p;
		int _id = *((DWORD*)&m_pEncodeBuf[p])&0x00ffffff;
		//_id>>=8;
		int pos = _id &( DIC_SIZE-1 );

		if(m_dic[pos].id !=_id && m_dic[pos].id != -1)
			pos = (_id^0x201f) & ( DIC_SIZE-1 );

		if(m_dic[pos].id == -1)
		{
			m_dic[pos].id = _id;
			m_dic[pos].offset = p;		
		}
		else if(m_dic[pos].id == _id)
		{
			if(match)
			{
				m_match_position = m_dic[pos].offset;
				int i;
				BYTE  *key = &m_pEncodeBuf[m_match_position];
				for (i = 3; i < F; i++)
					if(key[i]!=m_pEncodeBuf[p + i])  break;
				m_match_length = i;
			}
			m_dic[pos].offset = p;
		}
		//else
		//{
		//	lzdWord* pWord = m_dic[pos];	
		//	lzdWord* pWordNext = pWord;

		//	//if( pWord->offset!=_offset )
		//	//{

		//	while(pWordNext && pWord->id != _id)
		//	{
		//		pWord = pWordNext;
		//		pWordNext = pWordNext->pNextWord;
		//	}
		//	//pWord = pWordNext;
		//	//}
		//	if( pWordNext )
		//	{			
		//		int i;
		//		BYTE  *key = &m_pbuf[pWordNext->offset];
		//		if(match)
		//		{
		//			for (i = 3; i < F; i++)
		//				if(key[i]!=m_pbuf[p + i])  break;
		//			m_match_position = pWordNext->offset;
		//			m_match_length = i;
		//		}
		//		pWordNext->offset = p;
		//	}
		//	else
		//		pWord->pNextWord =  new lzdWord( _id,_offset );
		//}
	}
	void lzdCompress::DeleteNode( int p )
	{
		int _id = *((DWORD*)&m_pEncodeBuf[p])&0x00ffffff;
		//_id>>=8;
		int pos = _id & ( DIC_SIZE-1 );
		if( m_dic[pos].offset != p )
			pos = (_id^0x201f) & ( DIC_SIZE-1 );
		if( m_dic[pos].offset == p )
		{
			m_dic[pos].id = -1;
			m_dic[pos].offset = -1;
		}
	}
	int	 lzdCompress::Encode( lzdIStream *in, lzdOStream *out )
	{//压缩,成功返回0. 失败返回-1
		if(in==0 || out==0)
			return -1;
		//if(!in->good() || !out->good())
		//	return -1;
		m_istream = in;
		m_ostream = out;
		time_t dt = clock();

		unsigned char  code_buf[17], mask;
		int code_buf_ptr;
		int codesize = 0;
		int printcount = 0;
		int textsize = 0;
		int code_ip = m_ip;	//编码指针

		code_buf[0] = 0;
		code_buf_ptr = 1; mask = 1;
		m_match_position = 0;
		m_match_length = 0;

		int	  size,len,i,match_length;

		//将buffer填满
		int active_len = F;
		int fixed_len = 0;
		//读入最多F个字节
		size = in->Read((char*)m_iobuf,F);
		if(size <= 0) return -1;
		//将字节插入到窗口中
		for( i=0; i<size; i++ )
		{
			DeleteNode(m_ip);		/* Delete old strings and */
			m_pEncodeBuf[m_ip] = m_iobuf[i];
			//将窗口首部的前F-1个字节内容copy到尾部扩展区,方便计算时不用做循环运算
			if (m_ip < F - 1)
				m_pEncodeBuf[m_ip + WND_SIZE] = m_iobuf[i];  
			m_ip = (m_ip + 1) & (WND_SIZE - 1);
		}

		len = textsize = size;	
		while( len )
		{
			this->InsertNode(m_wordip);
			if (m_match_length > len) 
				m_match_length = len;	/* match_length may be spuriously long near the end of text. */
			if (m_match_length <= THRESHOLD) {
				m_match_length = 1;		/* Not long enough match.  Send one byte. */
				code_buf[0] |= mask;	/* 'send one byte' flag */
				code_buf[code_buf_ptr++] = m_pEncodeBuf[m_wordip];  /* Send uncoded. 将未编码(压缩)的字节放入到缓冲区*/
			} 
			else
			{
				code_buf[code_buf_ptr++] = (unsigned char) m_match_position;
				code_buf[code_buf_ptr++] = (unsigned char)
					(((m_match_position >> 4) & 0xf0)
					| (m_match_length - (THRESHOLD + 1)));  /* Send position and length pair. Note match_length > THRESHOLD. */
			}
			match_length = m_match_length;		

			if ((mask <<= 1) == 0) {  /* Shift mask left one bit. */
				m_ostream->Write( (char*)code_buf,(DWORD)code_buf_ptr );
				codesize += code_buf_ptr;
				code_buf[0] = 0;  code_buf_ptr = mask = 1;
			}

			size = in->Read((char*)m_iobuf,m_match_length);

			//使用一个循环将已经编码的字节从缓冲中删除,同时读入新字节.
			for (i = 0; i < size; i++)
			{
				DeleteNode(m_ip);		/* Delete old strings and */
				m_pEncodeBuf[m_ip] = m_iobuf[i];
				//将窗口首部的前F-1个字节内容copy到尾部扩展区,方便计算时不用做循环运算
				if (m_ip < F - 1)
					m_pEncodeBuf[m_ip + WND_SIZE] = m_iobuf[i];  
				m_ip = (m_ip + 1) & (WND_SIZE - 1);

			}		
			if ((textsize += size) > printcount) {
				printf("%12ld\r", textsize);  printcount += 1024;	
			}
			for ( i = 0; i < size-1; i++ )
			{
				m_wordip = (m_wordip + 1) & (WND_SIZE - 1);
				InsertNode(m_wordip,false);
			}
			for( i=size; i<match_length; i++ )		
			{
				if ( --len && i>0 )
				{
					m_wordip = (m_wordip + 1) & (WND_SIZE - 1);			
					InsertNode(m_wordip,false);		/* buffer may not be empty. */
				}
			}
			m_wordip = (m_wordip + 1) & (WND_SIZE - 1);
		}

		if (code_buf_ptr > 1) {		/* Send remaining code. */
			m_ostream->Write( (char*)code_buf,(DWORD)code_buf_ptr );
			codesize += code_buf_ptr;
			code_buf[0] = 0;  code_buf_ptr = mask = 1;
		}

		m_ostream->Flush( );
		int t = (int)(clock()-dt);
		log_debug("\n%d ms\n",t);
		log_debug("In : %ld bytes\n", textsize);	/* Encoding is done. */
		log_debug("Out: %ld bytes\n", codesize);
		log_debug("Out/In: %.3f\n", (double)codesize / textsize);

		return 0;
	}

	void lzdCompress::OutCode(BYTE* buf,int len,int offset)
	{//输出编码
		if(len<3 || offset<0)
		{//原样输出:len<=127
			len <<= 1;
			m_ostream->Write( (char*)&len,sizeof(BYTE) );
			len >>= 1;
			m_ostream->Write( (char*)buf,len );
			return;
		}
		else
		{
			lzdCode code;
			code.isCompressed = 1;
			code.len = len & 0x0fff;
			code.offset = offset & 0x7ffff;
			m_ostream->Write( (char*)&code,sizeof(lzdCode) );
		}
	}

	//---------------------------------------------------------------
	lzdDecompression::lzdDecompression(void)
	{
		m_pDecodeBuf = 0;
		m_ip = 0;
		m_bufsize = WND_SIZE+F;
		if(m_bufsize>0)
			m_pDecodeBuf = new BYTE[m_bufsize];
		if(m_pDecodeBuf)
			ZeroMemory(m_pDecodeBuf,m_bufsize);
	}

	lzdDecompression::~lzdDecompression(void)
	{
		SAFE_DELETE_ARRAY(m_pDecodeBuf);
	}
	void lzdDecompression::ClearWnd( )
	{
		if(m_pDecodeBuf)
			ZeroMemory(m_pDecodeBuf,m_bufsize);
		m_ip = 0;
	}
	int	lzdDecompression::Decode( lzdIStream *in, lzdOStream *out )
	{
		static std::auto_ptr<unsigned char> autobuf(new BYTE[m_bufsize]);	//系统退出时自动删除

		//time_t dt = clock();

		int  i, j, k, c;
		unsigned int  flags = 0;
		unsigned char  *outbuf = autobuf.get();
		unsigned char  outprt = 0;

		int len = in->Read((char*)m_iobuf,256);
		int ioPtr = 0;
		for ( ;ioPtr<len; ) 
		{
			if(ioPtr > 250)
			{
				memcpy(m_iobuf,m_iobuf+ioPtr,256-ioPtr);
				len = in->Read((char*)m_iobuf+(256-ioPtr),ioPtr);
				len += 256-ioPtr;
				ioPtr = 0;
			}
			if(((flags >>= 1) & 256) == 0) 
			{
				//if (in->Read(&c,1)!=1) break;
				if (ioPtr>=len)
					break;
				c = m_iobuf[ioPtr++];
				flags = c | 0xff00;		/* uses higher byte cleverly */
			}							/* to count eight */
			if(flags & 1) 
			{
				//if (in->Read(&c,1)!=1) break;
				if (ioPtr>=len)
					break;
				c = m_iobuf[ioPtr++];
				outbuf[outprt++] = c;//out->Write(&c,1);
				m_pDecodeBuf[m_ip++] = c;  
				m_ip &= (WND_SIZE - 1);	// m_ip = m_ip%WND_SIZE;
			} 
			else
			{
				i = 0;
				j = 0;
				if (ioPtr>=len)	break;
				i = m_iobuf[ioPtr++];
				if (ioPtr>=len)	break;
				j = m_iobuf[ioPtr++];
				i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD+1;

				for (k = 0; k < j; k++)
				{
					c = m_pDecodeBuf[(i + k) & (WND_SIZE - 1)];
					outbuf[outprt++] = c;
					m_pDecodeBuf[m_ip++] = c;
					m_ip &= (WND_SIZE - 1);
				}
			}
			if(outprt>=32)
			{
				out->Write( (char*)outbuf,outprt ); 
				outprt = 0;
			}
		}
		if(outprt>0)
		{
			out->Write( (char*)outbuf,outprt ); 
			outprt = 0;
		}
		out->Flush( );

		//int t = int(clock()-dt);
		//printf("\n%d ms",t);

		return 0;
	}

	//---------------------------------------------------------------
	lzdByteStream::lzdByteStream( void* pbuf, size_t len )
		:m_writBuf((char*)pbuf)
	{
		m_buflen = len;
		m_curpos = 0;
	}
	lzdByteStream::lzdByteStream( const void* pbuf, size_t len )
		:m_readBuf((const char*)pbuf)
	{
		m_buflen = len;
		m_curpos = 0;
	}
	int lzdByteStream::Read(void *buf, int count)
	{
		if(count+m_curpos > m_buflen)
			count = m_buflen-m_curpos;
		memcpy_s(buf,count,m_readBuf+m_curpos,count);
		m_curpos += count;
		return count;
	}
	int lzdByteStream::Write(const void *buf, int count)
	{
		if(count+m_curpos > m_buflen)
			count = m_buflen-m_curpos;
		memcpy_s(m_writBuf+m_curpos,m_buflen-m_curpos,buf,count);
		m_curpos += count;
		return count;
	}
	//---------------------------------------------------------------
	DWORD lzdEncode(const void* pdata,size_t len, void* encodeBuf,size_t bufLen)
	{
		static lzdCompress lz;
		lz.ClearWnd();
		lzdByteStream outstream(encodeBuf,bufLen);
		lzdByteStream instream(pdata,len);
		lz.Encode(&instream,&outstream);
		return outstream.Len();
	}
	DWORD lzdDecode(const void* pdata,size_t len, void* decodeBuf,size_t bufLen)
	{
		static lzdDecompression lz;
		lz.ClearWnd();
		lzdByteStream outstream(decodeBuf,bufLen);
		lzdByteStream instream(pdata,len);
		lz.Decode(&instream,&outstream);
		return outstream.Len();
	}
}