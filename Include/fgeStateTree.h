/** 
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games. All rights reserved.
** Kernel functions
*
*  �汾	 :	1.0
*  ����  :  ״̬��������״̬��
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
	* �ӿڹ��ܣ���ͬ״̬��Ľӿ���
	* �����������淶��״̬����Ҫ�Ľӿ�
	* ��    ע��
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
		virtual void Begin(void)	{ }		//״̬��ʼ
		virtual void End(void)		{ }		//״̬����
		virtual void Update(void)	{ }		//״̬����

		virtual UINT MsgProc(UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

		//��ȡ״̬��
		const char* GetStateName(void)		{ return m_strName.c_str(); }
		//����״̬��
		void		SetStateName(const char* name) {
			if(name) m_strName = name; else m_strName=""; 
			if(m_id==0) m_id=StrHash(m_strName); 
		}

		void		SetStateID( int id )	{ m_id = id; }
		//��ȡ״̬���
		int			GetStateID( )			{ return m_id; }
		//���ø�״̬
		void		SetParentState(IState* pState){ m_pParentState = pState; }
		//��ȡ��״̬
		IState*		GetParentState()		{ return m_pParentState; }	

	protected:
		IState* m_pParentState;		// ��״̬
	private:
		int		m_id;				// ״̬id
		String	m_strName;			// ״̬����
	};

	/**
	* ״̬ģ�����
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
	* ״̬����
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

		// ע�����ģ��
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
	* ��    �ƣ�״̬�������ӿ�
	* ����������״̬���������ڹ������״̬
	*           ״̬�������к���һ��״̬��,����״̬����һ��״̬��װ����.��װ�����ڴ�����������Ķ���.
	* ��    ע��
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

		virtual T*			GetCurrentState( void ) = 0;								// ����ĩ��״̬
		virtual bool		InState(const char* StateName) = 0;
		virtual bool		InState(TYPEID StateID) = 0;

		virtual UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;	//��ǰ״ִ̬����Ϣ����
		virtual void		Update(void) = 0;											//��ǰ״ִ̬�и���״̬����
	};

	/*************************************************************************************
	* ��    �ƣ�״̬������
	* ����������״̬���������ڹ������״̬
	*           ״̬�������к���һ��״̬��,����״̬����һ��״̬��װ����.��װ�����ڴ�����������Ķ���.
	* ��    ע��
	***************************************************************************************/
	template < typename T, /*class Host, */class StateFactory=CVisualFactory< T > >
	class CStateManager : public IStateManager<T>
	{
	public:
		typedef std::list< T* > StateList;
		/*************************************************************************************
		* ��    �ƣ�״̬��
		* ����������ָ��״̬��״̬֮����߼���ϵ
		* ��    ע��
		***************************************************************************************/
		class StateTree : public CSingleton< StateTree >
		{
		private:
			/** 
			* ״̬���Ľڵ�
			*/
			struct StateTreeNode
			{
				int		depth;	//�ڵ����
				int		id;		//�ڵ���,���û��ָ��,��ʹ��hashֵ
				String	strName;//�ڵ���

				StateTreeNode*					parentNode;		//��״̬ģ����
				std::list< StateTreeNode* >	childStateMould;	// ��״̬ģ��

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

			StateTreeNode* m_pHeadTreeNode;	//״̬��
			std::map<int,StateTreeNode*> m_treeMap;//״̬��ӳ��ͼ.����״̬����,����״̬�л�ʱ��.

		public:
			StateTree( ){ m_pHeadTreeNode = 0; }
			StateTree( const String& conffig_file ){ Init( conffig_file); }
			~StateTree(){ SAFE_DELETE( m_pHeadTreeNode ); }
			/** �ͷ�״̬��
			*/
			void Destroy()
			{
				SAFE_DELETE( m_pHeadTreeNode );
				m_treeMap.clear( );
			}
			/** ��ʼ��״̬��,����һ��xml�����ļ�
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

			/** ����һ���ӵ�ǰ״̬��"to"��״̬��
			*/
			template < typename StateFactory,typename StateList >
				bool CreateStateList(StateList& lsCurrentState, const String& to, StateFactory* pFactory )
			{
				return CreateStateList<StateFactory>(lsCurrentState,fge::StrHash(to),pFactory );			
			}
			/** ����һ���ӵ�ǰ״̬��"to"��״̬��
			*/
			template < typename StateFactory,typename StateList >
				bool CreateStateList(StateList& lsCurrentState, int to, StateFactory* pFactory )
			{			
				typedef StateFactory::ObjectType ObjectType;
				typedef StateFactory::MouldType MouldType; 
				//typedef std::list< ObjectType* > StateList;
				if(!pFactory) return false;
				//���ͷ�ڵ�Ϊ��,������û�б���ʼ��.���ؼ�
				if(!m_pHeadTreeNode) return false;

				//������ʱ����
				ObjectType* pStateObject(0);
				int from = 0;

				// form����¼��ǰ״̬������״̬���Ĺ�ͬ���ڵ�,Ҳ�ǽ�Ҫ�޸ĵ��������ʼ�ڵ�
				if( !lsCurrentState.empty() ){
					from = lsCurrentState.back()->GetStateID();
				}
				else{
					//�����ǰ״̬Ϊ��,��״̬�ĸ��ڵ����
					//����״̬����
					pStateObject = pFactory->CreateComponent(m_pHeadTreeNode->id);
					if(!pStateObject) return false;
					//��ʼ��״̬
					pStateObject->SetStateName(m_pHeadTreeNode->strName.c_str());
					//����״̬����
					lsCurrentState.push_front(pStateObject);						
					//��ʼ��״̬
					pStateObject->Begin();

					from = m_pHeadTreeNode->id;
					if( from == to ) return true;
				}
				//�����ʼ�������ͬһ���ڵ�,�򷵻�
				if( from == to ) return false;

				//����Ŀ����
				std::map<int,StateTreeNode*>::iterator iTo = m_treeMap.find(to);
				if(iTo==m_treeMap.end())
					return false;
				//������ʼ���
				std::map<int,StateTreeNode*>::iterator iFrom = m_treeMap.find(from);
				if(iFrom==m_treeMap.end())
					return false;

				StateTreeNode* pFromNode = iFrom->second;
				StateTreeNode* pToNode = iTo->second;
				StateTreeNode* pParentNode = m_pHeadTreeNode;

				//�ƶ��ڵ㵽ͬһ���
				while( pFromNode && pFromNode->depth > pToNode->depth ) pFromNode = pFromNode->parentNode;
				while( pToNode && pToNode->depth > pFromNode->depth ) pToNode = pToNode->parentNode;

				//�鹲ͬ�ĸ��ڵ�:
				while( pFromNode && pToNode && pFromNode->parentNode != pToNode->parentNode ) 
				{ pFromNode=pFromNode->parentNode;	pToNode = pToNode->parentNode; 	}

				//û�й�ͬ�ĸ��ڵ�,���ʾ�����ڵ���ܲ���ͬһ״̬����:
				if( !pFromNode || !pToNode ) return false;

				//������ǰ״̬����ͬ��״̬
				int parentNodeId;
				if( pFromNode==pToNode ){ pParentNode=pFromNode; parentNodeId=pFromNode->id; }
				else{ pParentNode=pFromNode->parentNode; parentNodeId = pFromNode->parentNode->id; }

				while(!lsCurrentState.empty()&&lsCurrentState.back()->GetStateID()!=parentNodeId)
				{
					lsCurrentState.pop_back();
					lsCurrentState.back()->End( );
					pFactory->DestroyComponent(lsCurrentState.back());					
				}

				//����״̬ջ
				std::stack<StateTreeNode*> tempStateStack;
				pToNode = iTo->second;
				while( pToNode!=pParentNode ){
					tempStateStack.push( pToNode );
					pToNode = pToNode->parentNode;
				}
				
				//����״̬��
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
			//�ݹ�����״̬��
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
		* �������ܣ����캯��
		* ��    ע��
		*************************************************************************************/
		CStateManager( ){ m_pFactory=INSTANCE(StateFactory); m_newState=0;}
		CStateManager( StateFactory* pFactory){ m_pFactory=pFactory; m_newState=0; }
		/*************************************************************************************
		* �������ܣ���������
		* ��    ע������״̬������
		*************************************************************************************/
		virtual		~CStateManager(void)
		{
			// ������ǰ״̬
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
		* �������ܣ����õ�ǰ״̬,��������״̬�л�.
		* ��    ����StateName, ״̬���ַ�������.
		* �� �� ֵ��bool, �ɹ�����true,ʧ�ܷ���false.
		* ��    ע�������ַ���ת����Hashֵ���м���,����ֻ��ʹ�����ֻ���ID��ѡһ�ӿ�, ���ⷢ����ͻ
		*************************************************************************************/
		bool		SetState(char* StateName)
		{
			return SetState(StrHash(String(StateName)));
		}
		/*************************************************************************************
		* �������ܣ����õ�ǰ״̬,��������״̬�л�.
		* ��    ����StateID, ״̬ID
		* �� �� ֵ��bool, �ɹ�����true,ʧ�ܷ���false.
		* ��    ע�����ڳ���״̬��ת����Hashֵ��ΪID���м���,����ֻ��ʹ�����ֻ���ID��ѡһ�ӿ�, ���ⷢ����ͻ
		*************************************************************************************/
		bool		SetState(int StateID)
		{
			m_newState = 0;
			return INSTANCE(StateTree)->CreateStateList( m_lsCurrentState,StateID,m_pFactory );
		}
		/*************************************************************************************
		* �������ܣ�������״̬,����Update��ʱ���л�
		* ��    ����StateID, ״̬ID
		* �� �� ֵ����
		* ��    ע�����ڳ���״̬��ת����Hashֵ��ΪID���м���,����ֻ��ʹ�����ֻ���ID��ѡһ�ӿ�, ���ⷢ����ͻ
		*************************************************************************************/
		void		SetNewState(int StateID)
		{
			m_newState = StateID;
		}
		/*************************************************************************************
		* �������ܣ����ؽ�Ҫ�л�������״̬ID
		* ��    ������
		* �� �� ֵ��
		* ��    ע��
		*************************************************************************************/
		int			GetNewState( )		
		{
			return m_newState;
		}
		/*************************************************************************************
		* �������ܣ��õ���ǰ����״̬������
		* ��    ������
		* �� �� ֵ��
		* ��    ע����Ϊ��ǰ״̬��ʵ��һ��״̬��,���ﷵ�ص�����ĩ�˵�״̬
		*************************************************************************************/		
		const char* GetCurrentStateName( void )
		{
			if(m_lsCurrentState.empty()) return "";
			return m_lsCurrentState.back()->GetStateName();
		}

		// ����ĩ��״̬
		T*			GetCurrentState( void )
		{
			if(m_lsCurrentState.empty()) return NULL;
			return m_lsCurrentState.back();
		}

		// ���ݴ����״̬�ַ��������жϵ�ǰ�Ƿ��и�״̬
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

		// �ж�ָ��״̬�Ƿ�ǰ״̬����.
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
		* �������ܣ���ǰ״ִ̬����Ϣ����
		* ��    ����
		* �� �� ֵ��
		* ��    ע��
		*************************************************************************************/		
		UINT		MsgProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			if(m_lsCurrentState.empty()) return FALSE;
			// ��״̬����״̬
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
		* �������ܣ���ǰ״ִ̬�и���״̬����
		* ��    ����
		* �� �� ֵ��
		* ��    ע��
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
		//�õ�״̬����
		StateFactory*	GetFactory( ){ return m_pFactory; }
		void			SetFactory( StateFactory* pFactory ){ m_pFactory = pFactory; }
		//��ʼ��״̬��
		static  void	InitStateTreeFromFile( const char* xmlTree ){ INSTANCE(StateTree)->InitFormFile(xmlTree); }
		static  void	InitStateTreeFromMem( const char* xmlTree ){ INSTANCE(StateTree)->InitFormMem(xmlTree); }
	private:
		StateFactory*	m_pFactory;
		int				m_newState;
		StateList		m_lsCurrentState;			// ��ǰ״̬��
	};

	/************************************************************************/
	/* �򵥵��¼�ģ���Զ�״̬��                                             */
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

	//�����¼���,�����¼����������¼�,���ǽ��¼��ȴ�������,�ٷַ���������Ķ���. �����𵽽ӿ�ת������.
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

	//�¼�����,�����Զ���ص���֧��
	template<typename T>
	class FSM
	{
	public:
		typedef T State;
		typedef CVisualFactory< T > StateFactory;
	public:
		FSM(){ mNewState=NULL; mCurrentState=NULL; m_pFactory=NULL; };
		~FSM(){ }

		//������״̬
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