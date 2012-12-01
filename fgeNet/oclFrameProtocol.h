//------------------------------------------
// 作者:邓超
// 时间:2006.4.1
// 描述:协议头文件
//------------------------------------------
#pragma once
#include "oclProtocol.h"
#pragma pack(push)	//字节对齐
#pragma pack(1)
namespace ocl{
	template<typename T1, typename T2>
	struct TCombineMsg :public T1, public T2{ };
	
	/** 协议头 */
	struct NetMsg
	{
		DWORD	msg_len:22;		//消息长度
		DWORD	msg_num:10;		//消息数量
		DWORD	msg_id;			//消息编号
	};
	//PackMsg 将包体与NetMsg合到一起
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
			WORD	frameLen:14;	//Info长度,最大8K
			WORD	flag:2;			//帧标志 FRAME_BEGIN|FRAME_END
			//WORD	frameId;		//帧编号
		} OCLFRAME_HEAD, *POCLFRAME_HEAD;

		#define HEADLEN   2
		#define INFOLEN   FRAMELEN-HEADLEN

		union tag_ocl_frame
		{
			struct {
				ocl_frame_head	head;	//头信息
				byte	info[INFOLEN];	//信息内容
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
		//返回发送字节数
		template <class T>
		u_long	Send( const char* body, u_long size, T& op)	
		{
			char sendBuf[FRAMELEN];
			WORD m_sendid=1;
			//分包发送
			int idx = 0;
			int infolen = 0;
			OCLFRAME_HEAD fHead;
			fHead.flag = FRAME_BEGIN;
			const char* buf = body;
			u_long n = size;
			while( n>0 )
			{	
				//复制信息到包中
				infolen = n <= INFOLEN ? (int)n : INFOLEN;

				//填充当前帧信息
				fHead.frameLen = infolen+HEADLEN;
				if(n <= INFOLEN)
					fHead.flag |= FRAME_END;
				memcpy(sendBuf,&fHead,HEADLEN);

				//填充发送缓冲区
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
				{//如果当前还没有接收包头信息,则先接收包头信息.				
					assert(nRecvLen+bufSize>=HEADLEN);	//一帧的长度肯定大于信息头的长度
					u_long headSize = HEADLEN-bufSize;
					memcpy( recvBuf+bufSize,pData,headSize);
					//pData += headSize;
					//bufSize += headSize;
				}
				else
				{//Check
					OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
					if(bufSize>=pHead->frameLen)
						::OutputDebugStringA("缓冲区错误\n");

					//pHead = (OCLFRAME_HEAD *)body;
					//if(size==pHead->frameLen)
					//{
					//	::OutputDebugString("缓冲区错误");
					//}
				}

				//取得信息头
				OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
				//取得消息长度,判断一帧是否已经收完
				if( bufSize+nRecvLen >= pHead->frameLen )
				{
					if(pHead->frameLen<bufSize)
						::OutputDebugStringA("缓冲区错误\n");
					memcpy( recvBuf+bufSize,pData,pHead->frameLen-bufSize);
					//将消息发出
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
			{//检查是否接收完成,如果nRecvLen>0,表示这一帧数据还没有收完,则将当前收到的部分数据先保存在缓冲区中
				assert(nRecvLen<=FRAMELEN);
				memcpy(recvBuf,pData,nRecvLen);
				bufSize = nRecvLen;
				if( nRecvLen>HEADLEN )
				{//Check
					OCLFRAME_HEAD * pHead = (OCLFRAME_HEAD *)recvBuf;
					if(nRecvLen>=pHead->frameLen)
						::OutputDebugStringA("缓冲区错误\n");
				}
			}
			return bufSize; 
		}
	private:
		char		recvBuf[FRAMELEN];	// 接收缓冲区,可缓冲一帧的数据
		u_long		bufSize;			// 缓冲区大小
		u_long		lastRecvLen;
		u_long		nRecvLen;
	};
};
#pragma pack()
#pragma pack(pop)