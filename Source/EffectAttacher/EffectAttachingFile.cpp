#include "StdAfx.h"
#include ".\effectattachingfile.h"

EffectAttachingFile::EffectAttachingFile(void)
: m_AttachingFile(NULL)
{
	m_AttachingFileName = _T("EffectAttaching.csv"); //默认的连接特效用的文件名
	m_AttachingFile = NULL;
}

EffectAttachingFile::~EffectAttachingFile(void)
{
}

tstring EffectAttachingFile::MakeFileNameFromMdlPath(const TCHAR* szMdlPath)
{
	CString szTmp(szMdlPath);
	szTmp.MakeLower();
	szTmp.Replace(_T(".fak"),_T("_a.csv"));
	szTmp.Replace(_T(".fskel"),_T("_a.csv"));

	return tstring((LPCTSTR)szTmp);
}

bool EffectAttachingFile::Save(const TCHAR* szMdlPath)
{
	//创建（打开）EffectAttaching.csv（与模型路径相同），把连接点和特效文件路径保存。
	m_AttachingFile = new CFile;
	//tstring entityPath = MakeFileNameFromMdlPath(szMdlPath);
	
	m_AttachingFile->Open(szMdlPath,CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);

	//if(m_FileContent.empty())
	m_AttachingFile->Flush();

	//unicode
	WORD header = 0xFEFF;
	m_AttachingFile->Write(&header,2);
	m_AttachingFile->Write(m_FileContent.c_str(), m_FileContent.size()*sizeof(TCHAR));
	m_AttachingFile->Close();
	delete m_AttachingFile;
	m_AttachingFile=NULL;
	return true;
}

tstring EffectAttachingFile::Load(const TCHAR* szMdlPath)
{
	m_AttachingFile = new CFile;
	//tstring entityPath = MakeFileNameFromMdlPath(szMdlPath);
	if(m_AttachingFile->Open(szMdlPath,CFile::modeRead | CFile::shareDenyNone))
	{
		WORD header;
		m_AttachingFile->Read(&header,2);
		int fileLength = ( (int)m_AttachingFile->GetLength()-2 ) / sizeof(TCHAR);
		TCHAR * fileContent = new TCHAR[fileLength+1];
		m_AttachingFile->Read(fileContent, fileLength*sizeof(TCHAR) );
		fileContent[fileLength]=_T('\0');
		tstring returnFileContent = fileContent;
		delete fileContent;
		return returnFileContent;
	}
	else
	{
		//AfxMessageBox(_T("文件路径错误，或者其他错误导致的文件无法代开"));
		return _T("");
	}

	m_AttachingFile->Close();
	delete m_AttachingFile;
	m_AttachingFile = NULL;
	return _T("");
}

bool EffectAttachingFile::AddContent(tstring * iAttachingNodeName,tstring * iEffectFilePath)
{
	if(!m_FileContent.empty())
	{
		m_FileContent += _T(",");
	}
	m_FileContent += *iAttachingNodeName;
	m_FileContent += _T(",");
	m_FileContent += *iEffectFilePath;
	return true;
}

bool EffectAttachingFile::ClearFileContent(void)
{
	m_FileContent.clear();
	if(m_FileContent.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}
