// stdafx.cpp : 只包括标准包含文件的源文件
// SelfExtract.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

//将oldString翻译成其他语言
char *Translate(char *oldString)
{
	CFile f;
	if(lastOff>=sizeof(result))
		lastOff=0;
	if(FileBuffer=="")
	{
		try{
			f.Open("Language.txt",CFile::modeRead);
		}
		catch( char *)
		{
		}
		if(f.m_hFile==(HANDLE)-1)
		{
			FileBuffer=" ";
			return oldString;
		}
		int len;
		len=f.GetLength();
		char *pFileBuffer=new char[len+1];
		f.Read(pFileBuffer,len);
		pFileBuffer[len]=0;
		f.Close();
		FileBuffer.Format("%s",pFileBuffer);
		delete pFileBuffer;
	}

	CString szTemp;
	CString szValue;
	int oldPos;
	szTemp.Format("%s=",oldString);
	int off=FileBuffer.Find(szTemp);
	if(off>=0)
	{
		off+=szTemp.GetLength();
	}
	else
	{
		return oldString;
	}
	int offEnd=FileBuffer.Find(";",off);
	if(offEnd>=0)
	{
		szValue=FileBuffer.Mid(off,offEnd-off);
		if(szValue.GetLength()>0)
		{
			if(szValue.GetLength()+lastOff>sizeof(result)-2)
			{
				lastOff=0;
			}
			oldPos=lastOff;
			strcpy(&result[lastOff],szValue.GetBuffer());
			lastOff+=(szValue.GetLength()+1);
			return &result[oldPos];
		}
		else
		{
			return oldString;
		}
	}
	else
	{
		FileBuffer=FileBuffer + "\r\n" + oldString + "=;";
		return oldString;
	}
}

void SaveTranslate(char *FileName)
{
	CFile f;
	f.Open(FileName,CFile::modeCreate+CFile::modeWrite);
	f.Write(FileBuffer.GetBuffer(),FileBuffer.GetLength());
	f.Close();
}

void ReloadTranslate(char *Directory)
{
	CFile f;
	char path[300]={0};
	sprintf(path,"%sLanguage.txt",Directory);
	try{
		f.Open(path,CFile::modeRead);
	}
	catch( char *)
	{
	}
	if(f.m_hFile==(HANDLE)-1)
	{
		FileBuffer=" ";
		return;
	}
	int len;
	len=f.GetLength();
	char *pFileBuffer=new char[len+1];
	f.Read(pFileBuffer,len);
	pFileBuffer[len]=0;
	f.Close();
	FileBuffer.Format("%s",pFileBuffer);
	delete pFileBuffer;
}