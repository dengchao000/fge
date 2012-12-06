#include "StdAfx.h"
#include <fgexml.h>
#include <assert.h>
#include <fgeexception.h>
#include <fstream>
#include <fgematch.h>
#include <fgedebug.h>

namespace fge
{
	/************************************************************************/
	/*
	/************************************************************************/
	TAG::TAG()
	{
		name[0] = '\0';
		parseFun = NULL;           
	};
	TAG::TAG(const char* szTagName,PARSE func)
	{
		strcpy_s(name,szTagName);
		parseFun = func;
	};
	TAG::TAG(const TAG& t)
	{
		memcpy(name,t.name,TAG_LEN);
		parseFun = t.parseFun;
	}

	STagGroup::STagGroup( ){ 
		//AddTag("/>",&CXML::parseEndEmptyElment);
		AddTag("<![CDATA[",&CXML::parseCDataBegin);
		AddTag("<!--",&CXML::parseComment);
		AddTag("</",&CXML::parseEndElment);        
		AddTag("<?",&CXML::parseDeclaration);
		AddTag("<",&CXML::parseLf);
		//AddTag(">",&CXML::parseRf);
		//AddTag("]]>",&CXML::parseCDataEnd);
	}

	void	STagGroup::AddTag(const char* tag, PARSE func)
	{
		m_tag.push_back(TAG(tag,func));
	}
	TAG*	STagGroup::GetTagParseFunc(const char *tag)
	{
		std::list<TAG>::iterator i = m_tag.begin();
		while(i!=m_tag.end())
		{
			if(CXML::StrCmp(tag,i->name))
				return &(*i);
			i++;
		}
		return NULL;
	}

	static STagGroup	s_TagGroup;
	static	const char*	m_xmlEndTemp;
	//----------------------------------
	CXML::CXML(void)
	{
		m_head.name = "head";
		m_head.attrib.push_back( Attribute("encoding","gb2312") );
		m_root.parentElement =NULL;
		m_root.name = "AbsoluteRoot";
		m_pcr = &m_root;
	}

	CXML::~CXML(void)
	{ 
		Clear();		
	}
	void CXML::Clear()
	{
		// SAFE_DELETE_ARRAY(m_cdata);
		m_root.Clear( );
		m_root.name = "AbsoluteRoot";
		m_pcr = &m_root;
	}

	//bool CXML::SetRootElement(Element e)
	//{
	//	if(m_root.subElement.size()==0)
	//		m_root.subElement.push_back(e);
	//	else
	//		return false;
	//	return true;
	//}
	void CXML::Create(const char* filename,const char* encoding)
	{
		Clear( );
		m_fileName = filename;
		Element *pRoot = new Element();
		pRoot->name = "Root";
		m_root.AddSubElement( pRoot );
		m_head.attrib.clear( );
		m_head.attrib.push_back( Attribute("encoding",encoding) );
	}

