/*-------------------------------------------------------------*/
/*- Author:		Dengchao
/*- Create:		2010.8.16
/*-------------------------------------------------------------*/

#include "StdAfx.h"
#include "fgexml.h"
#include <assert.h>
#include <fgeexception.h>
#include <fstream>
#include <fgematch.h>
#include <fgedebug.h>
#include <fgeLog.h>

using namespace fge;
namespace xsl{	
//Ã¯π˝ø’∞◊
const char* SkipBlank(const char* pstr)
{
	while(*pstr)
	{
		switch(*pstr)
		{
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			pstr++;
			break;
		default:
			return pstr;
		}
	}
	return pstr;
}
const char* SkipSeparator(const char* pstr)
{
	while( pstr && *pstr)
	{	
		switch(*pstr)
		{
		case ' ':
		case ',':
		case ';':
		case '\n':
		case '\r':
		case '\t':
			pstr++;
			break;
		default:
			return pstr;
		}
	}
	return pstr;
}

const char* ReadText(const char* pstr,std::string &buf)
{	
	if(!pstr)
		return 0;
	buf="";
	pstr = SkipBlank(pstr);
	if( *pstr == '\'' )
	{
		pstr++;
		while( *pstr )
		{	
			if('\'' == *pstr){
				char c = *(pstr+1);
				if( c == ',' ||	c == ';' ||	c == ' ' ||	c == '(' ||	c == ')' ||	
					c == '=' ||	c == '{' ||	c == '}' ||
					c == '\r'||	c == '\n'|| c == '\t'
					)
					return pstr+1;
			}
			buf += *pstr++;
		}
	}
	else
	{
		while( *pstr)
		{	
			if( *pstr == ',' ||	*pstr == ';' ||	*pstr == ' ' ||	*pstr == '(' ||	*pstr == ')' ||	
				*pstr == '=' ||	*pstr == '{' ||	*pstr == '}' ||
				*pstr == '\r'||	*pstr == '\n'|| *pstr == '\t'
				)
				break;
			buf += *pstr++;
		}
	}
	return pstr;
}
const char* _ParseCmd( const char* as, Element& rt, int x=0 )
{
	int attribID = 0;
	const char *pstr = SkipBlank(as);
	std::string buf;
	Attribute attrib;
	int op(0);
	while( pstr && *pstr )
	{
		pstr = ReadText(pstr,buf);
		if(buf.compare(0,2,"--")==0)
		{
			while( pstr && *pstr )
			{
				if(*pstr=='\r') break;
				if(*pstr=='\n') break;
				pstr++;
			}
			pstr = SkipSeparator(pstr);
			continue;
		}
		pstr = SkipBlank(pstr);
		if(buf.empty())
		{
			if(!(pstr && *pstr!=')' && *pstr!='}'))
				return pstr;
			//else
				//throw std::exception("»±…Ÿ±Í ∂∑˚");
		}		
		char c = *pstr;
		if( op==0 )
		{
			if(c == '='){
				pstr = SkipBlank(pstr+1);
				if( *pstr == '(' ||  *pstr == '{'){
					Element* pele = new Element();
					pele->name=buf;
					pstr = _ParseCmd(pstr+1,*pele,x+1);
					if(!pstr || (*pstr!=')' && *pstr!='}') )
						throw std::exception("¿®∫≈≤ª∆•≈‰");
					rt.AddSubElement(pele);
					pstr++;
				}
				else
				{
					attrib.SetName(buf.c_str());
					op = 1;					
				}
			}
			else if(c == '(' || c == '{'){
				Element* pele = new Element();
				pele->name=buf;
				pstr = _ParseCmd(pstr+1,*pele,x+1);
				if(!pstr || (*pstr!=')' && *pstr!='}') )
					throw std::exception("¿®∫≈≤ª∆•≈‰");
				rt.AddSubElement(pele);
				pstr++;
			}
			else if(c == ')' || c=='}'){
				if( rt.name.size() && attribID==0 )
					rt.value = buf;
				else{
					attribID++;
					char szNameBuf[64];
					sprintf_s(szNameBuf,"[%d]",attribID);
					rt.attrib.push_back(Attribute(szNameBuf,buf));
				}
				return pstr;
			}
			else{
				if( rt.name.size()&& c!=',' )
					rt.value = buf;
				else{					
					attribID++;
					char szNameBuf[64];
					sprintf_s(szNameBuf,"[%d]",attribID);
					rt.attrib.push_back(Attribute(szNameBuf,buf));
				}
			}
		}
		else if( op==1 )
		{
			attribID++;
			attrib.SetValue(buf.c_str());
			rt.attrib.push_back(attrib);
			op = 0;
		}

		if(c == ')' || c=='}')
			return pstr;

		if(*pstr)
			pstr = SkipSeparator(pstr);
		else
			return pstr;
	}
	return pstr;
}

}
using namespace xsl;

bool CXML::ParseSXL( const char* xmlText )
{
	Clear( );
	Element* root = &m_root;
	try
	{
		const char* pstr = _ParseCmd(xmlText,*root);
	}
	catch( std::exception & e )
	{
		log_error(e.what());
		return false;
	}
	//m_root.AddSubElement(root);
	return true;
}

