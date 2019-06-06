#include "stdafx.h"

#include "F3DExpMesh.h"
#include <assert.h>
#include <algorithm>
#include "stdmat.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "MaxMath.h"
#include "MaxMesh.h"

namespace Cool3D
{
	const int LIGHT_MAP_CHANNEL=3;
	const int NORMAL_MAP_CHANNEL=1;
	
	F3DExpMesh::F3DExpMesh()
	{
	}

	Point3 F3DExpMesh::FindNormal(DWORD vertexIndex, DWORD smGroup, int faceIndex)
	{
		std::vector<NormalVerSm>::iterator iter=m_maxMeshNormals.begin();
		for (;iter!=m_maxMeshNormals.end();++iter)
		{
			if (iter->vertexIndex==vertexIndex && iter->smGroup==smGroup)
			{
				if (smGroup==0)
				{
					//当光滑组为0时，需要根据faceIndex来确定normal
					if (faceIndex==iter->faceIndex)
						return iter->normal;
				}
				else
					return iter->normal;
			}
		}
		return Point3(0,0,0);
	}

	Point3 F3DExpMesh::FindTangent(DWORD vertexIndex,DWORD smGroup,int faceIndex,DWORD _uv,int _uvDir)
	{
		std::vector<TangentVerSm>::iterator iter=m_maxMeshTangents.begin();
		for (;iter!=m_maxMeshTangents.end();++iter)
		{
			if (iter->vertexIndex==vertexIndex 
				&& iter->smGroup==smGroup 
				//&& iter->uv==_uv
				//&& iter->uvDir==_uvDir
				)
			{
				if (smGroup==0)
				{
					//当光滑组为0时，需要根据faceIndex来确定tangent
					if (faceIndex==iter->faceIndex)
						return iter->tangent;
				}
				else
					return iter->tangent;
			}
		}
		return Point3(0,0,0);
	}

	void F3DExpMesh::Build(INode *pNode,TimeValue time,F3DExpMeshMtlCallBack *pMtlCallBack,bool tri)
	{
		//应该判断是否需要build LightMap和NormalMap
		//这里只从Max模型本身判断，有LightMap或者NormalMap就进行build
		m_pMtlCallBack=pMtlCallBack;
		m_bTri=tri;
		ObjectState os=pNode->EvalWorldState(time);
		Mtl *pNodeMtl=pNode->GetMtl();
		Matrix3 tm=pNode->GetObjectTM(time);
		
		if(m_bTri)
		{
			bool bCCWFace=Max_TMNegParity(tm);
			BOOL needDel=FALSE;
			NullView view;
			Mesh* pMaxMesh=((GeomObject*)os.obj)->GetRenderMesh(time,pNode,view,needDel);
			Mesh &maxMesh=*pMaxMesh;
			
			//1.建立VertNormal table, 顺序对应pPos
			MAX_ComputeVertexNormals(pMaxMesh,m_maxMeshNormals);

			//2.建立VertTangent table，顺序对应pPos
			TVFace	*pTVFaceBump=NULL;
			if (maxMesh.mapSupport(NORMAL_MAP_CHANNEL))
				pTVFaceBump=maxMesh.mapFaces(NORMAL_MAP_CHANNEL);
			Point3 *pUVBump=NULL;
			if (maxMesh.mapSupport(NORMAL_MAP_CHANNEL))
				pUVBump=maxMesh.mapVerts(NORMAL_MAP_CHANNEL);
			MAX_ComputeVertexTangents(pMaxMesh,pTVFaceBump,pUVBump,m_maxMeshTangents);

			//3.循环每个Face, 每个face索引到的3个顶点逐个检测是否已在VertBuf中, 并且按照
			//		VertBuf为该创建新的face(vert索引)
			for(int i=0;i<maxMesh.numFaces;i++)
				MergeFace(pNodeMtl,maxMesh,i,bCCWFace);
		}
		else
		{
			assert(0 && _T("polygon export not implement yet."));
		}
		
		//--transform the vertices by object TM
		tm.TransformPoints(&(*m_posArray.begin()),m_posArray.size());
		
		tm=Inverse(Transpose(tm));
		tm.TransformVectors(&(*m_normalArray.begin()),m_normalArray.size());
		for(size_t i=0;i<m_normalArray.size();i++)
		{
			Point3& normal=m_normalArray[i];
			normal=Normalize(normal);
		}

		tm.TransformVectors(&(*m_tangentArray.begin()),m_tangentArray.size());
		for(size_t i=0;i<m_tangentArray.size();i++)
		{
			Point3& tangent=m_tangentArray[i];
			tangent=Normalize(tangent);
		}
	}
	
	F3DExpMesh::~F3DExpMesh()
	{
		ExpSubMesh *pSub=NULL;
		for(size_t i=0;i<m_subMeshs.size();i++)
		{
			pSub=m_subMeshs[i];
			delete pSub;
		}
		m_subMeshs.clear();
	}
	
