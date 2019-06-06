#pragma once

namespace WorldBase
{
	class NavSceneNode;
	class NavColliderCache
	{
	public:
		NavColliderCache(void);
		virtual ~NavColliderCache(void);

		void Create(const Vector3& moveStart,const Vector3& moveEnd,const Vector3& roleHalfSize);
		const AABBox& GetBox(){ return m_box;}

		void AddSceneNode(NavSceneNode* pSceneNode);

		bool AABBoxCollideBox(const AABBox& box);
		bool AABBoxCollideBoxTopHighest(const AABBox& box,float& intersectY,AABBox* pCollideBox,DWORD dwFlags=0);

	private:
		AABBox m_box;
		vector<NavSceneNode*> m_sceneNodes;
	};
}//namespace WorldBase