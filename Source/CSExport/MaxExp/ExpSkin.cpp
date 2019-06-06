#include "..\stdafx.h"
#include <assert.h>
#include ".\ExpSkin.h"
#include ".\MaxSkin.h"
#include ".\ExpSkeleton.h"
#include "..\..\F3DMaxExp\Lodder.h"
#include "..\..\MaxPublic\SkinSubMesh.h"
using namespace Cool3D;

namespace
{
	D3DVERTEXELEMENT9 dwDeclPNC[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0, 12, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{0, 24,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},
		D3DDECL_END()
	};

	struct Vert_PNC
	{
		Vector3	pos;
		Vector3	normal;
		DWORD	color;
	};
}

ExpSkinMesh::~ExpSkinMesh()
{
}

int ExpSkinMesh::GetBoneIndex(int boneID, vector<int>& bones)
{
	for (int i=0;i<bones.size();i++)
	{
		if (bones[i]==boneID)
		{
			assert(i<=255);
			return i;
		}
	}
	bones.push_back(boneID);
	assert(bones.size()-1<=255);
	return bones.size()-1;
}

int ExpSkinMesh::GetVertexIndex(int vertexID, vector<int>& vertices)
{
	for (int i=0;i<vertices.size();i++)
	{
		if (vertices[i]==vertexID)
			return i;
	}
	vertices.push_back(vertexID);
	return vertices.size()-1;
}

void ExpSkinMesh::Build(INode *pNode,TimeValue time,ExpSkeleton *pSkeleton)
{
	m_pMaxSkin=new MaxSkin(pNode,time);
	F3DExpMesh::Build(pNode,time,&m_mtlCallBack);

	//生成蒙皮顶点
	vector<BYTE> byVBuffer;
	m_vertices.clear();
	GenerateSkinVerts(byVBuffer);

	//生成LODMesh
	if( !GenerateLodMeshs(byVBuffer) )
		return;

	//生成骨骼的offsetMatrix
	GenerateBoneOffsetMats(pSkeleton);
}

void ExpSkinMesh::GenerateSkinVerts(vector<BYTE>& byVBuffer)
{
	assert(m_expVerts.size()==GetNumVert());
	int haveNoneBoneVert=0;

	m_pMaxSkin->BeginPhysiqueExport();
	int numVert=GetNumVert();
	byVBuffer.resize(numVert*sizeof(Vert_PNC));
	m_vertices.resize(numVert);
	for(int i=0;i<numVert;i++)
	{
		SkinVert& v=m_vertices[i];
		ExpVert& expVert=m_expVerts[i];
		BoneInfArray infArray;
		m_pMaxSkin->Vert_GetBoneInf(expVert.nPosNormal,infArray);
		for(size_t j=0;j<infArray.size();j++)
		{
			const BoneInfluence& inf=infArray[j];
			tagBoneInf ti;
			ti.weight=inf.weight;
			ti.boneId=m_pSkeleton->FindNodeByName(inf.szBoneName.c_str());
			assert(ti.boneId!=-1);
			Point3ToVector3_D3D(inf.offset,ti.offset);
			v.boneInfs.push_back(ti);
		}
		if(v.boneInfs.size()<1)
			haveNoneBoneVert++;

		Point3ToVector3_D3D(GetPos(i),v.pos);
		Point3ToVector3_D3D(GetNormal(i),v.normal);
		if (IfUV())
			Point3ToUV_D3D(GetUV(i),v.uv);
		else
			v.uv=Vector2(0.0f,0.0f);
		v.tangent=Vector3(0.0f,0.0f,0.0f);

		switch (m_vertType)
		{
		case EVType_PNGWIT:
			{
				if (IfTangent())
					Point3ToVector3_D3D(GetTangent(i), v.tangent);
			}
			break;
		}

		Vert_PNC PVert;
		PVert.pos=v.pos;
		PVert.normal=v.normal;
		PVert.color=i;
		::memcpy(&byVBuffer[i*sizeof(Vert_PNC)],&PVert,sizeof(Vert_PNC));
	}
	m_pMaxSkin->EndPhysiqueExport();
	if(haveNoneBoneVert)
		g_msgBox.ShowMsg(_T("WARNING:%d 个顶点没有绑定人和骨骼\r\n"),haveNoneBoneVert);
}

