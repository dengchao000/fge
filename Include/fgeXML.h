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
*  版本	 :	1.1
描述： 分析XML文件,可以对大部分语法进行有效的分析。 CDATA目前只支持全局唯一。
可以通过 AddTag 接口方便的扩展新的标签。 目前支持的标签有
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
	typedef const char* (CXML::* PARSE)(const char* xml);     //解析函数
	//标签结构
	struct	TAG
	{
		char name[TAG_LEN];//标签名称
		PARSE parseFun;    //解析函数   

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
		std::string			 name;			//节点名	
		std::string          value;         //值
		std::list<Attribute> attrib;		//属性节点
		std::list<Element*>	 subElement;	//子节点节点
		Element*			 parentElement;	//父节点
		int					 bDataValue;	//值为数据[[data ??? data]]	

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
	//数据结点,不用解析内容
	class	DataElement :public Element
	{
	public:
		virtual	void	Save( std::ostream& stream ,int level );
		virtual Element* Clone( );
	protected:
		virtual void	SaveTypeBinary( std::ostream& stream );
	};
	//注释结点
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

		bool			Open(const char* filename);		//打开文本xml文件
		bool			OpenSXL(const char* filename);
		bool			OpenBinary(const char* filename);//打开2进制xml文件
		bool			Parse(const char* xmlText,size_t size=0);		//解析xml
		const char*		ParseEx(const char* xmlText,size_t size=0);		//解析xmlText中的xml文档部分. 返回指向解析结束的位置.
		bool			ParseSXL(const char* xmlText);	//解析简化xml语言
		bool			IsOpen( );						//返回文件是否已经打开	

		//获取文档根结点
		Element*        GetRootElement();
		Element&		GetRootRoot()	 { return m_root; }
		//bool			SetRootElement(Element e);
		//执行广度优先搜索
		Element*		FindElement(const char* name,Element* pe=0);
		//查找下一下结点
		Element*		FindNext(const char* name);

		//创建一个xml文档
		void            Create(const char* filename,const char* encoding);

		/*----------------------------------------------------------------------*/
		/* 保存文档
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
		/* 获取结点的值
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
		/* 解析辅助函数
		/*----------------------------------------------------------------------*/
		static const char*		Find(const char* xml, const char* tag);
		static const char*		SkipTo(const char* xml, const char* tag);
		static const char*		SkipBlank(const char* xml);
		static bool				StrCmp(const char* xml, const char* key);
		static const char*		ReadWord(const char* xml,char* word,int bufsize,TAG **ppTag=0);   //pTag:用于返回关键字
		static const char*		ReadText(const char* xml,std::string &buf, char* end);
		static const char*		ReadTag(const char* xml,TAG **ppTag);                             //读取标签
		static int				IsAlpha( unsigned char anyByte);
		static int				IsAlphaNum( unsigned char anyByte);

	protected:
		/*----------------------------------------------------------------------*/
		/* 解析函数
		/*----------------------------------------------------------------------*/
		const char*		parseLf(const char* xml);//解析左括号         <                
		const char*		parseComment(const char* xml);//解析注释      !--
		const char*		parseDeclaration(const char* xml);//          ?
		const char*		parseRf(const char* xml);//解析右括号         >
		const char*		parseEndEmptyElment(const char* xml);//元素结束    /> 
		const char*		parseEndElment(const char* xml);//元素结束    </    
		const char*		parseCDataBegin(const char* xml);//元素结束   <![CDATA[ 
		const char*		parseCDataEnd(const char* xml);//元素结束     ]]> 

		const char*		readAttrib(Element &e, const char* xml);


		Element*		ParseBinary( std::istream& stream, Element* parent );

	protected:		
		std::string		m_fileName;
		Element	        m_root; //根路径
		Element	        m_head; //文件头
		Element*	    m_pcr;  //工作路径		
		std::queue<Element*> findEslements;
	};
}