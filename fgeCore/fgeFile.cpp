#include "StdAfx.h"
#include ".\fgefile.h"

namespace fge
{
	int __formatpath(int _Ch)
	{	// convert character to lower case, locale specific
		if(_Ch == '\\') return '/';
		return tolower(_Ch);
	}
	bool createDir(const char *szDirName)
	{//创建文件
		if (NULL == szDirName || strlen(szDirName) <= 0) return false;
		if (_access(szDirName,0)!=-1) return true; //已在目录
		char szCurDir[MAX_PATH];
		::ZeroMemory(szCurDir,MAX_PATH);
		int nDirNum = 0;
		for (int nIndex  = 0 ;nIndex<(int)strlen(szDirName) ;nIndex++)
		{
			szCurDir[nIndex] = szDirName[nIndex];
			if (szCurDir[nIndex] == '\\' || szCurDir[nIndex] == '/')
			{
				nDirNum++;
				if (nDirNum<= 1) continue;
				if (_access(szCurDir,0)!=-1) continue; //存在路径
				::CreateDirectoryA(szCurDir,NULL);			
			}
		}	
		if (_access(szCurDir,0)!=-1)
			return true; //存在路径
		::CreateDirectoryA(szCurDir,NULL);	
		return _access(szCurDir,0)!=-1;
	}


	bool rstrcmp( const std::string& str,const std::string& pattern)
	{
		std::string::const_reverse_iterator i = str.rbegin();
		std::string::const_reverse_iterator j = pattern.rbegin();
		while(i!=str.rend() && j!=pattern.rend())
			if(*i++!=*j++) return false;
		return true;
	}
	//绝对路径
	bool is_absolute_path(const char* path)
	{
		if (isalpha(unsigned char(path[0])) && path[1] == ':')
			return true;
		return false;
	}
	//相对路径
	bool is_reserved_dir (const char *fn)
	{
		return (fn [0] == '.' && (fn [1] == 0 || (fn [1] == '.' && fn [2] == 0)));
	}

	string concatenate_path(const string& base, const string& name)
	{
		if (base.empty() || is_absolute_path(name.c_str()))
			return name;
		else if(base.size()>0 && base[base.size()-1]!='\\' && base[base.size()-1]!='/')
			return base + '/' + name;
		else
			return base + name;
	}
	//路径匹配
	bool strcmp_mask(const char* pstr, const char* pattern, bool exact=true)	// 带通配符的比较函数
	{
		if(pstr == pattern)
			return true;
		if(!pstr || !pattern)
			return false;
		const char* xmash=0;	//是否匹配"*"
		while( *pstr && *pattern )
		{			
			if(*pattern == '?')
				goto _next;
			else if(*pattern == '*')
			{
				xmash = pattern++;
				while( *pstr && *pattern!=*pstr ) pstr++;
				if('\0' == *pstr)
					break;
			}
			if(*pstr != *pattern)
			{
				if(xmash){
					pattern = xmash;
					continue;
				}
				else
					return false;
			}

_next:
			pstr++;
			pattern++;
		}
		if(exact)
			return !(*pstr || *pattern);
		else
			return !(*pattern);
	}
	bool checkIgnoreList(const char* pstr, StringVector* ignore)
	{
		if(!ignore)
			return false;
		StringVector::iterator i = ignore->begin();
		for (i; i!=ignore->end(); i++)
		{
			if(strcmp_mask(pstr,i->c_str()))
				return true;
		}
		return false;
	}

	char getFirstChar( const char* pStr )
	{
		while(pStr && *pStr) 
		{
			if( *pStr!='\r' && *pStr!='\n' && *pStr!='\t' && *pStr!=' ')
				return *pStr;
			pStr++;
		}
		return '\0';
	}

	void findFiles(const string&  path, const string& pattern, bool recursive, 
		bool dirs, StringVector* simpleList, FileInfoList* detailList,StringVector* ignore,bool exact)
	{
		bool ms_IgnoreHidden = true;
		long lHandle, res;
		struct _finddata_t tagData;

		// pattern can contain a directory name, separate it from mask
		size_t pos1 = pattern.rfind ('/');
		size_t pos2 = pattern.rfind ('\\');
		if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
			pos1 = pos2;	//以最后一个为准
		string directory;
		string _pattern;
		if (pos1 != pattern.npos)
		{
			directory = pattern.substr (0, pos1 + 1);
			_pattern = pattern.substr(pos1+1,pattern.size());
			transform(directory.begin(), directory.end(), directory.begin(),__formatpath);
		}
		else
			_pattern = pattern;

		transform(_pattern.begin(), _pattern.end(), _pattern.begin(), __formatpath);

		string full_pattern = concatenate_path(path, directory);
		full_pattern = concatenate_path(full_pattern, "*.*");

		//遍历当前目录
		lHandle = (long)_findfirst(full_pattern.c_str(), &tagData);
		res = 0;

		while (lHandle != -1 && res != -1)
		{
			//dirs == true, 则只返回目录名
			bool isDir = (tagData.attrib & _A_SUBDIR)!=0;
			if (((dirs == isDir ) || (isDir && recursive)) &&
				(!ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0 ) &&
				(!dirs || !is_reserved_dir (tagData.name)))
			{
				if(!ignore || !checkIgnoreList(tagData.name,ignore))
				{
					if(dirs == isDir && strcmp_mask(tagData.name,_pattern.c_str(),exact))
					{
						if (simpleList)
						{
							std::string str = directory + tagData.name;
							transform(str.begin(), str.end(), str.begin(), __formatpath);
							simpleList->push_back(str);							
						}
						if (detailList)
						{
							FileInfo fi;
							fi.filename = directory + tagData.name;
							fi.basename = tagData.name;
							fi.path = directory;
							fi.size = tagData.size;
							fi.changeTime = tagData.time_write;
							detailList->push_back(fi);
						}
					}

					if (isDir && recursive && !is_reserved_dir (tagData.name)){
						string base_dir = directory;
						string mask("/");
						if (pos1 != pattern.npos)
							mask.append (pattern.substr (pos1 + 1));
						else
							mask.append (pattern);
						base_dir.append (tagData.name).append (mask);
						findFiles(path,base_dir, recursive, dirs, simpleList, detailList,ignore,exact);
					}
				}
			}
			res = _findnext( lHandle, &tagData );
		}
		// Close if we found any files
		if(lHandle != -1)
			_findclose(lHandle);
	}

