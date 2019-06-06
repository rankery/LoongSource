#pragma once
#include "NavSceneNode.h"
#include "NavSurface.h"

namespace WorldBase
{
	class NavSceneGraph;
	class NavResSceneMesh;

	/**	\class NavEntityNode
		\brief 实体结点
	*/
	class WORLD_BASE_API NavEntityNode : public NavSceneNode
	{
	public:
		NavEntityNode();
		virtual ~NavEntityNode();

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--NavSceneNode
		virtual void LoadRes(const TCHAR *szName);
		virtual void GetBox(AABBox& box);
		virtual bool AABBoxCollideBox(const AABBox& box,float curTime);
		virtual bool AABBoxCollideBoxTopHighest(const AABBox& box,float curTime,float& intersectY);
		virtual TResult RayCollideBox(const Ray& ray);
		virtual bool RayCollideBoxOnce(const Ray& ray);
		virtual bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out);
		virtual void SetWorldMat(const Matrix4& worldMat);
		virtual NavSceneNode* Clone();

		//--
		const Matrix4& GetWorldMat() const;
		const Matrix4& GetWorldMatInv() const;
		void SetScale(float xScale,float yScale,float zScale);
		void SetSurfaceType(ESurfaceType type);
		ESurfaceType GetSurfaceType();
		const TCHAR* GetResName();

		void Clone(NavEntityNode* pTarget);

		void DrawBox();
			

		NavResSceneMesh		*m_pRes;
		Matrix4				m_toWorld;
		Matrix4				m_toWorldInv;
		AABBox				m_worldBox;
		tstring				m_szResName;
		bool				m_bIsCollide;
		Vector3				m_scale;
		Vector3				m_scaleInv;
		ESurfaceType		m_surfaceType;

		DECL_RTTI_DYNC(NavEntityNode);
	};
}//namespace WorldBase