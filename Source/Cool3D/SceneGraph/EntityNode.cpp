#include "StdAfx.h"
#include ".\entitynode.h"
#include "..\ResSys\ResStaticMesh.h"
#include "..\ResSys\ResKeyFrame.h"
#include "..\ResSys\ResSceneMesh.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\ResSys\ResSkin.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\UserResMgr.h"
#include "..\ResSys\VertStream.h"
#include "..\Util\Exception.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\Math\Transform.h"
#include "..\Math\Frustum.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\Util\StlExt.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MMColor.h"
#include "..\RenderMtl\MMTransparent.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\Kernel.h"
#include "..\Cool3DOption.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\SceneGraph\SGSkinNode.h"
#include "..\RenderSys\DynamicMesh.h"
#include <locale>

namespace Cool3D
{
	const DWORD FORCE_UPDATE_TIMER = 1000;

	IMP_RTTI_DYNC(EntityNode,SceneNode);

	class EntityNode::Member
	{
	public:
		AnimationCtrl	*m_pAniCtrl;
		ResRenderable	*m_pRenderRes;
		bool			m_bUserRes;
		Matrix4			m_toWorld;
		Matrix4			m_InvtoWorld;
		AABBox			m_worldBox;
		AABBox			m_localBox;
		vector<IMaterial*>	m_mtls;
		DWORD			m_forceUpdateTimer;
		tstring			m_szResName;
		bool			m_bStaticNode;//是否静态SceneNode
		float			m_mtlDiffuseFactor;//漫反射材质增强系数
		bool			m_bTransparent;

		Member()
			: m_pAniCtrl(0)
			, m_pRenderRes(0)
			, m_bUserRes(false)
			, m_bStaticNode(false)
			, m_mtlDiffuseFactor(1.0f)
		{
			D3DXMatrixIdentity(&m_toWorld);
			D3DXMatrixIdentity(&m_InvtoWorld);
			m_forceUpdateTimer=Kernel::Inst()->GetAccumTimeDW();
			m_localBox = MathConst::SmallBox;
		}
		~Member()
		{
			SAFE_DELETE(m_pAniCtrl);
			if(m_pRenderRes!=NULL)
			{
				if(m_bUserRes)
				{
					UserResMgr *pMgr=ResMgr::Inst()->GetUserResMgr();
					pMgr->DelRes(m_pRenderRes);
				}
				else
					ResMgr::Inst()->ReleaseRenderable(m_pRenderRes);
				m_pRenderRes=NULL;			
			}
			FreePtrVector<IMaterial> freeMtls;
			freeMtls(m_mtls);
		}

		void ReleaseRes()
		{
			SAFE_DELETE(m_pAniCtrl);
			if(m_bUserRes)
			{
				UserResMgr *pMgr=ResMgr::Inst()->GetUserResMgr();
				pMgr->DelRes(m_pRenderRes);
			}
			else
				ResMgr::Inst()->ReleaseRenderable(m_pRenderRes);
			m_pRenderRes=NULL;

			FreePtrVector<IMaterial> freeMtls;
			freeMtls(m_mtls);
		}
	};
	//--
	EntityNode::EntityNode(void)
		: m_bHide(false), m_bMipMap(true), m_fTransparent(1.0f)
	{
		m_p=new Member;
	}

	EntityNode::~EntityNode(void)
	{
		delete m_p;
	}

	void EntityNode::Serialize(FArchive& ar)
	{
		ar<<m_p->m_pRenderRes->GetName();
		ar<<m_p->m_toWorld;
		ar<<m_p->m_worldBox.max;//必须保存box,否则当使用异步IO时,无法计算出正确的box,则可能一直没机会被update
		ar<<m_p->m_worldBox.min;
		ar<<m_p->m_mtlDiffuseFactor;
		//SerialObjPtrVector(ar,m_p->m_mtls);
		SceneNode::Serialize(ar);
	}
	
