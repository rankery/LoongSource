#include "StdAfx.h"
#include ".\sgroleglow.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MMAlphaMask.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\RenderMtl\MMUVPage.h"
#include "..\Math\Transform.h"
#include "..\Math\CameraBase.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Device\DrawX.h"
#include "AvatarNode.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\RenderSys\MtlRenderCacheFixed.h"


namespace Cool3D
{
	IMP_RTTI_DYNC(SGRoleGlow,SceneNode);

	class SGRoleGlow::Member
	{
	public:
		EStyle				m_style;		// 光晕风格
		tagParam			m_param;		// 光晕参数
		Color4ub			m_color;		// 光晕颜色
		bool				m_blendOne;		// alpha混合操作
		DynamicMesh			*m_pMesh;		// 网格
		MBitmap				m_mtl;			// 材质
		vector<Vector3>		m_posArray;		// 网格顶点数组
		int					m_headBoneIndex;// 首骨骼索引
		int					m_tailBoneIndex;// 尾骨骼索引
		float				m_scale;		// width和heightExtend缩放

		Member()
		{
			m_pMesh=NULL;
			m_headBoneIndex=-1;
			m_tailBoneIndex=-1;
			m_color=Color4ub(255,255,0,0);
			m_scale=1.0f;
			m_blendOne=true;
		}

		~Member()
		{
			SAFE_DELETE( m_pMesh );
		}

		void CreateMesh()
		{
			ASSERT(m_pMesh==NULL);

			switch(m_style)
			{
			case ES_Round:
			case ES_Bucket:
				CreateBucketMesh();
				break;
			default:
				ASSERT(0 && _T("unknow role glow style"));
			}
		}

		void CreateBucketMesh()
		{
			m_pMesh=new DynamicMesh;
			m_pMesh->Create(0,EDraw_TriangleStrip,EVType_PTD,4);

			VertStream *pVStream=m_pMesh->GetVertStream();
			ASSERT( pVStream && pVStream->GetNumVert()==4 );
			if(pVStream)
			{
				Vert_PTD *pVB=(Vert_PTD*)pVStream->GetRawBuffer();
				pVB[0].uv=Vector2( 0.0f, 0.0f );
				pVB[1].uv=Vector2( 0.0f, 1.0f );
				pVB[2].uv=Vector2( 1.0f, 0.0f );
				pVB[3].uv=Vector2( 1.0f, 1.0f );

				pVB[0].diffuse = m_color;
				pVB[1].diffuse = m_color;
				pVB[2].diffuse = m_color;
				pVB[3].diffuse = m_color;
			}
		}

		void UpdateRoundMesh(SceneGraph *pSG,AvatarNode* pParent)
		{
			CameraBase *pCamera=pSG->GetCamera();
			Vector3 vEye=pCamera->GetPos();

			NodeAnimationCtrl* pAniCtrl=pParent->GetSkeletonAniCtrl();
			if(pAniCtrl==NULL)
				return;

			//--获得骼索引
			if(m_headBoneIndex==-1)
			{
				m_headBoneIndex=GetAvatarBoneIndex(pParent,m_param.szHeadBone.c_str());
				if(m_headBoneIndex==-1)
					return;
			}

			//--计算中点位置
			Vector3 vMid=MathConst::Zero3;
			{
				Matrix4 mat=*pAniCtrl->GetBoneSpace(m_headBoneIndex);
				mat*=pParent->GetWorldMat();

				Transform tran;
				tran.LoadMatrix(&mat);
				tran.TransVec3(vMid);
			}

			//--计算右向量,上向量
			Vector3 vRight, vTop;
			{
				Vector3 vMidToEye=vEye-vMid;
				D3DXVec3Normalize(&vMidToEye,&vMidToEye);

				D3DXVec3Cross(&vRight,&MathConst::AxisY,&vMidToEye);
				D3DXVec3Normalize(&vRight,&vRight);

				D3DXVec3Cross(&vTop,&vRight,&vMidToEye);
				D3DXVec3Normalize(&vTop,&vTop);

				vRight*=(m_param.width*m_scale);
				vTop*=(m_param.width*m_scale);
			}

			//--设置网格
			{
				/**
				0---2
				|	|
				1---3
				*/
				VertStream *pVStream=m_pMesh->GetVertStream();
				if(pVStream)
				{
					ASSERT(pVStream->GetNumVert()==4);
					Vert_PTD *pVB=(Vert_PTD*)pVStream->GetRawBuffer();

					pVB[0].pos = vMid-vTop-vRight;
					pVB[1].pos = vMid+vTop-vRight;
					pVB[2].pos = vMid-vTop+vRight;
					pVB[3].pos = vMid+vTop+vRight;

					pVB[0].diffuse = m_color;
					pVB[1].diffuse = m_color;
					pVB[2].diffuse = m_color;
					pVB[3].diffuse = m_color;
				}
			}
		}

