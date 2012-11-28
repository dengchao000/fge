/** 
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  状态管理器与状态树
**/
#pragma once
//#include <fgeLog.h>
#include <fgeutil.h>
#include <fgeFactory.h>
//#include <fgeMemoryObject.h>
#include <fgeXml.h>
#include <fgesingleton.h>
#include <list>
#include <stack>

namespace fge
{
	/*************************************************************************************
	* 接口功能：不同状态类的接口类
	* 功能描述：规范各状态类需要的接口
	* 备    注：
	***************************************************************************************/
	class IState
	{
	protected:
		IState(void)
		{
			m_pParentState= NULL;
			m_id = 0;
		}
	public:
		virtual ~IState(void){}
		virtual void Begin(void)	{ }		//状态开始
		virtual void End(void)		{ }		//状态结束
		virtual void Update(void)	{ }		//状态更新

		virtual UINT MsgProc(UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

		//获取状态名
		const char* GetStateName(void)		{ return m_strName.c_str(); }
		//设置状态名
		void		SetStateName(const char* name) {
			if(name) m_strName = name; else m_strName=""; 
			if(m_id==0) m_id=StrHash(m_strName); 
		}

		void		SetStateID( int id )	{ m_id = id; }
		//获取状态编号
		int			GetStateID( )			{ return m_id; }
		//设置父状态
		void		SetParentState(IState* pState){ m_pParentState = pState; }
		//获取父状态
		IState*		GetParentState()		{ return m_pParentState; }	

	protected:
		IState* m_pParentState;		// 父状态
	private:
		int		m_id;				// 状态id
		String	m_strName;			// 状态名称
	};

	/**
	* 状态模板基类
	*/
	template< class Host >
	class TState: public IState
	{
	public:
		TState( ){ m_pInstance=0; }
		virtual ~TState( ){ m_pInstance=0; }

		void	SetInstance( Host* pInstance ){ m_pInstance=pInstance; }
		Host*	GetInstance( ) { return m_pInstance; }
	protected:
		Host *m_pInstance;
	};

	/**
	* 状态工厂
	*/
	template< class State >
	class TStateFactory : public fge::CVisualFactory< State >
	{
	public:
		typedef fge::CVisualFactory< State > Base;
		virtual void DestroyComponent( State* pObj ){ }

		template< class T >
		struct ImplStateMould :public fge::Mould<State>
		{
		public:
			ImplStateMould( ){ id = -1; pTemplate = 0; }
			ImplStateMould( const fge::String& _name ){ id = fge::StrHash(_name); name = _name; pTemplate = 0; }
			ImplStateMould( int _id ){ this->id = _id; pTemplate = 0; 	}
			virtual ~ImplStateMould( ){ 
				if(pTemplate)
					delete pTemplate;
				pTemplate=0; 
			}

			virtual State*				CreateObject( )	
			{
				if(!pTemplate)
				{
					pTemplate = new T(); 
					if(pTemplate)	//Builder 				
						Builder( pTemplate );
				}
				return pTemplate; 
			}				
			virtual int					GetTypeID( ){ return id; }
			virtual const fge::String&	GetName( ){ return name; }		
		protected:
			virtual void				Builder( T* pObject ){ pTemplate->SetStateID(id); }
		protected:
			T*			pTemplate;
			fge::String	name;
			int			id;	
		};
		template< class H, class T >
		struct ObjectState :public ImplStateMould<T>
		{
		public:
			ObjectState( H* pObj,const fge::String& _name )
				:ImplStateMould<T>(_name)
			{
				m_pObject = pObj;
			}
			ObjectState( H* pObj,int id )
				:ImplStateMould<T>(id)
			{
				m_pObject = pObj;
			}
			virtual void	Builder( T* pObject )
			{
				if(pObject)
				{
					pObject->SetStateID(id);
					pObject->SetStateName(name.c_str());
					pObject->SetInstance( m_pObject );
				}
			}				
		private:
			H*	m_pObject;
		};

		// 注册对象模板
		template< class C, class H >
		bool RegisterObjState( H* h, const fge::String& _name )
		{
			return Base::Register( new ObjectState<H,C>(h,_name) );
		}
		template< class C, class H >
		bool RegisterObjState( H* h, int id )
		{
			return Base::Register( new ObjectState<H,C>(h,id) );
		}
		template< class C >
		bool Register( const fge::String& _name )
		{
			return Base::Register( new ImplStateMould<C>(_name) );
		}
		template< class C >
		bool Register( int id )
		{
			return Base::Register( new ImplStateMould<C>(id) );
		}
		template< class T >
		bool Register( T* pMould )
		{
			return Base::Register( pMould );
		}
	};

