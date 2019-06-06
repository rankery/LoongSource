#include "StdAfx.h"
#include ".\dynamicmesh.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"
#include "..\Kernel.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\Math\CameraBase.h"
#include "..\SceneGraph\SceneGraph.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(DynamicMesh,RenderNode);

	DynamicMesh::DynamicMesh(void)
	{
		m_drawType=EDraw_TriangleList;
		m_mtlID=-1;
		m_numPrimitive=0;
		m_numDraw=0;
		m_bDrawAsSprites=false;		
		m_spriteSize=20.0f;

		m_bIdentityWorldMat = false;
		m_bZBias=false;
	}

	DynamicMesh::~DynamicMesh(void)
	{
	}

	int CalNumPrimitiveByNumVert(EDrawType drawType,int numVert)
	{
		int ret=0;
		switch(drawType)
		{
		case EDraw_PointList:
			ret=numVert;
			break;
		case EDraw_LineList:
			ret=numVert/2;
			break;
		case EDraw_LineStrip:
			ret=numVert-1;
			break;
		case EDraw_TriangleList:
			ret=numVert/3;
			break;
		case EDraw_TriangleStrip:
			ret=numVert-2;
			break;
		case EDraw_TriangleFan:
			ret=numVert-2;
			break;
		default:
			ASSERT(0 && "unknown draw type");
		}
		return ret;
	}

	void DynamicMesh::Create(int mtlID,EDrawType drawType,EVertType vertType,UINT numVert)
	{
		m_numPrimitive=CalNumPrimitiveByNumVert(drawType,numVert);
		m_numDraw=m_numPrimitive;
		m_mtlID=mtlID;
		m_drawType=drawType;
		m_vertStream.Create(vertType,numVert);
	}

	bool DynamicMesh::SupportLod()
	{
		return false;
	}

	void DynamicMesh::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		if(m_vertStream.GetNumVert() <=0
			|| m_vertStream.GetRawBuffer()==NULL)
			return;
		if(m_numDraw<=0)
			return;
		IDevice *pDev=Device();

		if(m_bIdentityWorldMat)
		{
			pDev->MatPush(EMS_World);
			pDev->MatLoad(EMS_World,&MathConst::IdentityMat);
			pDev->FFlushMatrix(true,false,false,false);//flush world matrix only
		}

		if( m_bZBias )
		{
			CameraBase *pCamera=pSG->GetCamera();
			Matrix4 prjZbias;
			pCamera->GetPrjZBias(-0.0001f,prjZbias);
			pDev->MatPush( EMS_Prj );
			pDev->MatLoad(EMS_Prj,&prjZbias);
			pDev->FFlushMatrix(false,false,true,false);
		}

		//--vert stream define
		IStreamDefine *pDef=pSys->GetStreamDefine(m_vertStream.GetType());
		pDev->SetStreamDefine(pDef);

		//--vert buffer
		IVertBuffer *pVB=pSys->AllocVB(m_vertStream.GetBufferSize());
		ASSERT(pVB!=NULL);
		pVB->CopyFromVertStream(&m_vertStream);
		pDev->SetVB(0,pVB);
		//pDev->SetCullMode(ECull_None);//test
		
		//--mtl & draw
		int numPass=RenderNode::BeginMtl(pSys,pSGNode->GetMtl(m_mtlID));
		for(int i=0;i<numPass;i++)
		{
			RenderNode::BeginMtlPass(i);
			if(m_bDrawAsSprites)
				pDev->DrawPointSprites(m_spriteSize,m_numDraw);
			else
				pDev->Draw(m_drawType,m_numDraw);

			RenderNode::EndMtlPass();
		}
		RenderNode::EndMtl();
		pSys->FreeVB(pVB);

		if(m_bIdentityWorldMat)
		{
			pDev->MatPop(EMS_World);
			pDev->FFlushMatrix(true,false,false,false);//flush world matrix only
		}

		if( m_bZBias )
		{
			pDev->MatPop(EMS_Prj);
			pDev->FFlushMatrix(false,false,true,false);
		}
	}

	EMtlTrait DynamicMesh::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_mtlID,pSGNode);
	}

	DWORD DynamicMesh::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_mtlID,pSGNode);
	}

	DWORD DynamicMesh::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_mtlID,pSGNode);
	}

	const AABBox& DynamicMesh::GetAABBox() const
	{
		return m_box;
	}

	void DynamicMesh::BeginBuild(int mtlID,EDrawType primitiveType)
	{
		m_mtlID=mtlID;
		m_drawType=primitiveType;
		//--clear all build buffers
		m_posBuffer.clear();
		m_uvwBuffer.clear();
		m_diffuseBuffer.clear();
	}

	void DynamicMesh::AddPos(const Vector3& pos)
	{
		m_posBuffer.push_back(pos);
	}

	Vector3& DynamicMesh::BuildGetPos(int i)
	{
		ASSERT(i>=0 && i<(int)m_posBuffer.size());
		return m_posBuffer[i];
	}

	void DynamicMesh::AddTexCoord(const Vector3& uvw)
	{
		m_uvwBuffer.push_back(uvw);
	}

	void DynamicMesh::AddDiffuse(const Color4f& diffuse)
	{
		m_diffuseBuffer.push_back(diffuse);
	}

	void DynamicMesh::EndBuild()
	{
		//--
		int numVert=m_posBuffer.size();
		if(numVert<=0)
		{
			return;
		}

		if(m_diffuseBuffer.empty())
		{
			if(m_vertStream.GetNumVert()!=numVert)
				Create(m_mtlID,m_drawType,EVType_PTp,numVert);

			ASSERT(m_vertStream.GetType()==EVType_PTp);
			Vert_PTp *pVert=(Vert_PTp *)m_vertStream.GetRawBuffer();
			ASSERT(m_posBuffer.size() == m_uvwBuffer.size());
			for(int i=0;i<numVert;i++)
			{
				pVert->pos=m_posBuffer[i];
				pVert->uvw=m_uvwBuffer[i];
				pVert++;
			}		
		}
		else
		{
			if(m_vertStream.GetNumVert()!=numVert)
				Create(m_mtlID,m_drawType,EVType_PTpD,numVert);

			ASSERT(m_vertStream.GetType()==EVType_PTpD);
			Vert_PTpD *pVert=(Vert_PTpD *)m_vertStream.GetRawBuffer();
			ASSERT(m_posBuffer.size() == m_uvwBuffer.size());
			ASSERT(m_posBuffer.size() == m_diffuseBuffer.size());
			for(int i=0;i<numVert;i++)
			{
				pVert->pos=m_posBuffer[i];
				pVert->uvw=m_uvwBuffer[i];
				pVert->diffuse=Color4ub(m_diffuseBuffer[i]);
				pVert++;
			}		
		}

		//--
		m_posBuffer.clear();
		m_uvwBuffer.clear();
		m_diffuseBuffer.clear();
	}

	void DynamicMesh::SetDrawLimit(int numPrimitive)
	{
		if(numPrimitive==-1)//-1的话,画出全部
		{
			m_numDraw=m_numPrimitive;
		}
		else
		{
			ASSERT(numPrimitive>=0 && numPrimitive<=m_numPrimitive);
			if(numPrimitive>m_numPrimitive)
				numPrimitive=m_numPrimitive;
			m_numDraw=numPrimitive;
		}
	}
}//namespace Cool3D