#pragma once
#include "NavSurface.h"

namespace WorldBase
{
	class NavQuadTreeSG;
	class NavResHeightmap;
	class NavColliderCache;
	class HalfByteMap;
	class NavSceneNode;
	class NavCarrierNode;
	class NavResBitMap;

	/** \class NavCollider
		\brief 碰撞检测器
	*/
	class WORLD_BASE_API NavCollider
	{
	public:
		enum EReturn			//函数返回值定义
		{
			ERet_None	= -1,	//
			ERet_Arrive,		//到达终点
			ERet_Blocking,		//被挡住
			ERet_WillDrop,		//将掉落
			ERet_WillVDrop,		//将垂直掉落
			ERet_WillSlide,		//将滑落
			ERet_WillSwim,		//将游泳
			ERet_WillInWater,	//将入水
			ERet_SwimGround,	//游泳搁浅
			ERet_Standup,		//进入可站立的位置
			ERet_Infinite,		//进入无限碰撞检测状态，可能上层逻辑有问题
			ERet_TooLong,		//距离太远
			ERet_CannotJump,	//无法起跳
			ERet_ToBeContinued,	//可以继续移动
		};

		struct tagMovePoint		//移动点
		{
			Vector3 pos;		//坐标
			float   dist;		//与起点距离
		};

		struct tagStepMoveData	//分段move数据
		{
			Vector3 xzDir;		
			float	xzTotalDist;
			float   xzCurDist;	
			float	curDist;	
			Vector3 curPos;		
		};

		struct tagStepSwimData	//分段swim数据
		{
			Vector3 xzDir;		
			float	xzTotalDist;
			float   xzCurDist;	
			float	curDist;	
			Vector3 curPos;		
		};

	public:
		NavCollider(void);
		~NavCollider(void);

		//--基础功能-----------------------------------------------------------------------

		/** 加载数据
		*/
		void LoadFromFile(const TCHAR* szMapName,IFS* pFS,bool bLoadGroundType=false);
		/** 释放数据
		*/
		void Destroy();
		/** 设置可见区域大小
		*/
		void SetViewZoneSize(const Vector3& size);
		/** 更新可见区域
		*/
		void InvalidViewZone(const Vector3& zoneCenter);
		/** 克隆对象
		*/
		void Clone(NavCollider* pTarget);

		//--模型碰撞检测-------------------------------------------------------------------

