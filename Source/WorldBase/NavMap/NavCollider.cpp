#include "StdAfx.h"
#include "NavCollider.h"
#include "NavQuadTreeSG.h"
#include "NavHeightmap.h"
#include "NavResHeightmap.h"
#include "NavResMgr.h"
#include "NavColliderCache.h"
#include "NavSceneNode.h"
#include "HalfByteMap.h"
#include "NavResBitMap.h"

namespace WorldBase
{
	const float g_collideStep=50.0f;			//碰撞检测步长
	const float g_moveFall=100.0f;				//移动时最大上下落差
	const float g_collideTimeOut=60.0f;			//碰撞检测超时（秒）
	const float g_gravitation=1200.0f;			//重力加速度
	const float g_halfGravitation=1200.0f*0.5f;	//重力加速度*0.5
	const float g_slideFall=50.0f;				//地形高度图两个点滑落落差
	const float g_tileSize=50.0f;				//高度图格子大小
	const float g_floatMax=1000000.0f;			//最大浮点数
	const float g_swimFloodRate=0.75f;			//当淹没身高多大比率时变成游泳状态
	const int g_riverPatchSize=128;				//河流块大小（格子）


	NavCollider::NavCollider(void)
		: m_pSG(NULL)
		, m_pHMap(NULL)
		, m_pModelMap(NULL)
		, m_pCangoMap(NULL)
		, m_pWaterArea(NULL)
		, m_pWaterHeight(NULL)
		, m_pGroundType(NULL)
		, m_bLoaded(false)
	{
	}

	NavCollider::~NavCollider(void)
	{
		Destroy();
	}

	void NavCollider::LoadFromFile( const TCHAR* szMapName,IFS* pFS,bool bLoadGroundType )
	{
		Destroy();

		//--加载场景四叉树
		m_pSG=new NavQuadTreeSG;
		m_pSG->LoadFromFile(pFS,szMapName);

		// 关闭异步IO
		bool bAsync=NavResMgr::Inst()->IsIOAsync();
		NavResMgr::Inst()->EnableIOAsync(false);

		TCHAR szPath[512];

		//--加载高度图资源
		_stprintf(szPath,_T("data\\map\\%s\\nav\\terrain.hmap"),szMapName);
		m_pHMap=(NavResHeightmap*)NavResMgr::Inst()->NewRes(szPath);
		
		//--加载模型位图资源
		_stprintf(szPath,_T("data\\map\\%s\\nav\\model.bmap"),szMapName);
		m_pModelMap=(NavResBitMap*)NavResMgr::Inst()->NewRes(szPath);

		//--加载可行走区域图资源
		_stprintf(szPath,_T("data\\map\\%s\\nav\\cango.bmap"),szMapName);
		m_pCangoMap=(NavResBitMap*)NavResMgr::Inst()->NewRes(szPath);

		//--加载水面区域图资源
		_stprintf(szPath,_T("data\\map\\%s\\nav\\waterarea.bmap"),szMapName);
		m_pWaterArea=(NavResBitMap*)NavResMgr::Inst()->NewRes(szPath);

		//--加载水面高度图
		_stprintf(szPath,_T("data\\map\\%s\\nav\\waterheight.hmap"),szMapName);
		m_pWaterHeight=(NavResHeightmap*)NavResMgr::Inst()->NewRes(szPath);

		NavResMgr::Inst()->EnableIOAsync(bAsync);

		//--加载地表类型数据
		if(bLoadGroundType)
		{
			m_pGroundType=new HalfByteMap;
			_stprintf(szPath,_T("data\\map\\%s\\nav\\groundtype.data"),szMapName);
			DWORD hFile=pFS->Open(szPath);
			THROW_NULL(hFile,_T("load nav groundtype.data fail"));

			m_pGroundType->ReadFile(pFS,hFile);
			pFS->Close(hFile);
		}
		m_bLoaded=true;
	}

	void NavCollider::Destroy()
	{
		NAV_RES_MGR_RELEASE_RES(m_pHMap);
		NAV_RES_MGR_RELEASE_RES(m_pModelMap);
		NAV_RES_MGR_RELEASE_RES(m_pCangoMap);
		NAV_RES_MGR_RELEASE_RES(m_pWaterArea);
		NAV_RES_MGR_RELEASE_RES(m_pWaterHeight);
		SAFE_DELETE(m_pSG);
		SAFE_DELETE(m_pGroundType);
		m_bLoaded=false;
	}

	void NavCollider::SetViewZoneSize( const Vector3& size )
	{
		m_pSG->SetViewZoneSize(size);
	}

	void NavCollider::InvalidViewZone( const Vector3& zoneCenter )
	{
		m_pSG->InvalidViewZone(zoneCenter);
	}

	void NavCollider::Clone(NavCollider* pTarget)
	{
		//--scene graph
		ASSERT(pTarget->m_pSG==NULL);
		pTarget->m_pSG=new NavQuadTreeSG;
		m_pSG->Clone(pTarget->m_pSG);

		//--model map
		ASSERT(pTarget->m_pModelMap==NULL);
		pTarget->m_pModelMap=(NavResBitMap*)NavResMgr::Inst()->NewRes(m_pModelMap->GetName());

		//--cango map
		ASSERT(pTarget->m_pCangoMap==NULL);
		pTarget->m_pCangoMap=(NavResBitMap*)NavResMgr::Inst()->NewRes(m_pCangoMap->GetName());

		//--height map
		ASSERT(pTarget->m_pHMap==NULL);
		pTarget->m_pHMap=(NavResHeightmap*)NavResMgr::Inst()->NewRes(m_pHMap->GetName());

		//--water area 
		ASSERT(pTarget->m_pWaterArea==NULL);
		pTarget->m_pWaterArea=(NavResBitMap*)NavResMgr::Inst()->NewRes(m_pWaterArea->GetName());

		//--water height 
		ASSERT(pTarget->m_pWaterHeight==NULL);
		pTarget->m_pWaterHeight=(NavResHeightmap*)NavResMgr::Inst()->NewRes(m_pWaterHeight->GetName());

		//--ground type
		ASSERT(pTarget->m_pGroundType==NULL);
		if(m_pGroundType!=NULL)//有可能没有加载
		{
			pTarget->m_pGroundType=new HalfByteMap;
			m_pGroundType->Clone(pTarget->m_pGroundType);
		}
		pTarget->m_bLoaded=m_bLoaded;
	}

	bool NavCollider::AABBoxCollideBoxTop( const AABBox& box,float curTime,float& intersectH,DWORD dwFlags,DWORD* pdwCarrierObjID)
	{
		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		intersectH=box.min.y;
		return m_pSG->AABBoxCollideBoxTop(box,curTime,intersectH,dwFlags,pdwCarrierObjID);
	}

	bool NavCollider::AABBoxCollideBoxTopAndTrn(const AABBox& box,float curTime,float groundH,float& intersectH,bool* pbOnGround,DWORD dwFlags,DWORD* pdwCarrierObjID)
	{
		if(pbOnGround)
			*pbOnGround=false;

		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		intersectH=box.min.y;

		bool bCollideBox=m_pSG->AABBoxCollideBoxTop(box,curTime,intersectH,dwFlags,pdwCarrierObjID);
		if(bCollideBox)
		{
			if(box.min.y<=groundH
				&&groundH>intersectH)
			{
				intersectH=groundH;

				if(pbOnGround)
					*pbOnGround=true;
			}

			return true;
		}
		else
		{
			if(box.min.y<=groundH)
			{
				intersectH=groundH;
				if(pbOnGround)
					*pbOnGround=true;

				return true;
			}
		}

		return false;
	}

