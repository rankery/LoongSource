#pragma once

class ExpClothMesh;

class FClothFile  
{
public:
	FClothFile();
	virtual ~FClothFile();

	bool WriteToFile(const TCHAR* szFileName,ExpCloth& cloth,ExpClothMesh *pMesh);

private:
	void GetClothTag(ExpClothMesh *pMesh,vector<DWORD>& oldCloth,vector<DWORD>& oldTag,vector<DWORD>& newCloth,vector<DWORD>& newTag,int& u,int& v);
	void CalcSpring(ExpClothMesh *pMesh,const vector<DWORD>& clothArray,int u,int v,vector<tagSpring>& springArray);

private:
	ExpClothMesh *m_pMesh;

};