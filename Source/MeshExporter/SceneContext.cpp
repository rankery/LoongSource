#include "ExpMesh.h"
#include "ExpMtl.h"
#include "SceneWriter.h"
#include "SceneContext.h"
#include "SceneEnumProc.h"
#include <iostream>
#include <sstream>
#include "MsgBox.h"
#include <assert.h>
#include "..\F3DMaxExp\MaxMath.h"

namespace Cool3D
{
	
	SceneContext::SceneContext()
	{
		
	}
	
	SceneContext::~SceneContext()
	{
		
	}	

	SceneContext g_sceneContext;
	SceneContext* SceneContext::Inst()
	{
		return &g_sceneContext;
	}
	
	bool SceneContext::WriteToFile(const TCHAR *szPath, bool bExpLMUV, bool bExpBUUV, const TCHAR* szOrigin)
	{
		std::auto_ptr<SceneWriter> pWriter(GreateWriter(szPath));
		if (bExpLMUV && !bExpBUUV)
			pWriter->SetVertType(EVType_PNT2);
		else if (!bExpLMUV && bExpBUUV)
			pWriter->SetVertType(EVType_PNGT);

		INode *pNode=GetNode(szOrigin);
		if( pNode!=NULL )
		{
			return pWriter->Write(szPath,pNode);
		}
		else
		{
			return pWriter->Write(szPath);
		}
	}
	
	void SceneContext::BuildNodeList(IScene* pScene)
	{
		m_pBoneNodes.clear();

		assert(pScene!=NULL);
		SceneEnumProc enumProc;
		pScene->EnumTree(&enumProc);

		std::ostringstream fmt;
		fmt<<_T("num mesh nodes=")<<m_pMeshNodes.size()<<_T("\r\n");
		g_msgBox.ShowMsg(fmt.str().c_str());		
	}
	
	void SceneContext::ExportData(TimeValue time)
	{
		int totalFace=0,totalVert=0;
		int i;
		//-- meshs
		for(i=0;i<m_pMeshNodes.size();i++)
		{
			ExpMesh *newMesh=new ExpMesh(m_pMeshNodes[i],time,true);
			m_pMeshs.push_back(newMesh);
			totalFace+=newMesh->GetNumFaces();
			totalVert+=newMesh->GetNumVert();
			/*g_msgBox.ShowMsg("export <%s>, numSubMeshs=%d,numFaces=%d numVerts=%d"
				,m_pMeshNodes[i]->NodeName()
				,newMesh->GetNumSubMesh(),newMesh->GetNumFaces(),newMesh->GetNumVert());
			if(newMesh->GetNumMapChannel() == 0)
				g_msgBox.ShowMsg("  NO TEXTURE MAP!\r\n");
			else
				g_msgBox.ShowMsg("  numMapChannel=%d\r\n",newMesh->GetNumMapChannel());
				*/
		}
		
		if(time==0)
		{
			g_msgBox.ShowMsg(_T("Mesh total faces=%d, total vertices=%d\r\n"),totalFace,totalVert);
			g_msgBox.ShowMsg(_T("Number of mtl = %d \r\n"),m_mtlLib.size());
		}
	}
		
	void SceneContext::AddMeshNode(INode *pNode)
	{
		if(pNode==NULL)
			return;
		////--检查名称,看是否是tag node
		const TCHAR* szName=pNode->GetName();
		bool bTagNode=false;
		if(strlen(szName)>4)
		{
			if(szName[0]==_T('t')
				&& szName[1]==_T('a')
				&& szName[2]==_T('g')
				&& szName[3]==_T('_'))//tag_XXXX
			{
				bTagNode=true;
			}
		}

		//--
		if(bTagNode)
		{
			m_tagNodeExp.AddMaxNode(pNode);
			g_msgBox.ShowMsg(_T("tagNode %s\r\n"),szName);
		}
		else
		{
			m_pMeshNodes.push_back(pNode);
			g_msgBox.ShowMsg(_T("mesh %s\r\n"),szName);
		}
	}

	void SceneContext::AddBoneNode( INode* pNode )
	{
		if(pNode==NULL)
			return;

		m_pBoneNodes.push_back(pNode);
	}
	
	void SceneContext::ClearExportData()
	{
		int i;
		for(i=0;i<m_mtlLib.size();i++)
			delete m_mtlLib[i];
		m_mtlLib.clear();
		
		for(i=0;i<m_pMeshs.size();i++)
			delete m_pMeshs[i];
		m_pMeshs.clear();
	}

	void SceneContext::ClearAll()
	{
		//--clear max nodes
		m_pMeshNodes.clear();
		m_tagNodeExp.Clear();

		//--clear customer data
		ClearExportData();
	}
	
	unsigned int SceneContext::MergeMtl(Mtl *pMaxMtl, int sub,Mtl *pBakedMtl)
	{
		int find=-1;
		//--find exist
		for(int i=0;i<m_mtlLib.size();i++)
		{
			if(m_mtlLib[i]->Equal2Max(pMaxMtl,sub,pBakedMtl))
			{
				find = i;
				break;
			}
		}

		//--build new one
		if(find == -1)
		{
			find=m_mtlLib.size();
			ExpMtl *pNewMtl=new ExpMtl(pMaxMtl,sub,find,pBakedMtl);
			m_mtlLib.push_back(pNewMtl);
		}

		return find;
	}

	bool SceneContext::GetMat( INode *pNode, D3DXMATRIX& mat ) const
	{
		if( pNode!=NULL )
		{
			// 获得node的模型空间矩阵的逆
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
			D3DXMatrixIdentity(&mat);

			if(bMirrored)
			{
				D3DXMatrixScaling(&mMirror,-1,-1,-1);
				mat*=mMirror;
			}	
			mat*=mRotate;
			mat*=mTrans;

			D3DXMatrixInverse(&mat,NULL,&mat);
			return true;
		}

		return false;
	}

	INode *SceneContext::GetNode( const TCHAR *szOrigin ) const
	{
		if( szOrigin!=NULL )
		{
			Cool3D::Vector3 tran;
			Cool3D::Quaternion quat;
			for( size_t i=0; i<m_pBoneNodes.size(); i++ )
			{
				if(_tcsicmp(m_pBoneNodes[i]->GetName(),szOrigin)==0)
				{
					return m_pBoneNodes[i];
				}
			}
		}

		return NULL;
	}	
}//namespace Cool3D