#include "StdAfx.h"
#include ".\RKeyFrame.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\RenderMtl\MKeyFrame.h"
#include "..\RenderMtl\MConstColor.h"
#define VFS_READ
#include "..\Public\SubMesh.h"
#include "..\Cool3DOption.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\SceneGraph\SceneGraph.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RKeyFrame,RenderNode);

	RKeyFrame::RKeyFrame(void)
	{
	}

	RKeyFrame::~RKeyFrame(void)
	{
	}

	void RKeyFrame::Create(const LodSubMesh* pSubMesh)
	{
		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	void RKeyFrame::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		//--shader
		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		if(pMtl!=NULL&&pMtl->IsResourceReady())
		{
			ASSERT(pMtl->IS_STATIC_CLASS(MKeyFrame));
			SetShader((MKeyFrame*)pMtl,pSGNode,pItem,pSG);
		}

		m_subMesh.Draw(nLod,pSys,this,pMtl);
	}

	void RKeyFrame::SetShader(MKeyFrame* pMtl,SceneNode* pSGNode,const RenderItem* pItem,SceneGraph* pSG)
	{
		MKeyFrame::EShader shader=MKeyFrame::ES_None;

		if(pMtl->m_pBump!=NULL
			&&Cool3DOption::Inst()->GetOption(Cool3DOption::EO_NormalMap)==Cool3DOption::ENM_Open)
		{
			if(pSG!=NULL&&pItem!=NULL)
			{
				tagLight* pPoint=pItem->pLights[0];
				if(pPoint==NULL)
					shader=MKeyFrame::ES_Direct;
				else
					shader=MKeyFrame::ES_DirectPoint1;

				//--设置vertexshader参数
				IVertShader* pVS=pMtl->m_pVS[shader]->GetVS();

				CameraBase* pCamera=pSG->GetCamera();
				Matrix4 matWorldViewPrj=pSGNode->GetWorldMat();
				matWorldViewPrj*=*pCamera->GetView();
				matWorldViewPrj*=*pCamera->GetPrj();

				const tagLight& sunLight=pSG->GetSunLight();
				Vector3 sunDir=-sunLight.dir;

				pVS->SetConstValue("g_matWorldViewPrj",&matWorldViewPrj,sizeof(Matrix4));
				pVS->SetConstValue("g_matWorld",&pSGNode->GetWorldMat(),sizeof(Matrix4));
				pVS->SetConstValue("g_directDir",&sunDir,sizeof(Vector3));
				pVS->SetConstValue("g_eyePos",&pCamera->GetPos(),sizeof(Vector3));

				if(pPoint!=NULL)
				{
					pVS->SetConstValue("g_pointRange",&pPoint->range,sizeof(float));
					pVS->SetConstValue("g_pointPos",&pPoint->pos,sizeof(Vector3));
					pVS->SetConstValue("g_pointAtten",&pPoint->attenuation.y,sizeof(float));
				}

				//--设置pixelshader参数
				IPixelShader* pPS=pMtl->m_pPS[shader]->GetPS();

				tagRenderColor& color=pMtl->m_pColor->m_color;
				Color4f sunDiffuse=sunLight.diffuse*color.diffuse;
				Color4f sunAmbient=sunLight.ambient*color.ambient;
				Color4f emissive=color.emissive;
				sunAmbient+=emissive;
				Color4f sunSpecular=sunLight.specular*color.specular;
				float power=color.power;

				pPS->SetColor4f("g_directDiffuse",sunDiffuse);
				pPS->SetColor4f("g_directAmbient",sunAmbient);
				pPS->SetColor4f("g_directSpecular",sunSpecular);
				pPS->SetConstValue("g_specularPower",&power,sizeof(float));

				if(pPoint!=NULL)
				{
					Color4f pointDiffuse=pPoint->diffuse*color.diffuse;
					Color4f pointSpecular=pPoint->specular*color.specular;

					pPS->SetColor4f("g_pointDiffuse",pointDiffuse);
					pPS->SetColor4f("g_pointSpecular",pointSpecular);
				}
			}
		}

		if(pMtl->m_shader!=shader)
		{
			pMtl->m_shader=shader;
			pMtl->InvalidateRenderCache();
		}
	}

	DWORD RKeyFrame::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD RKeyFrame::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_subMesh.GetMtlID(),pSGNode);
	}

	EMtlTrait RKeyFrame::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_subMesh.GetMtlID(),pSGNode);
	}

	const AABBox& RKeyFrame::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}

	int RKeyFrame::GetMtlID() const
	{
		return m_subMesh.GetMtlID();
	}
}//namespace Cool3D