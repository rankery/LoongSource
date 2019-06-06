#pragma once

#include "..\..\F3DMaxExp\MaxNodeType.h"
#include "..\..\F3DMaxExp\MaxMath.h"
#include "..\..\F3DMaxExp\F3DExpMesh.h"
#include "..\..\F3DMaxExp\F3DExpMtl.h"
#include "..\CSExport.h"

#include <vector>
using namespace std;

#include "MaxSkin.h"
#include "..\..\MaxPublic\SkinBoneDefine.h"
#include "..\..\MaxPublic\SkinSubMesh.h"
using namespace Cool3D;
#include "ExpSkeleton.h"
#include <D3D9.h>

class ExpClothMeshMtlCallBack : public F3DExpMeshMtlCallBack
{
public:
	ExpClothMeshMtlCallBack()	{}
	virtual ~ExpClothMeshMtlCallBack()	{}

	virtual int CallBack(Mtl *pMaxMtl, int sub,Mtl *pBakedMtl)
	{
		for(size_t i=0;i<m_mtls.size();i++)
		{
			if(m_mtls[i]->Equal2Max(pMaxMtl,sub,pBakedMtl))
				return i;
		}
		int myID=m_mtls.size();
		F3DExpMtl *pNewMtl=new F3DExpMtl(pMaxMtl,sub,myID,pBakedMtl);
		m_mtls.push_back(pNewMtl);
		return myID;
	}

public:
	vector<F3DExpMtl*>	m_mtls;
};

class ExpClothMesh : public F3DExpMesh
{
public:
	ExpClothMesh(ExpSkeleton *pSkeleton, IDirect3DDevice9* pDevice, EVertType vertType=EVType_PNT)	
	{
		m_pSkeleton=pSkeleton;
		m_pDevice9=pDevice;
		m_vertType=vertType;
	}
	~ExpClothMesh();

	void Build(INode *pNode,TimeValue time,ExpSkeleton *pSkeleton);

	SkinVert& GetClothVert(UINT i);
	LodSkinSubMesh* GetLodSubMesh(UINT i);
	ExpClothMeshMtlCallBack *GetMtl()	{	return &m_mtlCallBack;}
	void BuildAABBox(Vector3& max,Vector3& min);
	EVertType GetVertType() const { return m_vertType; }

protected:
	bool GenerateLodMeshs(const vector<BYTE>& byVBuffer);
	void GenerateLodMesh(ID3DXPMesh* pPMesh, FaceBuffer* pFaces, int numFaces);
	int GetBoneIndex(int boneID, vector<int>& bones);
	int GetVertexIndex(int vertexID, vector<int>& vertices);
	void GenerateBoneOffsetMats(ExpSkeleton *pSkeleton);
	void GenerateBoneOffsetMat(ExpSkeleton *pSkeleton, const FaceBuffer& faces, 
		int lod, vector<tagBoneOffsetMatrix>& boneOM);
	void GenerateSkinVerts(vector<BYTE>& byVBuffer);

private:
	MaxSkin					*m_pMaxSkin;
	ExpClothMeshMtlCallBack	m_mtlCallBack;
	ExpSkeleton				*m_pSkeleton;
	//--build data
	vector<SkinVert>		m_vertices;
	vector<LodClothSubMesh*>	m_lodSubMeshs;
	EVertType				m_vertType;
	IDirect3DDevice9*		m_pDevice9;
	vector<int>				m_usedBonesID;
};

class ExpCloth  
{
public:
	ExpCloth();
	virtual ~ExpCloth();

	int BuildFromMaxExportInterface(ExpInterface *pEI,ExpSkeleton *pSkeleton);

	UINT GetNumMeshNodes()	{	return m_maxNodes.size();}
	void AddMeshNode(INode *pNode);
	const TCHAR* GetMeshName(UINT i);
	ExpClothMesh *GetExpClothMesh(int i);
	vector<tagBlock>& GetBlocks(void) { return m_blocks; }

private:
	vector<INode*>			m_maxNodes;
	vector<ExpClothMesh*>	m_expMeshs;
	ExpSkeleton				*m_pSkeleton;
	EVertType				m_vertType;
	IDirect3D9*				m_pD3D9;
	IDirect3DDevice9*		m_pDevice9;
	bool					m_bEnablePM;
	vector<tagBlock>		m_blocks;
};