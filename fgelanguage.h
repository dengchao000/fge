/**
多语言相关处理
Dengchao. 2012.11.24
*/

#include <string>
namespace fge
{
	typedef std::wstring	tstring;

	typedef wchar_t wchar;
	typedef wchar *PWCHAR, *LPWCH, *PWCH;
	typedef const wchar    *LPCWSTR, *PCWSTR;
	
	LPCWSTR _t_gbk(LPCWSTR pstr){ return pstr; }
}
#define _t(s) fge::_t_gbk(L#s)