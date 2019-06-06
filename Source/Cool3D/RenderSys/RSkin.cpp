#include "StdAfx.h"
#include ".\RSkin.h"
#include "..\RenderMtl\MSkin.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\SceneGraph\SGSkinNode.h"
#include "..\SceneGraph\SGClothNode.h"
#include "..\SceneGraph\AvatarNode.h"
#include "..\Cool3DOption.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Device\IVertShader.h"
#include "..\Device\IPixelShader.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Animation\AnimationCtrl.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(RSkin,RenderNode);

	RSkin::RSkin(void)
	{
		m_pStaticVB=NULL;
		m_vertType=EVType_None;
		m_subMeshID=0;
	}

	RSkin::~RSkin(void)
	{
	}

	void RSkin::Create(const LodSubMesh* pSubMesh,IVertBuffer* pStaticVB,EVertType vertType,int subMeshID)
	{
		m_vertType=vertType;
		m_pStaticVB=pStaticVB;
		m_subMeshID=subMeshID;
		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	void RSkin::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		ASSERT(pSGNode->IS_STATIC_CLASS(SGSkinNode));
		SGSkinNode *pSGSkin=(SGSkinNode*)pSGNode;

		IDevice* pDev=Device();
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_None)
		{
			if(!pSGSkin->IsCreated())
				return;

			IStreamDefine *pDef=pSys->GetStreamDefine(pSGSkin->GetVertType());
			pDev->SetStreamDefine(pDef);
			pDev->SetVB(0,pSGSkin->GetVB());
		}
		else
		{
			ASSERT(m_pStaticVB!=NULL);
			IStreamDefine *pDef=pSys->GetStreamDefine(m_vertType);
			pDev->SetStreamDefine(pDef);
			pDev->SetVB(0,m_pStaticVB);
		}

		//--shader
		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		if(pMtl!=NULL&&pMtl->IsResourceReady())
		{
			ASSERT(pMtl->IS_STATIC_CLASS(MSkin));
			SetShader((MSkin*)pMtl,pSGNode,pItem,pSG);
		}

		m_subMesh.Draw(pSGSkin->GetLod(),pSys,this,pMtl);
	}

	void RSkin::SetShader(MSkin* pMtl,SceneNode* pSGNode,const RenderItem* pItem,SceneGraph* pSG)
	{
		//--选择shader
		MSkin::EShader shader=MSkin::ES_None;
		if(pMtl->m_pBump!=NULL
			&&Cool3DOption::Inst()->GetOption(Cool3DOption::EO_NormalMap)==Cool3DOption::ENM_Open)
		{
			if(pSG!=NULL&&pItem!=NULL)
			{
				const tagLight* pPoint=pItem->pLights[0];
				if(pPoint==NULL)
				{
					shader=MSkin::ES_Bump_Direct_Skin;
				}
				else
				{
					shader=MSkin::ES_Bump_Direct_Point1_Skin;
				}
			}
		}
		else
		{
			if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_Open)
			{
				if(pSG!=NULL&&pItem!=NULL)
				{
					const tagLight* pPoint=pItem->pLights[0];
					if(pPoint==NULL)
					{
						shader=MSkin::ES_Direct_Skin;
					}
					else
					{
						shader=MSkin::ES_Direct_Point1_Skin;
					}
				}
			}
		}

		//--设置shader参数
		if(shader!=MSkin::ES_None)
		{
			IVertShader* pVS=NULL;
			IPixelShader* pPS=NULL;

			if(pMtl->m_pVS[shader]!=NULL)
				pVS=pMtl->m_pVS[shader]->GetVS();
			if(pMtl->m_pPS[shader]!=NULL)
				pPS=pMtl->m_pPS[shader]->GetPS();

			switch(shader) 
			{
			case MSkin::ES_Bump_Direct_Skin:
				{
					SetBumpDirectVShader(pVS,pSG,pSGNode);
					SetBumpDirectPShader(pPS,pMtl,pSG);
					SetSkinVShader(pVS,pSGNode);
				}
				break;
			case MSkin::ES_Bump_Direct_Point1_Skin:
				{
					SetBumpDirectVShader(pVS,pSG,pSGNode);
					SetBumpDirectPShader(pPS,pMtl,pSG);
					SetBumpPoint1VShader(pVS,pItem);
					SetBumpPoint1PShader(pPS,pMtl,pItem);
					SetSkinVShader(pVS,pSGNode);
				}
				break;
			case MSkin::ES_Direct_Skin:
				{
					SetDirectVShader(pVS,pMtl,pSGNode,pSG);
					SetSkinVShader(pVS,pSGNode);
				}
				break;
			case MSkin::ES_Direct_Point1_Skin:
				{
					SetDirectVShader(pVS,pMtl,pSGNode,pSG);
					SetPoint1VShader(pVS,pMtl,pItem);
					SetSkinVShader(pVS,pSGNode);
				}
				break;
			case MSkin::ES_None:
				break;//do nothing
			default:
				ASSERT(0);
			}
		}

		//--更新rendercache
		if(pMtl->m_shader!=shader)
		{
			pMtl->m_shader=shader;
			pMtl->InvalidateRenderCache();
		}
	}

	void RSkin::SetBumpDirectVShader(IVertShader* pVS,SceneGraph* pSG,SceneNode* pSGNode)
	{
		ASSERT(pSG!=NULL);

		CameraBase* pCamera=pSG->GetCamera();
		Matrix4 matWorldViewPrj;
		D3DXMatrixMultiply(&matWorldViewPrj,&pSGNode->GetWorldMat(),pCamera->GetView());
		D3DXMatrixMultiply(&matWorldViewPrj,&matWorldViewPrj,pCamera->GetPrj());

		const tagLight& sunLight=pSG->GetSunLight();
		Vector3 sunDir=-sunLight.dir;

		pVS->SetMatrix("g_matWorldViewPrj",&matWorldViewPrj);
		pVS->SetMatrix("g_matWorld",&pSGNode->GetWorldMat());
		pVS->SetFloatArray("g_directDir",(float*)&sunDir,3);
		pVS->SetFloatArray("g_eyePos",(float*)&pCamera->GetPos(),3);
	}

	void RSkin::SetBumpDirectPShader(IPixelShader* pPS,MSkin* pMtl,SceneGraph* pSG)
	{
		ASSERT(pSG!=NULL);

		tagRenderColor& color=pMtl->m_pColor->m_color;
		const tagLight& sunLight=pSG->GetSunLight();

		Color4f sunDiffuse=sunLight.diffuse*color.diffuse;
		Color4f sunAmbient=sunLight.ambient*color.ambient;
		Color4f emissive=color.emissive;
		sunAmbient+=emissive;
		Color4f sunSpecular=sunLight.specular*color.specular;
		float power=color.power;

		pPS->SetVector("g_directDiffuse",(Vector4*)&sunDiffuse);
		pPS->SetVector("g_directAmbient",(Vector4*)&sunAmbient);
		pPS->SetVector("g_directSpecular",(Vector4*)&sunSpecular);
		pPS->SetFloat("g_specularPower",power);
	}

	void RSkin::SetBumpPoint1VShader(IVertShader* pVS,const RenderItem* pItem)
	{
		ASSERT(pItem!=NULL);

		pVS->SetFloat("g_pointRange",pItem->pLights[0]->range);
		pVS->SetFloatArray("g_pointPos",(float*)&pItem->pLights[0]->pos,3);
		pVS->SetFloat("g_pointAtten",pItem->pLights[0]->attenuation.y);
	}

	void RSkin::SetBumpPoint1PShader(IPixelShader* pPS,MSkin* pMtl,const RenderItem* pItem)
	{
		ASSERT(pItem!=NULL);
		tagRenderColor& color=pMtl->m_pColor->m_color;

		Color4f pointDiffuse=pItem->pLights[0]->diffuse*color.diffuse;
		Color4f pointSpecular=pItem->pLights[0]->specular*color.specular;

		pPS->SetVector("g_pointDiffuse",(Vector4*)&pointDiffuse);
		pPS->SetVector("g_pointSpecular",(Vector4*)&pointSpecular);
	}

	void RSkin::SetSkinVShader(IVertShader* pVS,SceneNode* pSGNode)
	{
		ASSERT( NULL != pSGNode );

		const tagBoneMatrixArray* pMatArray = NULL;

		if( pSGNode->IS_STATIC_CLASS(SGSkinNode) )
		{
			SGSkinNode* pNode = (SGSkinNode*)pSGNode;
			pMatArray = pNode->GetBoneMatrixArray( this );
		}
		else if( pSGNode->IS_STATIC_CLASS(SGClothNode) )
		{
			SGClothNode* pNode = (SGClothNode*)pSGNode;
			pMatArray = pNode->GetBoneMatrixArray( this );
		}

		if( NULL == pMatArray )
			return;

		if( pMatArray->nNumMat > 0 )
			pVS->SetMatrixArray( "g_boneMatrix", &pMatArray->matBone[0], pMatArray->nNumMat );
	}

	void RSkin::SetDirectVShader(IVertShader* pVS,MSkin* pMtl,SceneNode* pSGNode,SceneGraph* pSG)
	{
		ASSERT(pSG!=NULL);

		CameraBase* pCamera=pSG->GetCamera();
		Matrix4 matWorldViewPrj;
		D3DXMatrixMultiply(&matWorldViewPrj,&pSGNode->GetWorldMat(),pCamera->GetView());
		D3DXMatrixMultiply(&matWorldViewPrj,&matWorldViewPrj,pCamera->GetPrj());
		pVS->SetMatrix("g_matWorldViewPrj",&matWorldViewPrj);
		pVS->SetMatrix("g_matWorld",&pSGNode->GetWorldMat());

		tagRenderColor& color=pMtl->m_pColor->m_color;
		const tagLight& sunLight=pSG->GetSunLight();
		Vector3 sunDir=-sunLight.dir;
		Vec3Normalize(sunDir);
		Color4f sunDiffuse=sunLight.diffuse*color.diffuse;
		Color4f sunAmbient=sunLight.ambient*color.ambient;
		Color4f emissive=color.emissive;
		sunAmbient+=emissive;
		pVS->SetFloatArray("g_directDir",(float*)&sunDir,3);
		pVS->SetVector("g_directDiffuse",(Vector4*)&sunDiffuse);
		pVS->SetVector("g_directAmbient",(Vector4*)&sunAmbient);
	}

	void RSkin::SetPoint1VShader(IVertShader* pVS,MSkin* pMtl,const RenderItem* pItem)
	{
		ASSERT(pItem!=NULL);
		tagRenderColor& color=pMtl->m_pColor->m_color;

		Color4f pointDiffuse=pItem->pLights[0]->diffuse*color.diffuse;

		pVS->SetFloat("g_pointRange",pItem->pLights[0]->range);
		pVS->SetFloatArray("g_pointPos",(float*)&pItem->pLights[0]->pos,3);
		pVS->SetFloat("g_pointAtten",pItem->pLights[0]->attenuation.y);
		pVS->SetVector("g_pointDiffuse",(Vector4*)&pointDiffuse);
	}

	EMtlTrait RSkin::GetMtlTrait(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD RSkin::GetMtlTrait1(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait1(m_subMesh.GetMtlID(),pSGNode);
	}

	DWORD RSkin::GetMtlTrait2(SceneNode *pSGNode) const
	{
		return RenderNode::GetMtlTrait2(m_subMesh.GetMtlID(),pSGNode);
	}

	const AABBox& RSkin::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}
}//namespace Cool3D