	void F3DExpMesh::GetTransUV(Point3 &uv, Texmap *pMap)
	{
		if(pMap == NULL)
			return;
		Matrix3 uvtrans;
		pMap->GetUVTransform(uvtrans);
		uvtrans.TransformPoints(&uv,1);
		
		float crop_u,crop_v,crop_w,crop_h;
		IParamBlock2 *pParamBlock = pMap->GetParamBlock(0);
		if (pParamBlock != NULL)
		{
			pParamBlock->GetValue(0, 0, crop_u, FOREVER);
			pParamBlock->GetValue(1, 0, crop_v, FOREVER);
			pParamBlock->GetValue(2, 0, crop_w, FOREVER);
			pParamBlock->GetValue(3, 0, crop_h, FOREVER);
		}
		
		uv.x=uv.x*crop_w + crop_u;
		uv.y=uv.y*crop_h + crop_v;
	}
	
	unsigned short F3DExpMesh::MergeVert(Mesh& maxMesh,ExpVert& exp,int faceIndex)
	{
		Point3	*pPos=maxMesh.verts;
		Point3 *pUVDiffuse=NULL;
		Point3 *pUVLightMap=NULL;
		
		if(maxMesh.mapSupport(1))
			pUVDiffuse=maxMesh.mapVerts(1);
		
		if(maxMesh.mapSupport(LIGHT_MAP_CHANNEL))
			pUVLightMap=maxMesh.mapVerts(LIGHT_MAP_CHANNEL);
		
		int f=-1;
		for(size_t i=0;i<m_expVerts.size();i++)
		{
			if(m_expVerts[i].nPosNormal == exp.nPosNormal
				&& m_expVerts[i].nUVDiffuse==exp.nUVDiffuse
				&& m_expVerts[i].nUVLightMap==exp.nUVLightMap
				&& m_expVerts[i].pMap0==exp.pMap0 
				&& m_expVerts[i].pMap1==exp.pMap1
				&& ((exp.dwSmGrp!=0 && m_expVerts[i].dwSmGrp==exp.dwSmGrp)
					||(exp.dwSmGrp==0 && m_expVerts[i].dwSmGrp==0 && m_expVerts[i].nFaceIndex==exp.nFaceIndex))
				//&& m_expVerts[i].nUVBump==exp.nUVBump
				//&& m_expVerts[i].pMapBump==exp.pMapBump
				//&& m_expVerts[i].nUVBumpDir==exp.nUVBumpDir
				)
			{
				f=i;
				break;
			}
		}
		if(f == -1)
		{
			m_expVerts.push_back(exp);
			f=m_expVerts.size() - 1;
			
			Point3 uv;
			if(pUVDiffuse)
			{
				uv=pUVDiffuse[exp.nUVDiffuse];
				GetTransUV(uv,exp.pMap0);
				m_uvArray.push_back(uv);
			}
			
			//pos
			m_posArray.push_back(pPos[exp.nPosNormal]);
			//normal
			Point3 pt=FindNormal(exp.nPosNormal, exp.dwSmGrp, faceIndex);
			m_normalArray.push_back(pt);
			
			//light map
			if(pUVLightMap)
			{
				uv=pUVLightMap[exp.nUVLightMap];
				GetTransUV(uv,exp.pMap1);
				m_uv2Array.push_back(uv);
			}

			//tangent
			pt=FindTangent(exp.nPosNormal, exp.dwSmGrp, faceIndex, exp.nUVDiffuse, exp.nUVBumpDir);
			m_tangentArray.push_back(pt);
		}
		
		return f;
	}	
	
