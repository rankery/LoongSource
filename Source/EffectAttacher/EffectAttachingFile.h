#pragma once
#include "StdAfx.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

class EffectAttachingFile
{
public:
	EffectAttachingFile(void);
	~EffectAttachingFile(void);
	bool Save(const TCHAR* szMdlPath);
	tstring Load(const TCHAR* szMdlPath);
	bool AddContent(tstring * iAttachingNodeName=NULL,tstring * iEffectFilePath=NULL);
	bool ClearFileContent(void);
protected:
	tstring MakeFileNameFromMdlPath(const TCHAR* szMdlPath);

	tstring m_AttachingFileName;
	tstring m_FileContent;
	CFile *m_AttachingFile;
};
