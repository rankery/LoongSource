#include "StdAfx.h"
#include ".\fileutil.h"
#include "..\Util\Exception.h"

namespace F3D
{
	void FWriteString(FILE *fp,const char *str)
	{
		ASSERT(fp!=NULL);
		ASSERT(str!=NULL);

		DWORD strLen=strlen(str)+1;
		fwrite(&strLen,sizeof(DWORD),1,fp);
		fwrite(str,strLen,1,fp);
	}
	
	void FReadString(IFS *pFS,DWORD hFile,string& str)
	{
		ASSERT(pFS!=NULL);
		ASSERT(hFile!=NULL);
		
		DWORD strLen;
		pFS->Read(&strLen,sizeof(DWORD),hFile);
		ASSERT(strLen > 0);
		char *pTemp=new char[strLen];
		pFS->Read(pTemp,strLen,hFile);
		ASSERT(pTemp[strLen-1]==0);//必须是以0结尾
		str=pTemp;

		delete pTemp;
	}
}//namespace F3D