	bool NavCollider::AABBoxCollideBoxTopAndTrn( const AABBox& box,float curTime,float& intersectH,bool* pbOnGround,DWORD dwFlags,DWORD* pdwCarrierObjID )
	{
		//--
		if(pbOnGround)
			*pbOnGround=false;

		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		//--get ground height
		float x=(box.min.x+box.max.x)*0.5f;
		float z=(box.min.z+box.max.z)*0.5f;
		float groundH=SafeGetGroundHeightSlerp(x,z);

		//--collide box?
		bool bCollideBox=false;
		if(HaveModel(box.min.x,box.min.z,box.max.x,box.max.z))
		{
			intersectH=box.min.y;
			bCollideBox=m_pSG->AABBoxCollideBoxTop(box,curTime,intersectH,dwFlags,pdwCarrierObjID);
		}

		//--
		if(bCollideBox)
		{
			if(box.min.y<=groundH
				&&groundH>intersectH)
			{
				intersectH=groundH;

				if(pbOnGround)
					*pbOnGround=true;
			}

			return true;
		}
		else
		{
			if(box.min.y<=groundH)
			{
				intersectH=groundH;
				if(pbOnGround)
					*pbOnGround=true;

				return true;
			}
		}

		return false;
	}

	bool NavCollider::AABBoxCollideBox( const AABBox& box,float curTime,DWORD dwFlags,DWORD* pdwCarrierObjID )
	{
		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		return m_pSG->AABBoxCollideBox(box,curTime,dwFlags,pdwCarrierObjID);
	}

	bool NavCollider::AABBoxCollideBoxAndTrn(const AABBox& box,float curTime,float groundH,DWORD dwFlags,DWORD* pdwCarrierObjID)
	{
		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		return box.min.y<groundH
			||m_pSG->AABBoxCollideBox(box,curTime,dwFlags,pdwCarrierObjID);
	}

	bool NavCollider::AABBoxCollideBoxAndTrn( const AABBox& box,float curTime,DWORD dwFlags/*=0*/,DWORD* pdwCarrierObjID/*=NULL*/ )
	{
		//--
		if(pdwCarrierObjID!=NULL)
			*pdwCarrierObjID=-1;

		//--get ground height
		float x=(box.min.x+box.max.x)*0.5f;
		float z=(box.min.z+box.max.z)*0.5f;
		float groundH=SafeGetGroundHeightSlerp(x,z);

		//--have model?
		if(HaveModel(box.min.x,box.min.z,box.max.x,box.max.z))
		{
			return box.min.y<groundH
				||m_pSG->AABBoxCollideBox(box,curTime,dwFlags,pdwCarrierObjID);
		}
		else
		{
			return box.min.y<groundH;
		}
	}

	bool NavCollider::RayCollideBox( const Ray& ray,Vector3& intersectPos )
	{
		return m_pSG->RayCollideBox(ray,intersectPos);
	}

	bool NavCollider::RayCollideBox( const Ray& ray )
	{
		return m_pSG->RayCollideBox(ray);
	}

	bool NavCollider::RayCollideBoxAndTrn( const Ray& ray,Vector3& intersectPos )
	{
		ASSERT(m_pHMap->IsCreated());

		TResult tr(false,FLOAT_MAX);
		Vector3 pos;
		if(m_pHMap->GetHMap()->RayCollision(ray,pos,ray.length))
		{
			tr.first=true;
			tr.second=Vec3DistSq(ray.origin,pos);
			intersectPos=pos;
		}
		if(m_pSG->RayCollideBox(ray,pos))
		{
			tr.first=true;
			if(Vec3DistSq(ray.origin,pos)<tr.second)
				intersectPos=pos;
		}
		return tr.first;
	}

	bool NavCollider::RayCollideBoxAndTrn( const Ray& ray, DWORD dwFlags )
	{
		ASSERT(m_pHMap->IsCreated());

		Vector3 tmp;
		return m_pSG->RayCollideBox(ray,dwFlags)
			||m_pHMap->GetHMap()->RayCollision(ray,tmp,ray.length);
	}

	NavCollider::EReturn NavCollider::Move(const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bPassCheck/*=true*/,DWORD dwFlags/*=0*/,float fMoveFall/*=100.0f*/,bool bInWaterCheck/*=false*/)
	{
		Vector3 halfSize=roleSize*0.5f;
		path.clear();

		Vector3 dir=end-start;
		dir.y=0;
		float dist=Vec3Len(&dir);
		if(dist==0)
			return ERet_Arrive;
		dir/=dist;

		Vector3 curPos=start;
		float f=0;
		float totalDist=0;
		bool bArrive=false;
		for(int i=0;i<maxCollideTimes;i++)
		{
			Vector3 nextPos=curPos+dir*g_collideStep;
			if(f+g_collideStep>=dist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--快速判断角色所在区域是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			float groundH=SafeGetGroundHeightSlerp(nextPos.x,nextPos.z);
			//float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);


			//--找出落脚点
			bool bOnGround=false;
			if(bHaveModel)
			{
				box.max.y=nextPos.y+fMoveFall;
				box.min.y=-g_floatMax;
				if(!AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,dwFlags,NULL))
					return ERet_Blocking;
			}
			else
			{
				bOnGround=true;
				nextPos.y=groundH;
			}

			if(bPassCheck)
			{
				//--判断落差
				if(fabs(curPos.y-nextPos.y)>fMoveFall)
				{
					if(nextPos.y>curPos.y)
						return ERet_Blocking;
					else
						return ERet_WillDrop;
				}

				//--检查落脚点是否能通过
				if(bHaveModel)
				{
					box.min.y=nextPos.y+10.0f;
					box.max.y=box.min.y+roleSize.y-10.0f;
					if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
						return ERet_Blocking;
				}

			}

			//--放入一个路径点
			totalDist+=Vec3Dist(curPos,nextPos);
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=totalDist;
			path.push_back(newPoint);

			if(bPassCheck)
			{
				//--判断是否入水
				if(bInWaterCheck)
				{
					float waterH=0;
					if(GetWaterHeight(nextPos,waterH) && nextPos.y<=waterH)
					{
						path.back().pos.y=waterH;
						return ERet_WillInWater;
					}
				}
				//--判断是否会游泳
				float swimH=0;
				if(IfWillSwim(nextPos,roleSize.y,swimH))
				{
					path.back().pos.y=swimH;
					return ERet_WillSwim;
				}

				//--判断滑落
				if(bOnGround
					&&IfWillSlide(curPos.x,curPos.z))
					return ERet_WillSlide;
			}

			//--
			if(bArrive)
				return ERet_Arrive;

			f+=g_collideStep;
			curPos=nextPos;
		}

		return ERet_TooLong;
	}

	void NavCollider::StepMoveInit( const Vector3& start,const Vector3& end,tagStepMoveData& data )
	{
		//xz dir
		data.xzDir=end-start;
		data.xzDir.y=0;

		//xz total dist
		data.xzTotalDist=Vec3Len(&data.xzDir);

		if(data.xzTotalDist>0)
			data.xzDir/=data.xzTotalDist;

		//xz current dist
		data.xzCurDist=0;

		//current pos
		data.curPos=start;

		//current dist
		data.curDist=0;
	}

