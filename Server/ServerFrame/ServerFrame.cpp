#include "StdAfx.h"
#include "serverFrame.h"
#include "./luaScript.h"
#include "./netMsgListener.h"
#include <fgedebug.h>

using namespace protocol;
using namespace protocol_name;
using namespace luaHelp;
using namespace serverframe;

ConnectionID	currentClientID;
int L_SendMsg( lua_State* L )
{
	int n = lua_gettop(L);
	int len = 0;
	const char* pstream = lua::Serialize::Pack(L,1,n,len);
	if(pstream){
		if(currentClientID.nID){
			NetMsg msg;
			msg.msg_id = protocol_name::SCRIPT_MSG;
			msg.msg_len = sizeof(NetMsg)+len;
			msg.msg_num = 1;
			fge::DataBuf pack;
			pack.Write(msg);
			pack.Write(pstream,len);
			Net::SendToClient(pack(),pack.GetLen(),currentClientID);
		}
	}
	return 0;
}
void RegToLua()
{
	luaL_Reg xNet[] = {
		{"SendMsg",L_SendMsg},
		{NULL, NULL}
	};
	INSTANCE(LuaScript)->Register("net", xNet);
}

CServerFrame::CServerFrame(void)
{
}

CServerFrame::~CServerFrame(void)
{
}
void CServerFrame::OnInstall()
{
	m_msgQueue.Init(10000,0);
	fge::CXML xmlServer;
	try
	{
		std::string strIp="127.0.0.1";
		int nPort=9001;
		std::string strUpdateIp="127.0.0.1";
		int nUpdatePort=9002;
		if(xmlServer.OpenSXL("./config.txt"))
		{
			fge::Element* pelement = xmlServer.FindElement("net");
			if( pelement )
			{
				pelement->GetAttribute("ip",strIp);
				pelement->GetAttribute("port",nPort);
			}		
		}

		m_serverConn.AddListener(new CNetMsgListener());
		m_serverConn.Open(strIp.c_str(),nPort,0);

		m_updateConn.AddListener(new CUpdateMsgListener());
		if(!strUpdateIp.empty())
			m_updateConn.Open(strUpdateIp.c_str(),nUpdatePort);

		Net::s_pNetGameServer = &m_serverConn;
		Net::s_pNetUpdateServer = &m_updateConn;
		printf("服务器启动成功\n");
	}
	catch (const std::exception& e)
	{
		std::cout<<e.what()<<std::endl;
		fge::CLog::Write(e.what());
	}
	RegToLua( );
	INSTANCE(LuaScript)->state->DoFile("main.lua");
}
void CServerFrame::Update()
{
	INSTANCE(fge::CClock)->Update( );	
	m_plugMag.Update();
}