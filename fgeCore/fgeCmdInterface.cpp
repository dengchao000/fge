#include "StdAfx.h"
#include <fgecmdinterface.h>
#include <fgedebug.h>
using namespace fge;

S_CMDINFO::S_CMDINFO(char* szCmdName,char* szCmdHelp,CmdEvent::Event* cmdEvent)
{
	if(szCmdName)
		strcpy_s(this->szCmdName,szCmdName);
	else
		szCmdName[0] = '\0';
	if(szCmdHelp)
		strcpy_s(this->szCmdHelp,szCmdHelp);
	else
		szCmdHelp[0] = '\0';
	if(cmdEvent)
		this->cdmFun += cmdEvent;
};

CCmdInterface::CCmdInterface(void)
{
	//		//检测内存泄漏
	//#if defined(WIN32) && defined(_DEBUG) 
	//		int tmpDbgFlag;
	//
	//		tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	//		tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	//		tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	//		_CrtSetDbgFlag(tmpDbgFlag);			//设置内存泄漏跟踪标志
	//#endif //WIN32 & _DEBUG

	m_bRun = false;
	m_icmdcount_param = 0;
	AddCommand("help","列出所有命令",CEvent(this,&CCmdInterface::Help));
	AddCommand("quit","退出",CEvent(this,&CCmdInterface::Quit));

	for(int i=0; i<PARAM; i++)
	{
		m_args.argv[i] = new char[CMD_LEN+1]; 
		m_args.argv[i][CMD_LEN] = '\0';
	}
	strcpy_s(m_name,"Server");
}

CCmdInterface::~CCmdInterface(void)
{	
	for(int i=0; i<PARAM; i++)
		SAFE_DELETE_ARRAY(m_args.argv[i]);
	ClearListener();
}
/************************************************
功能:	删除命令行监听器
参数:	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::ClearListener( )
{
	std::list<Listener*>::iterator it = m_cmdListener.begin();
	while( it!= m_cmdListener.end() )
	{
		(*it)->OnDel();
		delete *it;
		it++;
	}
	m_cmdListener.clear( );
}
/************************************************
功能:	添加一个命令
参数:	[IN]szCmdName:char* 命令名
[IN]szCmdHelp:char* 命令帮助信息
[IN]cmdFun:CMDFUN 命令的处理函数(回调)		
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::AddCommand(char* szCmdName, char *szCmdHelp,CmdEvent::Event* cmdFun)
{
	//如果数组没有溢出,构造一个命令信息结构放入数组
	if(m_icmdcount_param<CMDCOUNT)
	{
		if(szCmdName)
			strcpy_s(m_cmdparam[m_icmdcount_param].szCmdName,szCmdName);
		else
			m_cmdparam[m_icmdcount_param].szCmdName[0] = '\0';

		if(szCmdHelp)
			strcpy_s(m_cmdparam[m_icmdcount_param].szCmdHelp,szCmdHelp);
		else
			m_cmdparam[m_icmdcount_param].szCmdHelp[0] = '\0';

		if(cmdFun)
			m_cmdparam[m_icmdcount_param].cdmFun += cmdFun;

		m_icmdcount_param++;
	}
}

/************************************************
功能:	查找有参命令对应的函数
参数:	[IN]cmd:char* 命令名		
返回值:	命令的处理函数相关信息
异常:   无
************************************************/
S_CMDINFO*	CCmdInterface::GetParamCmdInfo(const char *cmd)
{
	for(int i=0;i<m_icmdcount_param;i++)
	{
		if(strcmp(cmd,m_cmdparam[i].szCmdName) == 0)
			return &m_cmdparam[i];
	}	
	return NULL;

}

