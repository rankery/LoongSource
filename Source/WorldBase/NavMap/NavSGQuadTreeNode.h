#pragma once
#include "NavSurface.h"

namespace WorldBase
{
	class NavSceneNode;
	class NavQuadTreeData;
	class NavColliderCache;
	class NavQuadTreeSG;
	/**	\class NavSGQuadTreeNode
		\brief 四叉树空间分割的节点
	*/
	class NavSGQuadTreeNode : public RTTIObj,public ISerializable
	{
	public:
		NavSGQuadTreeNode(NavQuadTreeSG* pSG);
		virtual ~NavSGQuadTreeNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
				
		//--
		void InvalidViewData(const AABBox& viewZone,NavQuadTreeData *pDataSrc);

		bool AABBoxCollideBox(const AABBox& box,float curTime,DWORD dwFlags,NavSceneNode** ppSceneNode);
		bool AABBoxCollideBoxTopHighest(const AABBox& box,float curTime,float& intersectY,DWORD dwFlags,NavSceneNode** ppSceneNode);
		bool RayCollideBox(const Ray& ray,Vector3& intersectPos);
		bool RayCollideBox(const Ray& ray,DWORD dwFlags=0);
		bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out);

		int GetID() const				{	return m_id;}
		inline const AABBox& GetBox() const	{	return m_box;}

		void Clone(NavSGQuadTreeNode* pTarget);
		void AddSceneNodeToCache(NavColliderCache* pCache);
		UINT GetContentSize(void) const { return m_pContent.size(); }

	protected:
		void RayCollideR(const Ray& ray,TResult& lastTR);
		void RegisterDynamicNodesToSG();
		void UnRegisterDynamicNodesFromSG();
			
	protected:
		int		m_id;	//id in Quad tree scene graph
		int		m_lv;	//level in quad tree,lv 0 is root
		AABBox	m_box;

		//--冗余数据,主要是为了在没有child实例对象的时候检测child是否可见
		int					m_childrenID[4];
		AABBox				m_childrenBox[4];
		
		//--
		vector<NavSceneNode*>	m_pContent;
		NavSGQuadTreeNode*		m_pChildren[4];

		NavQuadTreeSG*			m_pSG;

		DECL_RTTI_DYNC(NavSGQuadTreeNode);
	};
}//namespace WorldBase