	void EntityNode::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_szResName;
		//LoadRes(szResName.c_str());
		Matrix4 mat;
		ar>>mat;
		SetWorldMat(&mat);
		ar>>m_p->m_worldBox.max;
		ar>>m_p->m_worldBox.min;
		ar>>m_p->m_mtlDiffuseFactor;
		ASSERT(m_p->m_mtls.size()==0);
		//DeserialObjPtrVector(ar,m_p->m_mtls);
		SceneNode::Deserialize(ar);

		LoadRes(m_p->m_szResName.c_str());
		m_p->m_bStaticNode=true;//从文件中读取的为静态SceneNode

		D3DXMatrixInverse( &m_p->m_InvtoWorld, NULL, &m_p->m_toWorld );
	}

	bool EntityNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		if(m_p->m_pRenderRes==NULL
			||!m_p->m_pRenderRes->IsCreated())
			return false;

		if(m_bHide)
			return false;

		return pFrumstum->BoxInFrustum(m_p->m_worldBox);
	}

	void EntityNode::LoadRes(const TCHAR* szName)
	{
		m_p->m_szResName=szName;

		m_p->m_pRenderRes=ResMgr::Inst()->NewRenderable(szName);
		
		if(m_p->m_pRenderRes==NULL)
		{
			UserResMgr *pMgr=ResMgr::Inst()->GetUserResMgr();
			THROW_NULLEX(pMgr,_T("Unknown res type"),szName);
			m_p->m_pRenderRes=pMgr->NewRes(szName);
			m_p->m_bUserRes=true;
		}
		else
		{
			m_p->m_bUserRes=false;
			if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
			{
				m_p->m_pAniCtrl=new NodeAnimationCtrl;
				m_p->m_pAniCtrl->BindAnimationRes(m_p->m_pRenderRes);
			}
			if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResKeyFrame))
			{
				ResKeyFrame *pResKF=(ResKeyFrame *)m_p->m_pRenderRes;
				pResKF->EnableMipMap(m_bMipMap);
				m_p->m_pAniCtrl=new VertexAnimationCtrl;
				m_p->m_pAniCtrl->BindAnimationRes(m_p->m_pRenderRes);
				m_p->m_worldBox=*(m_p->m_pRenderRes->GetAABBox());
			}
			else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResStaticMesh))
			{
				ResStaticMesh *pSMesh=(ResStaticMesh *)m_p->m_pRenderRes;
				pSMesh->EnableMipMap(m_bMipMap);
				m_p->m_worldBox=*(m_p->m_pRenderRes->GetAABBox());
			}
		}
	}

	TResult EntityNode::RayCollide(const Ray& ray)
	{
		if(m_p->m_pRenderRes==NULL
			|| !m_p->m_pRenderRes->IsCreated())
			return TResult(false,0);

		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			ResSceneMesh *pScnMesh=(ResSceneMesh*)m_p->m_pRenderRes;
			return pScnMesh->RayCollide(ray,&m_p->m_toWorld);
		}

		return TResult(false,0);
	}

	TResult EntityNode::RayCollideForDynaNode(const Ray& ray)
	{
		if(m_p->m_pRenderRes==NULL
			|| !m_p->m_pRenderRes->IsCreated())
			return TResult(false,0);

		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
		{
			TResult ret(false,FLOAT_MAX);
			int num=GetNumChildren();
			for(int i=0;i<num;i++)
			{
				const SceneNode *pChild=GetChild(i);
				if(pChild->IS_STATIC_CLASS(SGSkinNode))
				{
					SGSkinNode *pSkin=(SGSkinNode*)pChild;
					TResult tmpRet=pSkin->RayCollide(ray);
					if(tmpRet.first
						&& tmpRet.second<ret.second)
					{
						ret.first=true;
						ret.second=tmpRet.second;
					}
				}
			}
			return ret;
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResKeyFrame))
		{
			ResKeyFrame* pKeyFrame = (ResKeyFrame*)m_p->m_pRenderRes;
			VertexAnimationCtrl* pAniCtrl = (VertexAnimationCtrl*)m_p->m_pAniCtrl;
			if( pAniCtrl )
			{
				VertStream* pVS = pAniCtrl->GetFrameVertStream();
				if( pVS )
				{
					return pKeyFrame->RayCollide(ray,pVS,&m_p->m_toWorld);
				}
			}
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResStaticMesh))
		{
			ResStaticMesh *pStaticMesh=(ResStaticMesh*)m_p->m_pRenderRes;
			return pStaticMesh->RayCollide(ray,&m_p->m_toWorld);
		}

		return TResult(false,0);
	}

	ResRenderable* EntityNode::GetResObj() const
	{
		return m_p->m_pRenderRes;
	}

	AnimationCtrl* EntityNode::GetAniCtrl() const
	{
		return m_p->m_pAniCtrl;
	}

	UINT EntityNode::GetNumRenderNode() const
	{
		if(m_p->m_pRenderRes!=NULL)
			return m_p->m_pRenderRes->GetNumRenderNode();
		else
			return 0;
	}

	RenderNode *EntityNode::GetRenderNode(UINT i) const
	{
		if(m_p->m_pRenderRes==NULL)
			return NULL;
		return m_p->m_pRenderRes->GetRenderNode(i);
	}

	const Matrix4& EntityNode::GetWorldMat() const
	{
		return m_p->m_toWorld;
	}
	void EntityNode::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(pMat!=NULL);
		m_p->m_toWorld=*pMat;

		if(m_p->m_pRenderRes!=NULL)
		{
			if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
			{
				UpdateBoundingBox();
			}
			else
			{
				const AABBox *nodeBox=m_p->m_pRenderRes->GetAABBox();
				m_p->m_worldBox=*nodeBox;
				m_p->m_worldBox.Transform(m_p->m_toWorld);
			}
		}
	}

	void EntityNode::GetBox(AABBox& out) const
	{
		out=m_p->m_worldBox;
	}

	void EntityNode::Update(float deltaTime,SceneGraph *pSG)
	{
		if(m_p->m_pAniCtrl!=NULL)
			m_p->m_pAniCtrl->Update();

		//--update mtl modifier
		for(size_t i=0;i<m_p->m_mtls.size();i++)
		{
			IMaterial* pMtl=m_p->m_mtls[i];
			pMtl->UpdateModifiers(deltaTime);
		}

		SceneNode::Update(deltaTime,pSG);
	}

	void EntityNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		if(m_p->m_pAniCtrl!=NULL)
			m_p->m_pAniCtrl->ApplyTime(deltaTime);

		if(m_p->m_bStaticNode)
		{
			if(pSG->IsSyncInit()
				||Kernel::Inst()->GetAccumTimeDW()-m_p->m_forceUpdateTimer>=FORCE_UPDATE_TIMER)
			{
				m_p->m_forceUpdateTimer=Kernel::Inst()->GetAccumTimeDW();

				//--检测Mtl创建的情况
				if(m_p->m_pRenderRes!=NULL
					&& m_p->m_pRenderRes->IsCreated())
				{
					if(m_p->m_pRenderRes->GetNumMtl()>0
						&& m_p->m_mtls.size()==0)
					{
						if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_LightMap)==Cool3DOption::ELM_None)
							m_p->m_pRenderRes->CreateMtlArray(m_p->m_mtls,NULL,m_p->m_mtlDiffuseFactor);
						else
							m_p->m_pRenderRes->CreateMtlArray(m_p->m_mtls,GetLightmap(),m_p->m_mtlDiffuseFactor);

						//更新box
						SetWorldMat(&m_p->m_toWorld);
						if( NULL != m_p->m_pRenderRes->GetAABBox() )
						{
							m_p->m_localBox = *m_p->m_pRenderRes->GetAABBox();
						}
						SetTransparent(m_fTransparent);
					}
				}
			}//end if forceupdate time is up
		}
		else
		{
			//--检测Mtl创建的情况
			if(m_p->m_pRenderRes!=NULL
				&& m_p->m_pRenderRes->IsCreated())
			{
				if(m_p->m_pRenderRes->GetNumMtl()>0
					&& m_p->m_mtls.size()==0)
				{
					m_p->m_pRenderRes->CreateMtlArray(m_p->m_mtls,NULL,m_p->m_mtlDiffuseFactor);

					//更新box
					SetWorldMat(&m_p->m_toWorld);
					if( NULL != m_p->m_pRenderRes->GetAABBox() )
					{
						m_p->m_localBox = *m_p->m_pRenderRes->GetAABBox();
					}
					SetTransparent(m_fTransparent);
				}
			}
		}

		//--update children
		SceneNode::ForceUpdate(deltaTime,pSG);
	}

	IMaterial*	EntityNode::GetMtl(int mtlID)
	{
		if(m_p->m_mtls.size()==0)
			return NULL;
		if(mtlID>=0 && mtlID<(int)m_p->m_mtls.size())
		{
			return m_p->m_mtls[mtlID];
		}
		return NULL;
	}

	void EntityNode::ReplaceMtl(int mtlID,IMaterial *pMtl)
	{
		if(m_p->m_mtls.size()==0)
		{
			//这时可能Mtl还没有从Res创建
			//注意:这样会导致Res的Mtl不会被Load
			ASSERT(mtlID==0);
			m_p->m_mtls.push_back(pMtl);
			return;
		}
		if(mtlID>=0 && mtlID<(int)m_p->m_mtls.size())
		{
			IMaterial *pOld=m_p->m_mtls[mtlID];
			if(pOld!=pMtl)//判断是否是同一对象
			{
				SAFE_DELETE(pOld);
				m_p->m_mtls[mtlID]=pMtl;
			}
		}
		SetTransparent(m_fTransparent);
	}

	UINT EntityNode::GetNumMtl() const
	{
		return m_p->m_mtls.size();
	}

	const AABBox& EntityNode::GetAABBox() const
	{
		return m_p->m_worldBox;
	}

	void EntityNode::SetMtlDiffuseFactor( float factor )
	{
		float oldFactor=m_p->m_mtlDiffuseFactor;
		m_p->m_mtlDiffuseFactor=factor;

		for( UINT i=0;i<m_p->m_mtls.size();i++ )
		{
			IMaterial *pMtl=m_p->m_mtls[i];
			if( pMtl->IS_KIND_OF(MGeneral) )
			{
				MGeneral *pMG=(MGeneral*)pMtl;
				pMG->m_pColor->m_color.diffuse.R*=(m_p->m_mtlDiffuseFactor/oldFactor);
				pMG->m_pColor->m_color.diffuse.G*=(m_p->m_mtlDiffuseFactor/oldFactor);
				pMG->m_pColor->m_color.diffuse.B*=(m_p->m_mtlDiffuseFactor/oldFactor);
				pMtl->UpdateRenderCache();
			}
		}
	}

	void EntityNode::GetVertexBuffer(VertStream* vb)
	{
		//--
		VertStream* pVS=NULL;
		ASSERT(m_p->m_pRenderRes->IsCreated());
		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			ResSceneMesh* pMesh=(ResSceneMesh*)m_p->m_pRenderRes;
			pVS=pMesh->GetVertStream();
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResStaticMesh))
		{
			ResStaticMesh* pMesh=(ResStaticMesh*)m_p->m_pRenderRes;
			pVS=pMesh->GetVertStream();
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResKeyFrame))
		{
			ResKeyFrame* pKeyFrame=(ResKeyFrame*)m_p->m_pRenderRes;
			const VertexKeyFrame* pFrame=pKeyFrame->GetFrame(0);
			ASSERT(pFrame!=NULL);
			pVS=pFrame->m_pStream;
		}
		else
			ASSERT(0);

		//--
		vb->Create(EVType_PNDT,pVS->GetNumVert());

		Matrix4 matInv,matInvTrans;
		D3DXMatrixInverse(&matInv,NULL,&GetWorldMat());
		D3DXMatrixTranspose(&matInvTrans,&matInv);

		THROW_FALSE(pVS->GetType()==EVType_PNT2,_T("pVS->GetType()==EVType_PNT2"));
		Vert_PNDT* pDst=(Vert_PNDT*)vb->GetRawBuffer();
		Vert_PNT2* pSrc=(Vert_PNT2*)pVS->GetRawBuffer();
		for(UINT i=0;i<pVS->GetNumVert();i++)
		{
			D3DXVec3TransformCoord(&pDst->pos,&pSrc->pos,&GetWorldMat());
			D3DXVec3TransformNormal(&pDst->normal,&pSrc->normal,&matInvTrans);
			pDst->uv=pSrc->uv2;

			pDst++;
			pSrc++;
		}
	}

	void EntityNode::GetPosBuffer( VertStream* vb )
	{
		VertStream* pVS=NULL;
		ASSERT(m_p->m_pRenderRes->IsCreated());
		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			ResSceneMesh* pMesh=(ResSceneMesh*)m_p->m_pRenderRes;
			pVS=pMesh->GetVertStream();
		}
		else
			ASSERT(0);

		//--
		vb->Create(EVType_P,pVS->GetNumVert());

		THROW_FALSE(pVS->GetType()==EVType_PNT2,_T("pVS->GetType()==EVType_PNT2"));
		Vert_P*	pDst=(Vert_P*)vb->GetRawBuffer();
		Vert_PNT2* pSrc=(Vert_PNT2*)pVS->GetRawBuffer();
		for(UINT i=0;i<pVS->GetNumVert();i++)
		{
			D3DXVec3TransformCoord(&pDst->pos,&pSrc->pos,&GetWorldMat());

			pDst++;
			pSrc++;
		}
	}

	void EntityNode::GetIndexBuffer(vector<WORD>& ib)
	{
		ASSERT(m_p->m_pRenderRes->IsCreated());
		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			ResSceneMesh* pMesh=(ResSceneMesh*)m_p->m_pRenderRes;
			pMesh->GetIndexBuffer(ib);
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResStaticMesh))
		{
			ResStaticMesh* pMesh=(ResStaticMesh*)m_p->m_pRenderRes;
			pMesh->GetIndexBuffer(ib);
		}
		else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResKeyFrame))
		{
			ResKeyFrame* pKeyFrame=(ResKeyFrame*)m_p->m_pRenderRes;
			pKeyFrame->GetIndexBuffer(ib);
		}
		else
			ASSERT(0);
	}

	void EntityNode::BuildDecalMesh( const AABBox& box, DynamicMesh *pMesh, const Vector3* pNormal )
	{
		if( NULL == m_p->m_pRenderRes )
			return;

		if( !m_p->m_pRenderRes->IsCreated() )
			return;

		if(!m_p->m_worldBox.Intersects( box ) )
			return;

		if( m_p->m_pRenderRes->IS_STATIC_CLASS(ResSceneMesh) )
		{
			ResSceneMesh* pSceneMesh = (ResSceneMesh*)m_p->m_pRenderRes;

			//BeginSpeedTest(_T("EntityNode::BuildDecalMesh."));

			AABBox boxLocal = box;
			boxLocal.Transform( m_p->m_InvtoWorld );
			if( NULL != pNormal )
			{
				Vector3 vNormalLocal( 0.0f, 1.0f, 0.0f );
				D3DXVec3TransformNormal( &vNormalLocal, pNormal, &m_p->m_InvtoWorld );
				pSceneMesh->BuildDecalMesh( boxLocal, pMesh, m_p->m_toWorld, &vNormalLocal );
			}
			else
			{
				pSceneMesh->BuildDecalMesh( boxLocal, pMesh, m_p->m_toWorld, NULL );
			}

			//EndSpeedTest(_T("EntityNode::BuildDecalMesh."));
		}
	}

	void EntityNode::SetTransparent( float alpha )
	{
		for(size_t i=0;i<m_p->m_mtls.size();i++)
		{
			IMaterial* pMtl=m_p->m_mtls[i];
			if(!pMtl->IS_KIND_OF(MGeneral))
				return;

			MGeneral* pMG=(MGeneral*)pMtl;
			if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResKeyFrame))
			{
				ResKeyFrame *pResKF=(ResKeyFrame *)m_p->m_pRenderRes;
				pMG->SetTransparent(alpha,pResKF->GetMtlDesc(i));
			}
			else if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResStaticMesh))
			{
				ResStaticMesh *pSMesh=(ResStaticMesh *)m_p->m_pRenderRes;
				pMG->SetTransparent(alpha,pSMesh->GetMtlDesc(i));
			}
		}

		//--子Node
		if(m_p->m_pRenderRes!=NULL
			&& m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
		{
			int num=GetNumChildren();
			for(int i=0;i<num;i++)
			{
				SceneNode *pChild=GetChildVar(i);
				if(pChild->IS_STATIC_CLASS(SGSkinNode))
				{
					SGSkinNode *pSkin=(SGSkinNode*)pChild;
					pSkin->SetTransparent(alpha);
				}
			}
		}
		m_fTransparent=alpha;
	}


	void EntityNode::SetMulColor(const float r, const float g, const float b)
	{
		for(size_t i=0;i<m_p->m_mtls.size();i++)
		{
			IMaterial* pMtl=m_p->m_mtls[i];
			if(!pMtl->IS_KIND_OF(MGeneral))
				continue;

			MGeneral* pMG=(MGeneral*)pMtl;
			pMG->SetMulColor(r,g,b);
		}

		//--子Node
		if(m_p->m_pRenderRes!=NULL
			&& m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
		{
			int num=GetNumChildren();
			for(int i=0;i<num;i++)
			{
				SceneNode *pChild=GetChildVar(i);
				if(pChild->IS_STATIC_CLASS(SGSkinNode))
				{
					SGSkinNode *pSkin=(SGSkinNode*)pChild;
					pSkin->SetMulColor(r,g,b);
				}
			}
		}
	}

	bool EntityNode::IsLoaded()
	{
		if(m_p->m_pRenderRes==NULL
			|| !m_p->m_pRenderRes->IsCreated())
			return false;

		if(m_p->m_pRenderRes->GetNumMtl()>0&&m_p->m_mtls.empty())
			return false;

		for(size_t i=0;i<m_p->m_mtls.size();i++)
		{
			if(!m_p->m_mtls[i]->IsResourceReady())
				return false;
		}

		return true;
	}

	void EntityNode::UpdateBoundingBox()
	{
		if(m_p->m_pRenderRes->IS_STATIC_CLASS(ResSkeleton))
		{
			m_p->m_worldBox.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
			m_p->m_worldBox.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

			int num=GetNumChildren();
			//--合并所有Skin的object空间box,然后转换到world空间
			for(int i=0;i<num;i++)
			{
				const SceneNode *pChild=GetChild(i);
				if(pChild->IS_STATIC_CLASS(SGSkinNode))
				{
					const ResSkin *pRes=((const SGSkinNode*)pChild)->GetResSkin();
					m_p->m_worldBox.Merge(*(pRes->GetAABBox()));
				}
			}
			m_p->m_localBox = m_p->m_worldBox;
			m_p->m_worldBox.Transform(m_p->m_toWorld);
		}
	}

	const AABBox& EntityNode::GetLocalBox() const
	{
		return m_p->m_localBox;
	}
}//namespace Cool3D