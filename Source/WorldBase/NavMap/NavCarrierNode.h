#pragma once
#include "NavEntityNode.h"

namespace WorldBase
{
	/** \class NavCarrierNode
		\brief 载具结点
	*/
	class WORLD_BASE_API NavCarrierNode : public NavEntityNode
	{
		struct tagWayPoint//导航点		
		{
			Vector3 pos;
			float stopTime;

			float stopEndTime;
			float moveEndTime;
		};

	public:
		NavCarrierNode(void);
		virtual ~NavCarrierNode(void);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--NavSceneNode
		virtual bool AABBoxCollideBox(const AABBox& box,float curTime);
		virtual bool AABBoxCollideBoxTopHighest(const AABBox& box,float curTime,float& intersectY);
		virtual TResult RayCollideBox(const Ray& ray){ return TResult(false,0.0f);}
		virtual bool RayCollideBoxOnce(const Ray& ray){ return false;}
		virtual bool QuerySurfaceType(const AABBox& box,float curTime,float& intersectY,ESurfaceType& out);
		virtual NavSceneNode* Clone();

		//--
		/** 清除移动路径
		*/
		void ClearPath();
		/** 添加一个导航点
		*/
		void AddWayPoint(const Vector3& pos,float stopTime);
		/** 设置移动速度
		*/
		void SetSpeed(float speed);
		/** 获得移动速度
		*/
		float GetSpeed(){ return m_speed;}

		/** 开始运行
		*/
		void Run(float curTime,float timePos);
		/** 获得当前运行时长
		*/
		float GetRunTime(float curTime);
		/** 返回是否已运行
		*/
		bool IsRunning(){return m_bRunning;}


		/** 将坐标从Object空间变换到World空间
		*/
		void PosToWorld(const Vector3& localPos,float curTime,Vector3& out);
		/** 将坐标从World空间变换到Object空间
		*/
		void PosToLocal(const Vector3& worldPos,float curTime,Vector3& out);
		/** 将方向从World空间变换到Object空间
		*/
		void DirToLocal(const Vector3& worldDir,float curTime,Vector3& out);

		/** AABB在Object空间与this碰撞
		*/
		bool AABBoxCollideBoxInLocal(const AABBox& box);
		/** AABB在Object空间与this碰撞盒子顶部
		*/
		bool AABBoxCollideBoxTopHighestInLocal(const AABBox& box,float& intersectY);

		/** 设置世界空间AABB
		*/
		void SetWorldBox(const AABBox& box);

		/** 获得移动方向和速度
		*/
		void GetMoveDirAndSpeed(float curTime,Vector3& dir);
		/** 修改跳跃初速度和方向
		*/
		void AdjustJumpDirAndSpeed(float curTime,Vector3& xzDir,float& xzSpeed,float& ySpeed);
		/** 计算当前位位置
		*/
		void GetPos(float curTime,Vector3& out);

	private:
		bool m_bRunning;			//是否在运行
		float m_speed;				//移动速度
		float m_startRunTime;		//启动时间
		float m_lapRunTime;			//运行一圈需要多长时间

		vector<tagWayPoint> m_path;	//移动路径
		
		DECL_RTTI_DYNC(NavCarrierNode);
	};
}//namespace WorldBase