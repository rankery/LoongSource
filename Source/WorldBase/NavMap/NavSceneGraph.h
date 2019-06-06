#pragma once
#include "NavSurface.h"

namespace WorldBase
{
	/**	\class NavSceneGraph
		\brief 导航图场景管理器
	*/
	class WORLD_BASE_API NavSceneGraph : public RTTIObj
	{
	public:
		NavSceneGraph();
		virtual ~NavSceneGraph();

		virtual void LoadFromFile(IFS *pFS,const TCHAR *szMapName) =0;
		void SetViewZoneSize(const Vector3& size){ m_viewZoneSize=size;}
		/** 更新视野区域
		*/
		virtual void InvalidViewZone(const Vector3& zoneCenter) =0;

		virtual bool AABBoxCollideBoxTop(const AABBox& box,float curTime,float& intersectY,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL) =0;
		virtual bool AABBoxCollideBox(const AABBox& box,float curTime,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL) =0;
		virtual bool RayCollideBox(const Ray& ray,Vector3& intersectPos) =0;
		virtual bool RayCollideBox(const Ray& ray,DWORD dwFlags=0) =0;
		virtual bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out) =0;

	protected:
		void CalCurrentViewZone(const Vector3& zoneCenter,AABBox& out);

	protected:
		Vector3 m_viewZoneSize;

		DECL_RTTI(NavSceneGraph);
	};
}//namespace WorldBase