		void UpdateBucketMesh( SceneGraph *pSG, AvatarNode* pParent )
		{
			CameraBase *pCamera=pSG->GetCamera();
			Vector3 vEye=pCamera->GetPos();

			NodeAnimationCtrl* pAniCtrl=pParent->GetSkeletonAniCtrl();
			if(pAniCtrl==NULL)
				return;

			//--获得骼索引
			if(m_headBoneIndex==-1)
			{
				m_headBoneIndex=GetAvatarBoneIndex(pParent,m_param.szHeadBone.c_str());
				if(m_headBoneIndex==-1)
					return;
			}
			if(m_tailBoneIndex==-1)
			{
				m_tailBoneIndex=GetAvatarBoneIndex(pParent,m_param.szTailBone.c_str());
				if(m_tailBoneIndex==-1)
					return;
			}

			//--计算起始点位置
			Vector3 vHead=MathConst::Zero3;
			{
				Matrix4 mat=*pAniCtrl->GetBoneSpace(m_headBoneIndex);
				mat*=pParent->GetWorldMat();

				Transform tran;
				tran.LoadMatrix(&mat);
				tran.TransVec3(vHead);
			}

			//--计算结束点位置
			Vector3 vTail=MathConst::Zero3;
			{
				Matrix4 mat=*pAniCtrl->GetBoneSpace(m_tailBoneIndex);
				mat*=pParent->GetWorldMat();

				Transform tran;
				tran.LoadMatrix(&mat);
				tran.TransVec3(vTail);
			}

			//--
			Vector3 vHead2Tail=vTail-vHead;
			D3DXVec3Normalize(&vHead2Tail,&vHead2Tail);

			//--延长起始点和结束点
			if(m_param.heightExtend>0)
			{
				vHead-=(vHead2Tail*m_param.heightExtend*m_scale);
				vTail+=(vHead2Tail*m_param.heightExtend*m_scale);
			}

			//--计算右向量
			Vector3 vRight;
			{
				Vector3 vHead2Eye=vEye-vHead;
				D3DXVec3Normalize(&vHead2Eye,&vHead2Eye);

				D3DXVec3Cross(&vRight,&vHead2Eye,&vHead2Tail);
				D3DXVec3Normalize(&vRight,&vRight);

				vRight*=(m_param.width*m_scale);
			}

			//--设置网格
			{
				/**
					0---2
					|	|
					1---3
				*/
				VertStream *pVStream=m_pMesh->GetVertStream();
				if(pVStream)
				{
					ASSERT(pVStream->GetNumVert()==4);
					Vert_PTD *pVB=(Vert_PTD*)pVStream->GetRawBuffer();

					pVB[0].pos = vHead-vRight;
					pVB[1].pos = vTail-vRight;
					pVB[2].pos = vHead+vRight;
					pVB[3].pos = vTail+vRight;

					pVB[0].diffuse = m_color;
					pVB[1].diffuse = m_color;
					pVB[2].diffuse = m_color;
					pVB[3].diffuse = m_color;
				}
			}
		}

		int GetAvatarBoneIndex(AvatarNode* pParent,const TCHAR* szBoneName)
		{
			int boneIndex=-1;

			ResSkeleton *pSkel=const_cast<ResSkeleton*>(pParent->GetResource());
			if(pSkel!=NULL && pSkel->IsCreated())
			{
				boneIndex=pSkel->GetBoneID(szBoneName);
			}

			return boneIndex;
		}

		void SetBlendOne(bool bBlendOne)
		{
			if(m_blendOne==bBlendOne)
				return;

			m_blendOne=bBlendOne;

			for(UINT i=0;i<m_mtl.GetNumMods();i++)
			{
				const IMtlModifier* pMod=m_mtl.GetModifier(i);
				if(pMod->IS_STATIC_CLASS(MMFrameBlend))
				{
					MMFrameBlend* pBlend=(MMFrameBlend*)pMod;
					pBlend->m_dst=m_blendOne ? EBlend_One : EBlend_InvSrcAlpha;
				}
			}

			m_mtl.InvalidateRenderCache();
		}

	};//end Member