/************************************************
功能:	分析命令行,把一个字符串以空格分成多个单词.
参数:	[IN]cmd:char* 命令字符串		
返回值:	参数个数
异常:   无
************************************************/
int		CCmdInterface::AnalyzeCmd(const char *cmd)
{
	if( NULL == cmd)
	{
		m_args.argc=0;
		m_args.argv[0]="\0";
		return 0;
	}

	int paramCount = 0;	//参数个数设为0;
	int len = (int)strlen(cmd);
	if(len > CMD_LEN) len = CMD_LEN;

	//for(int i=0; i<PARAM; i++)
	//	ZeroMemory(m_argv[i],CMD_LEN);	//将内部的参数字符串数组清0
	bool bStr=false;
	int j=0;
	for(int i=0; i< len; i++)		
	{	
		if(!bStr && (cmd[i]=='\'' || cmd[i]=='\"'))
			bStr = true;
		else if(bStr||(cmd[i] != ' ' &&	cmd[i] != ','))				//如果不是空格,则Copy
			m_args.argv[paramCount][j++] = cmd[i];
		else							//否则换行,并将参数个数+1;
		{
			m_args.argv[paramCount][j]='\0';
			if(++paramCount==PARAM) 
				break;
			j=0;
		}
	}
	if( paramCount>0 && j==0 )	paramCount--;
	m_args.argv[paramCount][j]='\0';
	m_args.argc = paramCount+1;
	return m_args.argc;
}

/************************************************
功能:	循环读取命令,分析命令,执行命令
参数:	无	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::Run()
{
	OnSetup( );

	char cmd [CMD_LEN];
	S_CMDINFO * pCmdInfo = NULL;
	//int paramCount = 0;
	bool bAnalyze = false;

	m_runLock.Enter();
	m_bRun = true;
	while(m_bRun)
	{
		m_runLock.Leave();
		ZeroMemory(cmd,80);	
		std::cout <<m_name<<">:";
		gets_s(cmd);									//获取命令行

		bAnalyze = false;
		std::list<Listener*>::iterator i = m_cmdListener.begin();
		while( i!= m_cmdListener.end() )
		{
			if((*i)->OnInput( cmd ))
			{
				bAnalyze = true;
				break;
			}
			i++;
		}

		if(bAnalyze)
		{
			(*i)->OnDel();
			delete *i;
			m_cmdListener.erase(i);
		}
		else
		{
			AnalyzeCmd(cmd);
			pCmdInfo = GetParamCmdInfo(m_args.argv[0]);
			if(pCmdInfo)
			{
				pCmdInfo->cdmFun.Invoke(&m_args);
			}
			else
			{
				OnCommand( m_args.argc, m_args.argv );
			}
		}
		::Sleep(10);
		m_runLock.Enter();
	}	
	m_runLock.Leave();
	OnShutdown( );
}
void	CCmdInterface::Run(int argc, char* argv[])
{
	S_CMDINFO * pCmdInfo = NULL;
	if(argc>1)
	{
		std::cout<<argv[1]<<std::endl;
		pCmdInfo = GetParamCmdInfo(argv[1]);
	}
	CmdArgs arg;
	arg.argc = argc;
	for( int i=0; i<argc; i++ )
		arg.argv[i] = argv[i];
	if(pCmdInfo)	//如果找到了,通过函数指针回调类成员函数.
	{
		pCmdInfo->cdmFun.Invoke(&arg);//(this->*(pCmdInfo->cmdFun.hCmdHandleParam))(argc-1,argv+1);
	}
	else
		Help(&arg);
	Run();
}

/************************************************
功能:	Help命令处理函数
参数:	无	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::Help(CmdArgs* pPargs)
{
	std::cout<<"---------------------------------------------------------"<<std::endl;
	for(int i=0; i<m_icmdcount_param; i++)
		std::cout<<m_cmdparam[i].szCmdName<<": "<<m_cmdparam[i].szCmdHelp<<std::endl;
	std::cout<<"---------------------------------------------------------"<<std::endl;

}
/************************************************
功能:	设置对象名称
参数:	无	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::SetName(char* name)
{
	strcpy_s(m_name,name);
}
/************************************************
功能:	Quit命令处理函数
参数:	无	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::Quit(CmdArgs* pPargs)
{
	m_runLock.Enter();
	m_bRun = false;
	m_runLock.Leave();
}
/************************************************
功能:	Quit命令处理函数
参数:	无	
返回值:	无
异常:   无
************************************************/
void	CCmdInterface::OnCommand( int argc, char*	m_argv[] )
{
	std::cout<<"错误的命令"<<std::endl;
}