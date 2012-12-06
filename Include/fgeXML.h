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
*  �汾	 :	1.1
������ ����XML�ļ�,���ԶԴ󲿷��﷨������Ч�ķ����� CDATAĿǰֻ֧��ȫ��Ψһ��
����ͨ�� AddTag �ӿڷ������չ�µı�ǩ�� Ŀǰ֧�ֵı�ǩ��
</ 
<!--
<?
<
<![CDATA[
**/

#pragma once
#include <list>
#include <queue>
#include <fgeutil.h>
namespace fge
{
#define xml_vertion 0x01
	class CXML;
	enum{ TAG_LEN = 80 };
	enum{ TYPE_ELEMENT=1,TYPE_CDATA_ELEMENT, TYPE_COMMENT };
	typedef const char* (CXML::* PARSE)(const char* xml);     //��������
	//��ǩ�ṹ
	struct	TAG
	{
		char name[TAG_LEN];//��ǩ����
		PARSE parseFun;    //��������   

		TAG();
		TAG(const char* szTagName,PARSE func);
		TAG(const TAG& t);
	};
	class STagGroup{
	public:
		STagGroup( );		
		void	        AddTag(const char* tag, PARSE func);
		TAG*	        GetTagParseFunc(const char *tag);
	private:
		std::list<TAG>  m_tag;	//
	};

	class	XMLException :public std::exception
	{
	public:
		XMLException(const char* msg) :std::exception(msg) {}
	};

	struct	Attribute
	{
	private:
		std::string	name;
		std::string	value;
		int hash;
	public:
		Attribute() { hash=0; }
		Attribute( const char* _name, const char* _value ){
			name = _name;value = _value;
			hash = fge::strHash( _name );
		}
		Attribute( const  std::string _name, const  std::string _value ){
			name = _name;value = _value;
			hash = fge::strHash( _name.c_str() );
		}
		void	SetName( const char* _name )	{name = _name;	hash = fge::strHash( _name );}
		void	SetValue( const char* _value )	{	value = _value;	}

		const std::string& GetName( )	{ return name; }
		std::string&	  GetValue( )	{ return value; }
		int				  GetHash( )	{ return hash; }	
	};

	struct	Element
	{
		std::string			 name;			//�ڵ���	
		std::string          value;         //ֵ
		std::list<Attribute> attrib;		//���Խڵ�
		std::list<Element*>	 subElement;	//�ӽڵ�ڵ�
		Element*			 parentElement;	//���ڵ�
		int					 bDataValue;	//ֵΪ����[[data ??? data]]	

		typedef std::list<Attribute>::iterator attrib_iterator;
		typedef std::list<Element*>::iterator element_iterator;

		Element( ){
			parentElement = 0;
			bDataValue = 0;
		}
		~Element( );

		void			Clear();
		void			ClearSubElement();
		void			AddSubElement(Element* pSub);
		const char*		GetAttribute(const char* name);		
		bool			GetAttribute(const char* name,int& buf);
		bool			GetAttribute(const char* name,unsigned int& buf);
		bool			GetAttribute(const char* name,unsigned long& buf);
		bool			GetAttribute(const char* name,long& buf);
		bool			GetAttribute(const char* name,bool& buf);
		bool			GetAttribute(const char* name,float& buf);
		bool			GetAttribute(const char* name,std::string& str);

		const char*		GetValue( ){return value.c_str();}
		bool			GetValue(std::string& str);
		bool			GetValue(int& buf);
		bool			GetValue(unsigned int& buf);
		bool			GetValue(unsigned long& buf);
		bool			GetValue(long& buf);
		bool			GetValue(bool& buf);
		bool			GetValue(float& buf);

		Element*		GetSubElement(const char* name);
		Element*		GetSubElementEx(const char* name);
		template<class T>
		bool			GetSubElementValue( char* name, T& buf, Element* pe=0)
		{
			Element* e = GetSubElement(name);
			if(e==0) return false;
			return e->GetValue(buf);
		}

		bool			SetAttribute(const char* name, const char* value);
		bool			SetValue(const char* value);

		virtual	void	Save( std::ostream& stream,int level );
		virtual void	SaveBinary( std::ostream& stream );
		virtual void	ParseBinary( std::istream& stream );

		virtual Element* Clone( );

	private:
		Element& operator= ( const Element& e )	{ return *this; }
		Element( const Element& )				{ }

	protected:
		virtual void	SaveTypeBinary( std::ostream& stream );

	};
	//���ݽ��,���ý�������
	class	DataElement :public Element
	{
	public:
		virtual	void	Save( std::ostream& stream ,int level );
		virtual Element* Clone( );
	protected:
		virtual void	SaveTypeBinary( std::ostream& stream );
	};
	//ע�ͽ��
	class	CommentElement :public Element
	{
	public:
		virtual	void	Save( std::ostream& stream ,int level );
		virtual Element* Clone( );
	protected:
		virtual void	SaveTypeBinary( std::ostream& stream );
	};

	//--------------------------------------
	class	CXML
	{
		friend class STagGroup;
	public:
		CXML(void);
		virtual ~CXML(void);
	public:

		bool			Open(const char* filename);		//���ı�xml�ļ�
		bool			OpenSXL(const char* filename);
		bool			OpenBinary(const char* filename);//��2����xml�ļ�
		bool			Parse(const char* xmlText,size_t size=0);		//����xml
		const char*		ParseEx(const char* xmlText,size_t size=0);		//����xmlText�е�xml�ĵ�����. ����ָ�����������λ��.
		bool			ParseSXL(const char* xmlText);	//������xml����
		bool			IsOpen( );						//�����ļ��Ƿ��Ѿ���	

		//��ȡ�ĵ������
		Element*        GetRootElement();
		Element&		GetRootRoot()	 { return m_root; }
		//bool			SetRootElement(Element e);
		//ִ�й����������
		Element*		FindElement(const char* name,Element* pe=0);
		//������һ�½��
		Element*		FindNext(const char* name);

		//����һ��xml�ĵ�
		void            Create(const char* filename,const char* encoding);

		/*----------------------------------------------------------------------*/
		/* �����ĵ�
		/*----------------------------------------------------------------------*/
		const char*		GetEncoding( );
		void			Save( );
		void			Save(const char* encoding);
		void            Save(const char* filename,const char* encoding);
		void            Save(const char* filename,Element *root,const char* encoding);
		void            Save(std::ostream& stream,Element* e,const char* encoding, int level=0);
		void            SaveBinary(std::ostream& stream,Element* e,const char* encoding, int level=0);
		void			SaveBinary( const char* filename );
		void			SaveBinary( std::ostream& stream );
		void			ParseBinary( std::istream& stream );
		void            Clear();

		/*----------------------------------------------------------------------*/
		/* ��ȡ����ֵ
		/*----------------------------------------------------------------------*/
		template<class T>
		bool			GetElementValue( char* name, T& buf, Element* pe=0)
		{
			Element* e = FindElement(name,pe);
			if(e==0)
				return false;
			return e->GetValue(buf);
		}

		const char*		GetElementValue( char* name,Element* pe=0)
		{
			Element* e = FindElement(name,pe);
			if(e==0)
				return false;
			return e->GetValue( );
		}

		template<class T>
		bool			GetElementValueNext( char* name, T& buf)
		{
			Element* e = FindNext(name);
			if(e==0)
				return false;
			return e->GetValue(buf);
		}
	public:
		/*----------------------------------------------------------------------*/
		/* ������������
		/*----------------------------------------------------------------------*/
		static const char*		Find(const char* xml, const char* tag);
		static const char*		SkipTo(const char* xml, const char* tag);
		static const char*		SkipBlank(const char* xml);
		static bool				StrCmp(const char* xml, const char* key);
		static const char*		ReadWord(const char* xml,char* word,int bufsize,TAG **ppTag=0);   //pTag:���ڷ��عؼ���
		static const char*		ReadText(const char* xml,std::string &buf, char* end);
		static const char*		ReadTag(const char* xml,TAG **ppTag);                             //��ȡ��ǩ
		static int				IsAlpha( unsigned char anyByte);
		static int				IsAlphaNum( unsigned char anyByte);

	protected:
		/*----------------------------------------------------------------------*/
		/* ��������
		/*----------------------------------------------------------------------*/
		const char*		parseLf(const char* xml);//����������         <                
		const char*		parseComment(const char* xml);//����ע��      !--
		const char*		parseDeclaration(const char* xml);//          ?
		const char*		parseRf(const char* xml);//����������         >
		const char*		parseEndEmptyElment(const char* xml);//Ԫ�ؽ���    /> 
		const char*		parseEndElment(const char* xml);//Ԫ�ؽ���    </    
		const char*		parseCDataBegin(const char* xml);//Ԫ�ؽ���   <![CDATA[ 
		const char*		parseCDataEnd(const char* xml);//Ԫ�ؽ���     ]]> 

		const char*		readAttrib(Element &e, const char* xml);


		Element*		ParseBinary( std::istream& stream, Element* parent );

	protected:		
		std::string		m_fileName;
		Element	        m_root; //��·��
		Element	        m_head; //�ļ�ͷ
		Element*	    m_pcr;  //����·��		
		std::queue<Element*> findEslements;
	};
}