		/** AABB与模型AABB碰撞检测
		*/
		bool AABBoxCollideBox(const AABBox&  box,float curTime,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		bool AABBoxCollideBoxAndTrn(const AABBox& box,float curTime,float groundH,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		bool AABBoxCollideBoxAndTrn(const AABBox& box,float curTime,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		/** AABB与模型AABB碰撞检测，返回碰撞位置
		*/
		bool AABBoxCollideBoxTop(const AABBox& box,float curTime,float& intersectH,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		bool AABBoxCollideBoxTopAndTrn(const AABBox& box,float curTime,float groundH,float& intersectH,bool* pbOnGround=NULL,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		bool AABBoxCollideBoxTopAndTrn(const AABBox& box,float curTime,float& intersectH,bool* pbOnGround=NULL,DWORD dwFlags=0,DWORD* pdwCarrierObjID=NULL);
		/** 射线与模型AABB碰撞检测
		*/
		bool RayCollideBox(const Ray& ray,Vector3& intersectPos);
		bool RayCollideBoxAndTrn(const Ray& ray,Vector3& intersectPos);
		bool RayCollideBox(const Ray& ray);
		bool RayCollideBoxAndTrn(const Ray& ray,DWORD dwFlags=0);

		//--地形碰撞检测--------------------------------------------------------------------

		/** 快速判断xz平面的指定区域是否有模型
		*/
		bool HaveModel(float x,float z);
		bool HaveModel(float x1,float z1,float x2,float z2);
		/** 取得地表高度，xz坐标参数会自动进行截断
		*/
		float SafeGetGroundHeight(float x,float z);
		float SafeGetGroundHeight(float x1,float z1,float x2,float z2);
		/** 取得地表高度，与相邻格子的高度进行插值，xz坐标参数会自动进行截断
		*/
		float SafeGetGroundHeightSlerp(float x,float z);
		float SafeGetGroundHeightSlerp(float x1,float z1,float x2,float z2);
		/** 射线与地形碰撞检测
		*/
		bool RayCollideTrn(const Ray& ray,Vector3& intersectPos);
		/** 判断是否会滑落
		*/
		bool IfWillSlide(float x,float z);
		bool IfWillSlide(const Vector3& pos,const Vector3& roleSize);
		/** 计算滑落方向，xz坐标参数会自动进行截断
		*/
		bool SafeCalcSlideDir(float x,float z,Vector3& out);

		//--水面碰撞检测--------------------------------------------------------------------

		/** 判断指定位置是否会进入游泳状态
		*/
		bool IfWillSwim(const Vector3& curPos,float roleHeight);
		bool IfWillSwim(const Vector3& curPos,float roleHeight,float& footH);
		/** 判断指定位置是否在水中
		*/
		bool IfWillOnWater(const Vector3& curPos);
		bool IfWillOnWater(const Vector3& curPos,float& waterH);
		/** 计算游泳时，脚底的高度
		*/
		float CalcSwimFootHeight(const Vector3& curPos,float roleHeight,float defaultHeight);
		/** 获得水面高度
		*/
		bool  GetWaterHeight(const Vector3& curPos,float& out);
		float GetWaterHeightEx(const Vector3& curPos,float defaultHeight);
		/** 射线与水面碰撞检测
		*/
		bool RayCollideWater(const Ray& ray,Vector3& intersectPos);

		//--可行走区域--------------------------------------------------------------------
		inline bool IfCanGo(float x,float z,BitMap* pSpecCangoMap=NULL);

		//--高级功能------------------------------------------------------------------------

		/** 判断是否会垂直掉落
		*/
		bool IfWillVDrop(const Vector3& curPos,const Vector3& roleSize);

		/** 移动碰撞检测
		*/
		EReturn Move(const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bPassCheck=true,DWORD dwFlags=0,float fMoveFall=100.0f,bool bInWaterCheck=false);
		/** 分段移动初始化
		*/
		void StepMoveInit(const Vector3& start,const Vector3& end,tagStepMoveData& data);
		/** 分段移动
		*/
		EReturn StepMove(const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepMoveData& data,list<tagMovePoint>& path,bool bPassCheck=true,bool bInWaterCheck=false);
		/** 掉落碰撞检测
		*/
		EReturn Drop(const Vector3& start,const Vector3& dir,float xzSpeed,const Vector3& roleSize,float& totalTime,bool bInWaterCheck=false);
		/** 垂直掉落碰撞检测
		*/
		EReturn VDrop(const Vector3& start,const Vector3& roleSize,float& totalTime,bool bOnlyTrn=false,bool bInWaterCheck=false);
		/** 跳跃碰撞检测
		*/
		EReturn Jump(const Vector3& start,const Vector3& dir,float xzSpeed,float ySpeed,const Vector3& roleSize,float& totalTime,bool bOnlyTrn=false,BitMap* pSpecCangoMap=NULL,bool bInWaterCheck=false);
		/** 滑落碰撞检测
		*/
		EReturn Slide(const Vector3& start,const Vector3& roleSize,list<tagMovePoint>& path,bool bInWaterCheck=false);
		/** NPC移动碰撞检测
		*/
		bool NPCMove(const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bHeightCheck=false,bool bPassCheck=false);
		/** NPC分段移动初始化
		*/
		void NPCStepMoveInit(const Vector3& start,const Vector3& end,tagStepMoveData& data);
		/** NPC分段移动
		*/
		EReturn NPCStepMove(const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepMoveData& data,list<tagMovePoint>& path,bool bHeightCheck=false,bool bPassCheck=false);
		/** 游泳前进
		*/
		EReturn Swim(const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bPassCheck=true,bool bWaterWalk=false);
		/** 分段游泳初始化
		*/
		void StepSwimInit(const Vector3& start,const Vector3& end,tagStepSwimData& data);
		/** 分段游泳
		*/
		EReturn StepSwim(const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepSwimData& data,list<tagMovePoint>& path,bool bPassCheck=true,bool bWaterWalk=false);

		//--辅助功能---------------------------------------------------------------------------

		/** 根据移动路径和插值因子计算当前位置
		*/
		void CalcMovePos(const Vector3& start,list<tagMovePoint>& path,float factor,Vector3& out);
		/** 根据时间计算掉落位置
		*/
		void CalcDropPos(const Vector3& start,const Vector3& dir,float xzSpeed,float curTime,float totalTime,Vector3& out);
		/** 根据时间计算垂直掉落位置
		*/
		void CalcVDropPos(const Vector3& start,float curTime,float totalTime,Vector3& out);
		/** 根据时间计算跳跃位置
		*/
		void CalcJumpPos(const Vector3& start,const Vector3& dir,float xzSpeed,float ySpeed,float curTime,float totalTime,Vector3& out);
		/** 计算滑落插值因子
		*/
		float CalcSlideFactor(float totalDist,float curTime);

		//--门相关-----------------------------------------------------------------------------
		
		/** 开启/关闭所有门的碰撞
		*/
		void EnableAllDoorsCollide(bool bEnable);
		/** 开启/关闭指定门的碰撞
		*/
		void EnableDoorCollide(DWORD dwObjID,bool bEnable);
		/** 获得门的SceneNode,未找到返回NULL
		*/
		NavSceneNode* FindDoor(DWORD dwObjID);

		//--载具相关---------------------------------------------------------------------------
		
		/** 根据载具ObjID获得SceneNode
		*/
		NavCarrierNode* FindCarrier(DWORD dwObjID);

		//--其它功能---------------------------------------------------------------------------
		
		/** 返回指定位置是否支持自动寻路
		*/
		bool IfCanFindPath(const Vector3& pos,const Vector3& roleSize);
		/** 从指定位置垂直向下碰撞查询表面类型
		*/
		ESurfaceType QuerySurfaceType(const Vector3& pos,float curTime);
		/** 是否已加载
		*/
		bool IsLoaded() const { return m_bLoaded; }
	private:
		NavQuadTreeSG*		m_pSG;
		NavResHeightmap*	m_pHMap;
		NavResBitMap*		m_pModelMap;
		NavResBitMap*		m_pCangoMap;
		NavResBitMap*		m_pWaterArea;
		NavResHeightmap*	m_pWaterHeight;
		HalfByteMap*		m_pGroundType;
		bool                m_bLoaded;
	};

}//namespace WorldBase