#include "StdAfx.h"
#include "ServerApp.h"
#include <fgedebug.h>

using namespace protocol;
using namespace protocol_name;
using namespace serverframe;

//ConnectionID	currentClientID;
//int L_SendMsg( lua_State* L )
//{
//	int n = lua_gettop(L);
//	int len = 0;
//	const char* pstream = lua::Serialize::Pack(L,1,n,len);
//	if(pstream){
//		if(currentClientID.nID){
//			NetMsg msg;
//			msg.msg_id = protocol_name::SCRIPT_MSG;
//			msg.msg_len = sizeof(NetMsg)+len;
//			msg.msg_num = 1;
//			fge::DataBuf pack;
//			pack.Write(msg);
//			pack.Write(pstream,len);
//			Net::SendToClient(pack(),pack.GetLen(),currentClientID);
//		}
//	}
//	return 0;
//}
//void RegToLua()
//{
//	luaL_Reg xNet[] = {
//		{"SendMsg",L_SendMsg},
//		{NULL, NULL}
//	};
//	INSTANCE(LuaScript)->Register("net", xNet);
//}

ServerApp::ServerApp(void)
{
}

ServerApp::~ServerApp(void)
{
}
void ServerApp::OnInstall()
{
	CBaseService::OnInstall();

	std::string strIp_login="127.0.0.1";
	int nPort_login=7116;
	std::string strIp_gm="127.0.0.1";
	int nPort_gm=7117;
	std::string strIp_client="127.0.0.1";
	int nPort_client=7118;

	fge::CXML xmlServer;

	try
	{
		std::string strUpdateIp="127.0.0.1";
		if(xmlServer.OpenSXL("./config.txt"))
		{
			fge::Element* pelement = xmlServer.FindElement("GameServer");
			if( pelement )
			{
				pelement->GetAttribute("ip",strIp_client);
				pelement->GetAttribute("port",nPort_client);
			}
			pelement = xmlServer.FindElement("GmServer");
			if( pelement )
			{
				pelement->GetAttribute("ip",strIp_gm);
				pelement->GetAttribute("port",nPort_gm);
			}
			pelement = xmlServer.FindElement("LoginServer");
			if( pelement )
			{
				pelement->GetAttribute("ip",strIp_login);
				pelement->GetAttribute("port",nPort_login);
			}
		}

		//
		TcpServer* pNetClient = new TcpServer();
		pNetClient->AddListener(new ClientListener());
		pNetClient->Open(strIp_login,nPort_login,0);

		TcpServer* pNetGm = new TcpServer();
		pNetGm->AddListener(new GmListener());
		pNetGm->Open(strIp_gm,nPort_gm,0);

		TcpClient *pNetLogin = new TcpClient();
		pNetLogin->AddListener(new LoginListener());
		pNetLogin->Open(strIp_client,strIp_login);

		Net::s_pNetGm = pNetGm;
		Net::s_pNetClient = pNetClient;
		Net::s_pNetLogin = pNetLogin;
	}
	catch (const std::exception& e)
	{
		std::cout<<e.what()<<std::endl;
		fge::CLog::Write(e.what());
		return;
	}

	log_warn("服务器启动成功\n");

	//RegToLua( );
	//INSTANCE(LuaScript)->state->DoFile("main.lua");
}
void ServerApp::Update()
{
	INSTANCE(fge::CClock)->Update( );	
	m_plugMag.Update();
}
void ServerApp::OnUninstall()
{
	if(Net::s_pNetGm)
		Net::s_pNetGm->Close();
	if(Net::s_pNetClient)
		Net::s_pNetClient->Close();
	if(Net::s_pNetLogin)
		Net::s_pNetLogin->Close();

	SAFE_DELETE( Net::s_pNetGm );
	SAFE_DELETE( Net::s_pNetClient );
	SAFE_DELETE( Net::s_pNetLogin );
}