bool ExpSkinMesh::GenerateLodMeshs(const vector<BYTE>& byVBuffer)
{
	//LOD层次现在分为三层高、中、低
	//面数分别为原模型的1、1/2、1/8
	for(int i=0;i<GetNumSubMesh();i++)
	{
		ExpSubMesh *ps=GetSubMesh(i);
		LodSkinSubMesh *pm=new LodSkinSubMesh;
		pm->baseIndex=0;
		pm->mtlID=ps->m_nMtlID;
		pm->numVert=GetNumVert();

		FaceBuffer *pFaces=&pm->baseFaces;
		pFaces->clear();
		int numFaces=ps->m_faceArray.size();

		ID3DXMesh* pMesh=NULL;
		ID3DXPMesh* pPMesh=NULL;
		HRESULT hr = 0;
		hr=D3DXCreateMesh(numFaces,
			pm->numVert,
			D3DXMESH_MANAGED,
			dwDeclPNC,
			m_pDevice9,
			&pMesh);
		if(FAILED(hr))
			return false;

		BYTE *pbyVB;
		pMesh->LockVertexBuffer(0, (void**)&pbyVB);
		memcpy(pbyVB,&byVBuffer[0],byVBuffer.size());
		pMesh->UnlockVertexBuffer();

		WORD *pdwIB=NULL;
		pMesh->LockIndexBuffer(0, (void**)&pdwIB);
		for(int j=0;j<numFaces;j++)
		{
			Cool3D::Polygon& p=ps->m_faceArray[j];
			Cool3D::TriFace f;
			f.index[0]=p.indices[0];
			f.index[1]=p.indices[1];
			f.index[2]=p.indices[2];
			pFaces->push_back(f);

			pdwIB[j*3]=p.indices[0];
			pdwIB[j*3+1]=p.indices[1];
			pdwIB[j*3+2]=p.indices[2];
		}
		pMesh->UnlockIndexBuffer();

		std::vector<DWORD> adjacencyBuffer(pMesh->GetNumFaces()*3);
		pMesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

		LPD3DXBUFFER errorBuffer=NULL;
		const char* szError=NULL;

		hr = D3DXCleanMesh(
			D3DXCLEAN_BOWTIES,
			pMesh,
			(DWORD*)&adjacencyBuffer[0],
			&pMesh,
			(DWORD*)&adjacencyBuffer[0],
			&errorBuffer);
		if( FAILED(hr) )
		{
			szError=(const char*)errorBuffer->GetBufferPointer();
			g_msgBox.ShowMsg(szError);
			return false;
		}

		hr = D3DXValidMesh(pMesh,(DWORD*)&adjacencyBuffer[0],&errorBuffer);
		if(FAILED(hr))
		{
			szError=(const char*)errorBuffer->GetBufferPointer();
			g_msgBox.ShowMsg(szError);
			return false;
		}

		hr = D3DXGeneratePMesh(
			pMesh,
			(DWORD*)&adjacencyBuffer[0], // adjacency
			0,                  // default vertex attribute weights
			0,                  // default vertex weights
			1,                  // simplify as low as possible
			D3DXMESHSIMP_FACE,  // simplify by face count
			&pPMesh);
		if(FAILED(hr))
			return false;

		pm->numLod=2;
		GenerateLodMesh(pPMesh, &pm->lodFaces[0], numFaces*1/2);
		GenerateLodMesh(pPMesh, &pm->lodFaces[1], numFaces*1/8);

		m_lodSubMeshs.push_back(pm);
	}

	return true;
}

