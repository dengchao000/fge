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
			//设置代码页为简体中文，936是简体中文的代码页。
			currLocale = std::locale::global(std::locale(".936"));
		}
		~MyLocale(){
			//恢复原来的代码页
			std::locale::global(std::locale(currLocale));
		}
		std::locale currLocale;
	};
	#define setLocale( ) MyLocale _setLocale;
	//创建目录
	bool createDir(const char *szDirName);
	//从后向前比较字符串
	bool rstrcmp( const std::string& str,const std::string& pattern);
	//是否是绝对路径
	bool is_absolute_path(const char* path);
	//是否是相对路径
	bool is_reserved_dir (const char *fn);
	//连接文件路径
	string concatenate_path(const string& base, const string& name);
	//查找文件 exact=false 是否精确匹配
	void findFiles(const string&  path, const string& pattern, bool recursive, 
		bool dirs, StringVector* simpleList, FileInfoList* detailList,StringVector* ignore=0,bool exact=false);

	//文件类
	class File
	{
	public:
		File(){ hFile = 0; }
		~File(){ Close( ); }
		enum MODE{ MODE_READ=1, MODE_WRITE=2, MODE_BINARY=4, MODE_APPEND=8, MODE_RWB = 7,MODE_RW = 3 };
		//打开文件
		bool	Open( const char * fileName, int mode=MODE_READ|MODE_BINARY );		
		bool	Create( const char * fileName, int mode=MODE_WRITE|MODE_BINARY );
		bool	OpenAlways( const char * fileName, int mode=MODE_READ|MODE_BINARY );
		void	Seek(int64 offset,int type=SEEK_SET);
		uint64	Read( char* buff, uint64 size);
		uint64	Write( const char* buff, uint64 size);
		uint64  Write(const char* str);

		//得到文件大小,对二进制文件有效
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