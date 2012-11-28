//************************************************************
// Copyright (c) 2005,  CeateSky
// All rights reserved.
// 文件名称：UserInterface.h
// 摘    要：服务器字符界面基类.支持两种命令格式:[无参/有参]使用方法:从此继
//				1.写命令处理方法；
//				2.在子类的构造方法中使用AddCommand方法添加命令,
//					CMDFUN表示无参的命令
//					CMDFUN_PARAM表示有参数的命令函数指针.
//				3.重写子类的Help方法,将命令列出.
//			 此类从CThread继承,从控制台得到一字符串后,用AnalyzeCmd将字符串分析成命令名和参数,再用GetCmdInfo/GetParamCmdInfo
//			 查找命令例程地址,从面调用例程
// 参    考：
// 当前版本：1.0
// 作    者：邓超
// 开始日期：2005年12月20日
//************************************************************

#pragma once
#include <windows.h>
#include <iostream>
#include <list>
#include <fgeutil.h>
#include <fgedelegate.h>
#include <fgeThread.h>

namespace fge{
	class CCmdInterface;
	enum{ CMDCOUNT = 50,PARAM = 10,CMD_LEN = 80};

	//typedef void (CCmdInterface::* CMDFUN)();//命令函数格式定义
	//typedef void (CCmdInterface::* CMDFUN_PARAM)(int argc, char* argv[PARAM]);//命令函数格式定义
	
	struct CmdArgs/* : public IEventArgs*/
	{
		int		argc;
		char*	argv[PARAM];
	};
	typedef EventSet< IEvent1<void,CmdArgs*> > CmdEvent;
	//CmdEvent+=CEvent(this,OnCommand);
	//命令信息结构,包含了命令名,帮助信息,和函数地址.其中Event是一个函数指针的联合结构,用来指向一个函数地址.
	struct S_CMDINFO
	{
		char szCmdName[CMD_LEN+1];//命令名称
		char szCmdHelp[200];	//命令帮助信息
		CmdEvent	cdmFun;	

		S_CMDINFO()
		{
			szCmdName[0] = '\0';
			szCmdHelp[0] = '\0';
		};
		S_CMDINFO(char* szCmdName,char* szCmdHelp,CmdEvent::Event* cmdEvent);
	};

	//用户界面基类
	class CCmdInterface : public IConsign, public SharedObject
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener( ){ }
			virtual bool OnInput( char cmd[CMD_LEN] ) = 0;
			virtual void OnDel( ) = 0;
		};
	public:
		CCmdInterface(void);
		virtual ~CCmdInterface(void);
		virtual void	Run();
		virtual void	Run(int argc, _TCHAR* argv[]);
		void	SetName(char* name);
		void	SetListener( Listener* pListener ){ m_cmdListener.push_back( pListener ); }

		void	ClearListener( );

		void	AddCommand(char* szCmdName, char *szCmdHelp,CmdEvent::Event* cmdFun);

	protected:		
		virtual void	OnSetup( ){}
		virtual void	OnShutdown( ){}
		virtual void	Help( CmdArgs* pPargs );	
		virtual void	Quit( CmdArgs* pPargs );

	protected:	
		//线程处理函数

		S_CMDINFO*	GetCmdInfo(const char *cmd);				//查找命令对应的函数
		S_CMDINFO*	GetParamCmdInfo(const char *cmd);			//查找命令对应的函数
		int			AnalyzeCmd(const char *cmd);				//分析命令行

		virtual void OnCommand( int argc, char*	argv[] );
	private:
		fge::FreeLock m_runLock;
		bool		m_bRun;
		char		m_name[CMD_LEN];
		//S_CMDINFO	m_cmd[CMDCOUNT];	
		//int			m_icmdcount;
		S_CMDINFO	m_cmdparam[CMDCOUNT];	//
		int			m_icmdcount_param;
		CmdArgs		m_args;

		std::list<Listener*>	m_cmdListener;
	};
};