	NavCollider::EReturn NavCollider::StepMove( const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepMoveData& data,list<tagMovePoint>& path,bool bPassCheck,bool bInWaterCheck/*=false*/ )
	{
		//--如果起点和终点相同，返回已到达
		if(data.xzTotalDist==0)
			return ERet_Arrive;
			
		Vector3 halfSize=roleSize*0.5f;

		bool bArrive=false;
		float f=0;
		while(f<maxMoveDist)
		{
			Vector3 nextPos=data.curPos+data.xzDir*g_collideStep;
			if(data.xzCurDist+g_collideStep>=data.xzTotalDist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}
			data.xzCurDist+=g_collideStep;

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--快速判断角色所在区域是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);


			//--找出落脚点
			bool bOnGround=false;
			if(bHaveModel)
			{
				box.max.y=nextPos.y+g_moveFall;
				box.min.y=-g_floatMax;
				if(!AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
					return ERet_Blocking;
			}
			else
			{
				bOnGround=true;
				nextPos.y=groundH;
			}

			if(bPassCheck)
			{
				//--判断落差
				if(fabs(data.curPos.y-nextPos.y)>g_moveFall)
				{
					if(nextPos.y>data.curPos.y)
						return ERet_Blocking;
					else
						return ERet_WillDrop;
				}

				//--检查落脚点是否能通过
				if(bHaveModel)
				{
					box.min.y=nextPos.y+10.0f;
					box.max.y=box.min.y+roleSize.y-10.0f;
					if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
						return ERet_Blocking;
				}
			}

			//--放入一个路径点
			float dist=Vec3Dist(data.curPos,nextPos);
			data.curDist+=dist;
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=data.curDist;
			path.push_back(newPoint);

			if(bPassCheck)
			{
				//--判断是否入水
				if(bInWaterCheck)
				{
					float waterH=0;
					if(GetWaterHeight(nextPos,waterH) && nextPos.y<=waterH)
					{
						path.back().pos.y=waterH;
						return ERet_WillInWater;
					}
				}
				//--判断是否会游泳
				float swimH=0;
				if(IfWillSwim(nextPos,roleSize.y,swimH))
				{
					path.back().pos.y=swimH;
					return ERet_WillSwim;
				}

				//--判断滑落
				if(bOnGround
					&&IfWillSlide(data.curPos.x,data.curPos.z))
					return ERet_WillSlide;
			}

			//--
			if(bArrive)
				return ERet_Arrive;

			f+=dist;
			data.curPos=nextPos;
		}

		return ERet_ToBeContinued;
	}

	inline float _CalcDropCollideTime(float startH,float intersectH)
	{
		float time=0;
		if(startH>intersectH)
			time=sqrtf((startH-intersectH)/g_halfGravitation);
		return time;
	}

