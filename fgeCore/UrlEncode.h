#pragma once

#pragma warning( disable: 4786 )
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

/*
// 测试
int main( void )
{
    string str( 255, ' ' );
    for( string::value_type i=0; i<str.length(); ++i ) str[i] = i+1;
    string en = post_encode( str );
    const pair<bool,string> ret = post_decode( en );
    cout << boolalpha << ( ret.first && str==ret.second ) << endl;

    string str2 = "a1=b1&a2=b2&a3=b3&a4=b4%2B";
    const pair<bool,map<string,string> > params = post_decodes( str2.begin(), str2.end() );
    cout << boolalpha << params.first << endl;
    for( map<string,string>::const_iterator itor=params.second.begin(); itor!=params.second.end(); ++itor )
    {
        cout << itor->first << '\t' << itor->second << '\n';
    }

    system( "PAUSE" );
    return 0;
}

*/
// 编码
const string post_encode( const string::const_iterator& begin, const string::const_iterator& end )
{
    string ret;
    ret.reserve( distance(begin,end)*3 );

    for( string::const_iterator p=begin; p!=end; ++p )
    {
        if( *p == '+' )
            ret += "%2B";
		else if ( *p == ',' )
			ret += "%2C";
		else if ( *p == '|' )
			ret += "%7C";
        else if( *p == ' ' )
            ret += "%20";
        else if( *p == '%' )
            ret += "%25";
        else if( *p == '=' )
            ret += "%3D";
        else if( *p == '&' )
            ret += "%26";
        else if( *p<33 || *p>127 )
        {
            char tmp = 0;
            ret += '%';
            tmp = ((*p>>4)&0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  ret += tmp;
            tmp = ( *p    &0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  ret += tmp;
        }
        else
            ret += *p;
    }

    return ret;
}
const string post_encode( const string& scr )
{
    return post_encode( scr.begin(), scr.end() );
}

// 解码
const pair<bool,string> post_decode( const string::const_iterator& begin, const string::const_iterator& end )
{
    pair<bool,string> ret;
    ret.first = false;
    string& rs = ret.second;
    rs.reserve( distance(begin,end) );

    for( string::const_iterator p=begin; p!=end; ++p )
    {
        // 为了速度，这里忽略合法性验证
        if( *p == '+' )
            rs += ' ';
        else if( *p == '%' )
        {
            ++p;  if( p == end ) return ret;  char c1 = *p - '0'; if( c1 > 9 ) c1 -= 7;
            ++p;  if( p == end ) return ret;  char c2 = *p - '0'; if( c2 > 9 ) c2 -= 7;
            rs += ( (c1<<4) | c2 );
        }
        else
            rs += *p;
    }
    ret.first = true;
    return ret;
}
const pair<bool,string> post_decode( const string& scr )
{
    return post_decode( scr.begin(), scr.end() );
}

// 对整个post进行解码
const pair<bool,map<string,string> > post_decodes( const string::const_iterator& begin, const string::const_iterator& end )
{
    pair<bool,map<string,string> > ret;
    ret.first = false;
    map<string,string>& hs = ret.second;

    string::const_iterator p=begin,p1=begin,p2=begin;
    for( ; p!=end; ++p )
    {
        if( *p == '=' )
        {
            p2 = p;
        }
        else if( *p == '&' )
        {
            if( p1 >= p2 ) // error
                return ret;
            const pair<bool,string> _key = post_decode( p1, p2 );
            if( !_key.first || _key.second.empty() ) return ret;
            ++p2;
            const pair<bool,string> _val = post_decode( p2, p );
            if( !_val.first ) return ret;
            p1 = p;
            ++p1;

            hs[ _key.second ] = _val.second;
        }
    }
    if( p2 != begin )
    {
        if( p1 >= p2 ) // error
                return ret;
        const pair<bool,string> _key = post_decode( p1, p2 );
        if( !_key.first || _key.second.empty() ) return ret;
        ++p2;
        const pair<bool,string> _val = post_decode( p2, p );
        if( !_val.first ) return ret;

        hs[ _key.second ] = _val.second;
    }
    ret.first = true;
    return ret;
}
const pair<bool,map<string,string> > post_decodes( const string& scr )
{
    return post_decodes( scr.begin(), scr.end() );
}



