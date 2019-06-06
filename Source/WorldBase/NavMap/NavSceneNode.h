#pragma once
#include "NavSurface.h"

namespace WorldBase
{
	/**	\class NavSceneNode
		\brief 导航图场景结点	
	*/
	class WORLD_BASE_API NavSceneNode : public RTTIObj,public ISerializable
	{
	public:
		enum EFlags
		{
			EFlag_SupportPlayerPathFind=1,	//玩家自动寻路可通过？
			EFlag_SupportNPCNavMap=2,		//格子导航NPC可通过？
			EFlag_Door=4,					//门
			EFlag_NotDoor=8,				//非门
			EFlag_Carrier=16,				//载具
			EFlag_NotCarrier=32,			//非载具
			EFlag_Terrain=64,				//地形
		};
	public:
		NavSceneNode();
		virtual ~NavSceneNode();

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--
		virtual void LoadRes(const TCHAR *szName) =0;
		virtual void GetBox(AABBox& box) =0;
		virtual void SetWorldMat(const Matrix4& worldMat) =0;

		virtual bool AABBoxCollideBox(const AABBox& box,float curTime) =0;
		virtual bool AABBoxCollideBoxTopHighest(const AABBox& box,float curTime,float& intersectY) =0;
		virtual TResult RayCollideBox(const Ray& ray) =0;
		virtual bool RayCollideBoxOnce(const Ray& ray) =0;
		virtual bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out) =0;

		virtual NavSceneNode* Clone() =0;

		DWORD GetFlags(){ return m_dwFlags;}
		void SetFlags(DWORD dwFlags){ m_dwFlags=dwFlags;}

		void EnableCollide(bool bEnable){ m_bEnableCollide=bEnable;}
		bool IsEnableCollide(){ return  m_bEnableCollide;}

		void SetObjID(DWORD dwObjID){ m_dwObjID=dwObjID;}
		DWORD GetObjID(){ return m_dwObjID;}

	protected:
		DWORD m_dwFlags;		//标志位
		DWORD m_dwObjID;		//编辑器中指定的物件ID
		bool m_bEnableCollide;	//碰撞检测开关

		DECL_RTTI(NavSceneNode);
	};
}//namespace WorldBase