	/*************************************************************************************
	* 名    称：状态管理器接口
	* 功能描述：状态管理器用于管理个类状态
	*           状态管理器中含有一棵状态树,这是状态树是一个状态包装器树.包装器用于创建创建具体的对象.
	* 备    注：
	***************************************************************************************/
	template < typename T >
	class IStateManager
	{
	public:
		typedef std::list< T* > StateList;
	public:
		virtual ~IStateManager(void){}
	public:
		virtual bool		SetState(char* StateName) = 0;
		virtual bool		SetState(int StateID) = 0;
		virtual void		SetNewState(int StateID) = 0;
		virtual const char* GetCurrentStateName( void ) = 0;

		virtual T*			GetCurrentState( void ) = 0;								// 返回末端状态
		virtual bool		InState(const char* StateName) = 0;
		virtual bool		InState(TYPEID StateID) = 0;

		virtual UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;	//当前状态执行消息函数
		virtual void		Update(void) = 0;											//当前状态执行更新状态函数
	};

	/*************************************************************************************
	* 名    称：状态管理器
	* 功能描述：状态管理器用于管理个类状态
	*           状态管理器中含有一棵状态树,这是状态树是一个状态包装器树.包装器用于创建创建具体的对象.
	* 备    注：
	***************************************************************************************/
	template < typename T, /*class Host, */class StateFactory=CVisualFactory< T > >
	class CStateManager : public IStateManager<T>
	{
	public:
		typedef std::list< T* > StateList;
		/*************************************************************************************
		* 名    称：状态树
		* 功能描述：指定状态与状态之间的逻辑关系
		* 备    注：
		***************************************************************************************/
		class StateTree : public CSingleton< StateTree >
		{
		private:
			/** 
			* 状态树的节点
			*/
			struct StateTreeNode
			{
				int		depth;	//节点深度
				int		id;		//节点编号,如果没有指定,则使用hash值
				String	strName;//节点名

				StateTreeNode*					parentNode;		//父状态模板结点
				std::list< StateTreeNode* >	childStateMould;	// 子状态模板

				StateTreeNode( )	{ 
					id = 0; parentNode=0; depth = 0;
				}
				~StateTreeNode( )	{ 
					std::list< StateTreeNode* >::iterator i = childStateMould.begin();
					for( i; i!=childStateMould.end(); i++)
						delete *i;
					childStateMould.clear();
					parentNode = 0;
				}
			};	

			StateTreeNode* m_pHeadTreeNode;	//状态树
			std::map<int,StateTreeNode*> m_treeMap;//状态树映射图.加速状态查找,减少状态切换时间.

		public:
			StateTree( ){ m_pHeadTreeNode = 0; }
			StateTree( const String& conffig_file ){ Init( conffig_file); }
			~StateTree(){ SAFE_DELETE( m_pHeadTreeNode ); }
			/** 释放状态树
			*/
			void Destroy()
			{
				SAFE_DELETE( m_pHeadTreeNode );
				m_treeMap.clear( );
			}
			/** 初始化状态树,传入一个xml配置文件
			*/
			bool InitFormFile( const String& conffig_file )
			{
				Destroy();

				CXML xConfig;
				try{
					xConfig.Open(conffig_file.c_str());
					Element* pXmlNode = xConfig.GetRootElement( );
					m_pHeadTreeNode = CreateNode(NULL,pXmlNode,0);
				}
				catch(const XMLException & e){
					e.what();
					return false;
				}
				return true;
			}
			bool InitFormMem( const char* conffig_file )
			{
				if(!conffig_file) return false;

				Destroy();

				CXML xConfig;
				try{
					xConfig.Parse(conffig_file);
					Element* pXmlNode = xConfig.GetRootElement( );
					m_pHeadTreeNode = CreateNode(NULL,pXmlNode,0);
				}
				catch(const XMLException & e){
					e.what();
					return false;
				}
				return true;
			}

