#pragma once
#include <io.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <windows.h>
#include <fstream>
#include <algorithm>
#include "fgeDataBuff.h"
#include "fgeUtil.h"
#include "fgeLog.h"

using namespace std;
namespace fge
{
	typedef vector<string> StringVector;
	
	struct FileInfo {
		string	filename,path;
		string	basename;
		int64	size;
		int64	changeTime;
	};
	typedef vector<FileInfo> FileInfoList;
	struct MyLocale{
		MyLocale(){
			//���ô���ҳΪ�������ģ�936�Ǽ������ĵĴ���ҳ��
			currLocale = std::locale::global(std::locale(".936"));
		}
		~MyLocale(){
			//�ָ�ԭ���Ĵ���ҳ
			std::locale::global(std::locale(currLocale));
		}
		std::locale currLocale;
	};
	#define setLocale( ) MyLocale _setLocale;
	//����Ŀ¼
	bool createDir(const char *szDirName);
	//�Ӻ���ǰ�Ƚ��ַ���
	bool rstrcmp( const std::string& str,const std::string& pattern);
	//�Ƿ��Ǿ���·��
	bool is_absolute_path(const char* path);
	//�Ƿ������·��
	bool is_reserved_dir (const char *fn);
	//�����ļ�·��
	string concatenate_path(const string& base, const string& name);
	//�����ļ� exact=false �Ƿ�ȷƥ��
	void findFiles(const string&  path, const string& pattern, bool recursive, 
		bool dirs, StringVector* simpleList, FileInfoList* detailList,StringVector* ignore=0,bool exact=false);

	//�ļ���
	class File
	{
	public:
		File(){ hFile = 0; }
		~File(){ Close( ); }
		enum MODE{ MODE_READ=1, MODE_WRITE=2, MODE_BINARY=4, MODE_APPEND=8, MODE_RWB = 7,MODE_RW = 3 };
		//���ļ�
		bool	Open( const char * fileName, int mode=MODE_READ|MODE_BINARY );		
		bool	Create( const char * fileName, int mode=MODE_WRITE|MODE_BINARY );
		bool	OpenAlways( const char * fileName, int mode=MODE_READ|MODE_BINARY );
		void	Seek(int64 offset,int type=SEEK_SET);
		uint64	Read( char* buff, uint64 size);
		uint64	Write( const char* buff, uint64 size);
		uint64  Write(const char* str);

		//�õ��ļ���С,�Զ������ļ���Ч
		int64	TellSize();
		bool	IsOpen( );
		void	Close( );

		template <class T>
		uint64  Write(const T& x){	return Write((char*)&x,sizeof(T));	}		
		template <class T>
		uint64  Read(const T& x){	return Read((char*)&x,sizeof(T));	}

	public:
		static  uint64	GetFileSize( const char * fileName );
		static  uint	GetFileWriteTime( const char * fileName );
		static  bool	CopyFile(const char *szSrcName, const char *szDestName);
	private:
		FILE* hFile;
		int	m_fileMode;
	};

}