void ExpSkinMesh::GenerateBoneOffsetMat(ExpSkeleton *pSkeleton, const FaceBuffer& faces, int lod, 
										vector<tagBoneOffsetMatrix>& boneOM)
{

	vector<int> subMeshVertices;
	for (int faceIndex=0;faceIndex<faces.size();faceIndex++)
	{
		const TriFace& face=faces[faceIndex];
		for (int i=0;i<3;i++)
			GetVertexIndex(face.index[i], subMeshVertices);
	}
	vector<int> subMeshBones;
	for (int vertIndex=0;vertIndex<subMeshVertices.size();vertIndex++)
	{
		SkinVert& v=m_vertices[subMeshVertices[vertIndex]];
		vector<BYTE> bonesIndex;
		bonesIndex.resize(v.boneInfs.size());
		for (int j=0;j<v.boneInfs.size();j++)
		{
			tagBoneInf& ti=v.boneInfs[j];
			bonesIndex[j]=GetBoneIndex(ti.boneId,subMeshBones);
		}
		v.blendWeight=Vector4(0.0f,0.0f,0.0f,0.0f);
		v.blendIndices=0;
		for (int infIndex=0;infIndex<v.boneInfs.size();infIndex++)
		{
			switch (infIndex)
			{
			case 0:
				v.blendWeight.x=v.boneInfs[infIndex].weight;
				v.blendIndices|=bonesIndex[infIndex];
				break;
			case 1:
				v.blendWeight.y=v.boneInfs[infIndex].weight;
				v.blendIndices|=bonesIndex[infIndex]<<8;
				break;
			case 2:
				v.blendWeight.z=v.boneInfs[infIndex].weight;
				v.blendIndices|=bonesIndex[infIndex]<<16;
				break;
			case 3:
				v.blendWeight.w=v.boneInfs[infIndex].weight;
				v.blendIndices|=bonesIndex[infIndex]<<24;
				break;
			}
		}
	}

	for (int boneIndex=0;boneIndex<subMeshBones.size();boneIndex++)
	{
		tagBoneOffsetMatrix boneOffsetMat;
		boneOffsetMat.boneId=subMeshBones[boneIndex];
		INode* pNode=pSkeleton->GetNode(boneOffsetMat.boneId);

		Vector3 trans;
		Quaternion quat;
		Max_GetNodeLocalAni(pNode,NULL,0,trans,quat);//取模型空间中的位移和旋转

		Matrix3 tt=pNode->GetNodeTM(0);
		bool bMirrored=Max_TMNegParity(tt);

		D3DXMATRIX mTrans;
		D3DXMATRIX mRotate;
		D3DXMATRIX mMirror;
		D3DXMatrixTranslation(&mTrans,trans.x,trans.y,trans.z);
		D3DXMatrixRotationQuaternion(&mRotate,&quat);
		D3DXMATRIX mOffset;
		D3DXMatrixIdentity(&mOffset);

		if(bMirrored)
		{
			D3DXMatrixScaling(&mMirror,-1,-1,-1);
			mOffset*=mMirror;
		}	
		mOffset*=mRotate;
		mOffset*=mTrans;

		D3DXMatrixInverse(&mOffset,NULL,&mOffset);
		boneOffsetMat.offsetMatrix=mOffset;
		boneOM.push_back(boneOffsetMat);
	}
}

void ExpSkinMesh::GenerateBoneOffsetMats(ExpSkeleton *pSkeleton)
{
	//计算每个subMesh的顶点集合
	//计算每个subMesh的骨骼集合
	//在每个subMesh的顶点循环，赋给顶点的骨骼索引为subMesh的顶点索引

	//在每个subMesh的骨骼循环，计算骨骼的偏移矩阵
	//插入sumMesh偏移矩阵集合
	//m_subMeshBoneOMs.resize(m_subMeshs.size());
	for (int smIndex=0;smIndex<m_subMeshs.size();smIndex++)
	{
		LodSkinSubMesh* subMesh=m_lodSubMeshs[smIndex];

		GenerateBoneOffsetMat(pSkeleton,subMesh->baseFaces,0,subMesh->baseBoneOM);
	}
}

void ExpSkinMesh::GenerateLodMesh(ID3DXPMesh* pPMesh, FaceBuffer* pFaces, int numFaces)
{
	assert(pPMesh!=NULL && pFaces!=NULL);
	void *pVBLock=NULL;
	WORD *pIBLock=NULL;
	IDirect3DVertexBuffer9 *pVB=NULL;
	IDirect3DIndexBuffer9 *pIB=NULL;

	pPMesh->SetNumFaces(numFaces);
	int newNumFaces=pPMesh->GetNumFaces();
	pPMesh->GetVertexBuffer(&pVB);

	pVB->Lock(0,0,&pVBLock,D3DLOCK_READONLY);
	pPMesh->GetIndexBuffer(&pIB);
	pIB->Lock(0,0,(void**)&pIBLock,D3DLOCK_READONLY);
	for(int j=0;j<newNumFaces;j++)
	{
		Cool3D::TriFace f;
		Vert_PNC *tmp=(Vert_PNC *)pVBLock;
		f.index[0]=tmp[pIBLock[j*3]].color;
		f.index[1]=tmp[pIBLock[j*3+1]].color;
		f.index[2]=tmp[pIBLock[j*3+2]].color;
		pFaces->push_back(f);
	}
	pIB->Unlock();
	pVB->Unlock();
}

SkinVert& ExpSkinMesh::GetSkinVert(UINT i)
{
	assert(i<m_vertices.size());
	return m_vertices[i];
}