	SGRoleGlow::SGRoleGlow(void)
	{
		m_p=new Member;
		m_bEnable=true;
	}

	SGRoleGlow::~SGRoleGlow(void)
	{
		delete m_p;
	}

	bool SGRoleGlow::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
	}

	void SGRoleGlow::LoadRes(const TCHAR* szName)
	{
		m_p->m_mtl.SetTexture(0,szName);
		m_p->m_mtl.m_bTwoSide=true;
		m_p->m_mtl.m_bEnableLight=false;
		m_p->m_mtl.m_addressU=m_p->m_mtl.m_addressV=ETAddress_Clamp;
		m_p->m_mtl.m_mipFilter=ETFilter_LINEAR;
		m_p->m_mtl.m_minFilter=m_p->m_mtl.m_magFilter=ETFilter_LINEAR;

		MMAlphaMask *pAlphaMask=new MMAlphaMask;
		pAlphaMask->m_alphaTestFunc=ECMP_Greater;
		pAlphaMask->m_alphaRef=0;
		m_p->m_mtl.AttachModifier(pAlphaMask);

		MMFrameBlend *pBlend=new MMFrameBlend;
		pBlend->m_blendEnable=true;
		pBlend->m_src=EBlend_SrcAlpha;
		pBlend->m_dst= m_p->m_blendOne ? EBlend_One : EBlend_InvSrcAlpha;
		m_p->m_mtl.AttachModifier(pBlend);

		if(m_p->m_param.uDim!=1 
			|| m_p->m_param.vDim!=1)
		{
			MMUVPage *pUVPage = new MMUVPage;
			pUVPage->SetParam(m_p->m_param.uDim,m_p->m_param.vDim,m_p->m_param.fps,false);
			m_p->m_mtl.AttachModifier(pUVPage);
		}	
	}

	UINT SGRoleGlow::GetNumRenderNode() const
	{
		if(!m_bEnable||!m_p->m_pMesh)
			return 0;
		return 1;
	}

	RenderNode *SGRoleGlow::GetRenderNode(UINT i) const
	{
		ASSERT(i==0);
		return m_p->m_pMesh;
	}

	const Matrix4& SGRoleGlow::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGRoleGlow::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT( 0 && _T("can not set RoleGlow world matrix") );
	}

	void SGRoleGlow::GetBox(AABBox& out) const
	{
		out=MathConst::SmallBox;
	}

	IMaterial*	SGRoleGlow::GetMtl(int mtlID)
	{
		ASSERT(mtlID==0);
		return &m_p->m_mtl;
	}

	void SGRoleGlow::Update(float deltaTime, SceneGraph *pSG)
	{
		if(!m_bEnable)
			return;

		if(GetParent()==NULL)
			return;
		if(!GetParent()->IS_KIND_OF(AvatarNode))
			return;

		AvatarNode *pParent=(AvatarNode*)GetParent();

		//--创建网格
		if(m_p->m_pMesh==NULL)
		{
			m_p->CreateMesh();
		}

		if(m_p->m_pMesh!=NULL)
		{
			//--更新网格
			switch(m_p->m_style)
			{
			case ES_Bucket:
				m_p->UpdateBucketMesh(pSG,pParent);
				break;
			case ES_Round:
				m_p->UpdateRoundMesh(pSG,pParent);
				break;
			default:
				ASSERT( 0 && _T("unknow role glow style") );
			}

			//--更新贴图动画
			m_p->m_mtl.UpdateModifiers(deltaTime);
		}

	}

	void SGRoleGlow::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//have no child,do nothing
	}

	void SGRoleGlow::Init(EStyle style,tagParam& param)												  
	{
		m_p->m_style=style;
		m_p->m_param=param;
	}

	void SGRoleGlow::SetColor(Color4ub& color)
	{
		m_p->m_color=color;
	}

	void SGRoleGlow::SetScale(float scale)
	{
		m_p->m_scale=scale;
	}

	void SGRoleGlow::SetBlendOne(bool bBlendOne)
	{
		m_p->SetBlendOne(bBlendOne);
	}

}//namespace Cool3D