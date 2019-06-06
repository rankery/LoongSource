#include "StdAfx.h"
#include "NavColliderCache.h"
#include "NavSceneNode.h"

namespace WorldBase
	{
	const float g_floatMax=1000000.0f;

	NavColliderCache::NavColliderCache(void)
	{
	}

	NavColliderCache::~NavColliderCache(void)
	{
	}

	void NavColliderCache::Create( const Vector3& moveStart,const Vector3& moveEnd,const Vector3& roleHalfSize )
	{
		m_box.min.x=moveStart.x-roleHalfSize.x;
		m_box.min.y=-g_floatMax;
		m_box.min.z=moveStart.z-roleHalfSize.z;
		m_box.max.x=moveStart.x+roleHalfSize.x;
		m_box.max.y=g_floatMax;
		m_box.max.z=moveStart.z+roleHalfSize.z;

		AABBox box1;
		box1.min.x=moveStart.x-roleHalfSize.x;
		box1.min.y=-g_floatMax;
		box1.min.z=moveStart.z-roleHalfSize.z;
		box1.max.x=moveStart.x+roleHalfSize.x;
		box1.max.y=g_floatMax;
		box1.max.z=moveStart.z+roleHalfSize.z;

		m_box.Merge(box1);
	}

	void NavColliderCache::AddSceneNode( NavSceneNode* pSceneNode )
	{
		m_sceneNodes.push_back(pSceneNode);
	}

	bool NavColliderCache::AABBoxCollideBox( const AABBox& box )
	{
		return false;
	}

	bool NavColliderCache::AABBoxCollideBoxTopHighest( const AABBox& box,float& intersectY,AABBox* pCollideBox,DWORD dwFlags/*=0*/ )
	{
		return false;
	}
}//namespace WorldBase