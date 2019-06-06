#include "StdAfx.h"
#include ".\sgskybox.h"
#include "..\RenderSys\RSkyBox.h"
#include "..\RenderMtl\MSky.h"
#include "SceneGraph.h"
#include "..\Math\Transform.h"

namespace Cool3D
{
	//将skybox分为三部分：顶部、前右、后左，底部不做处理
	IMP_RTTI_DYNC(SGSkyBox,SGSkyNode);

	class SGSkyBox::Member
	{
	public:
		Member(void)
		{
			m_toWorld=MathConst::IdentityMat;
			m_offset=Vector3(0.0f,0.0f,0.0f);
			m_yaw=0.0f;
			m_roll=0.0f;
			m_pitch=0.0f;
		}
	public:
		RSkyBox		m_renderNode;
		MSky		m_mtl[3];
		float		m_yaw;
		float		m_roll;
		float		m_pitch;
		Matrix4		m_toWorld;
		Vector3		m_offset;
	};

	SGSkyBox::SGSkyBox(void)
	{
		m_p=new Member;
	}

	SGSkyBox::~SGSkyBox(void)
	{
		delete m_p;
	}

	void SGSkyBox::Serialize(FArchive& ar)
	{
	}

	void SGSkyBox::Deserialize(FArchive& ar)
	{
	}

	UINT SGSkyBox::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode *SGSkyBox::GetRenderNode(UINT i) const
	{
		ASSERT(0==i);
		return &m_p->m_renderNode;
	}

	IMaterial*	SGSkyBox::GetMtl(int mtlID)
	{
		if(mtlID>=0 && mtlID<3)
			return &m_p->m_mtl[mtlID];
		else
			return NULL;
	}

	void SGSkyBox::Update(float deltaTime,SceneGraph *pSG)
	{
		//更新skybox的世界坐标
		Transform trans;
		trans.Translate(pSG->GetCamera()->GetPos()+m_p->m_offset);
		trans.Rotate(m_p->m_yaw,m_p->m_pitch,m_p->m_roll);
		m_p->m_toWorld=*trans.GetMatrix();

		// m_p->m_yaw += 0.000005f;

		SceneNode::Update(deltaTime,pSG);
	}

	void SGSkyBox::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{

	}

	const Matrix4& SGSkyBox::GetWorldMat() const
	{
		return m_p->m_toWorld;
	}

	void SGSkyBox::SetOffset(const Vector3& offset)
	{
		m_p->m_offset=offset;
	}

	void SGSkyBox::Create(const TCHAR* szTopTex,const TCHAR* szFrontRightTex,
		const TCHAR* szBackLeftTex,const Vector3& drawSize)
	{
		//0-顶	1-前右	2-后左
		//--建立材质
		m_p->m_mtl[0].SetTexture(0,szTopTex);
		m_p->m_mtl[0].m_bEnableLight=false;
		m_p->m_mtl[0].m_bTwoSide=true;
		m_p->m_mtl[0].m_addressU=ETAddress_Clamp;
		m_p->m_mtl[0].m_addressV=ETAddress_Clamp;
		
		m_p->m_mtl[1].SetTexture(0,szFrontRightTex);
		m_p->m_mtl[1].m_bEnableLight=false;
		m_p->m_mtl[1].m_bTwoSide=true;
		m_p->m_mtl[1].m_addressU=ETAddress_Clamp;
		m_p->m_mtl[1].m_addressV=ETAddress_Clamp;

		m_p->m_mtl[2].SetTexture(0,szBackLeftTex);
		m_p->m_mtl[2].m_bEnableLight=false;
		m_p->m_mtl[2].m_bTwoSide=true;
		m_p->m_mtl[2].m_addressU=ETAddress_Clamp;
		m_p->m_mtl[2].m_addressV=ETAddress_Clamp;

		m_p->m_renderNode.CreateMeshs( drawSize );
	}

	void SGSkyBox::SetRotate( float yaw,float pitch,float roll )
	{
		m_p->m_yaw=yaw;
		m_p->m_pitch=pitch;
		m_p->m_roll=roll;
	}

	void SGSkyBox::EnableRenderShade( const bool bEnable )
	{
		m_p->m_renderNode.EnableRenderShade( bEnable );
	}

	void SGSkyBox::SetShadeColor( const DWORD dwColor )
	{
		m_p->m_renderNode.SetShadeColor( dwColor );
	}
}//namespace Cool3D