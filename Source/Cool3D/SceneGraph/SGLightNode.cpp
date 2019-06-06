#include "StdAfx.h"
#include ".\sglightnode.h"
#include "..\Device\Light.h"
#include "..\Math\Frustum.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGLightNode,SceneNode);

	class SGLightNode::Member
	{
	public:
		tagLight	m_light;	//in world space
		AABBox		m_box;		//in world space
		Matrix4		m_toWorld;
		Vector3		m_localPos;//pos in object space
		Vector3		m_localDir;//direction in object space
		bool		m_bIsDynamic;
		float		m_noBlockRange;
		float		m_lightModulus;
	public:
		Member():m_localPos(0,0,0),m_localDir(0,0,1)
		{
			D3DXMatrixIdentity(&m_toWorld);
			m_bIsDynamic=true;
			m_noBlockRange=0.0f;
			m_lightModulus=1.0f;
		}
		~Member()	{}

		void CalLightBox()
		{
			switch(m_light.type)
			{
			case Light_Point:
				{
					float size=m_light.range;
					m_box.max=Vector3(size,size,size);
					m_box.min=Vector3(-size,-size,-size);
					m_box.max+=m_light.pos;
					m_box.min+=m_light.pos;
				}
				break;
			case Light_Spot:
				break;
			}
		}
	};

	SGLightNode::SGLightNode(void)
	{
		m_p=new Member;
	}

	SGLightNode::~SGLightNode(void)
	{
		delete m_p;
	}

	void SGLightNode::Serialize(FArchive& ar)
	{
		ar<<m_p->m_light.type;
		ar<<m_p->m_light.pos;
		ar<<m_p->m_light.dir;
		ar<<m_p->m_light.range;
		ar<<m_p->m_light.diffuse;
		ar<<m_p->m_light.ambient;
		ar<<m_p->m_light.specular;
		ar<<m_p->m_light.attenuation;
		ar<<m_p->m_light.spot;
		ar<<m_p->m_toWorld;
		ar<<m_p->m_bIsDynamic;
		ar<<m_p->m_noBlockRange;
		ar<<m_p->m_lightModulus;
		SceneNode::Serialize(ar);
	}

	void SGLightNode::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_light.type;
		ar>>m_p->m_light.pos;
		ar>>m_p->m_light.dir;
		ar>>m_p->m_light.range;
		ar>>m_p->m_light.diffuse;
		ar>>m_p->m_light.ambient;
		ar>>m_p->m_light.specular;
		ar>>m_p->m_light.attenuation;
		ar>>m_p->m_light.spot;
		ar>>m_p->m_toWorld;
		ar>>m_p->m_bIsDynamic;
		ar>>m_p->m_noBlockRange;
		ar>>m_p->m_lightModulus;
		m_p->CalLightBox();
		SceneNode::Deserialize(ar);
	}

	tagLight* SGLightNode::GetLight()
	{
		return &m_p->m_light;
	}

	bool SGLightNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		switch(m_p->m_light.type)
		{
		case Light_None:
			return false;
		case Light_Point:
			return pFrumstum->BoxInFrustum(m_p->m_box);
		}
		return false;
	}

	void SGLightNode::Update(float deltaTime,SceneGraph *pSG)
	{
		const SceneNode *pParent=GetParent();
		if(pParent)//随父节点移动
		{
			SetWorldMat(&pParent->GetWorldMat());
		}
	}

	void SGLightNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//do thing
	}

	void SGLightNode::LoadRes(const TCHAR* szName)
	{
		ASSERT(0 && _T("Light can't bind resource"));
	}

	UINT SGLightNode::GetNumRenderNode() const
	{
		return 0;//light没有渲染节点 
	}

	RenderNode *SGLightNode::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const Matrix4& SGLightNode::GetWorldMat() const
	{
		return m_p->m_toWorld;
	}

	void SGLightNode::SetWorldMat(const Matrix4 *pMat)
	{
		m_p->m_toWorld=(*pMat);

		m_p->m_light.pos=m_p->m_localPos*m_p->m_toWorld;
		m_p->m_light.dir=m_p->m_localDir*m_p->m_toWorld;
		
		m_p->CalLightBox();
	}

	void SGLightNode::GetBox(AABBox& out) const
	{
		out=m_p->m_box;
	}

	IMaterial*	SGLightNode::GetMtl(int mtlID)
	{
		return NULL;//light have no mtl
	}

	float SGLightNode::LightCost(const AABBox& box)
	{
		if(m_p->m_light.type==Light_Direct)//方向光不用做距离判断
		{
			return 1.0f;
		}

		if(!m_p->m_box.Intersects(box))//两个体不相交则没有影响
			return -1.0f;

		
		if(m_p->m_light.type==Light_Point)
		{
			float dist=box.DistToPoint(m_p->m_light.pos);
			if(dist > m_p->m_light.range)
				return 0.0f;
			else
			{
				const Vector3& a=m_p->m_light.attenuation;
				return 1.0f/(a.x+a.y*dist+a.z*dist*dist);//衰减系数
			}
		}
		else if(m_p->m_light.type==Light_Spot)
		{
			//todo 计算Spot影响系数
			return 1.0f;
		}
		else
			return -1.0f;
	}

	void SGLightNode::SetLight(const tagLight *pLgt)
	{
		m_p->m_light=*pLgt;
		m_p->m_localPos=pLgt->pos;
		m_p->m_localDir=pLgt->dir;

		m_p->CalLightBox();
	}

	void SGLightNode::SetDynamic(bool bDynamic)
	{
		m_p->m_bIsDynamic=bDynamic;
	}

	bool SGLightNode::IsDynamic()
	{
		return m_p->m_bIsDynamic;
	}

	float SGLightNode::GetNoBlockRange() const
	{
		return m_p->m_noBlockRange;
	}

	void SGLightNode::SetNoBlockRange( float range )
	{
		m_p->m_noBlockRange=range;
	}

	void SGLightNode::SetLightModulus( float modulus )
	{
		m_p->m_lightModulus=modulus;
	}

	float SGLightNode::GetLightModulus() const
	{
		return m_p->m_lightModulus;
	}
}//namespace Cool3D