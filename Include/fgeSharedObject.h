/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :      共享对象. 在须要保存对象指针时,常会遇到野指针的问题. 比如对象在其它地方被删除了. 
*			为了解决这个问题. 可以使用基于引用计数的弱引用指针对象来保存这个须要引用的指针.
*			当引用的对象被删除时,可以通过计数器来检查,从而避免野指针.
*			    实现方式有多种,原理都大同小异. 这里SharedPtr与SharedObject之间共享一个Counter对象.
*			与boost中shared_ptr不同的是,SharedPtr不负责对象的生命周期管理. 相当与weak_ptr的功能.
*			但设计和使用上有所区别. SharedPtr的对象可以通过 SharedObject及子类对象直接生成.
*			例: SharedPtr<T> pObj = &t; T是一个类型. t是这个类的一个实例. 当t被释放后, pObj.Lock() == 0;
**/
#pragma once
namespace fge{
	//--------------------------
	//共享对象计数
	class SharedObject;
	namespace{
		//使用无名名字空间,保证SharedCounter只能为本文件服务. 避免命名冲突
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
	//共享对象的指针对象
	template<typename T>
	class SharedPtr{
	public:
		//默认构造函数
		SharedPtr(): px(0),pc(0){ }
		//析构函数
		~SharedPtr(){
			Reset(); 
		}
		//转型构造函数
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
		//Copy构造函数
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


		//模板Copy构造函数
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


		//模板赋值函数
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

		//模板赋值函数
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

		//赋值函数
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

		//模板赋值函数
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
		//默认构造函数
		SharedPtr(): px(0),pc(0){ }
		//析构函数
		~SharedPtr(){
			Reset(); 
		}
		//转型构造函数
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
		//Copy构造函数
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


		//模板Copy构造函数
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


		//模板赋值函数
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

		//模板赋值函数
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

		//赋值函数
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

		//模板赋值函数
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
	//请从此类继承.
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

		//此函数为SharedPtr服务
		SharedCounterPtr	getCounter( )	{ return m_pCount; }
	protected:	
		SharedCounterPtr	m_pCount;
	};
	template< class T >
	class _SharedObject : public T, public SharedObject{};
}