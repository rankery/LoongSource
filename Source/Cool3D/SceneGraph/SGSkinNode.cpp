#include "StdAfx.h"
#include ".\sgskinnode.h"
#include "AvatarNode.h"
#include "EntityNode.h"
#include "..\Animation\AniSkin_SW.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\Util\Exception.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResSkin.h"
#include "..\ResSys\VertStream.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\Util\StlExt.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Cool3DOption.h"
#include "..\RenderSys\RSkin.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MMFrameBlend.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGSkinNode,SceneNode);

	SGSkinNode::SGSkinNode(void)
		: m_pRes(0), m_pAniSkin(0), m_pVB(0), m_nLod(0), m_bEnable(true), m_fTransparent(1.0f)
	{
	}

	SGSkinNode::~SGSkinNode(void)
	{
		m_vecBoneMatrixArray.clear();
		ResMgr::Inst()->ReleaseRenderable(m_pRes);
		m_pRes=NULL;
		FreePtrVector<IMaterial> freeMtls;
		freeMtls(m_mtls);
		SAFE_DELETE(m_pAniSkin);
		SAFE_DELETE(m_pVB);
	}

	void SGSkinNode::Serialize(FArchive& ar)
	{
		ar<<m_pRes->GetName();
		SerialObjPtrVector(ar,m_mtls);
		SceneNode::Serialize(ar);
	}
	
	void SGSkinNode::Deserialize(FArchive& ar)
	{
		tstring szResName;
		ar>>szResName;
		LoadRes(szResName.c_str());
		ASSERT(m_mtls.size()==0);
		DeserialObjPtrVector(ar,m_mtls);
		SceneNode::Deserialize(ar);
	}

	bool SGSkinNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;//由父节点做视锥测试,自己不做
	}
	
	UINT SGSkinNode::GetNumRenderNode() const
	{
		if(m_bEnable && m_pRes->IsCreated())
			return m_pRes->GetNumRenderNode();
		return 0;
	}
	
	RenderNode *SGSkinNode::GetRenderNode(UINT i) const
	{
		if(m_pRes->IsCreated())
			return m_pRes->GetRenderNode(i);
		return NULL;
	}

	const Matrix4& SGSkinNode::GetWorldMat() const
	{
		ASSERT(SceneNode::GetParent());
		return GetParent()->GetWorldMat();
	}

	void SGSkinNode::SetWorldMat(const Matrix4 *pMat)
	{}

	void SGSkinNode::GetBox(AABBox& out) const
	{
		GetParent()->GetBox(out);//temp
	}


	void SGSkinNode::LoadRes(const TCHAR* szName)
	{
		m_pRes=(ResSkin*)(ResMgr::Inst()->NewRenderable(szName));
	}

	void SGSkinNode::Update(float deltaTime,SceneGraph *pSG)
	{
		AABBox box;
		GetBox(box);
		m_nLod=pSG->CalcDetailLevel(box,NULL);

		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_HWSkin)==Cool3DOption::EHWS_None)
		{
			UpdateVB(m_nLod);
		}
		else
		{
			CalcBoneMatrix(m_nLod);
		}

		for(size_t i=0;i<m_mtls.size();i++)
		{
			IMaterial* pMtl=m_mtls[i];
			pMtl->UpdateModifiers(deltaTime);
		}
	}

	void SGSkinNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//--检测Mtl创建的情况
		if(m_pRes!=NULL
			&& m_pRes->IsCreated())
		{
			if(m_pRes->GetNumMtl()>0
				&& m_mtls.size()==0)
			{
				m_pRes->CreateMtlArray(m_mtls,NULL);
				if(m_fTransparent<1.0f)
					SetTransparent(m_fTransparent);
			}
		}
	}

	UINT SGSkinNode::GetNumMtl() const
	{
		return m_mtls.size();
	}

	IMaterial*	SGSkinNode::GetMtl(int mtlID)
	{
		if(mtlID>=0 && mtlID<(int)m_mtls.size())
			return m_mtls[mtlID];
		return NULL;
	}

	EVertType SGSkinNode::GetVertType() const
	{
		if(m_pAniSkin==NULL)
			return EVType_PNT;
		return m_pAniSkin->GetVertStream()->GetType();
	}

	void SGSkinNode::UpdateVB(int lod)
	{
		//--创建
		if(m_pAniSkin==NULL)
		{
			if(m_pRes!=NULL && m_pRes->IsCreated())
			{
				m_pAniSkin=new AniSkin_SW;
				m_pAniSkin->Create(m_pRes);

				const VertStream *pVerts=m_pAniSkin->GetVertStream();
				m_pVB=Device()->NewVB();
				m_pVB->Create(pVerts->GetBufferSize(),EB_DynamicWriteOnly,pVerts->GetStride());
			}
		}
		
		//--
		if(m_pAniSkin==NULL || m_pVB==NULL)
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
		m_pAniSkin->ComputeSkinVB(pAniCtrl,lod);
		
		//--update vb
        m_pVB->CopyFromVertStream(m_pAniSkin->GetVertStream());

		//--no child,所以不用调用SceneNode::Update();
	}

	void SGSkinNode::CalcBoneMatrix(int lod)
	{
		if(m_pRes==NULL||!m_pRes->IsCreated())
			return;

		if( m_vecBoneMatrixArray.empty() && m_pRes->GetNumRenderNode() > 0 )
		{
			m_vecBoneMatrixArray.resize( m_pRes->GetNumRenderNode() );
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

		for( UINT nNode = 0; nNode < m_pRes->GetNumRenderNode(); nNode++ )
		{
			RenderNode* pNode = m_pRes->GetRenderNode( nNode );
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

	void SGSkinNode::ReplaceMtl(int mtlID,IMaterial *pMtl)
	{
		if(m_mtls.size()==0)
			return;
		if(mtlID>=0 && mtlID<(int)m_mtls.size())
		{
			IMaterial *pOld=m_mtls[mtlID];
			if(pOld!=pMtl)//判断是否是同一对象
			{
				SAFE_DELETE(pOld);
				m_mtls[mtlID]=pMtl;
			}
		}
	}

	TResult SGSkinNode::RayCollide(const Ray& ray)
	{
		if( !IsCreated() || !m_pRes || !m_pRes->IsCreated() || !m_pAniSkin )
			return TResult(false,0);

		return m_pRes->RayCollide(ray,&GetWorldMat(),m_pAniSkin->GetVertStream());
	}

	const tagBoneMatrixArray* const SGSkinNode::GetBoneMatrixArray( const RSkin* const pSkin )
	{
		ASSERT( NULL != m_pRes );
		ASSERT( m_pRes->IsCreated() );

		for( int i = 0; i < (int)m_vecBoneMatrixArray.size(); i++ )
		{
			if( pSkin == m_pRes->GetRenderNode( i ) )
				return &m_vecBoneMatrixArray[i];
		}
		return NULL;
	}

	void SGSkinNode::SetTransparent( float alpha )
	{
		for(size_t i=0;i<m_mtls.size();i++)
		{
			IMaterial* pMtl=m_mtls[i];
			if(!pMtl->IS_KIND_OF(MGeneral))
				return;

			MGeneral* pMG=(MGeneral*)pMtl;
			pMG->SetTransparent(alpha,m_pRes->GetMtlDesc(i));
		}
		m_fTransparent=alpha;
	}

	const tagMtlDesc& SGSkinNode::GetMtlDesc( const int i ) const
	{
		ASSERT( NULL != m_pRes );
		return m_pRes->GetMtlDesc( i );
	}

	void SGSkinNode::SetMulColor(const float r, const float g, const float b)
	{
		for(size_t i=0;i<m_mtls.size();i++)
		{
			IMaterial* pMtl=m_mtls[i];
			if(!pMtl->IS_KIND_OF(MGeneral))
				continue;

			MGeneral* pMG=(MGeneral*)pMtl;
			pMG->SetMulColor(r,g,b);
		}
	}
}//namespace Cool3D