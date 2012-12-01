//------------------------------------------
// ����:�˳�
// ʱ��:2006.4.1
// ����:Э��ͷ�ļ�
//------------------------------------------
#pragma once
#include "oclProtocol.h"
#pragma pack(push)	//�ֽڶ���
#pragma pack(1)
namespace ocl{
	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };
	
	/** Э��ͷ */
	struct NetMsg
	{
		DWORD	msg_len:22;		//��Ϣ����
		DWORD	msg_num:10;		//��Ϣ����
		DWORD	msg_id;			//��Ϣ���
	};
	//PackMsg ��������NetMsg�ϵ�һ��
	template<typename T2>
	struct PackMsg :public NetMsg, public T2{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};
	template<>
	struct PackMsg<NetMsg> :public NetMsg{ PackMsg(){msg_len=sizeof(PackMsg); msg_num=1;}};


	class FrameProtocol
	{
	public:
		enum{ FRAME_BEGIN=1, FRAME_END=2 };
	private:
		#define FRAMELEN 1024*2
		typedef struct ocl_frame_head
		{			
			WORD	frameLen:14;	//Info����,���8K
			WORD	flag:2;			//֡��־ FRAME_BEGIN|FRAME_END
			//WORD	frameId;		//֡���
		} OCLFRAME_HEAD, *POCLFRAME_HEAD;

		#define HEADLEN   2
		#define INFOLEN   FRAMELEN-HEADLEN

		union tag_ocl_frame
		{
			struct {
				ocl_frame_head	head;	//ͷ��Ϣ
				byte	info[INFOLEN];	//��Ϣ����
			};
			char	cBody[FRAMELEN];
		};
		typedef tag_ocl_frame	OCLFRAME;
		typedef tag_ocl_frame*	POCLFRAME;

	public:
		FrameProtocol( ){ 
			bufSize= 0;
		}
		virtual ~FrameProtocol( ){ }
		//���ط����ֽ���
		template <class T>
		u_long	Send( const char* body, u_long size, T& op)	
		{
			char sendBuf[FRAMELEN];
			WORD m_sendid=1;
			//�ְ�����
			int idx = 0;
			int infolen = 0;
			OCLFRAME_HEAD fHead;
			fHead.flag = FRAME_BEGIN;
			const char* buf = body;
			u_long n = size;
			while( n>0 )
			{	
				//������Ϣ������
				infolen = n <= INFOLEN ? (int)n : INFOLEN;

				//��䵱ǰ֡��Ϣ
				fHead.frameLen = infolen+HEADLEN;
				if(n <= INFOLEN)
					fHead.flag |= FRAME_END;
				memcpy(sendBuf,&fHead,HEADLEN);

				//��䷢�ͻ�����
				memcpy(sendBuf+HEADLEN,buf+idx,infolen);
				u_long rt = op(sendBuf,fHead.frameLen);
				
				fHead.flag = 0;
				idx+=infolen;
				n -= infolen;

				if(rt==0)
					return 0;
			}
			return size;
		}
		template <class T>
		u_long	Recv( const char* body, u_long size, T& op )
		{
			lastRecvLen = nRecvLen;
			nRecvLen = size;
			const char* pData = body;
			if( bufSize>0 )
			{
				if( bufSize<HEADLEN )
				{//�����ǰ��û�н��հ�ͷ��Ϣ,���Ƚ��հ�ͷ��Ϣ.				
					assert(nRecvLen+bufSize>=HEADLEN);	//һ֡�ĳ��ȿ϶�������Ϣͷ�ĳ���
					u_long headSize = HEADLEN-bufSize;
					memcpy( recvBuf+bufSize,pData,headSize);
					//pData += headSize;
					//bufSize += headSize;
				}
				else
				{//Check
					OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
					if(bufSize>=pHead->frameLen)
						::OutputDebugStringA("����������\n");

					//pHead = (OCLFRAME_HEAD *)body;
					//if(size==pHead->frameLen)
					//{
					//	::OutputDebugString("����������");
					//}
				}

				//ȡ����Ϣͷ
				OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
				//ȡ����Ϣ����,�ж�һ֡�Ƿ��Ѿ�����
				if( bufSize+nRecvLen >= pHead->frameLen )
				{
					if(pHead->frameLen<bufSize)
						::OutputDebugStringA("����������\n");
					memcpy( recvBuf+bufSize,pData,pHead->frameLen-bufSize);
					//����Ϣ����
					op( recvBuf+HEADLEN,pHead->frameLen-HEADLEN, (byte)pHead->flag );					
					pData += pHead->frameLen-bufSize;
					nRecvLen -= pHead->frameLen-bufSize;
					//bufSize = 0;
				}
				else
				{
					//assert(false);
					memcpy( recvBuf+bufSize,pData,nRecvLen);
					bufSize += nRecvLen;
					return bufSize;
				}
			}
			if( nRecvLen==0 )
			{
				bufSize = 0; 
				return 0;
			}
			OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)pData;
			if(nRecvLen>size)
			{
				::OutputDebugStringA("nRecvLen>size!!!!!!!!!!!\n");
				return 0;
			}
			while( nRecvLen>HEADLEN && pHead->frameLen<= nRecvLen )
			{
				if(nRecvLen>size)
				{
					::OutputDebugStringA("nRecvLen>size!!!!!!!!!!!\n");
					return 0;
				}
				op(pData+HEADLEN,pHead->frameLen-HEADLEN,(byte)pHead->flag);
				pData += pHead->frameLen;
				nRecvLen -= pHead->frameLen;
				pHead = (OCLFRAME_HEAD *)pData;
			}
			bufSize = nRecvLen;
			if( nRecvLen>0 )
			{//����Ƿ�������,���nRecvLen>0,��ʾ��һ֡���ݻ�û������,�򽫵�ǰ�յ��Ĳ��������ȱ����ڻ�������
				assert(nRecvLen<=FRAMELEN);
				memcpy(recvBuf,pData,nRecvLen);
				bufSize = nRecvLen;
				if( nRecvLen>HEADLEN )
				{//Check
					OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
					if(nRecvLen>=pHead->frameLen)
						::OutputDebugStringA("����������\n");
				}
			}
			return bufSize; 
		}
	private:
		char		recvBuf[FRAMELEN];	// ���ջ�����,�ɻ���һ֡������
		u_long		bufSize;			// ��������С
		u_long		lastRecvLen;
		u_long		nRecvLen;
	};
};
#pragma pack()
#pragma pack(pop)