	bool CXML::IsOpen( )
	{
		return m_root.subElement.size()==1;
	}
	bool CXML::Open(const char* filename)
	{
		Clear( );		

		std::ifstream file;
		file.open(filename,std::ios_base::in|std::ios_base::binary);
		if(!file.good())
		{
			return false;
		}

		m_fileName = filename;
		file.seekg(0,std::ios_base::end ); 
		DWORD size = file.tellg( );
		bool  result = false;
		char* buf = new char[size+1];
		//ZeroMemory(buf,size+1);
		try{
			file.seekg(0);
			file.read(buf,size);
			//file.Read(buf,size);
			m_xmlEndTemp = buf+size;
			buf[size] = '\0';
			result = Parse(buf);
		}
		catch(XMLException& e)
		{
			SAFE_DELETE_ARRAY(buf);
			file.close();
			throw e;
		}
		m_xmlEndTemp = 0;
		SAFE_DELETE_ARRAY(buf);
		file.close();
		return result;
	}
	bool CXML::OpenSXL(const char* filename)
	{
		Clear( );

		std::ifstream file;
		file.open(filename,std::ios_base::in|std::ios_base::binary);
		if(!file.good())
			return false;
		m_fileName = filename;

		file.seekg(0,std::ios_base::end ); 
		DWORD size = file.tellg( );
		bool  result = false;
		char* buf = new char[size+1];
		try{
			file.seekg(0);
			file.read(buf,size);
			m_xmlEndTemp = buf+size;
			buf[size] = '\0';
			result = ParseSXL(buf);
		}
		catch(XMLException& e)
		{
			SAFE_DELETE_ARRAY(buf);
			file.close();
			throw e;
		}
		m_xmlEndTemp = 0;
		SAFE_DELETE_ARRAY(buf);
		file.close();
		return result;
	}
	bool CXML::OpenBinary(const char* filename)
	{
		std::ifstream file;
		file.open(filename,std::ios_base::in|std::ios_base::binary);
		if(!file.good())
		{
			return false;
		}
		this->ParseBinary( file );
		file.close();
		return true;
	}
	//<
	const char* CXML::parseLf(const char* xml)//解析左括号;
	{
		char szBuf[80];
		TAG *tag = 0;

		xml+=1;//xml = SkipTo(xml,"<");
		xml = ReadWord(xml,szBuf,80,&tag);
		//TAG *tag = GetTagParseFunc(szBuf);
		if(szBuf[0])
		{
			//读取一个元素,元素可以为空元素，但是必须关闭
			Element *e = new Element;
			e->name = szBuf; 

			m_pcr->AddSubElement(e);
			m_pcr = e;//&(*(--m_pcr->subElement.end()));
			//读取属性
			xml = readAttrib(*m_pcr,xml);
			//xml == ">" || "/>"
			if(StrCmp(xml,"/>"))
				return parseEndEmptyElment(xml);
			else if(StrCmp(xml,">"))
				xml++;
			else
				throw XMLException("标签未正确关闭");

			const char* xmlNext = 0;
			const char* xmlRet = 0;
			if(StrCmp(xml,"[[data"))
			{
				xml+=strlen("[[data");
				m_pcr->bDataValue = *xml;
				sprintf_s(szBuf,"%cdata]]",*xml++);
				//xmlNext = Find(xml," data]]");
				xmlNext = Find(xml,szBuf);
				xmlRet = xmlNext + strlen("[[data ");
			}
			else
			{
				xmlRet = xmlNext = Find(xml,"<");
			}

			//读值
			if(!xmlNext)
				throw XMLException("标签未正确关闭");
			if(xmlNext > xml)
			{
				int n = (int)(xmlNext-xml);

				while(n>0 && ( *xml=='\r' || *xml=='\n' || *xml=='\t') )
				{
					xml++;
					n--;
				}
				//m_pcr->value.resize(n);
				for(int i=0; i<n; i++)
				{
					if(xml[i]!='\r')
						m_pcr->value += xml[i];
				}
				//m_pcr->value[n] = '\0';
			}
			return xmlRet;
		}
		else
			throw XMLException("无元素名或关键字错误.");
		return SkipTo(xml,">");
	}
	//?
	const char* CXML::parseDeclaration(const char* xml)
	{
		//char szBuf[80];
		//ReadWord(xml,szBuf,80);
		xml+=2;//xml = SkipTo(xml,"<?");
		char szBuf[80];
		TAG *tag = 0;
		xml = ReadWord(xml,szBuf,80,&tag);
		if(!strcmp(szBuf,"xml"))
		{
			xml = readAttrib(m_head,xml);
			if(!StrCmp(xml,"?>"))
				throw XMLException("?>标签未正确关闭");
		}
		return SkipTo(xml,"?>");
	}
	//!--
	const char* CXML::parseComment(const char* xml)
	{       
		////读值
		//xml = SkipTo(xml,"<!--");
		//const char* xmlNext = Find(xml,"-->");
		//if(!xmlNext)
		//	throw XMLException("注释未正确关闭");
		//Element* pElement = new CommentElement;
		//pElement->parentElement = m_pcr;
		//if(xmlNext > xml)
		//{
		//	size_t n = xmlNext-xml;
		//	pElement->value.append(xml,n);
		//}
		//m_pcr->subElement.push_back(pElement);
		//return SkipBlank(xmlNext+3);
		xml+=4;//SkipTo(xml,"<!--");
		return SkipTo(xml,"-->");
	}
	//解析右括号 >
	const char* CXML::parseRf(const char* xml)
	{
		//if(m_pcr)
		//    m_pcr = m_pcr->parentElement;
		//else
		//    throw XMLException(" > 不匹配.");

		return SkipTo(xml,">");
	}
	//元素结束 </...> 要求有<element> 的配对   
	const char* CXML::parseEndElment(const char* xml)
	{
		char szBuf[80];
		TAG  *tag = 0;

		xml+=2;//xml = SkipTo(xml,"</");
		xml = ReadWord(xml,szBuf,80,&tag);

		if(*xml!='>')
			throw XMLException("缺少>.");
		if(m_pcr && m_pcr->name.compare(szBuf)==0 )
			m_pcr = m_pcr->parentElement;
		else
			throw XMLException("元素不匹配.");
		return xml+1;
	}
	//元素结束 />    
	const char* CXML::parseEndEmptyElment(const char* xml)
	{
		if(m_pcr)
			m_pcr = m_pcr->parentElement;
		else
			throw XMLException("元素不匹配.");
		return SkipTo(xml,"/>");
	}