			/** 创建一个从当前状态到"to"的状态表
			*/
			template < typename StateFactory,typename StateList >
				bool CreateStateList(StateList& lsCurrentState, const String& to, StateFactory* pFactory )
			{
				return CreateStateList<StateFactory>(lsCurrentState,fge::StrHash(to),pFactory );			
			}
			/** 创建一个从当前状态到"to"的状态表
			*/
			template < typename StateFactory,typename StateList >
				bool CreateStateList(StateList& lsCurrentState, int to, StateFactory* pFactory )
			{			
				typedef StateFactory::ObjectType ObjectType;
				typedef StateFactory::MouldType MouldType; 
				//typedef std::list< ObjectType* > StateList;
				if(!pFactory) return false;
				//如果头节点为空,则树还没有被初始化.返回假
				if(!m_pHeadTreeNode) return false;

				//声明临时变量
				ObjectType* pStateObject(0);
				int from = 0;

				// form将记录当前状态链与新状态链的共同父节点,也是将要修改的链表的起始节点
				if( !lsCurrentState.empty() ){
					from = lsCurrentState.back()->GetStateID();
				}
				else{
					//如果当前状态为空,则将状态的根节点插入
					//创建状态对象
					pStateObject = pFactory->CreateComponent(m_pHeadTreeNode->id);
					if(!pStateObject) return false;
					//初始化状态
					pStateObject->SetStateName(m_pHeadTreeNode->strName.c_str());
					//插入状态链表
					lsCurrentState.push_front(pStateObject);						
					//开始新状态
					pStateObject->Begin();

					from = m_pHeadTreeNode->id;
					if( from == to ) return true;
				}
				//如果起始与结束是同一个节点,则返回
				if( from == to ) return false;

				//查找目标结点
				std::map<int,StateTreeNode*>::iterator iTo = m_treeMap.find(to);
				if(iTo==m_treeMap.end())
					return false;
				//查找起始结点
				std::map<int,StateTreeNode*>::iterator iFrom = m_treeMap.find(from);
				if(iFrom==m_treeMap.end())
					return false;

				StateTreeNode* pFromNode = iFrom->second;
				StateTreeNode* pToNode = iTo->second;
				StateTreeNode* pParentNode = m_pHeadTreeNode;

				//移动节点到同一层次
				while( pFromNode && pFromNode->depth > pToNode->depth ) pFromNode = pFromNode->parentNode;
				while( pToNode && pToNode->depth > pFromNode->depth ) pToNode = pToNode->parentNode;

				//查共同的父节点:
				while( pFromNode && pToNode && pFromNode->parentNode != pToNode->parentNode ) 
				{ pFromNode=pFromNode->parentNode;	pToNode = pToNode->parentNode; 	}

				//没有共同的父节点,则表示两个节点可能不在同一状态树上:
				if( !pFromNode || !pToNode ) return false;

				//结束当前状态到共同父状态
				int parentNodeId;
				if( pFromNode==pToNode ){ pParentNode=pFromNode; parentNodeId=pFromNode->id; }
				else{ pParentNode=pFromNode->parentNode; parentNodeId = pFromNode->parentNode->id; }

				while(!lsCurrentState.empty()&&lsCurrentState.back()->GetStateID()!=parentNodeId)
				{
					lsCurrentState.pop_back();
					lsCurrentState.back()->End( );
					pFactory->DestroyComponent(lsCurrentState.back());					
				}

				//创建状态栈
				std::stack<StateTreeNode*> tempStateStack;
				pToNode = iTo->second;
				while( pToNode!=pParentNode ){
					tempStateStack.push( pToNode );
					pToNode = pToNode->parentNode;
				}
				
				//创建状态链
				ObjectType* pParentState = lsCurrentState.back( );
				while(!tempStateStack.empty()){
					pToNode = tempStateStack.top();
					pStateObject = pFactory->CreateComponent(pToNode->id);
					if( pStateObject ){
						pStateObject->SetStateName(pToNode->strName.c_str());
						pStateObject->SetParentState(pParentState);
						lsCurrentState.push_back(pStateObject);
						pStateObject->Begin();
						pParentState = pStateObject;
					}
					tempStateStack.pop();
				}

				return true;
			}
		protected:
			//-------------
			StateTreeNode*	CreateTreeNode( StateTreeNode* pParent, Element* pXmlNode ,int depth )					
			{
				StateTreeNode* pNode = new StateTreeNode;
				if(!pNode) return NULL;

				pNode->parentNode = pParent;
				pNode->strName = pXmlNode->name.c_str();
				pNode->id = StrHash(pXmlNode->name);
				pXmlNode->GetAttribute("id",pNode->id);
				pNode->depth = depth;

				return pNode; 
			}
			//递归生成状态树
			StateTreeNode*	CreateNode( StateTreeNode* pParent, Element* pXmlNode ,int depth )
			{
				if( !pXmlNode ) return NULL;
				StateTreeNode* pNode = CreateTreeNode( pParent,pXmlNode,depth );
				if( !pNode ) return NULL;

				if( pParent ) pParent->childStateMould.push_back(pNode);
				m_treeMap[pNode->id] = pNode;

				std::list<Element*>::iterator i=pXmlNode->subElement.begin();
				for( i; i!= pXmlNode->subElement.end(); i++ ){
					CreateNode( pNode, *i, depth+1 );
				}
				return pNode;
			}
		};

