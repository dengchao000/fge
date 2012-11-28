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
*  ����  :      �������. ����Ҫ�������ָ��ʱ,��������Ұָ�������. ��������������ط���ɾ����. 
*			Ϊ�˽���������. ����ʹ�û������ü�����������ָ����������������Ҫ���õ�ָ��.
*			�����õĶ���ɾ��ʱ,����ͨ�������������,�Ӷ�����Ұָ��.
*			    ʵ�ַ�ʽ�ж���,ԭ����ͬС��. ����SharedPtr��SharedObject֮�乲��һ��Counter����.
*			��boost��shared_ptr��ͬ����,SharedPtr�����������������ڹ���. �൱��weak_ptr�Ĺ���.
*			����ƺ�ʹ������������. SharedPtr�Ķ������ͨ�� SharedObject���������ֱ������.
*			��: SharedPtr<T> pObj = &t; T��һ������. t��������һ��ʵ��. ��t���ͷź�, pObj.Lock() == 0;
**/
#pragma once
namespace fge{
	//--------------------------
	//����������
	class SharedObject;
	namespace{
		//ʹ���������ֿռ�,��֤SharedCounterֻ��Ϊ���ļ�����. ����������ͻ
		class SharedCounter{		
		public:
			friend class SharedObject;
			SharedCounter( )		{ m_refcount = 0; m_viable = 1; }
			~SharedCounter( )		{ m_refcount = 0; }
			BOOL	IsViable( )		{ return m_viable; }
			void	AddReference()	{ m_refcount++; }
			void	Release()		{ if(!m_refcount) delete this; else m_refcount--; }
			void	AddViable()		{ m_viable++; }
			void	ReleaseViable()	{ m_viable--; }
			DWORD	GetReference()	{ return m_refcount; }
			DWORD	GetViable()		{ return m_viable; }
		

		private:
			void	Destroy()		{ m_viable = 0; Release(); }
		private:
			DWORD	m_refcount;
			DWORD	m_viable;
		};
	}

	//--------------------------
	//��������ָ�����
	template<typename T>
	class SharedPtr{
	public:
		//Ĭ�Ϲ��캯��
		SharedPtr(): px(0),pc(0){ }
		//��������
		~SharedPtr(){
			Reset(); 
		}
		//ת�͹��캯��
		template<typename Y>
		SharedPtr(Y *r)
		{
			px = r;
			if( px ) 
			{
				pc = r->getCounter( );
				pc->AddReference( );
			}
			else
				pc = 0;
		}
		//Copy���캯��
		SharedPtr(SharedPtr const& r)
		{
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddReference( );
			}
			else
				pc = 0;
		}


		//ģ��Copy���캯��
		template< typename Y >
		SharedPtr(SharedPtr<Y> const& r)
		{
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddReference( );
			}
			else
				pc = 0;
		}


		//ģ�帳ֵ����
		template< typename Y >
		SharedPtr& operator=(SharedPtr<Y> const& r)
		{
			Reset( );
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddReference( );
			}
			else
				pc = 0;
			return *this;
		}

		//ģ�帳ֵ����
		template< typename Y >
		SharedPtr& operator=(Y* r)
		{
			Reset( );
			px = r;
			if( px ) 
			{
				pc = r->getCounter( );
				pc->AddReference( );
			}
			else
				pc = 0;
			return *this;
		}

		//��ֵ����
		SharedPtr& operator=(SharedPtr const& r)
		{
			Reset( );
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddReference( );
			}
			else
				pc = 0;
			return *this;
		}

		//ģ�帳ֵ����
		bool operator==(T* r)
		{
			return r == Get();
		}

		void Reset()	
		{ 
			px = 0; 
			if(pc)
				pc->Release();
			pc = 0; 
		}

	public:
		T* Get() const			{ if(pc && pc->IsViable()) return px; else return 0; }
		T* Lock() const			{ if(pc && pc->IsViable()) return px; else return 0; }
		bool operator! () const { return Get() == 0; }
		operator bool () const	{ return Get() != 0; }
		T* operator->()			{ if(pc && pc->IsViable()) return px; else throw NullPointer(); }
	private:
		friend class SharedPtr;
		T * px;				// contained pointer
		SharedCounter* pc;
	};
	template<typename T>
	class SharedPtr<T*>{
	public:
		//Ĭ�Ϲ��캯��
		SharedPtr(): px(0),pc(0){ }
		//��������
		~SharedPtr(){
			Reset(); 
		}
		//ת�͹��캯��
		template<typename Y>
		SharedPtr(Y *r)
		{
			px = r;
			if( px ) 
			{
				pc = r->getCounter( );
				//pc->AddViable( );
			}
			else
				pc = 0;
		}
		//Copy���캯��
		SharedPtr(SharedPtr const& r)
		{
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddViable( );
			}
			else
				pc = 0;
		}


		//ģ��Copy���캯��
		template< typename Y >
		SharedPtr(SharedPtr<Y> const& r)
		{
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddViable( );
			}
			else
				pc = 0;
		}


		//ģ�帳ֵ����
		template< typename Y >
		SharedPtr& operator=(SharedPtr<Y> const& r)
		{
			Reset( );
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddViable( );
			}
			else
				pc = 0;
			return *this;
		}

		//ģ�帳ֵ����
		template< typename Y >
		SharedPtr& operator=(Y* r)
		{
			if(r == px) return *this;

			Reset( );
			px = r;
			if( px ) 
			{
				pc = r->getCounter( );
				//pc->AddViable( );
			}
			else
				pc = 0;
			return *this;
		}

		//��ֵ����
		SharedPtr& operator=(SharedPtr const& r)
		{
			Reset( );
			px = r.px;
			if( px ) 
			{
				pc = r.pc;
				pc->AddViable( );
			}
			else
				pc = 0;
			return *this;
		}

		//ģ�帳ֵ����
		bool operator==(T* r)
		{
			return r == Get();
		}

		void Reset()		
		{ 
			if(pc){
				if(px && pc->GetViable()==1)
					delete px;
				else
					pc->ReleaseViable();
				
			}
			px = 0;
			pc = 0; 
		}

	public:
		T* Get() const			{ if(pc && pc->IsViable()) return px; else return 0; }
		T* Lock() const			{ if(pc && pc->IsViable()) return px; else return 0; }
		bool operator! () const { return Get() == 0; }
		operator bool () const	{ return Get() != 0; }

	private:
		friend class SharedPtr;
		T * px;				// contained pointer
		SharedCounter* pc;
	};

	//--------------------------
	//��Ӵ���̳�.
	class SharedObject
	{
	public:
		friend class SharedPtr<SharedObject>;
		typedef SharedCounter* SharedCounterPtr;
	public:
		SharedObject( )
		{ 
			m_pCount = new SharedCounter();
		};
		virtual ~SharedObject()	
		{
			if(m_pCount)
				m_pCount->Destroy();
		};

		//�˺���ΪSharedPtr����
		SharedCounterPtr	getCounter( )	{ return m_pCount; }
	protected:	
		SharedCounterPtr	m_pCount;
	};
	template< class T >
	class _SharedObject : public T, public SharedObject{};
}