	//元素结束  <![CDATA[ 
	const char* CXML::parseCDataBegin(const char* xml)
	{
		xml = SkipTo(xml,"<![CDATA[");
		//读值
		const char* xmlNext = Find(xml,"]]>");
		if(!xmlNext)
			throw XMLException("CDATA未正确关闭");
		Element* pElement = new DataElement;		
		if(xmlNext > xml)
		{
			size_t n = xmlNext-xml;
			pElement->value.append(xml,n);
		}
		m_pcr->AddSubElement(pElement);
		return SkipBlank(xmlNext+3);
	}
	//元素结束  ]]>
	const char* CXML::parseCDataEnd(const char* xml)
	{
		return SkipTo(xml,"]]>");
	}

	//读取元素的属性 '>'结束
	const char* CXML::readAttrib(Element &e,const char* xml)
	{
		char szBuf[80];
		Attribute attrib;
		TAG     *tag = 0;
		do{
			xml = ReadWord(xml,szBuf,80,&tag);
			//if(tag)
			//    return (this->*(tag->parseFun))(xml);
			if(!szBuf[0])
				break;

			attrib.SetName( szBuf );
			xml = SkipBlank(xml);
			if(*xml != '=')
				throw XMLException("属性赋值不正确,缺少=号");
			xml++;
			xml = SkipBlank(xml);
			if(*xml != '"')
				throw XMLException("属性赋值不正确,缺少\"号");
			xml++;

			//read value
			xml = ReadText(xml,attrib.GetValue(),"\"");
			//if(!szBuf[0])
			//    throw XMLException("属性赋值不正确");
			//attrib.value=szBuf;

			//if(*xml != '"')
			//    throw XMLException("属性赋值不正确");
			xml++;
			e.attrib.push_back(attrib);
			xml = SkipBlank(xml);
		}while(1);
		return xml;
	}

	
	//解析xml文本
	bool CXML::Parse(const char* xmlText,size_t size/*=0*/)
	{
		Clear( );
		if(xmlText == 0)
			return false;
		if(size)
			m_xmlEndTemp = xmlText+size;
		else
			m_xmlEndTemp = xmlText+strlen(xmlText);

		xmlText = Find(xmlText,"<");
		if(xmlText == 0)
			throw XMLException("无标签");

		const char* msg = xmlText;
		TAG *tag = 0;   
		try{     
			while(*msg && msg<m_xmlEndTemp)
			{
				tag = 0;  
				msg = ReadTag(msg,&tag);
				if(tag == 0)
					break;
				msg = SkipBlank((this->*(tag->parseFun))(msg));
			}
		}
		catch( const XMLException& e )
		{
			if(msg<m_xmlEndTemp){
				std::string strmsg = e.what();
				size_t len = m_xmlEndTemp>msg? m_xmlEndTemp-msg : 0;
				if( tag!=0 )
				{
					if(len>80) len=80;
					strmsg += ". 在:";	
					strmsg.append(msg,len);	
					strmsg += "....附近!";
				}
				std::ofstream file_dump;
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);
				char _name[256];
				sprintf_s( _name,"%s_%d_%d_%d_.txt","xml_exception",sysTime.wHour,sysTime.wSecond,sysTime.wMinute );
				file_dump.open(_name,std::ios::out);
				if(file_dump.is_open())
					file_dump.write(xmlText,(std::streamsize)strlen(xmlText));			
				file_dump.close();

				m_xmlEndTemp = 0;
				throw XMLException(strmsg.c_str());
			}
		}

		m_xmlEndTemp = 0;

