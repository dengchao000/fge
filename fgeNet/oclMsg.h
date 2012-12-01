/*
Copyright (C) 2007-2009, WangYou Technology

���� :	�˳�
QQ   :	23427470
Mail : aron_d@yeah.net 

���� : ����IO�ӿ� 
*/
#pragma once
#include <Winsock2.h>
namespace ocl
{
	const int buflen = 1024*8;

	class Packer
	{
	public:
		Packer(void){ ReSet(); }
		~Packer(void){ }

		// �Ѱ�ͷ�Ͱ������ϳ�һ���ַ���
		template <typename PACK_HEAD>
		static void PackProtocol(PACK_HEAD * head, void * content, char * pack, int * packLen)
		{
			::memcpy(pack, head, sizeof(PACK_HEAD));
			::memcpy(pack + sizeof(PACK_HEAD), content, head->msg_len);
			if(packLen)
				*packLen = head->msg_len + sizeof(PACK_HEAD);
		}

		//��һ���ַ����ֽ�ɰ�ͷ�Ͱ���
		template <typename PACK_HEAD>
		static void UnPackProtocol(char * pack, int packLen, PACK_HEAD * head, void * content)
		{
			memcpy(head, pack, sizeof(PACK_HEAD));
			memcpy(content, pack + sizeof(PACK_HEAD), packLen - sizeof(PACK_HEAD));
		}

		//ͨ��һ���ַ����������ͷ��ָ��
		template <typename PACK_HEAD>
		static PACK_HEAD * GetHead(char * pack)
		{
			return ((PACK_HEAD*)pack);
		}

		//ͨ��һ���ַ������������ָ��
		template <typename PACK_HEAD>
		static void * GetContent(char * pack)
		{
			return (void *)(pack + sizeof(PACK_HEAD));
		}

		//�����û���.
		void ReSet()
		{
			ZeroMemory(m_szSendData,buflen);
			m_pSendData = m_szSendData;
			m_len = 0;
			m_Num = 0;
		}
		//������ӻ���ɹ�,���ص�ǰ�������ĳ���,���򷵻�0;
		int	AddToBuf(const void* body,int len){
			if((buflen - m_len) >=len)
			{
				memcpy(m_pSendData,body,len);
				m_pSendData+=len;
				m_len+=len;
				m_Num++;
				return m_len;
			}
			else
				return 0;
		}
		//����N����ͬ��
		int AddToBuf(const void* body,int len,int nNum)
		{
			if((buflen - m_len) >=len)
			{
				memcpy(m_pSendData,body,len);
				m_pSendData+=len;
				m_len+=len;
				m_Num+= nNum;
				return m_len;
			}
			else
				return 0;
		}
		//���ص�ǰ�������ĳ���
		int GetBufferLen(){
			return m_len;
		}
		// �õ���������󳤶�
		int GetBufferMaxLen(){
			return buflen;
		}

		//���ػ������еİ������
		int GetBufferNum()
		{
			return m_Num;
		}
		//���ػ�������ͷָ��
		char*	operator()()
		{
			return m_szSendData;
		}

		void SetNullPack(int packlen = 1)
		{
			ReSet();
			m_len = packlen;
		}
	private:
		char		m_szSendData[buflen];			//һ��װ���ݵ�BODY_BUF_LEN����
		char*		m_pSendData;					//ָ��ǰװ���ݵ��ַ�������λ��
		int			m_len;							//��ǰ����
		int			m_Num;							//�������
	};
	class ByteStreamReader
	{
	public:
		ByteStreamReader( u_char* pStream, u_long len )
		{
			m_pStream = pStream;
			nStreamLen = len;
		}

		int	Read( void* pbuf, u_long size )
		{
			if(size > nStreamLen)
				return 0;

			memcpy(pbuf,m_pStream,size);
			m_pStream+=size;
			nStreamLen-=size;

			return size;
		}
		u_char*		GetCurrentStream( ){ return m_pStream; }
	private:
		u_char*		m_pStream;
		u_long		nStreamLen;
	};
	class MsgBlock
	{
	public:
		MsgBlock( ){
			pStream = 0;
			nStreamLen = 0;
			nRef = 1;
		}
		MsgBlock( u_char* block, u_long len, const ConnectionID& id ){
			pStream = block;
			nStreamLen = len;
			clientId = id;
			nRef = 1;
		}
		virtual ~MsgBlock( ){
			pStream = 0;
			nStreamLen = 0;
		}

		u_char*	GetStream( ){ return pStream; }
		u_long	GetStreamLen( ){ return nStreamLen; }
		const ConnectionID& GetConnID( ) const { return clientId; }

		void	AddRef( ){ nRef++; }
		void	Release( ){ if(nRef==1) delete this; else nRef--; }
	protected:
		ConnectionID	clientId;
		u_char*			pStream;
		u_long			nStreamLen;
		u_long			nRef;
	};

	class AutoDelMsg : public MsgBlock
	{
	public:
		virtual ~AutoDelMsg( ){
			SAFE_DELETE_ARRAY( pStream );
		}
		AutoDelMsg( const u_char* block, u_long len, const ConnectionID& id )
		{
			if(len>0)
			{
				pStream = new u_char[len];
				memcpy(pStream,block,(size_t)len);
			}
			else
				pStream = 0;
			nStreamLen = len;
			clientId = id;
		}
	};

	class MessageBlockQueue
	{
	public:
		MessageBlockQueue(void){ }
		virtual ~MessageBlockQueue(void){ Clear(); }

		bool		Push( MsgBlock* pblock )
		{
			if(pblock==0)
				return false;
			fge::Lock<fge::FreeLock> lock(m_csMsgQuere);
			m_msgQueue.push_back( pblock );
			return true;
		}
		MsgBlock*	Pop( )
		{
			fge::Lock<fge::FreeLock> lock(m_csMsgQuere);

			MsgBlock* pblock = 0;
			if(m_msgQueue.empty())
				return 0;
			else 
			{
				pblock = *m_msgQueue.begin();
				m_msgQueue.pop_front( );
			}
			return pblock;
		}

		void		Clear()
		{
			fge::Lock<fge::FreeLock> lock(m_csMsgQuere);
			std::list<MsgBlock*>::iterator i = m_msgQueue.begin( );
			for( i; i!=m_msgQueue.end(); i++ )
			{
				if(*i)	(*i)->Release( );
			}
			m_msgQueue.clear();
		}
	private:
		std::list<MsgBlock*>	m_msgQueue;
		fge::FreeLock	m_csMsgQuere;
	};
}