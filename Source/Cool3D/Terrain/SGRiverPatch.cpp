#include "stdafx.h"
#include ".\SGRiverPatch.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\UserResMgr.h"
#include "..\Math\Frustum.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Kernel.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\Cool3DOption.h"
#include "..\RenderMtl\MWaterReflect.h"
#include "..\RenderMtl\MWaterRefract.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGRiverPatch,SceneNode);

	class SGRiverPatch::Member
	{
	public:
		ResRiver			*m_pRiverRes;		//河流公共资源
		ResRiverPatch		*m_pPatchRes;		//河流块资源
		tstring				m_szRiverResName;	//河流资源文件
		tstring				m_szPatchResName;	//河流块资源文件
		IMaterial			*m_pWaterMtl;		//水面材质
		MWaterReflect		*m_pReflectMtl;		//反射效果材质
		MWaterRefract		*m_pRefractMtl;		//折射效果材质

		DWORD				m_lastForceUpdateTime;

	public:
		Member()
		{
			m_pRiverRes=NULL;
			m_pPatchRes=NULL;
			m_pWaterMtl=NULL;
			m_pReflectMtl=NULL;
			m_pRefractMtl=NULL;

			m_lastForceUpdateTime=Kernel::Inst()->GetAccumTimeDW();
		}

		~Member()
		{
			SAFE_DELETE(m_pWaterMtl);
			SAFE_DELETE(m_pReflectMtl);
			SAFE_DELETE(m_pRefractMtl);

			RES_MGR_RELEASE_RENDERABLE(m_pRiverRes);
			RES_MGR_RELEASE_RENDERABLE(m_pPatchRes);
		}
	};

	SGRiverPatch::SGRiverPatch()
	{
		m_p=new Member;
	}

	SGRiverPatch::~SGRiverPatch()
	{
		SAFE_DELETE(m_p);
	}

	void SGRiverPatch::Serialize(FArchive& ar)
	{
		ar<<m_p->m_pRiverRes->GetName();
		ar<<m_p->m_pPatchRes->GetName();

		SceneNode::Serialize(ar);
	}

	void SGRiverPatch::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_szRiverResName;
		ar>>m_p->m_szPatchResName;

		LoadRiverRes(m_p->m_szRiverResName.c_str());
		LoadPatchRes(m_p->m_szPatchResName.c_str());

		SceneNode::Deserialize(ar);
	}

	bool SGRiverPatch::IsInFrustum(const Frustum *pFrumstum) const
	{
		if(m_p->m_pPatchRes==NULL
			||!m_p->m_pPatchRes->IsCreated())
			return false;
		return pFrumstum->BoxInFrustum(*m_p->m_pPatchRes->GetAABBox());
	}

	void SGRiverPatch::LoadRes(const TCHAR *szName)
	{
		//
	}

	void SGRiverPatch::LoadRiverRes(const TCHAR *szName)
	{
		ASSERT(m_p->m_pRiverRes==NULL);

		m_p->m_pRiverRes=(ResRiver*)ResMgr::Inst()->NewRenderable(szName);
	}

	void SGRiverPatch::LoadPatchRes(const TCHAR* szName)
	{
		ASSERT(m_p->m_pPatchRes==NULL);

		m_p->m_pPatchRes=(ResRiverPatch *)ResMgr::Inst()->NewRenderable(szName);
	}

	UINT SGRiverPatch::GetNumRenderNode() const
	{
		return m_p->m_pPatchRes->GetNumRenderNode();
	}

	RenderNode * SGRiverPatch::GetRenderNode( UINT i ) const
	{
		ASSERT(i<m_p->m_pPatchRes->GetNumRenderNode());

		return m_p->m_pPatchRes->GetRenderNode(i);
	}

	const Matrix4& SGRiverPatch::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGRiverPatch::SetWorldMat( const Matrix4 *pMat )
	{
		ASSERT(0 && _T("cann't set world transform to a river patch"));
	}

	void SGRiverPatch::GetBox( AABBox& out ) const
	{
		if(m_p->m_pPatchRes)
			out=*m_p->m_pPatchRes->GetAABBox();
	}

	IMaterial* SGRiverPatch::GetMtl( int mtlID )
	{
		if( !IsEnableEffect() )
			return m_p->m_pWaterMtl;

		int waterEffect=Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterReflect);
		switch (waterEffect)
		{
		case Cool3DOption::EWR_High:
		case Cool3DOption::EWR_Mid:
			{
				if( Cool3DOption::EWRR_Open == Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterRefract) )
					return m_p->m_pRefractMtl;
				else
					return m_p->m_pReflectMtl;
			}
			break;
		case Cool3DOption::EWR_None:
			{
				return m_p->m_pWaterMtl;
			}
			break;
		default:
			return NULL;
		}
	}

	void SGRiverPatch::Update( float deltaTime,SceneGraph *pSG )
	{
		if(m_p->m_pWaterMtl!=NULL)
		{
			m_p->m_pWaterMtl->UpdateModifiers(deltaTime);
		}

		SceneNode::Update(deltaTime,pSG);
	}

	void SGRiverPatch::ForceUpdate( float deltaTime,SceneGraph *pSG )
	{
		if( m_p->m_pPatchRes!=NULL
			&& m_p->m_pPatchRes->IsCreated()
			&& m_p->m_pRiverRes!=NULL
			&& m_p->m_pRiverRes->IsCreated())
		{
			if( m_p->m_pWaterMtl==NULL )
			{
				m_p->m_pWaterMtl=m_p->m_pPatchRes->CreateWaterMtl();
			}

			//--创建反射效果材质
			if( Cool3DOption::EWR_None!=Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterReflect)
				&& m_p->m_pRiverRes->IsEnableEffect()
				&& Cool3DOption::Inst()->IsSupport(Cool3DOption::EO_WaterReflect,Cool3DOption::EWR_High) )
			{
				if( Cool3DOption::EWRR_Open == Cool3DOption::Inst()->GetOption(Cool3DOption::EO_WaterRefract) &&
					Cool3DOption::Inst()->IsSupport(Cool3DOption::EO_WaterRefract,Cool3DOption::EWRR_Open) )
				{
					if( NULL == m_p->m_pRefractMtl )
						m_p->m_pRefractMtl=m_p->m_pPatchRes->CreateRefractMtl();
					SAFE_DELETE( m_p->m_pReflectMtl );
				}
				else
				{
					if( NULL == m_p->m_pReflectMtl )
						m_p->m_pReflectMtl=m_p->m_pPatchRes->CreateReflectMtl();
					SAFE_DELETE( m_p->m_pRefractMtl );
				}

			}
		}

		SceneNode::ForceUpdate(deltaTime,pSG);
	}

	ResRiverPatch * SGRiverPatch::GetPatchRes()
	{
		return m_p->m_pPatchRes;
	}

	ResRiver * SGRiverPatch::GetRiverRes()
	{
		return m_p->m_pRiverRes;
	}

	bool SGRiverPatch::IsEnableEffect()
	{
		if( m_p->m_pRiverRes->IsCreated() )
			return m_p->m_pRiverRes->IsEnableEffect();
		else
			return false;
	}

	float SGRiverPatch::GetSurfaceHeight()
	{
		if( m_p->m_pRiverRes->IsCreated() )
			return m_p->m_pRiverRes->GetHeight();
		else
			return 0.0f;
	}

	void SGRiverPatch::SetReflectMap( IRenderTexture* pReflectMap )
	{
		ASSERT(pReflectMap!=NULL);

		if( NULL != m_p->m_pReflectMtl && NULL == m_p->m_pRefractMtl )
		{
			m_p->m_pReflectMtl->m_pReflectMap->m_pRTex=pReflectMap;
			m_p->m_pReflectMtl->UpdateRenderCache();
		}
		else if( NULL == m_p->m_pReflectMtl && NULL != m_p->m_pRefractMtl )
		{
			m_p->m_pRefractMtl->m_pReflectMap->m_pRTex=pReflectMap;
			m_p->m_pRefractMtl->UpdateRenderCache();
		}
	}

	void SGRiverPatch::SetRefractMap( IRenderTexture* pRefractMap )
	{
		ASSERT(pRefractMap!=NULL);

		if( NULL == m_p->m_pRefractMtl )
			return;

		m_p->m_pRefractMtl->m_pRefractMap->m_pRTex=pRefractMap;
		m_p->m_pRefractMtl->UpdateRenderCache();
	}

	UINT SGRiverPatch::GetRiverID()
	{
		return m_p->m_pRiverRes->GetRiverID();
	}
}//namespace Cool3D