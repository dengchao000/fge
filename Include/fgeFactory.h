/*
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
���ƣ�		������
������		�Զ���������, ���һ����ϣ�������ڹ�����������ӹ�����Ԫ����IComponent�̳У�
���������빤��������������������ֻ����Ԫ�����ͣ�����ʹ��QueryComponent��ѯ
Ԫ������.
ʾ����     
//file: TestComponent.h
class TestComponent :public IComponent
{
private:
ISA(IComponent,TestComponent);
static TestComponent sTC; 
public:
TestComponent(void){FACTORY_REGISTER("TestComponent",TestComponent,sTC);}//��Ҫ
virtual ~TestComponent(void){}

virtual TestComponent* Clone() 
{
TestComponent* pComponent = new TestComponent();
return pComponent;
}
};
//file: TestComponent.cpp
TestComponent TestComponent::sTC;
**/

#pragma once
#include <fgestti.h>
#include <fgeutil.h>
#include <fgeSingleton.h>
#include <list>
#include <iostream>
namespace fge{
	//��ͨ������
	template < typename T >
	class TemplateFactory
	{
	public:
		TemplateFactory(void){}
		virtual ~TemplateFactory(void){}
	public:

		virtual bool Register(T* pComponent, const String& classid)
		{
			int _id = fge::StrHash(classid);
			_Product cmp = {_id, pComponent};
			mComponents.push_back(cmp);
			return true;
		}
		virtual bool Register(T* pComponent, const int classid )
		{
			_Product cmp = {classid, pComponent};
			mComponents.push_back(cmp);
			return true;
		}
		virtual T* CreateComponent(const String& classid)
		{
			int _id = fge::StrHash(classid);
			std::list<_Product>::iterator i = mComponents.begin();
			while(i!=mComponents.end())
			{
				if(_id == i->id)
					return i->pcm->Clone();
				i++;
			}
			return 0;
		}

	protected:
		struct _Product{
			//String name;
			int id;
			T*  pcm;			
		};
		std::list<_Product> mComponents;
	};
	//typedef TFactory<IProduct> Factory;: public fge::StaticConstructor<A>, 

	//ע���࣬��֤ÿ����ֻ��ע��һ��
	template <typename T, typename FactoryType>
	class TRegister
	{
	private:
		TRegister( T& v, const String& name )
		{	
			std::cout<<TEXT("ע������:")<<name.c_str()<<std::endl; 
			INSTANCE(FactoryType)->Register(&v,name);
		}
		TRegister( T& v, const int id )
		{	
			std::cout<<TEXT("ע������:")<<id<<std::endl; 
			INSTANCE(FactoryType)->Register(&v,id);
		}
	public:
		inline static void Register(T& v, const String& name)
		{
			static TRegister i(v,name);
		}
		inline static void Register(T& v, const int id)
		{
			static TRegister i(v,id);
		}

	};
	#define FACTORY_REGISTER(type,v,f) TRegister< type,f >::Register(v,#type);

	//����Ԫ��
	template<class T>
	class ITemplate
	{
	public:
		BASE(ITemplate);
	public:
		ITemplate() {}
		virtual ~ITemplate(){}

		virtual T*			Clone() = 0;		//��¡,���ڴ�������,
		virtual void		Copy(T* pObj) = 0;  //����,���ڸ�������.

		template< typename Y >	Y* QueryComponent(){return turnto<Y>(this);}
	};

	//--------------------------------------------------
	//����ģ��ӿ�
	template<typename T>
	struct Mould
	{
		virtual ~Mould( ){ };
		virtual T*				CreateObject() = 0;
		virtual int				GetTypeID() = 0;
		virtual const String&	GetName() = 0;		
	};

	//���󹤳���
	//T: ���������Ķ�������
	//T_Mould: ��������ʱʹ�õ�ģ��. ��ͨ�����ģ��ȥCreateObject[Clone]
	template < typename T, typename T_Mould=Mould< T > >
	class CVisualFactory
	{
	public:
		typedef T			ObjectType;	
		typedef T_Mould		MouldType;	
	public:
		CVisualFactory(void){}
		virtual ~CVisualFactory(void){ Clear(); }


		template <typename ImplType>
		struct ImplMould :public T_Mould
		{
		public:
			ImplMould( ){ id = -1; }
			ImplMould( const String& _name ){ id = fge::StrHash(_name); name = _name; }
			ImplMould( int _id ){ this->id = _id;	}
			virtual ~ImplMould( ){ };

			virtual T*				CreateObject( ){ return new ImplType();}			
			virtual int				GetTypeID( ){ return id; }
			virtual const String&	GetName( ){ return name; }		

		private:
			String		name;
			int			id;	
		};
	public:
		// ע�����ģ��
		virtual bool Register( T_Mould* pMould )
		{
			mComponents[pMould->GetTypeID()] = pMould;
			return true;
		}
		// ��������
		virtual T* CreateComponent( const String& className )
		{
			return CreateComponent( fge::StrHash(className) );
		}
		// ��������
		virtual T* CreateComponent(int type)
		{
			// ���Ҷ���ģ��
			std::map<int, _Product >::iterator i = mComponents.find(type);
			if(i!=mComponents.end())
			{
				// ʹ��ģ��ȥ����(��¡)����
				return ((_Product)i->second)->CreateObject();
			}
			return 0;
		}
		virtual void DestroyComponent( T* pObj )
		{
			if( pObj==0 ) return;
			delete pObj;
			pObj = 0;
		}	

	private:
		// ��չ���ʹ�õ�ģ��
		void	Clear( )
		{
			std::map<int, _Product >::iterator i = mComponents.begin();
			while(i!=mComponents.end())
			{
				delete i->second;
				i++;
			}
			mComponents.clear( );
		}

	protected:
		typedef T_Mould*	_Product;		

		std::map<int, _Product > mComponents;	//�������ģ��
	};
}