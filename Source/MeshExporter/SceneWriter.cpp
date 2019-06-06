#include <Windows.h>
#include "SceneWriter.h"
#include "StaticMesh\StaticMeshWriter.h"
#include "SceneMesh\SceneMeshFile.h"
#include "ExpMtl.h"
#include "Math\Max2D3D.h"
#include <string>
#include <assert.h>
#include "msgBox.h"
#include ".\Actor\ActorKeyFrameFile.h"

namespace Cool3D
{
	
	SceneWriter::SceneWriter()
	{
		m_vertType=EVType_PNT;
	}
	
	SceneWriter::~SceneWriter()
	{
		
	}
	
	void SceneWriter::Prepare()
	{
		
	}
	
	SceneWriter *GreateWriter(const TCHAR* szPath)
	{
		int len=_tcslen(szPath);
		assert(len >= 4);
		SceneWriter *pWriter=NULL;
		tstring ext=szPath+(len-3);

		if(_tcsicmp(ext.c_str(),_T("FSM")) == 0)
			pWriter=new StaticMeshWriter;
		else if(_tcsicmp(ext.c_str(),_T("SCN")) == 0)
			pWriter=new SceneMeshFile;
		else if(_tcsicmp(ext.c_str(),_T("Fak")) == 0)
			pWriter=new ActorKeyFrameFile;
		
		return pWriter;
	}
	
	int SceneWriter::WriteMtlFromSceneContext(FILE *fp, const TCHAR* szFilePath)
	{
		tstring m_szExportPath=szFilePath;
		int p=m_szExportPath.find_last_of(_T('\\'));
		if(p!=tstring::npos)
			m_szExportPath.erase(p);

		tagMtlDesc mtl;
		int numMtl=SceneContext::Inst()->GetNumMtls();
		for(int i=0;i<numMtl;i++)
		{
			SceneContext::Inst()->GetMtl(i)->GetMtlDesc(mtl);
			fwrite(&mtl,sizeof(tagMtlDesc),1,fp);
		}
		
		return numMtl;
	}

	void SceneWriter::GetVert(ExpMesh *pMesh, std::vector<BYTE>& out)
	{
		UINT vertCount=pMesh->GetNumVert();
		if (vertCount<=0)
			return;

		UINT vertSize=GetVertSize(m_vertType);
		UINT oldCount=out.size();
		out.resize(oldCount+vertSize*vertCount);
		BYTE *pVert=&out[oldCount];
		switch (m_vertType)
		{
		case EVType_PNT:
			{
				Vert_PNT *tmp=NULL;
				for(int i=0;i<vertCount;i++)
				{
					tmp=(Vert_PNT *)pVert;
					Point3ToVector3_D3D(pMesh->GetPos(i),tmp->pos);
					Point3ToVector3_D3D(pMesh->GetNormal(i),tmp->normal);

					if(pMesh->IfUV())
					{
						Point3ToUV_D3D(pMesh->GetUV(i),tmp->uv);
					}
					else
						tmp->uv=Vector2(0.0f,0.0f);

					pVert+=vertSize;
				}
			}
			break;
		case EVType_PNGT:
			{
				Vert_PNGT *tmp=NULL;
				for (int i=0;i<vertCount;i++)
				{
					tmp=(Vert_PNGT *)pVert;
					Point3ToVector3_D3D(pMesh->GetPos(i),tmp->pos);
					Point3ToVector3_D3D(pMesh->GetNormal(i),tmp->normal);

					if (pMesh->IfTangent())
						Point3ToVector3_D3D(pMesh->GetTangent(i),tmp->tangent);
					else
						tmp->tangent=Vector3(0.0f,0.0f,0.0f);

					if (pMesh->IfUV())
						Point3ToUV_D3D(pMesh->GetUV(i),tmp->uv);
					else
						tmp->uv=Vector2(0.0f,0.0f);

					pVert+=vertSize;
				}
			}
			break;
		case EVType_PNT2:
			{
				Vert_PNT2 *tmp=NULL;
				for (int i=0;i<vertCount;i++)
				{
					tmp=(Vert_PNT2 *)pVert;
					Point3ToVector3_D3D(pMesh->GetPos(i),tmp->pos);
					Point3ToVector3_D3D(pMesh->GetNormal(i),tmp->normal);

					if(pMesh->IfUV())
						Point3ToUV_D3D(pMesh->GetUV(i),tmp->uv);
					else
						tmp->uv=Vector2(0.0f,0.0f);

					if (pMesh->IfUV2())
						Point3ToUV_D3D(pMesh->GetUV2(i),tmp->uv2);
					else
						tmp->uv2=Vector2(0.0f,0.0f);

					pVert+=vertSize;
				}
			}
			break;
		}
	}

	void SceneWriter::GetSubMeshs(WORD baseIndex,WORD numVert,ExpMesh* pMesh, std::vector<SubMesh>& out)
	{
		TriFace tmpFace;
		SubMesh tmp;
		tmp.baseIndex=baseIndex;
		tmp.numVert=numVert;
		
		for(int i=0;i<pMesh->GetNumSubMesh();i++)
		{
			ExpSubMesh *pSub=pMesh->GetSubMesh(i);
			tmp.mtlID=pSub->m_nMtlID;
			tmp.baseFaces.clear();

			Vector3& max=tmp.max;
			Vector3& min=tmp.min;
			max.x=max.y=max.z=-FLOAT_MAX;
			min.x=min.y=min.z=FLOAT_MAX;
			
			for(size_t j=0;j<pSub->m_faceArray.size();j++)
			{
				assert(pSub->m_faceArray[j].numVert==3);
				for(int k=0;k<3;k++)
				{
					tmpFace.index[k]=pSub->m_faceArray[j].indices[k];
									
					//--bounding box
					Vector3 pos;
					Point3ToVector3_D3D(pMesh->GetPos(tmpFace.index[k]),pos);
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
				}//endof for(k)
				tmp.baseFaces.push_back(tmpFace);
			}//endof for(each face)
			
			out.push_back(tmp);
		}//endof for(each sub mesh)
	}

	void SceneWriter::BuildLodSubMesh(const SubMesh& src,LodSubMesh& out)
	{
		out.CopySubMesh(&src);
		out.numLod=0;
		//--build lod
	}

	void SceneWriter::GetPos(BYTE *m_pBuf, UINT i,Vector3& out)
	{
		switch(m_vertType)
		{
		case EVType_PNGT:
			{
				Vert_PNGT *pV=(Vert_PNGT *)m_pBuf;
				pV+=i;
				out=pV->pos;
			}
			break;
		case EVType_PNT:
			{
				Vert_PNT *pV=(Vert_PNT *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PNT2:
			{
				Vert_PNT2 *pV=(Vert_PNT2 *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PT2:
			{
				Vert_PT2 *pV=(Vert_PT2 *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PD:
			{
				Vert_PD *pV=(Vert_PD *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PTD:
			{
				Vert_PTD *pV=(Vert_PTD *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PTp:
			{
				Vert_PTp *pV=(Vert_PTp *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_Water:
			{
				Vert_Water *pV=(Vert_Water *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		case EVType_PT:
			{
				Vert_PT *pV=(Vert_PT *)m_pBuf;
				pV+=i;
				out=pV->pos;
				break;
			}
		}
	}
}//namespace Cool3D