	public:
		/*************************************************************************************
		* 函数功能：构造函数
		* 备    注：
		*************************************************************************************/
		CStateManager( ){ m_pFactory=INSTANCE(StateFactory); m_newState=0;}
		CStateManager( StateFactory* pFactory){ m_pFactory=pFactory; m_newState=0; }
		/*************************************************************************************
		* 函数功能：析构函数
		* 备    注：析构状态管理树
		*************************************************************************************/
		virtual		~CStateManager(void)
		{
			// 结束当前状态
			while( !m_lsCurrentState.empty() )
			{
				m_lsCurrentState.back()->End( );
				m_pFactory->DestroyComponent(m_lsCurrentState.back());
				m_lsCurrentState.pop_back();
			}
		}

		void		Init( StateFactory* pFactory) { m_pFactory=pFactory; }

	public:
		/*************************************************************************************
		* 函数功能：设置当前状态,立即进行状态切换.
		* 参    数：StateName, 状态的字符串描述.
		* 返 回 值：bool, 成功返回true,失败返回false.
		* 备    注：程序将字符串转换成Hash值进行计算,所以只能使用名字或者ID二选一接口, 以免发生冲突
		*************************************************************************************/
		bool		SetState(char* StateName)
		{
			return SetState(StrHash(String(StateName)));
		}
		/*************************************************************************************
		* 函数功能：设置当前状态,立即进行状态切换.
		* 参    数：StateID, 状态ID
		* 返 回 值：bool, 成功返回true,失败返回false.
		* 备    注：由于程序将状态名转换成Hash值作为ID进行计算,所以只能使用名字或者ID二选一接口, 以免发生冲突
		*************************************************************************************/
		bool		SetState(int StateID)
		{
			m_newState = 0;
			return INSTANCE(StateTree)->CreateStateList( m_lsCurrentState,StateID,m_pFactory );
		}
		/*************************************************************************************
		* 函数功能：设置新状态,将在Update的时候切换
		* 参    数：StateID, 状态ID
		* 返 回 值：无
		* 备    注：由于程序将状态名转换成Hash值作为ID进行计算,所以只能使用名字或者ID二选一接口, 以免发生冲突
		*************************************************************************************/
		void		SetNewState(int StateID)
		{
			m_newState = StateID;
		}
		/*************************************************************************************
		* 函数功能：返回将要切换到的新状态ID
		* 参    数：无
		* 返 回 值：
		* 备    注：
		*************************************************************************************/
		int			GetNewState( )		
		{
			return m_newState;
		}
		/*************************************************************************************
		* 函数功能：得到当前最终状态的名字
		* 参    数：无
		* 返 回 值：
		* 备    注：因为当前状态其实是一个状态链,这里返回的是链末端的状态
		*************************************************************************************/		
		const char* GetCurrentStateName( void )
		{
			if(m_lsCurrentState.empty()) return "";
			return m_lsCurrentState.back()->GetStateName();
		}

		// 返回末端状态
		T*			GetCurrentState( void )
		{
			if(m_lsCurrentState.empty()) return NULL;
			return m_lsCurrentState.back();
		}

		// 根据传入的状态字符串名字判断当前是否有该状态
		bool		InState(const char* StateName)
		{
			if(!StateName)
				return false;
			StateList::iterator i=m_lsCurrentState.begin();
			for( i; i!=m_lsCurrentState.end(); i++ )
			{
				if(!strcmp((*i)->GetStateName(),StateName))
					return true;
			}
			return false;
		}	

		// 判断指定状态是否当前状态链中.
		bool		InState(TYPEID StateID)
		{
			StateList::iterator i=m_lsCurrentState.begin();
			for( i; i!=m_lsCurrentState.end(); i++ )
			{
				if( (*i)->GetStateID()==StateID )
					return true;
			}
			return false;
		}

