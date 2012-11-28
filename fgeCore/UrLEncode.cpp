#include "URLEncode.h"

CString CURLEncode::Convert( const CString strData )
{
	CString strResult = "";

	for( int i = 0; i < strData.GetLength(); i++ ) 
	{
		char c = strData[i];

		if( ( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c == '/' || c == '.' ) 
		{
			strResult += c;
		}
		else
		{
			int j = (int)c;
			if( j < 0 ) 
			{
				j += 256;
			}
			
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			strResult += '%';
			strResult += dec2hexChar( i1 );
			strResult += dec2hexChar( i0 ); 
		}
	}

	return strResult;
}

char CURLEncode::dec2hexChar( int n ) 
{
	if ( 0 <= n && n <= 9 )
		return char( short('0') + n );
	else if ( 10 <= n && n <= 15 ) 
		return char( short('A') + n - 10 );

	return 0;
}