LodSkinSubMesh* ExpSkinMesh::GetLodSubMesh(UINT i)
{
	assert(i<m_lodSubMeshs.size());
	return m_lodSubMeshs[i];
}

void ExpSkinMesh::BuildAABBox(Vector3& max,Vector3& min)
{
	max.x=max.y=max.z=-FLOAT_MAX;
	min.x=min.y=min.z=FLOAT_MAX;
	
	for(int i=0;i<m_vertices.size();i++)
	{
		Vector3 pos;
		Point3ToVector3_D3D(GetPos(i),pos);
		if(pos.x > max.x)
			max.x=pos.x;
		if(pos.y > max.y)
			max.y=pos.y;
		if(pos.z > max.z)
			max.z=pos.z;
		
		if(pos.x < min.x)
			min.x=pos.x;
		if(pos.y < min.y)
			min.y=pos.y;
		if(pos.z < min.z)
			min.z=pos.z;
	}
}

//-----------------------------------------
class SkinEnumProc : public ITreeEnumProc  
{
public:
	SkinEnumProc(ExpSkin *pExp)
	{
		m_pExp=pExp;
	}
	~SkinEnumProc()	{}
	virtual int callback( INode *pNode )
	{
		if(MaxNode_IsBone(pNode) || MaxNode_IsDummy(pNode))
			return TREE_CONTINUE;
		if(pNode->IsHidden())
			return TREE_CONTINUE;

		if(MaxNode_IsMesh(pNode)
			&& _tcscmp(_T("Bip01 Footsteps"),pNode->GetName())!=0
			)
		{
			if(IsTagNode(pNode))
			{
				g_msgBox.ShowMsg(_T("标记节点已经忽略,name=%s\r\n"),pNode->GetName());
			}
			else
				m_pExp->AddMeshNode(pNode);
		}
		return TREE_CONTINUE;
	}
private:
	ExpSkin	*m_pExp;
};
//-------------------------------------

ExpSkin::ExpSkin()
{
	m_pDevice9=NULL;
	m_vertType=EVType_PNT;
	m_pD3D9=::Direct3DCreate9(D3D_SDK_VERSION);
	m_bEnablePM=false;
	if(!m_pD3D9)
	{
		g_msgBox.ShowMsg(_T("Direct3DCreate9() - 失败"));
		return;
	}

	HRESULT hr = 0;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed=TRUE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	hr=m_pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT,			// primary adapter
		D3DDEVTYPE_HAL,				// device type
		g_msgBox.GetHWND(),						// window associated with device
		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// vertex processing
		&d3dpp,						// present parameters
		&m_pDevice9);				// return created device
	assert(!FAILED(hr));
}

ExpSkin::~ExpSkin()
{
	if(!m_pDevice9)
	{
		m_pDevice9->Release();
	}
	if(!m_pD3D9)
	{
		m_pD3D9->Release();
	}
	for(int i=0;i<m_expMeshs.size();i++)
		delete m_expMeshs[i];
	m_expMeshs.clear();
}

int ExpSkin::BuildFromMaxExportInterface(ExpInterface *pEI,ExpSkeleton *pSkeleton)
{
	m_pSkeleton=pSkeleton;

	//--
	SkinEnumProc enumProc(this);
	pEI->theScene->EnumTree(&enumProc);

	return m_maxNodes.size();
}

const TCHAR* ExpSkin::GetMeshName(UINT i)
{
	return m_maxNodes[i]->GetName();
}

ExpSkinMesh *ExpSkin::GetExpSkinMesh(int i)
{
	if(i<0 || i>=m_expMeshs.size())
		return NULL;
	return m_expMeshs[i];
}

void ExpSkin::SetVertInfo(bool bExpBUUV)
{
	if (bExpBUUV)
		m_vertType=EVType_PNGWIT;
	else
		m_vertType=EVType_PNWIT;
}

void ExpSkin::AddMeshNode(INode *pNode)
{
	m_maxNodes.push_back(pNode);

	//--
	TimeValue time=0;
	ExpSkinMesh *pExp=new ExpSkinMesh(m_pSkeleton,m_pDevice9,m_vertType);
	pExp->Build(pNode,time,m_pSkeleton);
	m_expMeshs.push_back(pExp);

	g_msgBox.ShowMsg(_T("Mesh node:%s,export numFace=%d,numVert=%d\r\n")
		,pNode->GetName(),pExp->GetNumFaces(),pExp->GetNumVert());
}
