//************************************************************
// Copyright (c) 2005,  CeateSky
// All rights reserved.
// �ļ����ƣ�UserInterface.h
// ժ    Ҫ���������ַ��������.֧�����������ʽ:[�޲�/�в�]ʹ�÷���:�Ӵ˼�
//				1.д���������
//				2.������Ĺ��췽����ʹ��AddCommand�����������,
//					CMDFUN��ʾ�޲ε�����
//					CMDFUN_PARAM��ʾ�в����������ָ��.
//				3.��д�����Help����,�������г�.
//			 �����CThread�̳�,�ӿ���̨�õ�һ�ַ�����,��AnalyzeCmd���ַ����������������Ͳ���,����GetCmdInfo/GetParamCmdInfo
//			 �����������̵�ַ,�����������
// ��    ����
// ��ǰ�汾��1.0
// ��    �ߣ��˳�
// ��ʼ���ڣ�2005��12��20��
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

	//typedef void (CCmdInterface::* CMDFUN)();//�������ʽ����
	//typedef void (CCmdInterface::* CMDFUN_PARAM)(int argc, char* argv[PARAM]);//�������ʽ����
	
	struct CmdArgs/* : public IEventArgs*/
	{
		int		argc;
		char*	argv[PARAM];
	};
	typedef EventSet< IEvent1<void,CmdArgs*> > CmdEvent;
	//CmdEvent+=CEvent(this,OnCommand);
	//������Ϣ�ṹ,������������,������Ϣ,�ͺ�����ַ.����Event��һ������ָ������Ͻṹ,����ָ��һ��������ַ.
	struct S_CMDINFO
	{
		char szCmdName[CMD_LEN+1];//��������
		char szCmdHelp[200];	//���������Ϣ
		CmdEvent	cdmFun;	

		S_CMDINFO()
		{
			szCmdName[0] = '\0';
			szCmdHelp[0] = '\0';
		};
		S_CMDINFO(char* szCmdName,char* szCmdHelp,CmdEvent::Event* cmdEvent);
	};

	//�û��������
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
		//�̴߳�����

		S_CMDINFO*	GetCmdInfo(const char *cmd);				//���������Ӧ�ĺ���
		S_CMDINFO*	GetParamCmdInfo(const char *cmd);			//���������Ӧ�ĺ���
		int			AnalyzeCmd(const char *cmd);				//����������

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