	NavCollider::EReturn NavCollider::Drop( const Vector3& start,const Vector3& dir,float xzSpeed,const Vector3& roleSize,float& totalTime,bool bInWaterCheck/*=false*/)
	{
		Vector3 halfSize=roleSize*0.5f;

		Vector3 xzDir=dir;
		xzDir.y=0;
		Vec3Normalize(xzDir);

		Vector3 oldPos=start;
		float collideInterval=g_collideStep/xzSpeed;
		for(float t=collideInterval;t<g_collideTimeOut;t+=collideInterval)
		{
			Vector3 curPos=start+xzDir*(xzSpeed*t);
			curPos.y=start.y-(g_halfGravitation*t*t);//1/2gt^2

			//--检查可行走区域
			if(!IfCanGo(curPos.x,curPos.z))
			{
				totalTime=t-collideInterval;
				return ERet_WillVDrop;
			}

			//--生成角色AABB
			AABBox box;
			box.min.x=curPos.x-halfSize.x;
			box.max.x=curPos.x+halfSize.x;
			box.min.z=curPos.z-halfSize.z;
			box.max.z=curPos.z+halfSize.z;

			//--快速判断当前位置是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(curPos.x,curPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			if(bHaveModel)
			{
				//--测试是否有碰撞
				box.min.y=curPos.y;
				box.max.y=curPos.y+roleSize.y;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
				{
					//--判断是否碰撞到可站立的地方
					float intersectH=0;
					bool bOnGround=false;
					if(AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,intersectH,&bOnGround,0,NULL))
					{
						//是否可通过?
						box.min.y=intersectH+10.0f;
						box.max.y=intersectH+roleSize.y-10.0f;
						if(!AABBoxCollideBoxAndTrn(box,0.0f,groundH)
							&& intersectH<start.y)
						{
							curPos.y=intersectH;

							//--判断是否入水
							if(bInWaterCheck)
							{
								float waterH=0;
								if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
								{
									totalTime=_CalcDropCollideTime(start.y,waterH);
									return ERet_WillInWater;
								}
							}
							//判断是否会进入游泳状态
							if(IfWillSwim(curPos,roleSize.y,curPos.y))
							{
								//计算到达碰撞点的时间
								totalTime=_CalcDropCollideTime(start.y,curPos.y);
								return ERet_WillSwim;
							}

							//计算到达碰撞点的时间
							totalTime=sqrtf((start.y-intersectH)/g_halfGravitation);

							//判断滑落
							if(bOnGround
								&&IfWillSlide(curPos.x,curPos.z))
							{
								return ERet_WillSlide;
							}
							else
							{
								return ERet_Standup;
							}
						}
					}
					totalTime=t-collideInterval;
					return ERet_WillVDrop;
				}
				else
				{
					//--判断是否入水
					if(bInWaterCheck)
					{
						float waterH=0;
						if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
						{
							totalTime=_CalcDropCollideTime(start.y,waterH);
							return ERet_WillInWater;
						}
					}
					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						//计算到达碰撞点的时间
						totalTime=_CalcDropCollideTime(start.y,curPos.y);
						return ERet_WillSwim;
					}
				}
			}
			else
			{
				//--测试是否有碰撞
				if(curPos.y<=groundH)
				{
					if(groundH<start.y)
					{
						curPos.y=groundH;

						//--判断是否入水
						if(bInWaterCheck)
						{
							float waterH=0;
							if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
							{
								totalTime=_CalcDropCollideTime(start.y,waterH);
								return ERet_WillInWater;
							}
						}
						//判断是否会进入游泳状态
						if(IfWillSwim(curPos,roleSize.y,curPos.y))
						{
							//计算到达碰撞点的时间
							totalTime=_CalcDropCollideTime(start.y,curPos.y);
							return ERet_WillSwim;
						}

						//--计算到达碰撞点的时间
						totalTime=sqrtf((start.y-groundH)/g_halfGravitation);

						//--判断滑落
						if(IfWillSlide(curPos.x,curPos.z))
						{
							return ERet_WillSlide;
						}
						else
						{
							return ERet_Standup;
						}
					}
					totalTime=t-collideInterval;
					return ERet_WillVDrop;
				}
				else
				{
					//--判断是否入水
					if(bInWaterCheck)
					{
						float waterH=0;
						if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
						{
							totalTime=_CalcDropCollideTime(start.y,waterH);
							return ERet_WillInWater;
						}
					}
					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						//计算到达碰撞点的时间
						totalTime=_CalcDropCollideTime(start.y,curPos.y);
						return ERet_WillSwim;
					}
				}
			}

			//--调整collideInterval
			float dropSpeed=Vec3Dist(oldPos,curPos)/collideInterval;
			collideInterval=g_collideStep/dropSpeed;

			oldPos=curPos;
		}

		ASSERT(0 && "infinite drop!");
		return ERet_Infinite;
	}

	void NavCollider::CalcDropPos( const Vector3& start,const Vector3& dir,float xzSpeed,float curTime,float totalTime,Vector3& out )
	{
		if(curTime>totalTime)
			curTime=totalTime;

		Vector3 xzDir=dir;
		xzDir.y=0;
		Vec3Normalize(xzDir);

		out=start+xzDir*(xzSpeed*curTime);
		out.y=start.y-(g_halfGravitation*curTime*curTime);//1/2gt^2
	}

	void NavCollider::CalcMovePos( const Vector3& start,list<tagMovePoint>& path,float factor,Vector3& out )
	{
		if(path.size()==0)
			return;
		if(factor<0)
			factor=0;
		if(factor>1)
			factor=1;

		float dist=path.back().dist*factor;

		Vector3 prevPos=start;
		float preDist=0;
		list<tagMovePoint>::iterator iterLast=path.end();
		iterLast--;
		for(list<tagMovePoint>::iterator iter=path.begin();
			iter!=path.end();++iter)
		{
			if(iter==iterLast
				||dist<=iter->dist)
			{
				float r=(dist-preDist)/(iter->dist-preDist);
				if(r>1)r=1;
				out.x=LinearLerp(prevPos.x,iter->pos.x,r);
				out.y=LinearLerp(prevPos.y,iter->pos.y,r);
				out.z=LinearLerp(prevPos.z,iter->pos.z,r);
				return;
			}

			prevPos=iter->pos;
			preDist=iter->dist;
		}
	}

	NavCollider::EReturn NavCollider::VDrop( const Vector3& start,const Vector3& roleSize,float& totalTime,bool bOnlyTrn,bool bInWaterCheck/*=false*/)
	{
		Vector3 halfSize=roleSize*0.5f;

		Vector3 curPos=start;
		float collideInterval=sqrtf(g_collideStep/g_halfGravitation);
		for(float t=collideInterval;t<g_collideTimeOut;t+=collideInterval)
		{
			curPos.y=start.y-(g_halfGravitation*t*t);//1/2gt^2

			//--计算角色AABB
			AABBox box;
			box.min.x=curPos.x-halfSize.x;
			box.max.x=curPos.x+halfSize.x;
			box.min.z=curPos.z-halfSize.z;
			box.max.z=curPos.z+halfSize.z;

			//--快速判断当前位置是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(curPos.x,curPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			//--测试是否有碰撞
			if(bHaveModel
				&&!bOnlyTrn)
			{
				box.min.y=curPos.y;
				box.max.y=curPos.y+roleSize.y;
				float intersectH;
				bool bOnGround=false;
				if(AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,intersectH,&bOnGround,0,NULL))
				{
					curPos.y=intersectH;

					//--判断是否入水
					if(bInWaterCheck)
					{
						float waterH=0;
						if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
						{
							totalTime=_CalcDropCollideTime(start.y,waterH);
							return ERet_WillInWater;
						}
					}
					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						//计算到达碰撞点的时间
						totalTime=_CalcDropCollideTime(start.y,curPos.y);
						return ERet_WillSwim;
					}

					if(intersectH<start.y)
					{
						//--计算到达碰撞点的时间
						totalTime=sqrtf((start.y-intersectH)/g_halfGravitation);

						//--判断滑落
						if(bOnGround
							&&IfWillSlide(curPos.x,curPos.z))
						{
							return ERet_WillSlide;
						}
						else
						{
							return ERet_Standup;
						}
					}
					else
					{
						totalTime=0;
						return ERet_Standup;
					}
				}
				else
				{
					//--判断是否入水
					if(bInWaterCheck)
					{
						float waterH=0;
						if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
						{
							totalTime=_CalcDropCollideTime(start.y,waterH);
							return ERet_WillInWater;
						}
					}
					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						//计算到达碰撞点的时间
						totalTime=_CalcDropCollideTime(start.y,curPos.y);
						return ERet_WillSwim;
					}
				}
			}
			else
			{
				if(groundH<start.y)
				{
					curPos.y=groundH;

					//--判断是否入水
					if(bInWaterCheck)
					{
						float waterH=0;
						if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH && waterH<start.y)
						{
							totalTime=_CalcDropCollideTime(start.y,waterH);
							return ERet_WillInWater;
						}
					}
					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						//计算到达碰撞点的时间
						totalTime=_CalcDropCollideTime(start.y,curPos.y);
						return ERet_WillSwim;
					}

					//--计算到达碰撞点的时间
					totalTime=sqrtf((start.y-groundH)/g_halfGravitation);

					//--判断滑落
					if(IfWillSlide(curPos.x,curPos.z))
					{
						return ERet_WillSlide;
					}
					else
					{
						return ERet_Standup;
					}
				}
				else
				{
					totalTime=0;

					//判断是否会进入游泳状态
					if(IfWillSwim(curPos,roleSize.y,curPos.y))
					{
						return ERet_WillSwim;
					}

					return ERet_Standup;
				}
			}

			//--调整collideInterval
			float dropSpeed=(start.y-curPos.y)/t;
			collideInterval=g_collideStep/dropSpeed;
		}

		ASSERT(0 && "infinite vdrop!");
		return ERet_Infinite;
	}

	void NavCollider::CalcVDropPos( const Vector3& start,float curTime,float totalTime,Vector3& out )
	{
		if(curTime>totalTime)
			curTime=totalTime;

		out=start;
		out.y=start.y-(g_halfGravitation*curTime*curTime);//1/2gt^2
	}

	inline bool _CalcJumpCollideTime(float ySpeed,float startH,float intersectH,float& time)
	{
		float a=g_halfGravitation;
		float b=-ySpeed;
		float c=intersectH-startH;
		float d=b*b-4.0f*a*c;
		if(d>0)
		{
			time=(-b+sqrtf(d))/(2.0f*a);
			return true;
		}
		return false;
	}

	NavCollider::EReturn NavCollider::Jump( const Vector3& start,const Vector3& dir,float xzSpeed,float ySpeed,const Vector3& roleSize,float& totalTime,bool bOnlyTrn,BitMap* pSpecCangoMap,bool bInWaterCheck/*=false*/ )
	{
		Vector3 halfSize=roleSize*0.5f;

		Vector3 xzDir=dir;
		xzDir.y=0;
		Vec3Normalize(xzDir);

		Vector3 oldPos=start;
		float collideInterval=g_collideStep/ySpeed;
		for(float t=collideInterval;t<g_collideTimeOut;t+=collideInterval)
		{
			Vector3 curPos=start+xzDir*(xzSpeed*t);
			curPos.y=start.y+(ySpeed*t-g_halfGravitation*t*t);//v0*t-0.5*g*t^2;

			//--检查可行走区域
			if(!IfCanGo(curPos.x,curPos.z,pSpecCangoMap))
			{
				totalTime=t-collideInterval;
				return ERet_WillVDrop;
			}

			//--计算角色AABB
			AABBox box;
			box.min.x=curPos.x-halfSize.x;
			box.max.x=curPos.x+halfSize.x;
			box.min.z=curPos.z-halfSize.z;
			box.max.z=curPos.z+halfSize.z;
			box.min.y=curPos.y;
			box.max.y=curPos.y+roleSize.y;

			//--快速判断当前位置是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(curPos.x,curPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			if(ySpeed>g_gravitation*t)//上升阶段
			{
				if(bHaveModel
					&&!bOnlyTrn)
				{
					if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
					{
						totalTime=t-collideInterval;
						if(totalTime>0)
							return ERet_WillVDrop;
						else
							return ERet_CannotJump;
					}
				}
				else
				{
					if(curPos.y<=groundH)
					{
						totalTime=t-collideInterval;
						if(totalTime>0)
							return ERet_WillVDrop;
						else
							return ERet_CannotJump;
					}
				}
			}
			else//下降阶段
			{
				if(bHaveModel
					&&!bOnlyTrn)
				{
					//--测试是否碰撞
					if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
					{
						//--测试是否碰撞到可站立的地方
						float intersectH;
						bool bOnGround=false;
						if(AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,intersectH,&bOnGround,0,NULL))
						{
							//是否可通过?
							box.min.y=intersectH+10.0f;
							box.max.y=intersectH+roleSize.y-10.0f;
							if(!AABBoxCollideBoxAndTrn(box,0.0f,groundH))
							{
								//判断是否会进入游泳状态
								curPos.y=intersectH;

								//--判断是否入水
								if(bInWaterCheck)
								{
									float waterH=0;
									if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH)
									{
										if(!_CalcJumpCollideTime(ySpeed,start.y,waterH,totalTime))
											totalTime=t;
										return ERet_WillInWater;
									}
								}
								if(IfWillSwim(curPos,roleSize.y,curPos.y))
								{
									//计算到达游泳脚底位置的时间
									if(!_CalcJumpCollideTime(ySpeed,start.y,curPos.y,totalTime))
										totalTime=t;
									return ERet_WillSwim;
								}

								//计算到达碰撞点的时间
								if(_CalcJumpCollideTime(ySpeed,start.y,intersectH,totalTime))
								{
									if(bOnGround)
									{
										//判断滑落
										if(IfWillSlide(curPos.x,curPos.z))
											return ERet_WillSlide;
										else
											return ERet_Standup;
									}
									else
									{
										return ERet_Standup;
									}
								}
							}
						}

						totalTime=t-collideInterval;
						if(totalTime>0)
							return ERet_WillVDrop;
						else
							return ERet_CannotJump;
					}
					else
					{
						//--判断是否入水
						if(bInWaterCheck)
						{
							float waterH=0;
							if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH)
							{
								if(!_CalcJumpCollideTime(ySpeed,start.y,waterH,totalTime))
									totalTime=t;
								return ERet_WillInWater;
							}
						}
						//判断是否会进入游泳状态
						if(IfWillSwim(curPos,roleSize.y,curPos.y))
						{
							//计算到达游泳脚底位置的时间
							if(!_CalcJumpCollideTime(ySpeed,start.y,curPos.y,totalTime))
								totalTime=t;
							return ERet_WillSwim;
						}
					}
				}
				else
				{
					if(curPos.y<=groundH)//与地面碰撞?
					{
						curPos.y=groundH;

						//--判断是否入水
						if(bInWaterCheck)
						{
							float waterH=0;
							if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH)
							{
								if(!_CalcJumpCollideTime(ySpeed,start.y,waterH,totalTime))
									totalTime=t;
								return ERet_WillInWater;
							}
						}
						//判断是否会进入游泳状态
						if(IfWillSwim(curPos,roleSize.y,curPos.y))
						{
							//计算到达游泳脚底位置的时间
							if(!_CalcJumpCollideTime(ySpeed,start.y,curPos.y,totalTime))
								totalTime=t;
							return ERet_WillSwim;
						}

						//计算到达碰撞点的时间
						if(_CalcJumpCollideTime(ySpeed,start.y,groundH,totalTime))
						{
							//判断滑落
							if(IfWillSlide(curPos.x,curPos.z))
								return ERet_WillSlide;
							else
								return ERet_Standup;
						}
						else
						{
							totalTime=t-collideInterval;
							if(totalTime>0)
								return ERet_WillVDrop;
							else
								return ERet_CannotJump;
						}
					}
					else
					{
						//--判断是否入水
						if(bInWaterCheck)
						{
							float waterH=0;
							if(GetWaterHeight(curPos,waterH) && curPos.y<=waterH)
							{
								if(!_CalcJumpCollideTime(ySpeed,start.y,waterH,totalTime))
									totalTime=t;
								return ERet_WillInWater;
							}
						}
						//判断是否会进入游泳状态
						if(IfWillSwim(curPos,roleSize.y,curPos.y))
						{
							//计算到达游泳脚底位置的时间
							if(!_CalcJumpCollideTime(ySpeed,start.y,curPos.y,totalTime))
								totalTime=t;
							return ERet_WillSwim;
						}
					}
				}
			}//end of if(ySpeed>g_gravitation*t)

			//调整collideInterval
			float dropSpeed=Vec3Dist(oldPos,curPos)/collideInterval;
			collideInterval=g_collideStep/dropSpeed;

			oldPos=curPos;
		}

		ASSERT(0 && "infinite jump!");
		return ERet_Infinite;
	}

	void NavCollider::CalcJumpPos( const Vector3& start,const Vector3& dir,float xzSpeed,float ySpeed,float curTime,float totalTime,Vector3& out )
	{
		if(curTime>totalTime)
			curTime=totalTime;

		Vector3 xzDir=dir;
		xzDir.y=0;
		Vec3Normalize(xzDir);

		out=start+xzDir*(xzSpeed*curTime);
		out.y=start.y+(ySpeed*curTime-g_halfGravitation*curTime*curTime);//v0t-1/2gt^2
	}

	bool NavCollider::IfWillSlide( float x,float z )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->IfWillSlide(x,z,g_slideFall);
	}

	bool NavCollider::IfWillSlide( const Vector3& pos,const Vector3& roleSize )
	{
		ASSERT(m_pHMap->IsCreated());

		Vector3 halfSize=roleSize*0.5f;

		AABBox box;
		box.min.x=pos.x-halfSize.x;
		box.max.x=pos.x+halfSize.x;
		box.min.z=pos.z-halfSize.z;
		box.max.z=pos.z+halfSize.z;
		box.min.y=pos.y-10.0f;
		box.max.y=pos.y+roleSize.y;

		float intersectH;
		bool bOnGround=false;
		if(!AABBoxCollideBoxTopAndTrn(box,0.0f,intersectH,&bOnGround,NavSceneNode::EFlag_NotCarrier,NULL)
			||!bOnGround)
			return false;

		return m_pHMap->GetHMap()->IfWillSlide(pos.x,pos.z,g_slideFall);
	}

	bool NavCollider::SafeCalcSlideDir( float x,float z,Vector3& out )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeCalcSlideDir(x,z,out);
	}

	float NavCollider::SafeGetGroundHeight(float x,float z)
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeight(x,z);
	}

	float NavCollider::SafeGetGroundHeight( float x1,float z1,float x2,float z2 )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeight(x1,z1,x2,z2);
	}

	float NavCollider::SafeGetGroundHeightSlerp(float x,float z)
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeightSlerp(x,z);
	}

	float NavCollider::SafeGetGroundHeightSlerp( float x1,float z1,float x2,float z2 )
	{
		ASSERT(m_pHMap->IsCreated());
		return m_pHMap->GetHMap()->SafeGetGroundHeightSlerp(x1,z1,x2,z2);
	}

	inline POINT _World2Tile(Vector3& pos)
	{
		POINT tile;
		tile.x=long(pos.x/g_tileSize);
		tile.y=long(pos.z/g_tileSize);
		return tile;
	}

	inline POINT _World2Tile(float x,float z)
	{
		POINT tile;
		tile.x=long(x/g_tileSize);
		tile.y=long(z/g_tileSize);
		return tile;
	}

	NavCollider::EReturn NavCollider::Slide( const Vector3& start,const Vector3& roleSize,list<tagMovePoint>& path,bool bInWaterCheck/*=false*/ )
	{
		Vector3 halfSize=roleSize*0.5f;

		path.clear();

		Vector3 slideDir;
		if(!SafeCalcSlideDir(start.x,start.z,slideDir))
			return ERet_Blocking;

		Vector3 curPos=start;
		POINT curTile=_World2Tile(curPos);
		float f=0;
		float totalDist=0;
		for(int i=0;i<100;i++)
		{
			Vector3 nextPos=curPos+slideDir*g_collideStep;

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--计算角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;
			box.max.y=nextPos.y+roleSize.y;
			box.min.y=-g_floatMax;

			//--快速判断当前位置是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得当前位置地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			bool bOnGround=false;
			POINT nextTile;
			if(bHaveModel)
			{
				//--找出落脚点
				if(!AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
					return ERet_Blocking;

				//--滑到模型上?
				if(!bOnGround)
					return ERet_Blocking;

				//--检查落脚点是否能通过
				box.min.y=nextPos.y+10.0f;
				box.max.y=box.min.y+roleSize.y-10.0f;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))//不可通过?
					return ERet_Blocking;

				//--判断落差
				nextTile=_World2Tile(nextPos);
				if(nextTile.x!=curTile.x
					||nextTile.y!=curTile.y)
				{
					if(curPos.y-nextPos.y<g_slideFall)
					{
						//--判断是否能朝其它方向滑
						if(IfWillSlide(nextPos.x,nextPos.z))
						{
							if(!SafeCalcSlideDir(nextPos.x,nextPos.z,slideDir))
								return ERet_Blocking;
						}
						else
							return ERet_Blocking;
					}
				}
			}
			else
			{
				nextPos.y=groundH;
				bOnGround=true;

				//--判断落差
				nextTile=_World2Tile(nextPos);
				if(nextTile.x!=curTile.x
					||nextTile.y!=curTile.y)
				{
					if(curPos.y-nextPos.y<g_slideFall)
					{
						//--判断是否能朝其它方向滑
						if(IfWillSlide(nextPos.x,nextPos.z))
						{
							if(!SafeCalcSlideDir(nextPos.x,nextPos.z,slideDir))
								return ERet_Blocking;
						}
						else
							return ERet_Blocking;
					}
				}
			}

			//--放入一个路径点
			totalDist+=Vec3Dist(curPos,nextPos);
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=totalDist;
			path.push_back(newPoint);

			//--判断是否入水
			if(bInWaterCheck)
			{
				float waterH=0;
				if(GetWaterHeight(nextPos,waterH) && nextPos.y<=waterH)
				{
					path.back().pos.y=waterH;
					return ERet_WillInWater;
				}
			}

			//--判断是否会游泳
			float swimH=0;
			if(IfWillSwim(nextPos,roleSize.y,swimH))
			{
				path.back().pos.y=swimH;
				return ERet_WillSwim;
			}

			//--
			f+=g_collideStep;
			curPos=nextPos;
			curTile=nextTile;
		}

		return ERet_TooLong;
	}

	bool NavCollider::HaveModel( float x,float z )
	{
		ASSERT(m_pModelMap->IsCreated());

		int tx=int(x/g_tileSize);
		int tz=int(z/g_tileSize);
		if(tx<0)return false;
		if(tz<0)return false;
		if(tx>=(int)m_pModelMap->GetBitMap()->Width())return false;
		if(tz>=(int)m_pModelMap->GetBitMap()->Height())return false;
		return m_pModelMap->GetBitMap()->GetValue(tx,tz);
	}

	bool NavCollider::HaveModel( float x1,float z1,float x2,float z2 )
	{
		ASSERT(m_pModelMap->IsCreated());

		int tx1=int(x1/g_tileSize);
		int tz1=int(z1/g_tileSize);
		int tx2=int(x2/g_tileSize);
		int tz2=int(z2/g_tileSize);
		if(tx1<0)return false;
		if(tz1<0)return false;
		if(tx2>=(int)m_pModelMap->GetBitMap()->Width())return false;
		if(tz2>=(int)m_pModelMap->GetBitMap()->Height())return false;
		int x,z;
		for(z=tz1;z<=tz2;z++)
		{
			for(x=tx1;x<=tx2;x++)
			{
				if(m_pModelMap->GetBitMap()->GetValue(x,z))
					return true;
			}
		}
		return false;
	}

	float NavCollider::CalcSlideFactor( float totalDist,float curTime )
	{
		float curDist=g_halfGravitation*curTime*curTime;
		float factor=curDist/totalDist;
		if(factor>1)factor=1;
		return factor;
	}

	bool NavCollider::NPCMove( const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bHeightCheck,bool bPassCheck )
	{
		Vector3 halfSize=roleSize*0.5f;
		path.clear();

		Vector3 dir=end-start;
		float dist=Vec3Len(&dir);
		if(dist==0)
			return true;
		dir/=dist;

		Vector3 curPos=start;
		float f=0;
		float totalDist=0;
		bool bArrive=false;
		for(int i=0;i<maxCollideTimes;i++)
		{
			Vector3 nextPos=curPos+dir*g_collideStep;
			if(f+g_collideStep>=dist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return false;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeight(box.min.x,box.min.z,box.max.x,box.max.z);

			//--找出落脚点
			box.max.y=nextPos.y+g_moveFall;
			box.min.y=-g_floatMax;
			bool bOnGround;
			if(!AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
				return false;

			if(bHeightCheck)
			{
				//--判断落差
				if(fabs(curPos.y-nextPos.y)>g_moveFall)
					return false;
			}

			if(bPassCheck)
			{
				//--检查落脚点是否能通过
				box.min.y=nextPos.y+10.0f;
				box.max.y=box.min.y+roleSize.y-10.0f;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
					return false;
			}

			//--放入一个路径点
			totalDist+=Vec3Dist(curPos,nextPos);
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=totalDist;
			path.push_back(newPoint);

			//--
			if(bArrive)
				return true;

			f+=g_collideStep;
			curPos=nextPos;
		}

		return false;		
	}

	void NavCollider::NPCStepMoveInit( const Vector3& start,const Vector3& end,tagStepMoveData& data )
	{
		//line dir
		data.xzDir=end-start;
		
		//line total distance
		data.xzTotalDist=Vec3Len(&data.xzDir);

		if(data.xzTotalDist>0)
			data.xzDir/=data.xzTotalDist;

		//line current distance
		data.xzCurDist=0;

		//current pos
		data.curPos=start;

		//current dist
		data.curDist=0;
	}

	NavCollider::EReturn NavCollider::NPCStepMove( const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepMoveData& data,list<tagMovePoint>& path,bool bHeightCheck,bool bPassCheck )
	{
		Vector3 halfSize=roleSize*0.5f;

		bool bArrive=false;
		float f=0;
		while(f<maxMoveDist)
		{
			Vector3 nextPos=data.curPos+data.xzDir*g_collideStep;
			if(data.xzCurDist+g_collideStep>=data.xzTotalDist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}
			data.xzCurDist+=g_collideStep;

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeight(box.min.x,box.min.z,box.max.x,box.max.z);

			//--找出落脚点
			box.max.y=nextPos.y+g_moveFall;
			box.min.y=-g_floatMax;
			bool bOnGround;
			if(!AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
				return ERet_Blocking;

			if(bHeightCheck)
			{
				//--判断落差
				if(fabs(data.curPos.y-nextPos.y)>g_moveFall)
					return ERet_Blocking;
			}

			if(bPassCheck)
			{
				//--检查落脚点是否能通过
				box.min.y=nextPos.y+10.0f;
				box.max.y=box.min.y+roleSize.y-10.0f;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
					return ERet_Blocking;
			}

			//--放入一个路径点
			float dist=Vec3Dist(data.curPos,nextPos);
			data.curDist+=dist;
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=data.curDist;
			path.push_back(newPoint);

			//--
			if(bArrive)
				return ERet_Arrive;

			f+=dist;
			data.curPos=nextPos;
		}

		return ERet_ToBeContinued;		
	}

	bool NavCollider::IfCanFindPath( const Vector3& pos,const Vector3& roleSize )
	{
		Vector3 halfSize=roleSize*0.5f;

		AABBox box;
		box.min.x=pos.x-halfSize.x;
		box.max.x=pos.x+halfSize.x;
		box.min.z=pos.z-halfSize.z;
		box.max.z=pos.z+halfSize.z;
		box.min.y=pos.y-50.0f;
		box.max.y=pos.y+roleSize.y;

		float intersectH;
		return AABBoxCollideBoxTopAndTrn(box,0.0f,intersectH,NULL,NavSceneNode::EFlag_SupportPlayerPathFind,NULL);
	}

	bool NavCollider::IfCanGo( float x,float z,BitMap* pSpecCangoMap )
	{
		ASSERT(m_pCangoMap->IsCreated());

		if(pSpecCangoMap!=NULL)
			return pSpecCangoMap->SafeGetValue(int(x/g_tileSize),int(z/g_tileSize));
		else
			return m_pCangoMap->GetBitMap()->SafeGetValue(int(x/g_tileSize),int(z/g_tileSize));
	}

	bool NavCollider::IfWillSwim( const Vector3& curPos,float roleHeight,float& footH )
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		//--检查当前是否为水面区域
		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return false;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		float waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		//--
		float underWaterH=waterH-curPos.y;
		if(underWaterH/roleHeight<g_swimFloodRate)
			return false;

		footH=waterH-roleHeight*g_swimFloodRate;
		return true;
	}

	bool NavCollider::IfWillSwim( const Vector3& curPos,float roleHeight )
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		//--检查当前是否为水面区域
		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return false;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		float waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		//--
		float underWaterH=waterH-curPos.y;
		if(underWaterH/roleHeight<g_swimFloodRate)
			return false;

		return true;
	}

	bool NavCollider::IfWillOnWater(const Vector3& curPos)
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return false;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		float waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		//--
		return curPos.y<=waterH;
	}

	bool NavCollider::IfWillOnWater(const Vector3& curPos,float& waterH)
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return false;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		//--
		return curPos.y<=waterH;
	}

	float NavCollider::CalcSwimFootHeight( const Vector3& curPos,float roleHeight,float defaultHeight )
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		float footH=defaultHeight;

		//--检查当前是否为水面区域
		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return footH;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		float waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		footH=waterH-roleHeight*g_swimFloodRate;
		return footH;
	}

	float NavCollider::GetWaterHeightEx( const Vector3& curPos,float defaultHeight )
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		float waterH=defaultHeight;

		//--检查当前是否为水面区域
		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return waterH;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		waterH=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		return waterH;
	}

	bool NavCollider::GetWaterHeight( const Vector3& curPos,float& out )
	{
		ASSERT(m_pWaterArea->IsCreated());
		ASSERT(m_pWaterHeight->IsCreated());

		//--检查当前是否为水面区域
		int tileX=int(curPos.x/g_tileSize);
		int tileZ=int(curPos.z/g_tileSize);
		if(!m_pWaterArea->GetBitMap()->SafeGetValue(tileX,tileZ))
			return false;

		//--得到水面高度
		int patchX=tileX/g_riverPatchSize;
		int patchZ=tileZ/g_riverPatchSize;
		out=m_pWaterHeight->GetHMap()->GetSafeHeight(patchX,patchZ);

		return true;
	}

	bool NavCollider::RayCollideWater( const Ray& ray,Vector3& intersectPos )
	{
		const float step=10.0f;

		Vector3 linePt;
		float waterH;
		for(float len=0;len<ray.length;len+=step)
		{
			linePt=ray.dir*len+ray.origin;
			if(GetWaterHeight(linePt,waterH)
				&&linePt.y<=waterH)
			{
				intersectPos=linePt;
				intersectPos.y=waterH;
				return true;
			}
		}

		return false;
	}

	NavCollider::EReturn NavCollider::Swim( const Vector3& start,const Vector3& end,const Vector3& roleSize,int maxCollideTimes,list<tagMovePoint>& path,bool bPassCheck/*=true*/,bool bWaterWalk/*=false*/ )
	{
		Vector3 halfSize=roleSize*0.5f;
		path.clear();

		Vector3 dir=end-start;
		dir.y=0;
		float dist=Vec3Len(&dir);
		if(dist==0)
			return ERet_Arrive;
		dir/=dist;

		Vector3 curPos=start;

		float f=0;
		float totalDist=0;
		bool bArrive=false;
		for(int i=0;i<maxCollideTimes;i++)
		{
			Vector3 nextPos=curPos+dir*g_collideStep;

			if( !bWaterWalk )
			{
				//--计算游泳时脚底的高度
				nextPos.y=CalcSwimFootHeight(nextPos,roleSize.y,nextPos.y);
			}
			else
			{
				//--获得水面高度
				nextPos.y=GetWaterHeightEx(nextPos,nextPos.y);
			}

			if(f+g_collideStep>=dist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--快速判断角色所在区域是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			if(bPassCheck)
			{
				//--检查是否碰到障碍物
				box.min.y=nextPos.y;
				box.max.y=box.min.y+roleSize.y;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
				{
					//--检查是否碰到地面或盒子顶部
					box.max.y=nextPos.y+g_moveFall;
					box.min.y=nextPos.y;
					bool bOnGround=false;
					if(AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
					{
						//--判断落差
						if(nextPos.y-curPos.y>g_moveFall)
							return ERet_Blocking;

						//--检查落脚点是否可通过
						box.min.y=nextPos.y+10.0f;
						box.max.y=box.min.y+roleSize.y-10.0f;
						if(!AABBoxCollideBoxAndTrn(box,0.0f,groundH))
						{
							//--放入路径点
							totalDist+=Vec3Dist(curPos,nextPos);
							tagMovePoint newPoint;
							newPoint.pos=nextPos;
							newPoint.dist=totalDist;
							path.push_back(newPoint);

							return ERet_SwimGround;
						}
					}

					return ERet_Blocking;
				}
			}

			//--放入一个路径点
			totalDist+=Vec3Dist(curPos,nextPos);
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=totalDist;
			path.push_back(newPoint);

			//--
			if(bArrive)
				return ERet_Arrive;

			f+=g_collideStep;
			curPos=nextPos;
		}

		return ERet_TooLong;
	}

	void NavCollider::StepSwimInit( const Vector3& start,const Vector3& end,tagStepSwimData& data )
	{
		//xz dir
		data.xzDir=end-start;
		data.xzDir.y=0;

		//xz total dist
		data.xzTotalDist=Vec3Len(&data.xzDir);

		if(data.xzTotalDist>0)
			data.xzDir/=data.xzTotalDist;

		//xz current dist
		data.xzCurDist=0;

		//current pos
		data.curPos=start;

		//current dist
		data.curDist=0;
	}

	NavCollider::EReturn NavCollider::StepSwim( const Vector3& start,const Vector3& end,const Vector3& roleSize,float maxMoveDist,tagStepSwimData& data,list<tagMovePoint>& path,bool bPassCheck/*=true*/,bool bWaterWalk/*=false*/ )
	{
		//--如果起点和终点相同，返回已到达
		if(data.xzTotalDist==0)
			return ERet_Arrive;

		Vector3 halfSize=roleSize*0.5f;

		bool bArrive=false;
		float f=0;
		while(f<maxMoveDist)
		{
			Vector3 nextPos=data.curPos+data.xzDir*g_collideStep;

			if( !bWaterWalk )
			{
				//--计算游泳时脚底的高度
				nextPos.y=CalcSwimFootHeight(nextPos,roleSize.y,nextPos.y);
			}
			else
			{
				//--获得水面高度
				nextPos.y=GetWaterHeightEx(nextPos,nextPos.y);
			}

			if(data.xzCurDist+g_collideStep>=data.xzTotalDist)
			{
				nextPos.x=end.x;
				nextPos.z=end.z;
				bArrive=true;
			}
			data.xzCurDist+=g_collideStep;

			//--检查可行走区域
			if(!IfCanGo(nextPos.x,nextPos.z))
				return ERet_Blocking;

			//--生成角色AABB
			AABBox box;
			box.min.x=nextPos.x-halfSize.x;
			box.max.x=nextPos.x+halfSize.x;
			box.min.z=nextPos.z-halfSize.z;
			box.max.z=nextPos.z+halfSize.z;

			//--快速判断角色所在区域是否有模型
			bool bHaveModel=HaveModel(box.min.x,box.min.z,box.max.x,box.max.z);

			//--取得地面高度
			//float groundH=SafeGetGroundHeight(nextPos.x,nextPos.z);
			float groundH=SafeGetGroundHeightSlerp(box.min.x,box.min.z,box.max.x,box.max.z);

			if(bPassCheck)
			{
				//--检查是否碰到障碍物
				box.min.y=nextPos.y;
				box.max.y=box.min.y+roleSize.y;
				if(AABBoxCollideBoxAndTrn(box,0.0f,groundH))
				{
					//--检查是否碰到地面或盒子顶部
					box.max.y=nextPos.y+g_moveFall;
					box.min.y=nextPos.y;
					bool bOnGround=false;
					if(AABBoxCollideBoxTopAndTrn(box,0.0f,groundH,nextPos.y,&bOnGround,0,NULL))
					{
						//--检查落脚点是否可通过
						box.min.y=nextPos.y+10.0f;
						box.max.y=box.min.y+roleSize.y-10.0f;
						if(!AABBoxCollideBoxAndTrn(box,0.0f,groundH))
						{
							//--放入路径点
							data.curDist+=Vec3Dist(data.curPos,nextPos);
							tagMovePoint newPoint;
							newPoint.pos=nextPos;
							newPoint.dist=data.curDist;
							path.push_back(newPoint);

							return ERet_SwimGround;
						}
					}

					return ERet_Blocking;
				}
			}

			//--放入一个路径点
			float dist=Vec3Dist(data.curPos,nextPos);
			data.curDist+=dist;
			tagMovePoint newPoint;
			newPoint.pos=nextPos;
			newPoint.dist=data.curDist;
			path.push_back(newPoint);

			//--
			if(bArrive)
				return ERet_Arrive;

			f+=dist;
			data.curPos=nextPos;
		}

		return ERet_ToBeContinued;
	}

	ESurfaceType NavCollider::QuerySurfaceType( const Vector3& pos,float curTime )
	{
		ESurfaceType out;

		//--生成AABB
		AABBox box;
		box.min.x=pos.x-1.0f;
		box.min.y=-g_floatMax;
		box.min.z=pos.z-1.0f;
		box.max.x=pos.x+1.0f;
		box.max.y=pos.y+1.0f;
		box.max.z=pos.z+1.0f;

		//--获取地面高度
		float groundH=SafeGetGroundHeight(box.min.x,box.min.z,box.max.x,box.max.z);

		//--获得地面类型
		ESurfaceType groundType=(ESurfaceType)m_pGroundType->SafeGetValue(int(pos.x/g_tileSize),int(pos.z/g_tileSize));

		//--判断当前位置是否有模型
		if(HaveModel(box.min.x,box.min.z,box.max.x,box.max.z))
		{
			//--获得模型类型
			float intersectH=box.min.y;
			ESurfaceType modelType;
			bool bCollideBox=m_pSG->QuerySurfaceType(box,curTime,intersectH,modelType);

			//--
			if(bCollideBox)
			{
				if(box.min.y<=groundH
					&&groundH>intersectH+100.0f)//因为高度图有时会拉高到模型高度，所以加一点误差判断
				{
					out=groundType;
				}
				else
				{
					out=modelType;
				}
			}
			else
			{
				out=groundType;
			}
		}
		else
		{
			out=groundType;
		}

		return out;
	}

	void NavCollider::EnableAllDoorsCollide( bool bEnable )
	{
		m_pSG->EnableAllDoorsCollide(bEnable);
	}

	void NavCollider::EnableDoorCollide( DWORD dwObjID,bool bEnable )
	{
		m_pSG->EnableDoorCollide(dwObjID,bEnable);
	}

	NavSceneNode* NavCollider::FindDoor( DWORD dwObjID )
	{
		return m_pSG->FindDoor(dwObjID);
	}

	NavCarrierNode* NavCollider::FindCarrier( DWORD dwObjID )
	{
		return m_pSG->FindCarrier(dwObjID);
	}

	bool NavCollider::IfWillVDrop( const Vector3& curPos,const Vector3& roleSize )
	{
		Vector3 halfSize=roleSize*0.5f;

		AABBox box;
		box.min.x=curPos.x-halfSize.x;
		box.max.x=curPos.x+halfSize.x;
		box.min.z=curPos.z-halfSize.z;
		box.max.z=curPos.z+halfSize.z;
		box.min.y=curPos.y-10.0f;
		box.max.y=curPos.y+roleSize.y;

		if(!AABBoxCollideBoxAndTrn(box,0.0f,(DWORD)NavSceneNode::EFlag_NotCarrier,NULL))
			return true;

		return false;
	}
}//namespace WorldBase