	bool	File::Open( const char * fileName, int mode )
	{
		if(hFile) fclose(hFile);
		std::string strMode="";

		if(mode&MODE_READ) strMode += 'r';
		else if(mode&MODE_APPEND) strMode += 'a';
		if(mode&MODE_WRITE) strMode += '+';
		if(mode&MODE_BINARY) strMode += 'b';
		fopen_s(&hFile,fileName,strMode.c_str());
		m_fileMode = mode;
		return hFile!=NULL;
	}

	bool	File::Create( const char * fileName, int mode )
	{
		if(hFile) fclose(hFile);
		std::string strMode="";
		if(mode&MODE_WRITE) strMode += 'w';
		if(mode&MODE_READ) strMode += '+';			
		if(mode&MODE_BINARY) strMode += 'b';
		fopen_s(&hFile,fileName,strMode.c_str());
		m_fileMode = mode;
		return hFile!=NULL;
	}
	bool	File::OpenAlways( const char * fileName, int mode )
	{
		if(!Open(fileName,mode))
			return Create(fileName,mode);
		return true;
	}
	void	File::Seek(int64 offset,int type)
	{
		if(hFile)
		{
			_fseeki64(hFile,offset,type);
		}
	}
	uint64	File::Read(char* buff, uint64 size)
	{
		if(!hFile || !(m_fileMode&MODE_READ)) return 0;
		return fread(buff,1,(size_t)size,hFile);
	}
	uint64	File::Write(const char* buff, uint64 size)
	{
		if(!hFile || !(m_fileMode&MODE_WRITE)) return 0;
		return fwrite(buff,1,(size_t)size,hFile);
	}
	//得到文件大小,对二进制文件有效
	int64	File::TellSize()
	{
		if(!hFile) return 0;
		int64 currpos = _ftelli64(hFile);
		_fseeki64(hFile,0,SEEK_END);
		int64 endpos = _ftelli64(hFile);
		_fseeki64(hFile,currpos,SEEK_SET);

		return endpos;
	}
	uint64  File::Write(const char* str)
	{
		return Write(str,strlen(str));
	}
	bool	File::IsOpen( ){ return hFile!=0; }
	void	File::Close( ) { if(hFile){fclose(hFile); hFile =0; }}
	uint64	File::GetFileSize( const char * fileName )
	{
		if(!fileName) return 0;
		struct stat tagStat;
		bool ret = (::stat(fileName, &tagStat) == 0);
		if(ret)
			return tagStat.st_size;
		else
			return 0;
	}
	uint	File::GetFileWriteTime( const char * fileName )
	{
		if(!fileName) return 0;
		struct stat tagStat;
		bool ret = (::stat(fileName, &tagStat) == 0);
		if(ret)
			return (uint)tagStat.st_mtime;
		else
			return 0;
	}
	bool  File::CopyFile(const char *szSrcName, const char *szDestName)
	{//创建文件
		char szDir[MAX_PATH];
		::GetCurrentDirectoryA(MAX_PATH,szDir);	

		if (NULL == szSrcName || NULL == szDestName) return false;
		char szCurDir[MAX_PATH];
		::ZeroMemory(szCurDir,MAX_PATH);
		int nDirNum = 0;
		int destLen = (int)strlen(szDestName);
		for (int nIndex  = 0; nIndex<destLen; nIndex++)
		{
			szCurDir[nIndex] = szDestName[nIndex];
			if (szCurDir[nIndex] == '\\' || szCurDir[nIndex] == '/')
			{
				nDirNum++;
				if (nDirNum<= 1) continue;
				if (_access(szCurDir,0)!=-1) continue; //存在路径
				::CreateDirectoryA(szCurDir,NULL);
			}
		}
		if (!SetCurrentDirectoryA(szDir)) 
			return true; //存在路径
		fge::File fin;
		fge::File fout;
		if(!fin.Open(szSrcName))
		{
			log_warn("读取文件失败:%s",szSrcName);
			return false;
		}
		if(!fout.Create(szCurDir))
		{
			log_warn("创建文件失败:%s",szCurDir);
			return false;
		}

		char szbuf[1024];
		uint64 readNum(0);
		while(readNum = fin.Read(szbuf,1024))
		{
			fout.Write(szbuf,readNum);
		}
		return true;
	}
}