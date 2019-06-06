#include "stdafx.h"
#include ".\GrassPatch.h"
#include "..\SceneGraph\SceneGraph.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\Math\Frustum.h"
#include "..\EditorSupport\TerrainEditor.h"
#include "..\Util\StlExt.h"
#include "..\RenderMtl\MGeneral.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	const DWORD FORCE_UPDATE_TIMER = 1000;
	IMP_RTTI_DYNC(GrassPatch,SceneNode);

	GrassPatch::GrassPatch(void)
	{
		m_pRes=NULL;
		m_lastForceUpdateTime=Kernel::Inst()->GetAccumTimeDW();
	}

	GrassPatch::~GrassPatch(void)
	{
		Destroy();
	}

	void GrassPatch::Serialize(FArchive& ar)
	{
		tstring szResName=m_pRes->GetName();
		ar<<szResName;

		SceneNode::Serialize(ar);
	}

	void GrassPatch::Deserialize(FArchive& ar)
	{
		tstring szResName;
		ar>>szResName;
		LoadRes(szResName.c_str());

		SceneNode::Deserialize(ar);
	}

	void GrassPatch::Destroy()
	{
		RES_MGR_RELEASE_RENDERABLE(m_pRes);

		FreePtrVector<IMaterial> freeMtls;
		freeMtls(m_mtls);
	}

	bool GrassPatch::IsInFrustum(const Frustum *pFrumstum) const
	{
		if(Cool3DOption::Inst()->GetOption(Cool3DOption::EO_DrawGrass)==Cool3DOption::EDG_None)//¹Ø±Õ»¨²ÝÏÔÊ¾?
			return false;

		if(m_pRes==NULL || !m_pRes->IsCreated())
			return false;
		return pFrumstum->BoxInFrustum(*m_pRes->GetAABBox());
	}

	void GrassPatch::LoadRes(const TCHAR* szName)
	{
		Destroy();
		m_pRes=(ResGrass*)ResMgr::Inst()->NewRenderable(szName);
	}

	UINT GrassPatch::GetNumRenderNode() const
	{
		if (m_pRes==NULL || !m_pRes->IsCreated())
			return 0;

		return m_pRes->GetNumRenderNode();
	}

	RenderNode* GrassPatch::GetRenderNode(UINT i) const
	{
		if (m_pRes==NULL || !m_pRes->IsCreated())
			return NULL;

		return m_pRes->GetRenderNode(i);
	}

	const Matrix4& GrassPatch::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void GrassPatch::GetBox(AABBox& out) const
	{
		if(m_pRes!=NULL && m_pRes->IsCreated())
			out=*(m_pRes->GetAABBox());
	}

	IMaterial* GrassPatch::GetMtl(int mtlID)
	{
		if(mtlID>=0&&mtlID<(int)m_mtls.size())
			return m_mtls[mtlID];
		else
			return NULL;
	}

	const TCHAR* GrassPatch::GetResName() const
	{
		return m_pRes->GetName();
	}

	void GrassPatch::Update(float deltaTime,SceneGraph *pSG)
	{
		SceneNode::Update(deltaTime,pSG);
	}

	void GrassPatch::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		if(pSG->IsSyncInit()
			||Kernel::Inst()->GetAccumTimeDW()-m_lastForceUpdateTime>FORCE_UPDATE_TIMER)
		{
			m_lastForceUpdateTime=Kernel::Inst()->GetAccumTimeDW();

			if (m_pRes!=NULL
				&&m_pRes->IsCreated())
			{
				if (m_pRes->GetNumMtl()>0 && m_mtls.size()==0)
					m_pRes->CreateMtlArray(m_mtls,NULL);
			}
		}

		SceneNode::ForceUpdate(deltaTime,pSG);
	}

	void GrassPatch::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("cann't set world transform to a grass patch"));
	}
}//namespace Cool3D