		/*************************************************************************************
		* 函数功能：当前状态执行消息函数
		* 参    数：
		* 返 回 值：
		* 备    注：
		*************************************************************************************/		
		UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			if(m_lsCurrentState.empty()) return FALSE;
			// 子状态到父状态
			StateList::reverse_iterator i = m_lsCurrentState.rbegin();
			UINT rt(0);

			for( i; i!=m_lsCurrentState.rend(); i++ )
			{
				if( rt==(*i)->MsgProc (message, wParam, lParam) )
					return rt;
			}
			return 0;
		}

		/*************************************************************************************
		* 函数功能：当前状态执行更新状态函数
		* 参    数：
		* 返 回 值：
		* 备    注：
		*************************************************************************************/		
		void		Update(void)
		{
			if( m_newState!=0 )
			{
				SetState( m_newState );
				m_newState = 0;
			}
			
			StateList::iterator i = m_lsCurrentState.begin();
			for( i; i!=m_lsCurrentState.end(); i++ )
				(*i)->Update();
		}

	public:
		//得到状态工厂
		StateFactory*	GetFactory( ){ return m_pFactory; }
		void			SetFactory( StateFactory* pFactory ){ m_pFactory = pFactory; }
		//初始化状态树
		static  void	InitStateTreeFromFile( const char* xmlTree ){ INSTANCE(StateTree)->InitFormFile(xmlTree); }
		static  void	InitStateTreeFromMem( const char* xmlTree ){ INSTANCE(StateTree)->InitFormMem(xmlTree); }
	private:
		StateFactory*	m_pFactory;
		int				m_newState;
		StateList		m_lsCurrentState;			// 当前状态链
	};

	/************************************************************************/
	/* 简单的事件模型自动状态机                                             */
	/************************************************************************/
	class IStateEvent
	{
	public:
		IStateEvent(void){};
		virtual ~IStateEvent(void){};

		virtual void	Begin( ) = 0;
		virtual void	Update( ) = 0;
		virtual void	End( ) = 0;
	};

	//代理事件类,代理事件本身不处理事件,而是将事件先代理下来,再分发给被代理的对象. 可以起到接口转换作用.
	template<typename T>
	class TStateEvent : public IStateEvent
	{
	public:
		typedef void (T::*Handle)(  );
	private:
		T*	mThis;
		Handle mBegin;
		Handle mUpdate;
		Handle mEnd;
	public:
		TStateEvent(){ mThis=NULL; }
		TStateEvent(T* refThis, Handle begin, Handle update, Handle end)
		{ 
			mThis = refThis;
			mBegin = begin;
			mUpdate = update;
			mEnd = end;
		}
		virtual void	Begin( )	{ if(mThis) INVOKE(mThis,mBegin)( ); }
		virtual void	Update( )	{ if(mThis) INVOKE(mThis,mUpdate)( ); }
		virtual void	End( )		{ if(mThis) INVOKE(mThis,mEnd)( ); }
	};

	//事件集合,用来对多个回调的支持
	template<typename T>
	class FSM
	{
	public:
		typedef T State;
		typedef CVisualFactory< T > StateFactory;
	public:
		FSM(){ mNewState=NULL; mCurrentState=NULL; m_pFactory=NULL; };
		~FSM(){ }

		//设置新状态
		virtual void	SetState( State* pState )
		{
			mNewState = 0;
			if(mCurrentState != pState)
				mNewState = pState;
		}
		virtual void	UpdateState()
		{
			if(mNewState)
			{
				if(mCurrentState)
					mCurrentState->End( );
				mCurrentState = mNewState; 
				mNewState = 0;
				mCurrentState->Begin();				
			}
			if(mCurrentState)
				mCurrentState->Update();
		}
		virtual void	CloseState()
		{
			if( mCurrentState ) mCurrentState->End( );
			mCurrentState = 0;
		}

		State*	GetNewState( ){ return mNewState; }
		State*	GetCurrentState( ){ return mCurrentState; }
		
		void	SetFactory( StateFactory* pFactory ){ m_pFactory = pFactory; }		
		bool	SetState(int StateID)
		{
			if(m_pFactory)
			{
				State* pState = m_pFactory->CreateComponent(StateID);
				if(!pState)
					return false;
				SetState(pState);
				return true;
			}
			return false;
		}
	private:
		State* mCurrentState;
		State* mNewState;
		StateFactory*	m_pFactory;
	};
}