	void F3DExpMesh::MergeFace(Mtl* pNodeMtl, Mesh& maxMesh, int index,bool bSwapFace)
	{
		Face	*pFaces=maxMesh.faces;
		/*The mesh mapping channel may be specified as one of the following: 
		0: Vertex Color channel.
		1: Default mapping channel (the TVert array).
		2 through MAX_MESHMAPS-1: The new mapping channels available in release 3.0.
		*/
		TVFace	*pTVFaces=NULL;
		TVFace	*pTVFaces2=NULL;
		
		//MAX建模人员按指定通道展开UV
		if(maxMesh.mapSupport(1))
			pTVFaces=maxMesh.mapFaces(1);
		
		if(maxMesh.mapSupport(LIGHT_MAP_CHANNEL))
			pTVFaces2=maxMesh.mapFaces(LIGHT_MAP_CHANNEL);
		
		//-- make sub mesh or find exist one
		Texmap *pMap0=NULL;
		Texmap *pMap1=NULL;
		ExpSubMesh *pSubMesh=NULL;
		if(pNodeMtl == NULL)//none mtl
		{
			if(m_subMeshs.size() == 0)
			{
				ExpSubMesh *pNewSubMesh=new ExpSubMesh;
				pNewSubMesh->m_nMtlID=-1;
				m_subMeshs.push_back(pNewSubMesh);
			}
			pSubMesh=m_subMeshs[0];
		}
		else 
		{
			Mtl *pBaseMtl=NULL;
			Mtl *pBakedMtl=NULL;
			if(pNodeMtl->ClassID()==Class_ID(BAKE_SHELL_CLASS_ID, 0))//support max light map
			{
				pBaseMtl=pNodeMtl->GetSubMtl(0);
				pBakedMtl=pNodeMtl->GetSubMtl(1);
				assert(pBaseMtl!=NULL);
				assert(pBakedMtl!=NULL);
				
				pMap1=pBakedMtl->GetSubTexmap(ID_DI);
			}
			else
				pBaseMtl=pNodeMtl;
			
			
			if(pBaseMtl->NumSubMtls() == 0)//stdmtl
			{
				if(m_subMeshs.size() == 0)
				{
					ExpSubMesh *pNewSubMesh=new ExpSubMesh;
					pNewSubMesh->m_nMtlID=m_pMtlCallBack->CallBack(pBaseMtl,-1,pBakedMtl);
					m_subMeshs.push_back(pNewSubMesh);
				}
				pSubMesh=m_subMeshs[0];
				pMap0=pBaseMtl->GetSubTexmap(ID_DI);
			}
			else//multi-sub mtl
			{
				MtlID mtlid=pFaces[index].getMatID();
				Mtl* pSubMtl = pBaseMtl->GetSubMtl(mtlid);
				if (!pSubMtl)
					return;

				pMap0=pSubMtl->GetSubTexmap(ID_DI);
				unsigned int mtlID=m_pMtlCallBack->CallBack(pBaseMtl,mtlid,pBakedMtl);
				for(size_t i=0;i<m_subMeshs.size();i++)
				{
					if(m_subMeshs[i]->m_nMtlID==mtlID)
					{
						pSubMesh=m_subMeshs[i];
						break;
					}
				}
				if(pSubMesh == NULL)
				{
					ExpSubMesh *pNewSubMesh=new ExpSubMesh;
					pNewSubMesh->m_nMtlID=mtlID;
					m_subMeshs.push_back(pNewSubMesh);
					pSubMesh=pNewSubMesh;
				}
			}
		}
		
		//--export face to sub mesh
		Polygon newFace(3);
		int i=index;
		Point3 P,Q,T,B,N,N1,N2;
		Point3 *verts=maxMesh.verts;
		Point3 v0, v1, v2;
		int uvDiffuse=-1;
		int uvLightMap=-1;
		int uvBump=-1;
		int uvBumpDir=-1;

		if (maxMesh.mapSupport(NORMAL_MAP_CHANNEL))
		{
			Point3 *pUVBump=maxMesh.mapVerts(NORMAL_MAP_CHANNEL);
			//计算uv朝向
			v0 = verts[pFaces[i].v[0]];
			v1 = verts[pFaces[i].v[1]];
			v2 = verts[pFaces[i].v[2]];
			P=v1-v0;Q=v2-v0;
			N1=pUVBump[pTVFaces[i].t[1]]-pUVBump[pTVFaces[i].t[0]];
			N2=pUVBump[pTVFaces[i].t[2]]-pUVBump[pTVFaces[i].t[0]];
			float divisor=N1.x*N2.y-N1.y*N2.x;
			T=(N2.y*P-N1.y*Q)/divisor;
			B=(N1.x*Q-N2.x*P)/divisor;
			N=(v1-v0)^(v2-v1);
			//计算tangent、binormal和normal三个向量组成的行列式的值的符号来判断UV的方向
			//T-a1 B-a2 N-a3
			float determinant=T.x*(B.y*N.z-B.z*N.y)-T.y*(B.x*N.z-B.z*N.x)+T.z*(B.x*N.y-B.y*N.x);
			uvBumpDir=determinant>0?0:1;
		}

		for(int j=0;j<3;j++)
		{
			if (pTVFaces)
				uvDiffuse=pTVFaces[i].t[j];
			if (pTVFaces2)
				uvLightMap=pTVFaces2[i].t[j];

			ExpVert expv(pFaces[i].v[j],pFaces[i].getSmGroup(),index,
				uvDiffuse,pMap0,uvLightMap,pMap1,uvBumpDir);
			newFace.SetIndex(j,MergeVert(maxMesh,expv,i));
		}
		if(bSwapFace)
		{
			unsigned short tmp=newFace.indices[0];
			newFace.indices[0]=newFace.indices[2];
			newFace.indices[2]=tmp;
		}
		pSubMesh->m_faceArray.push_back(newFace);
	}
}//namespace Cool3D