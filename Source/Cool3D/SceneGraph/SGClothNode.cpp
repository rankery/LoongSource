#include "stdafx.h"
#include "SGClothNode.h"
#include "AvatarNode.h"
#include "EntityNode.h"
#include "..\Animation\AniCloth_SW.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\Util\Exception.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResCloth.h"
#include "..\ResSys\VertStream.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\Util\StlExt.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Cool3DOption.h"
#include "..\RenderSys\RCloth.h"
#include "..\RenderSys\RSkin.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\Animation\AnimationCtrl.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGClothNode,SceneNode);

	SGClothNode::SGClothNode(void)
		: m_pRes(0)
		, m_pAniCloth(0)
		, m_pVB(0)
		, m_nLod(0)
		, m_bEnable(true)
	{
	}

	SGClothNode::~SGClothNode(void)
	{
		m_vecBoneMatrixArray.clear();
		ResMgr::Inst()->ReleaseRenderable(m_pRes);
		m_pRes=NULL;
		FreePtrVector<IMaterial> freeMtls;
		freeMtls(m_mtls);
		SAFE_DELETE(m_pAniCloth);
		SAFE_DELETE(m_pVB);
	}

	void SGClothNode::Serialize(FArchive& ar)
	{
		ar<<m_pRes->GetName();
		SerialObjPtrVector(ar,m_mtls);
		SceneNode::Serialize(ar);
	}

	void SGClothNode::Deserialize(FArchive& ar)
	{
		tstring szResName;
		ar>>szResName;
		LoadRes(szResName.c_str());
		ASSERT(m_mtls.size()==0);
		DeserialObjPtrVector(ar,m_mtls);
		SceneNode::Deserialize(ar);
	}

	bool SGClothNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;//由父节点做视锥测试,自己不做
	}

	UINT SGClothNode::GetNumRenderNode() const
	{
		if(m_bEnable && m_pRes->IsCreated())
			return m_pRes->GetNumRenderNode();
		return 0;
	}

	RenderNode *SGClothNode::GetRenderNode(UINT i) const
	{
		if(m_pRes->IsCreated())
			return m_pRes->GetRenderNode(i);
		return NULL;
	}

	const Matrix4& SGClothNode::GetWorldMat() const
	{
		ASSERT(SceneNode::GetParent());
		return GetParent()->GetWorldMat();
	}

	void SGClothNode::GetBox(AABBox& out) const
	{
		// todo 取父节点box
		GetParent()->GetBox(out);
	}

	void SGClothNode::LoadRes(const TCHAR* szName)
	{
		m_pRes=(ResCloth*)(ResMgr::Inst()->NewRenderable(szName));
	}

	void SGClothNode::Update(float deltaTime,SceneGraph *pSG)
	{
		AABBox box;
		GetBox(box);
		m_nLod=pSG->CalcDetailLevel(box,NULL);

		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_Open)
		{
			CalcBoneMatrix(m_nLod);
		}

		UpdateSkinVB(m_nLod);

		// 更新顶点信息
		m_pVB->CopyFromVertStream(m_pAniCloth->GetVertStream());

		for(size_t i=0;i<m_mtls.size();i++)
		{
			IMaterial* pMtl=m_mtls[i];
			pMtl->UpdateModifiers(deltaTime);
		}
	}

	void SGClothNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		// 检测Mtl创建的情况
		if( m_pRes!=NULL && m_pRes->IsCreated() )
		{
			if( m_pRes->GetNumMtl()>0 && m_mtls.size()==0 )
				m_pRes->CreateMtlArray(m_mtls,NULL);

			// 布料动画必须每隔固定时间进行模拟，所以放在这里
			UpdateClothVB(deltaTime);
		}
	}

	UINT SGClothNode::GetNumMtl() const
	{
		return m_mtls.size();
	}

	IMaterial*	SGClothNode::GetMtl(int mtlID)
	{
		if( mtlID>=0 && mtlID<(int)m_mtls.size() )
			return m_mtls[mtlID];

		return NULL;
	}

	EVertType SGClothNode::GetVertType() const
	{
		if( m_pAniCloth == NULL )
			return EVType_PNT;

		return m_pAniCloth->GetVertStream()->GetType();
	}

	void SGClothNode::UpdateSkinVB(int lod)
	{
		if( m_pAniCloth == NULL )
		{
			if( m_pRes!=NULL && m_pRes->IsCreated() )
			{
				m_pAniCloth=new AniCloth_SW;
				m_pAniCloth->Create(m_pRes);

				const VertStream *pVerts=m_pAniCloth->GetVertStream();
				m_pVB=Device()->NewVB();
				m_pVB->Create(pVerts->GetBufferSize(),EB_DynamicWriteOnly,pVerts->GetStride());
			}
		}

		if( m_pAniCloth==NULL || m_pVB==NULL )
			return;

		ASSERT(GetParent()!=NULL);
		NodeAnimationCtrl *pAniCtrl=NULL;
		if( GetParent()->IS_KIND_OF(EntityNode) )
		{
			EntityNode *pParent=(EntityNode*)GetParent();
			pAniCtrl=(NodeAnimationCtrl*)pParent->GetAniCtrl();
		}
		else if( GetParent()->IS_KIND_OF(AvatarNode) )
		{
			AvatarNode *pParent=(AvatarNode*)GetParent();
			pAniCtrl=const_cast<NodeAnimationCtrl *>(pParent->GetSkeletonAniCtrl());
		}
		ASSERT(pAniCtrl!=NULL);

		//父节点新Update,所以aniCtrl已经更新
		m_pAniCloth->ComputeSkinVB(pAniCtrl,lod);
	}

	void SGClothNode::UpdateClothVB( float deltaTime )
	{
		if( m_pAniCloth==NULL )
		{
			if( m_pRes!=NULL && m_pRes->IsCreated() )
			{
				m_pAniCloth=new AniCloth_SW;
				m_pAniCloth->Create(m_pRes);

				const VertStream *pVerts=m_pAniCloth->GetVertStream();
				m_pVB=Device()->NewVB();
				m_pVB->Create(pVerts->GetBufferSize(),EB_DynamicWriteOnly,pVerts->GetStride());
			}
		}

		if( m_pAniCloth==NULL || m_pVB==NULL )
			return;

		ASSERT(GetParent()!=NULL);
		NodeAnimationCtrl *pAniCtrl=NULL;
		if( GetParent()->IS_KIND_OF(EntityNode) )
		{
			EntityNode *pParent=(EntityNode*)GetParent();
			pAniCtrl=(NodeAnimationCtrl*)pParent->GetAniCtrl();
		}
		else if( GetParent()->IS_KIND_OF(AvatarNode) )
		{
			AvatarNode *pParent=(AvatarNode*)GetParent();
			pAniCtrl=const_cast<NodeAnimationCtrl *>(pParent->GetSkeletonAniCtrl());
		}
		ASSERT(pAniCtrl!=NULL);

		//父节点新Update,所以aniCtrl已经更新
		m_pAniCloth->ComputeClothVB(pAniCtrl,GetWorldMat(),deltaTime);
	}

	void SGClothNode::ReplaceMtl(int mtlID,IMaterial *pMtl)
	{
		if( m_mtls.size()==0 )
			return;

		if( mtlID>=0 && mtlID<(int)m_mtls.size() )
		{
			IMaterial *pOld=m_mtls[mtlID];
			if( pOld != pMtl )//判断是否是同一对象
			{
				SAFE_DELETE(pOld);
				m_mtls[mtlID]=pMtl;
			}
		}
	}

	TResult SGClothNode::RayCollide(const Ray& ray)
	{
		if( !IsCreated() || !m_pRes || !m_pRes->IsCreated() || !m_pAniCloth )
			return TResult(false,0);

		return m_pRes->RayCollide(ray,&GetWorldMat(),m_pAniCloth->GetVertStream());
	}

	const tagBoneMatrixArray* const SGClothNode::GetBoneMatrixArray( const RSkin* const pSkin )
	{
		ASSERT( NULL != m_pRes );
		ASSERT( m_pRes->IsCreated() );

		for( int i=0; i<(int)m_vecBoneMatrixArray.size(); i++ )
		{
			if( pSkin == m_pRes->GetSkinRenderNode(i) )
				return &m_vecBoneMatrixArray[i];
		}
		return NULL;
	}

	void SGClothNode::BeginTransparent()
	{
		for( size_t i=0; i<m_mtls.size(); i++ )
		{
			IMaterial* pMtl=m_mtls[i];

			MMFrameBlend* pBlend=new MMFrameBlend;
			pBlend->m_blendEnable=true;
			pBlend->m_src=EBlend_SrcAlpha;
			pBlend->m_dst=EBlend_InvSrcAlpha;

			pBlend->SetName(_T("transparent"));

			pMtl->AttachModifier(pBlend);
			pMtl->InvalidateRenderCache();
		}
	}

	void SGClothNode::SetTransparent( float alpha )
	{
		for( size_t i=0; i<m_mtls.size(); i++ )
		{
			IMaterial* pMtl=m_mtls[i];
			if( pMtl->IS_KIND_OF(MGeneral) )
			{
				MGeneral* pMG=(MGeneral*)pMtl;
				if( pMG->m_pColor != NULL )
				{
					pMG->m_pColor->m_color.diffuse.A=alpha;
					pMG->UpdateRenderCache();
				}
			}
		}
	}

	void SGClothNode::EndTransparent()
	{
		for( size_t i=0; i<m_mtls.size(); i++)
		{
			IMaterial* pMtl=m_mtls[i];

			if(pMtl->RemoveModifier(_T("transparent")))
				pMtl->InvalidateRenderCache();
		}
	}

	void SGClothNode::CalcBoneMatrix( int lod )
	{
		if(m_pRes==NULL||!m_pRes->IsCreated())
			return;

		if( m_pRes->GetNumSkinRenderNode() == 0 )
			return;

		if( m_vecBoneMatrixArray.empty() )
		{
			m_vecBoneMatrixArray.resize( m_pRes->GetNumSkinRenderNode() );
		}

		ASSERT(GetParent()!=NULL);
		NodeAnimationCtrl *pAniCtrl=NULL;
		if( GetParent()->IS_KIND_OF(EntityNode) )
		{
			EntityNode *pParent=(EntityNode*)GetParent();
			pAniCtrl=(NodeAnimationCtrl*)pParent->GetAniCtrl();
		}
		else if( GetParent()->IS_KIND_OF(AvatarNode) )
		{
			AvatarNode *pParent=(AvatarNode*)GetParent();
			pAniCtrl=const_cast<NodeAnimationCtrl *>(pParent->GetSkeletonAniCtrl());
		}
		ASSERT(pAniCtrl!=NULL);

		for( UINT nNode = 0; nNode < m_pRes->GetNumSkinRenderNode(); nNode++ )
		{
			RenderNode* pNode = m_pRes->GetSkinRenderNode( nNode );
			ASSERT( pNode->IS_STATIC_CLASS(RSkin ) );
			RSkin* pSkin = (RSkin*)pNode;
			int nSubMeshID = pSkin->GetSubMeshID();

			const vector<tagBoneOffsetMatrix>& boneOffsetMatrix = m_pRes->GetBoneOffsetMatrix( nSubMeshID );
			m_vecBoneMatrixArray[nNode].nNumMat = min( MAX_BONE_MATRIX, (int)boneOffsetMatrix.size() );
			for(int nMat = 0; nMat < m_vecBoneMatrixArray[nNode].nNumMat; nMat++ )
			{
				int boneID = boneOffsetMatrix[nMat].boneId;
				D3DXMatrixMultiply( &m_vecBoneMatrixArray[nNode].matBone[nMat], &boneOffsetMatrix[nMat].offsetMatrix, pAniCtrl->GetBoneSpace(boneID) );
			}
		}
	}
}//namespace Cool3D