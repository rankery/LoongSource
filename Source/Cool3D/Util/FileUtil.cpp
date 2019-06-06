#include "StdAfx.h"
#include ".\fileutil.h"

namespace Cool3D
{

	void SearchFiles(const TCHAR* szFind,vector<tstring>& out)
	{
		WIN32_FIND_DATA fd;
		HANDLE hSearch=FindFirstFile(szFind, &fd); 
		if(hSearch==INVALID_HANDLE_VALUE) 
			return;

		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szPath[_MAX_PATH];
		_tsplitpath_s(szFind,szDrive,_MAX_DRIVE,szPath,_MAX_PATH,NULL,0,NULL,0);

		TCHAR szFullPath[512];
		do 
		{
			if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)//Ìø¹ýÎÄ¼þ¼Ð
				continue;

			_sntprintf_s(szFullPath,512,_T("%s%s%s"),szDrive,szPath,fd.cFileName);
			tstring str=szFullPath;	
			out.push_back(str);
		} 
		while(FindNextFile(hSearch,&fd)==TRUE);

		FindClose(hSearch);
	}

	bool DeleteFiles(const TCHAR* szFind)
	{
		vector<tstring> files;
		SearchFiles(szFind,files);

		for(vector<tstring>::iterator iter=files.begin();
			iter!=files.end();iter++)
		{
			tstring& szFile=*iter;
			if(FALSE==::DeleteFile(szFile.c_str()))
				return false;
		}

		return true;
	}

	bool DeleteAllFiles(const TCHAR* szDir)
	{
		TCHAR szFind[512];
		_sntprintf_s(szFind,512,_T("%s*.*"),szDir);

		return DeleteFiles(szFind);
	}

}//namespace Cool3D