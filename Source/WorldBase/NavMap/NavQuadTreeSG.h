#pragma once
#include "navscenegraph.h"
#include "NavQuadTreeData.h"

class IFS;
namespace WorldBase
{
	class NavSGQuadTreeNode;
	class NavColliderCache;
	class NavEntityNode;
	class NavSceneNode;
	class NavCarrierNode;
	/**	\class NavQuadTreeSG 
		\brief 大型的四叉树场景管理,提供对连续世界的支持
	*/
	class NavQuadTreeSG :	public NavSceneGraph
	{
	public:
		NavQuadTreeSG(void);
		virtual ~NavQuadTreeSG(void);

		//--NavSceneGraph
		virtual void LoadFromFile(IFS *pFS,const TCHAR* szMapName);
		virtual void InvalidViewZone(const Vector3& zoneCenter);
		virtual bool AABBoxCollideBox(const AABBox& box,float curTime,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		virtual bool AABBoxCollideBoxTop(const AABBox& box,float curTime,float& intersectY,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		virtual bool RayCollideBox(const Ray& ray,Vector3& intersectPos);
		virtual bool RayCollideBox(const Ray& ray,DWORD dwFlags=0);
		virtual bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out);

		//--
		void Clone(NavQuadTreeSG* pTarget);
		void AddSceneNodeToCache(NavColliderCache* pCache);
		/** 注册门
		*/
		void RegisterDoor(DWORD dwObjID,NavSceneNode* pDoor);
		/** 注销门
		*/
		void UnregisterDoor(DWORD dwObjID);
		/** 打开\关闭所有的门的碰撞
		*/
		void EnableAllDoorsCollide(bool bEnable);
		/** 打开\关闭门的碰撞
		*/
		void EnableDoorCollide(DWORD dwObjID,bool bEnable);
		/** 获得门的SceneNode,未找到返回NULL
		*/
		NavSceneNode* FindDoor(DWORD dwObjID);
		/** 注册载具
		*/
		void RegisterCarrier(DWORD dwObjID,NavCarrierNode* pCarrier);
		/** 注销载具
		*/
		void UnregisterCarrier(DWORD dwObjID);
		/** 查找载具
		*/
		NavCarrierNode* FindCarrier(DWORD dwObjID);


	private:
		NavQuadTreeData				m_data;
		NavSGQuadTreeNode			*m_pRoot;
		map<DWORD,NavSceneNode*>	m_doorMap;//门
		map<DWORD,NavCarrierNode*>  m_carrierMap;//载具

		DECL_RTTI_DYNC(NavQuadTreeSG);
	};
}//namespace WorldBase