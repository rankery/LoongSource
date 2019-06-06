#pragma once

class ExpSkinMesh;

class FSkinFile  
{
public:
	FSkinFile();
	virtual ~FSkinFile();

	bool WriteToFile(const TCHAR* szFileName,ExpSkinMesh *pMesh);

private:
	ExpSkinMesh *m_pMesh;

};