		if(m_pcr != &m_root)
			throw XMLException("标签未正确关闭");
		if(m_pcr->subElement.size()!=1)
			throw XMLException("有且只能有一个根元素");

		return true;

	}
	//解析xml,返回文档结尾
	const char* CXML::ParseEx(const char* xmlText,size_t size/*=0*/)
	{
		Clear( );
		if(xmlText == 0)
			return xmlText;
		if(size)
			m_xmlEndTemp = xmlText+size;
		else
			m_xmlEndTemp = xmlText+strlen(xmlText);

		xmlText = Find(xmlText,"<");
		if(xmlText == 0)
			return xmlText;
		const char* msg = xmlText;
		try
		{
			TAG *tag = 0;        
			while(*msg && msg<m_xmlEndTemp)
			{
				tag = 0;  
				msg = ReadTag(msg,&tag);
				if(tag == 0)
					break;
				msg = (this->*(tag->parseFun))(msg);
				if(m_pcr == &m_root)
					return msg;
			}
			if(m_pcr != &m_root)
				throw XMLException("标签未正确关闭");
			if(m_pcr->subElement.size()!=1)
				throw XMLException("有且只能有一个根元素");
		}
		catch( const XMLException& e )
		{
			if(msg<m_xmlEndTemp){
				m_xmlEndTemp = 0;
				throw e;
			}
		}
		m_xmlEndTemp = 0;

		return msg;
	}
	//-----------
	//辅助函数
	//查找
	const char* CXML::Find(const char* xml, const char* tag)
	{
		if( !xml ) return 0;
		//子串搜索
		const char* tempTag = 0;
		size_t	tag_size = strlen(tag);
		if( tag_size==1 )
		{
			while(*xml)
			{
				if(*tag==*xml)
					return xml;
				xml++;
			}
			return 0;
		}
		else
		{
			fge::CByteMatch m((const BYTE*)tag,strlen(tag));
			if(m_xmlEndTemp!=0)
				return (const char*)m.Find((const BYTE*)xml,m_xmlEndTemp-xml);
			else
				return (const char*)m.Find((const BYTE*)xml,(int)strlen(xml));
		}
	}
	//跳过
	const char* CXML::SkipTo(const char* xml, const char* tag)
	{
		if( !xml ) return 0;
		//子串搜索
		const char* tempTag = 0;
		size_t	tag_size = strlen(tag);
		if( tag_size==1 )
		{
			while(*xml)
			{
				if(*tag==*xml)
				{
					tempTag = tag;
					break;
				}
				xml++;
			}
		}
		else
		{
			fge::CByteMatch m((const BYTE*)tag,tag_size);
			if(m_xmlEndTemp!=0)
				tempTag = (const char*)m.Find((const BYTE*)xml,m_xmlEndTemp-xml);
			else
				tempTag = (const char*)m.Find((const BYTE*)xml,(int)strlen(xml));			
		}
		if(!tempTag)
			throw XMLException("标签未正确关闭");
		else
			return tempTag+tag_size;
	}
	//跳过空白
	const char* CXML::SkipBlank(const char* xml)
	{
		while(*xml)
		{
			switch(*xml)
			{
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				xml++;
				break;
			default:
				return xml;
			}
		}
		return xml;
	}
	//字符串比较
	bool CXML::StrCmp(const char* xml, const char* key)
	{
		while(*xml)
		{
			if(*key == *xml){               
				key++;
				xml++;
			}
			else
				break;
		}
		return !(*key);
	}
	//读取一个单词   
	const char* CXML::ReadWord(const char* xml,char* word,int bufsize,TAG **ppTag)
	{
		assert(bufsize>0 && word);
		char* tempWord = word;
		*tempWord = '\0';

		xml = SkipBlank(xml);
		//名称可以以字母,数字,下划线,[,]开头, 中间可以夹杂 点, 冒号, 破折号,[].
		if( xml && *xml 
			&& ( IsAlphaNum((unsigned char) *xml) 
			|| IsAlpha((unsigned char) *xml)
			|| *xml == '_'
			|| *xml == '['
			|| *xml == ']') )
		{
			while( xml && *xml
				&& ( IsAlphaNum( (unsigned char ) *xml ) 
				|| *xml == '_'
				|| *xml == '-'
				|| *xml == '.'
				|| *xml == ':'
				|| *xml == '['
				|| *xml == ']') )
			{
				*tempWord++ = *xml++;
			}
			*tempWord = '\0';
		} 

		return SkipBlank(xml);
	}
	//读取文本内容
	const char* CXML::ReadText(const char* xml,std::string &buf, char* end)
	{
		buf.clear();
		//读值
		const char* xmlNext = Find(xml,end);
		if(!xmlNext)
			throw XMLException("文本中有非法字符:<");
		if(xmlNext > xml)
		{
			buf.assign(xml, xmlNext-xml);
		}
		return xmlNext;
	}
	//读取标签
	const char* CXML::ReadTag(const char* xml,TAG **ppTag)
	{
		assert(ppTag);
		xml = Find(xml,"<");
		if(!xml)
			return 0;

		*ppTag = s_TagGroup.GetTagParseFunc(xml);
		
		return xml;
	}
	int CXML::IsAlpha( unsigned char anyByte)
	{
		if ( anyByte < 127 )
			return isalpha( anyByte );
		else
			return 1;

	}

	int CXML::IsAlphaNum( unsigned char anyByte)
	{
		if ( anyByte < 127 )
			return isalnum( anyByte );
		else
			return 1;
	}

	const char*	CXML::GetEncoding( )
	{
		return m_head.GetAttribute("encoding");
	}
	void CXML::Save( const char* encoding )
	{
		if(m_fileName.size()>0)
			Save(m_fileName.c_str(),encoding);
	}
	//保存
	void CXML::Save( )
	{
		if(m_fileName.size()>0)
			Save(m_fileName.c_str(),m_head.GetAttribute("encoding"));
	}
	void CXML::Save(const char* filename,const char* encoding)
	{ 
		Save(filename,GetRootElement(),encoding);
	}
	void CXML::Save(const char* filename,Element *root,const char* encoding)
	{
		if(!root)
			return;
		m_fileName = filename;
		std::ofstream file;
		file.open(filename);
		if(file.good() == false)
			throw XMLException("不能创建文件");

		file<<"<?xml version=\"1.0\" encoding=\""<<encoding<<"\"?>"<<std::endl;;
		root->Save( file , 0 );
		file.close();
	}
	void CXML::Save(std::ostream& stream,Element* e,const char* encoding, int level)
	{
		stream<<"<?xml version=\"1.0\" encoding=\""<<encoding<<"\"?>"<<std::endl;
		e->Save(stream,0);
	}

	void CXML::SaveBinary( const char* filename )
	{
		std::ofstream file;
		file.open(filename, std::ios_base::out|std::ios_base::binary);
		SaveBinary( file );
		file.close();
	}
	void CXML::SaveBinary( std::ostream& stream )
	{
		SaveBinary(stream,GetRootElement(),m_head.GetAttribute("encoding"),0);
	}
	void CXML::SaveBinary(std::ostream& stream,Element* e,const char* encoding, int level)
	{
		std::streamsize len = (std::streamsize)strlen( encoding );
		stream.write((const char*)&len,sizeof(std::streamsize));
		stream.write(encoding,len);
		e->SaveBinary(stream);
	}
	void CXML::ParseBinary( std::istream& stream )
	{
		this->Clear( );

		stream.seekg(0);
		std::streamsize len = 0;
		std::string value;

		stream.read((char*)&len,sizeof(std::streamsize));
		value.resize(len);
		stream.read(&value[0],len);

		m_head.attrib.push_back( Attribute("encoding",value) );

		if(stream.good())
			ParseBinary( stream, &m_root );
	}
	Element* CXML::ParseBinary( std::istream& stream, Element* parent )
	{
		Element* pE = 0;
		int type = 0;
		stream.read( (char*)&type, sizeof(type) );

		switch( type )
		{
		case TYPE_ELEMENT:
			pE = new Element;
			break;
		case TYPE_CDATA_ELEMENT:
			pE = new DataElement;
			break;
		case TYPE_COMMENT:
			pE = new CommentElement;
			break;
		default:
			return 0;
		}
		pE->ParseBinary( stream );
		parent->AddSubElement( pE );

		int num = 0;
		//读子节点
		stream.read((char*)&num,sizeof(num));
		for( int i=0; i<num; i++ )
		{
			ParseBinary(stream, pE);
		}

		return pE;
	}
	Element* CXML::GetRootElement() 
	{ 
		if(m_root.attrib.size() || m_root.subElement.size()!=1) 
			return &m_root;
		return *m_root.subElement.begin();
	}
	//执行广度优先搜索
	Element* CXML::FindElement(const char* name,Element* pe)
	{
		if(!pe)
			pe = this->GetRootElement();
		while(!findEslements.empty())
			findEslements.pop();

		while(pe && pe->name.compare(name)!=0)
		{
			std::list<Element*>::iterator i=pe->subElement.begin();
			while(i!=pe->subElement.end())
				findEslements.push(*i++);

			if(findEslements.size() == 0)
				return 0;	
			else
				pe = findEslements.front();
			findEslements.pop();
		}
		//findEslements.pop();
		return pe;
	}
	Element* CXML::FindNext(const char* name)
	{
		if(findEslements.size() == 0)
			return 0;	

		Element* pe = findEslements.front();
		findEslements.pop();
		while(pe && pe->name.compare(name)!=0)
		{
			std::list<Element*>::iterator i=pe->subElement.begin();
			while(i!=pe->subElement.end())
				findEslements.push(*i++);

			if(findEslements.size() == 0)
				return 0;	
			else
				pe = findEslements.front();
			findEslements.pop();
		}
		//findEslements.pop();
		return pe;
	}

	//---------------------------------------------------------------
	Element::~Element( )
	{
		Clear( );
	}
	Element* Element::Clone( )
	{
		Element *pe = new Element();

		pe->name = name;
		pe->value = value;
		pe->bDataValue = bDataValue;
		pe->attrib = attrib;
		pe->parentElement = parentElement;

		for(std::list<Element*>::iterator i=subElement.begin(); i!=subElement.end(); i++)
		{
			pe->AddSubElement((*i)->Clone( ));
		}

		return pe;
	}
	void Element::Clear()
	{
		bDataValue = false;
		name.clear();
		value.clear();
		attrib.clear();
		parentElement = 0;

		std::list<Element*>::iterator i = subElement.begin( );
		while( i!=subElement.end( ) )
		{
			delete *i;
			i++;
		}
		subElement.clear( );
	}
	void Element::ClearSubElement()
	{
		std::list<Element*>::iterator i = subElement.begin( );
		while( i!=subElement.end( ) )
		{
			delete *i;
			i++;
		}
		subElement.clear( );
	}
	void Element::AddSubElement(Element* pSub)
	{
		if(pSub)
		{
			subElement.push_back(pSub);
			pSub->parentElement = this;
		}
	}
	const char* Element::GetAttribute(const char* name)
	{
		int hash = fge::strHash( name );
		std::list<Attribute>::iterator i=attrib.begin();
		while(i!=attrib.end())
		{
			if(i->GetHash() == hash && i->GetName().compare(name) == 0)
				return i->GetValue().c_str();
			i++;
		}
		return 0;
	}
	//-得到属性值
	bool Element::GetAttribute(const char* name,int& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtol(pvalue,(char**)NULL,16);
			else
				buf = atoi(pvalue);
			return true;
		}				
		return false;
	}
	bool Element::GetAttribute(const char* name,unsigned int& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtoul(pvalue,(char**)NULL,16);
			else
				buf = strtoul(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetAttribute(const char* name,unsigned long& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtoul(pvalue,(char**)NULL,16);
			else
				buf = strtoul(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetAttribute(const char* name,long& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtol(pvalue,(char**)NULL,16);
			else
				buf = strtol(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetAttribute(const char* name,bool& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			buf = atoi(pvalue)!=0;
			return true;
		}				
		return false;
	}
	bool Element::GetAttribute(const char* name,float& buf)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			buf = (float)atof(pvalue);
			return true;
		}				
		return false;
	}
	bool	Element::GetAttribute(const char* name,std::string& str)
	{
		const char* pvalue = GetAttribute(name);
		if(pvalue)
		{
			str = pvalue;
			return true;
		}				
		return false;
	}

	//-得到值
	bool Element::GetValue(std::string& str)
	{
		if(value.empty())
			return false;
		str = value;
		return true;
	}
	bool Element::GetValue(int& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtol(pvalue,(char**)NULL,16);
			else
				buf = atoi(pvalue);
			return true;
		}				
		return false;
	}
	bool Element::GetValue(unsigned int& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtoul(pvalue,(char**)NULL,16);
			else
				buf = strtoul(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetValue(unsigned long& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtoul(pvalue,(char**)NULL,16);
			else
				buf = strtoul(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetValue(long& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			if(strncmp(pvalue,"0x",2)==0)
				buf = strtol(pvalue,(char**)NULL,16);
			else
				buf = strtol(pvalue,(char**)NULL,10);
			return true;
		}				
		return false;
	}
	bool Element::GetValue(bool& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			buf = atoi(pvalue)!=0;
			return true;
		}				
		return false;
	}
	bool Element::GetValue(float& buf)
	{
		const char* pvalue = value.c_str();
		if(pvalue)
		{
			buf = (float)atof(pvalue);
			return true;
		}				
		return false;
	}
	Element* Element::GetSubElement(const char* name)
	{
		if(!name)
			return 0;
		std::list<Element*> eList;
		std::list<Element*>::iterator i = subElement.begin();	//子节点节点
		while( i!= subElement.end() )
		{
			if( (*i)->name.compare(name)==0 )
				return *i;
			if( (*i)->subElement.size() )
				eList.push_back(*i);
				//return (*i)->GetSubElement(name);
			i++;
		}
		i = eList.begin();
		while( i!= eList.end() )
		{
			Element* rt = (*i)->GetSubElement(name);
			if( rt )
				return rt;
			i++;
		}
		return 0;
	}
	Element* Element::GetSubElementEx(const char* name)
	{
		if(!name)
			return 0;
		
		const char* _str = name;
		while( *_str && *_str!='.' ) _str++;

		size_t len = _str - name;
		if( len == 0 && *_str=='.' )
			return GetSubElementEx( _str );
			
		std::list<Element*>::iterator i = subElement.begin();
		while( i!= subElement.end() )
		{
			if( strncmp((*i)->name.c_str(),name,len)==0 )
			{
				if(*_str)
					return (*i)->GetSubElementEx(_str+1);
				else
					return (*i);
			}
			i++;
		}
		return 0;
	}
	//
	bool Element::SetAttribute(const char* name, const char* value)
	{
		int nameid = fge::strHash( name );
		std::list<Attribute>::iterator i=attrib.begin();
		while(i!=attrib.end())
		{
			if( nameid == i->GetHash( ) &&
				i->GetName().compare(name) == 0)
			{
				i->SetValue(value);
				return true;
			}
			i++;
		}
		attrib.push_back(Attribute(name,value));
		return false;
	}
	bool Element::SetValue(const char* value)
	{
		this->value = value;
		return true;
	}
	//保存
	void Element::Save( std::ostream& stream ,int level )
	{
		//for(int i=0; i<level; i++)	stream<<'\t';
		//写元素名
		stream<<'<'<<name;
		//写属性
		for(std::list<Attribute>::iterator i = attrib.begin(); i!=attrib.end(); i++)
		{
			stream<<' '<<i->GetName().c_str()<<"=\""<<i->GetValue().c_str()<<'"';
		}
		stream<<'>';

		//写值
		if(bDataValue)
		{
			stream<<"[[data"<<(char)bDataValue;
		}
		stream<<value;
		if(bDataValue)
		{
			stream<<(char)bDataValue<<"data]]";
		}

		//写子元素
		if(subElement.size())
		{
			//换行
			if(value.length()==0)
			{
				stream<<std::endl;
			}
			for(std::list<Element*>::iterator i = subElement.begin(); i!=subElement.end(); i++)
			{
				if(value.length()==0)
					for(int i=0; i<=level; i++)	stream<<'\t';
				(*i)->Save(stream,level+1);
				stream<<std::endl;
			}			
			for(int i=0; i<level; i++)	stream<<'\t';
		}
		//写结束标记
		stream<<"</"<<name<<'>';
	}
	void Element::SaveBinary( std::ostream& stream )
	{
		SaveTypeBinary(stream);
		stream.tellp( );
		//name,value;
		std::streamsize size = (std::streamsize)name.size( );
		stream.write((const char*)&size,sizeof(size));
		stream.write(name.c_str(),(std::streamsize)name.length());

		stream.write((const char*)&bDataValue,sizeof(bDataValue));

		size = (std::streamsize)value.size( );
		stream.write((const char*)&size,sizeof(size));
		stream.write(value.c_str(),(std::streamsize)value.length());

		//属性
		size = (std::streamsize)attrib.size( );
		stream.write((const char*)&size,sizeof(size));
		for( std::list<Attribute>::iterator i=attrib.begin(); i!=attrib.end(); i++ )
		{
			size = (std::streamsize)i->GetName().size();
			stream.write((const char*)&size,sizeof(size));
			stream.write(i->GetName().c_str(),(std::streamsize)i->GetName().length());

			size = (std::streamsize)i->GetValue().size();
			stream.write((const char*)&size,sizeof(size));
			//stream<<(int)i->value.size();
			stream.write(i->GetValue().c_str(),(std::streamsize)i->GetValue().length());
		}
		//子节点
		size = (std::streamsize)subElement.size( );
		stream.write((const char*)&size,sizeof(size));
		for( std::list<Element*>::iterator iSub=subElement.begin(); iSub!=subElement.end(); iSub++ )
		{
			(*iSub)->SaveBinary( stream );
		}
	}
	void Element::SaveTypeBinary( std::ostream& stream )
	{
		int type = TYPE_ELEMENT;
		stream.write( (const char*)&type, sizeof(type) );
	}
	void Element::ParseBinary( std::istream& stream )
	{
		std::streamsize size,num = 0;
		stream.read((char*)&size,sizeof(size));
		name.resize(size);
		stream.read(&name[0],size);
		//name[size]='\0';

		stream.read((char*)&bDataValue,sizeof(bDataValue));
		stream.read((char*)&size,sizeof(size));
		value.resize(size);
		stream.read(&value[0],size);
		//value[size]='\0';

		//属性
		stream.read((char*)&num,sizeof(num));

		std::string	name;
		std::string	value;
		for( std::streamsize i=0; i<num; i++ )
		{
			stream.read((char*)&size,sizeof(size));
			name.resize(size);
			stream.read(&name[0],size);
			//a.name[size]='\0';

			stream.read((char*)&size,sizeof(size));
			value.resize(size);
			stream.read(&value[0],size);
			//a.value[size]='\0';
			this->attrib.push_back( Attribute(name,value) );
		}
		////子节点
		//size = subElement.size( );
		//stream.write((const char*)&size,sizeof(size));
		//for( std::list<Element*>::iterator iSub=subElement.begin(); iSub!=subElement.end(); iSub++ )
		//{
		//	(*iSub)->SaveBinary( stream );
		//}
	}


	//--------------------------------------------------------
	Element* DataElement::Clone( )
	{
		Element *pe = new DataElement();

		pe->name = name;
		pe->value = value;
		pe->bDataValue = bDataValue;
		pe->attrib = attrib;
		pe->parentElement = parentElement;

		for(std::list<Element*>::iterator i=subElement.begin(); i!=subElement.end(); i++)
		{
			pe->AddSubElement( (*i)->Clone( ) );
		}

		return pe;
	}
	void DataElement::Save( std::ostream& stream ,int level )
	{
		for(int i=0; i<level; i++)	stream<<'\t';
		stream<<"<![CDATA[";
		//写值
		if(value.c_str())
		{
			stream<<value.c_str();
		}
		//写结束标记
		stream<<"]]>"<<std::endl;
	}

	void DataElement::SaveTypeBinary( std::ostream& stream )
	{
		int type = TYPE_CDATA_ELEMENT;
		stream.write( (const char*)&type, sizeof(type) );
	}
	//--------------------------------------------------------
	Element* CommentElement::Clone( )
	{
		Element *pe = new CommentElement();

		pe->name = name;
		pe->value = value;
		pe->bDataValue = bDataValue;
		pe->attrib = attrib;
		pe->parentElement = parentElement;

		for(std::list<Element*>::iterator i=subElement.begin(); i!=subElement.end(); i++)
		{
			pe->AddSubElement( (*i)->Clone( ) );
		}

		return pe;
	}
	void CommentElement::Save( std::ostream& stream ,int level )
	{
		for(int i=0; i<level; i++)	stream<<'\t';
		stream<<"<!--";
		//写值
		stream<<value.c_str();
		//写结束标记
		stream<<"-->"<<std::endl;
	}

	void CommentElement::SaveTypeBinary( std::ostream& stream )
	{
		int type = TYPE_COMMENT;
		stream.write( (const char*)&type, sizeof(type) );
	}
}