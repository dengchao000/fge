/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2012.12.1
** 描述:	通过host_name获取IP地址
*/
#include "stdafx.h"
#include <WinSock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
namespace ocl{
class CDns
{
public:
	CDns()
	{
		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
		}

	}
	~CDns()
	{
	}
	std::string GetHostByName(const char * host_name)
	{
		std::string strIP="";
		if(!host_name)
			return strIP;
		char szError[300];
		DWORD dwError;
		
		struct hostent *remoteHost;

		sprintf_s(szError,"Calling gethostbyname with %s\n", host_name);
		remoteHost = gethostbyname(host_name);

		if (remoteHost == NULL) {
			dwError = WSAGetLastError();
			if (dwError != 0) {
				if (dwError == WSAHOST_NOT_FOUND) {
					OutputDebugStr("Host not found\n");
					return strIP;
				} else if (dwError == WSANO_DATA) {
					OutputDebugStr("No data record found\n");
					return strIP;
				} else {
					sprintf_s(szError,"Function failed with error: %ld\n", dwError);
					OutputDebugStr(szError);
					return strIP;
				}
			}
		} 
		else
		{
			struct in_addr addr;
			int i = 0;
			if (remoteHost->h_addrtype == AF_INET)
			{
				while (remoteHost->h_addr_list[i] != 0) {
					addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
					sprintf_s(szError,"\tIP Address #%d: %s\n", i, inet_ntoa(addr));
					OutputDebugStr(szError);
					strIP = inet_ntoa(addr);
					break;
				}
			}
			else if (remoteHost->h_addrtype == AF_NETBIOS)
			{   
				OutputDebugStr("NETBIOS address was returned\n");
			}   
		}
		return strIP;
	}
};
}