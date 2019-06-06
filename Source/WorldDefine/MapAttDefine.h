#pragma once
#include "base_define.h"

#define TILE_SCALE 50
#define	VIS_DIST 96	// 可视地砖的大小（64个格子）

//-----------------------------------------------------------------------------
// 地图类型
//-----------------------------------------------------------------------------
enum EMapType
{
	EMT_Normal				=	0,		// 普通地图
	EMT_Instance			=	1,		// 普通副本（包括随机副本，奇遇副本）
	EMT_System				=	2,		// 系统创建的稳定副本（包括序章，新手村）
	EMT_PVP					=	3,		// PVP型副本
	EMT_Build				=	4,		// 建筑副本
	EMT_ScriptCreate		=	5,		// 脚本创建的副本
};

//-----------------------------------------------------------------------------
// 地图子类型
//-----------------------------------------------------------------------------
enum ENormalMapType
{
	ENMT_Normal			=	0,		// 野外
	ENMT_City			=	1,		// 城市
	ENMT_Dragon			=	2,		// 地下城
	ENMT_Activity		=	3,		// 活动场景
	ENMT_TrainingField	=	4,		// 练功场
	// 天空城？
};


//-----------------------------------------------------------------------------
// 副本子类型
//-----------------------------------------------------------------------------
enum EInstanceMapType
{
	EIMT_Normal			=	0,		// 普通
	EIMT_Rand			=	1,		// 随机
	EIMT_PVP			=	2,		// PVP
	EIMT_Preface		=	3,		// 序章
	EIMT_Build			=	4,		// 建筑
};

//------------------------------------------------------------------------------
// 副本难度
//------------------------------------------------------------------------------
enum EInstanceHardMode
{
	EIHM_NULL			=  -1,
	EIHM_Normal			=	0,		// 普通
	EIHM_Elite			=	1,		// 精英
	EIHM_Devil			=	2,		// 魔王
};


//------------------------------------------------------------------------------
// 生成内容规则
//------------------------------------------------------------------------------
enum EInstanceCreateMode
{
	EICM_Appoint		=	0,		// 指定
	EICM_AvgLevel		=	1,		// 队伍平均等级
	EICM_LeaderLevel	=	2,		// 队长等级
	EICM_MaxLevel		=	3,		// 小队最高等级
	EICM_MinLevel		=	4,		// 小队最低等级
};


//------------------------------------------------------------------------------
// 出口类型
//------------------------------------------------------------------------------
enum EExportMode
{
	EEM_Born			=	0,		// 出生点
	EEM_Reborn			=	1,		// 复活点
	EEM_Appoint			=	2,		// 指定坐标
	EEM_Current			=	3,		// 原地
};

//------------------------------------------------------------------------------
// 死亡事件
//------------------------------------------------------------------------------
enum ERebornMode
{
	EDE_NULL			=	0,		
	EDE_Leave			=	1,		// 复活且离开副本
	EDE_Current			=	2,		// 自动原地复活
	EDE_Appoint			=	3,		// 自动指定地复活
};

//------------------------------------------------------------------------------
// 完成条件
//------------------------------------------------------------------------------
enum ECompleteConditionNor
{
	// 普通难度
	ECC_NorInsistTime		=	0,		// 坚持时间
	// dwCompleteVal1: 时间
	// dwCompleteVal2: 保留
	ECC_NorRoleNum			=	1,		// 坚持人数
	// dwCompleteVal1: 时间
	// dwCompleteVal2: 人数
	ECC_NorKillAllMon		=	2,		// 清空怪物
	ECC_NorKillBoss			=	3,		// 击杀指定怪物
	// dwCompleteVal1: 怪物ID
	// dwCompleteVal2: 数量
	ECC_NorGetItem			=	4,		// 获得指定物品
	// dwCompleteVal1: 物品ID
	// dwCompleteVal2: 数量
};

enum ECompleteConditionEli
{
	// 精英难度
	ECC_EliteInsistTime		=	0,		// 坚持时间
	ECC_EliteRoleNum		=	1,		// 坚持人数
	ECC_EliteKillAllMon		=	2,		// 清空怪物
	ECC_EliteKillBoss		=	3,		// 击杀指定怪物
	ECC_EliteGetItem		=	4,		// 获得指定物品
};

enum ECompleteConditionDev
{
	// 魔王难度
	ECC_DevilInsistTime		=	0,		// 坚持时间
	ECC_DevilRoleNum		=	1,		// 坚持人数
	ECC_DevilKillAllMon		=	2,		// 清空怪物
	ECC_DevilKillBoss		=	3,		// 击杀指定怪物
	ECC_DevileGetItem		=	4,		// 获得指定物品
};

//------------------------------------------------------------------------------
// 达成条件后事件
//------------------------------------------------------------------------------
enum ECompleteEvent
{
	ECE_NULL				=	0,	
	ECE_LeaveInstance		=	1,		// 离开副本
};

//--Map文件头----------------------------------------------------------------------------
const char* const MapObjFileMagic ="GMB";
struct tagMapHeader
{
	DWORD				dwMagic;
	DWORD				dwVer;

	DWORD				dwGlobalID;			//全局唯一ID
	TCHAR				szSkyBoxTopTex[X_LONG_NAME];		// 天空盒上面纹理
	TCHAR				szSkyBoxFrontRightTex[X_LONG_NAME];	// 天空盒前右纹理
	TCHAR				szSkyBoxBackLeftTex[X_LONG_NAME];	// 天空盒后左纹理
	FLOAT				fSkyBoxXsize;		//天空盒X大小
	FLOAT				fSkyBoxYsize;		//天空盒Y大小
	FLOAT				fSkyBoxZsize;		//天空盒Z大小

	INT					nNumNPC;
	DWORD				dwNpcOffset;

	INT					nNumStaticObj;
	DWORD				dwStaticObjOffset;

	BYTE				byDistFog[16];
	BYTE				bySunLight[104];

	INT					nNumWayPoint;
	DWORD				dwWayPointOffset;

	INT					nNumTrigger;
	DWORD				dwTriggerOffset;

	INT					nNumSound;
	DWORD				dwSoundOffset;

	INT					nNumPointLight;		//点光源个数
	DWORD				dwPointLightOffset;	//点光源在文件中的偏移位置

	FLOAT				fSkyBoxOffX;		//天空盒X偏移
	FLOAT				fSkyBoxOffY;		//天空盒Y偏移
	FLOAT				fSkyBoxOffZ;		//天空盒Z偏移

	INT					nNumMapRect;		//地图区域的个数
	DWORD				dwMapRectOffset;	//地图区域在文中的偏移位置
	
	FLOAT				fSunModulus;		//太阳光增强系数
	FLOAT				fSkyYaw;			//天空盒旋转角色

	INT					nNumPathPoint;		//寻路导航点个数
	DWORD				dwPathPointOffset;	//寻路导航点在文件中的偏移位置

	INT					nNumSpawnPoint;		//刷怪点个数
	DWORD				dwSpawnPointOffset; //刷怪点在文件中的偏移位置

	INT					nNumDynamicBlock;	//动态障碍物个数
	DWORD				dwDynamicBlockOffset;//动态障碍物在文件中的偏移位置

	INT					nNumEventTrigger;	//事件触发器个数
	DWORD				dwEventTriggerOffset;//事件触发器在文件中的偏移位置

	INT					nNumNPCNav;			//npc 格子导航图生成起点个数
	DWORD				dwNPCNavOffset;

	INT					nNumPlayerNav;		//玩家格子导航图生成起点
	DWORD				dwPlayerNavOffset;

	BYTE				bySkyCol[16];

	BYTE				byRenderSkyShade;  //是否显示天空遮罩
	DWORD               dwSkyShadeCol;     //天空遮罩的颜色

	INT					nNumMapDoor;		//门
	DWORD				dwMapDoorOffset;

	INT					nNumMapCarrier;
	DWORD				dwMapCarrierOffset;

	DWORD				dwDynamicDiffCol; // 动态平行光diffuse
	DWORD				dwDynamicAmbCol;  // 动态平行光Ambient
	DWORD				dwDynamicSpecCol; // 动态平行光Specular

	INT					nNumMapTriggerEffect;		//触发特效
	DWORD				dwMapTriggerEffectOffset;

	BYTE				byReserve[200-12-8-4-4-8-8-8-8-16-16-1-4-8-8-8-12-8];//12是后加的天空盒偏移，-8为地图区域所用, -4为太阳光增强系数，-8为寻路导航点个数及偏移,-8为刷怪点个数
														//-8为动态障碍物 -8为事件触发器,16为npc 玩家格子导航图生成起点,-16天光,-1是否显示天空遮罩,-4天空遮罩的颜色,-8门,-8载具,-12动态平行光颜色
														//-8触发特效

	tagMapHeader()
	{
		memset(this,0,sizeof(tagMapHeader));
	}
};

//--静态地物----------------------------------------------------------------------------
struct tagStaticMapObj
{
	DWORD				dwMapID;			//此物体在map中的id
	TCHAR				szMdlPath[MAX_PATH];//模型
	FLOAT				fPos[3];
	FLOAT				fRotate[3];
	FLOAT				fScale[3];

	TCHAR				szScriptName[32];	//调查触发器则填入脚本名称
	DWORD				dwPickItemID;		//调查之后可以捡起的物品ID

	bool				bLock;				//判断是否已锁定,地图编辑器使用
	bool				bFlag;
	bool				bShowMiniMap;		//是否在小地图中显示
	bool				bLightMap;			//LightMap开关，格式：bool，缺省值：true
	BYTE				byReserved[8];		//bool	bHighLight;			//高光效果开关，格式：bool，缺省值：true
											//FLOAT	fHiLightIntensity;	//高光强度，格式：float，缺省值：1.4
	BYTE				byViewLevel;		//视野距离，格式：byte，缺省值：0	0-自动计算 1-近 2-中 3-远
	bool				bPathfinding;		//是否自动寻径
	BYTE				bSurfaceType;		//表面类型，现用于播放音效用
	float				fMtlDiffuseFactor;	//漫反射材质增强系数
	bool				bNoShadow;			//不产生阴影 缺省值:false产生阴影 true不产生阴影
	bool				bNotSupportNPCNavMap;//不支持NPC格子导航图

	BYTE				byBoxSize;			//重新生成的碰撞盒大小
	BYTE				byReserve[91];

	tagStaticMapObj()
	{
		memset(this,0,sizeof(*this));
		fScale[0]=fScale[1]=fScale[2]=1.0f;
		fMtlDiffuseFactor=1.5f;
		byBoxSize = 0;
	}
};

//--事件触发器-----------------------------------------------------------------
enum ETiggerCondition						//触发条件
{
	ETC_Null=0,
};

enum ETiggerResult							//触发结果
{
	ETR_Null=0,
};

struct tagMapEventTrigger			
{
	DWORD				dwObjID;
	ETiggerCondition	eCondition;			//条件
	DWORD				dwConditionParam1;	//条件参数1
	DWORD 				dwConditionParam2;	//条件参数2
	ETiggerResult		eResult;			//结果
	DWORD				dwResultParam1;		//结果参数1
	DWORD				dwResultParam2;		//结果参数2

	bool				bLock;
	bool				bFlag;
	BYTE				byReserve[128];

	tagMapEventTrigger(){ memset(this, 0, sizeof(*this)); }
};

//--触发器---------------------------------------------------------------------
enum EMapTrigger							//触发器类型
{
	EMT_Null=0,
	EMT_GotoNewMap,							//场景切换区域
	EMT_Script,								//脚本触发
};

struct tagMapTrigger
{
	DWORD 				dwObjID;
	EMapTrigger			eType;				//类型
	vector<Vector3>		region;				//多边形区域
	AABBox				box;				//包围盒
	FLOAT				fHeight;			//高度
	TCHAR				szMapName[X_LONG_NAME];		//EMT_GotoNewMap 的目标地图
	TCHAR				szWayPoint[X_SHORT_NAME];    	//EMT_GotoNewMap 的目标地图位置
	TCHAR				szScriptName[X_SHORT_NAME];	//EMT_Script 的脚本名称
	DWORD				dwParam;			//参数

	bool				bLock;
	bool				bFlag;
	DWORD				dwQuestSerial;
	BYTE				byReserve[124];

	tagMapTrigger() 
	{ 
		dwObjID = 0;
		eType = EMT_Null;
		region.clear();
		fHeight = 0.0f;
		memset(szMapName, 0, sizeof(szMapName));
		memset(szWayPoint, 0, sizeof(szWayPoint));
		memset(szScriptName, 0, sizeof(szScriptName));
		dwParam = 0;
		bLock = false;
		bFlag = false;
		memset(byReserve, 0, sizeof(byReserve));
	}	
};


//--地图区域------------------------------------------------------------------
enum EMapArea								//地图区域类型
{
	EMA_Null=0,
	EMA_PVP,								//PVP区域
	EMA_Safe,								//安全区域
	EMA_Stall,								//摆摊区域
	EMA_Prison,								//牢狱区域
	EMA_Display,							//显示区域
	EMA_Script,								//脚本区域
};

struct tagMapArea					
{
	DWORD 				dwObjID;
	EMapArea			eType;				//类型
	vector<Vector3>		region;				//多边形顶点
	AABBox				box;				//包围盒
	FLOAT				fHeight;			//高度

	bool				bLock;
	bool				bFlag;
	DWORD               dwMiniMapSize;      //缩略图大小
	bool                bDistFog;           //是否单独设置雾
	BYTE				byDistFog[16];      //雾
	BYTE				byReserve[107];

	tagMapArea() 
	{ 
		dwObjID = 0;
		eType = EMA_Null;
		region.clear();
		fHeight = 0.0f;
		bLock = false;
		bFlag = false;
		dwMiniMapSize=0;
		bDistFog=false;
		memset(byDistFog, 0, sizeof(byDistFog));
		memset(byReserve, 0, sizeof(byReserve));
	}
};


//--导航点------------------------------------------------------------------
struct tagMapWayPoint
{
	DWORD 				dwObjID;			
	Vector3				vPos;				//盒子底部中心坐标
	Vector3				vRange;				//范围	
	TCHAR				szName[X_SHORT_NAME];//导航点名称
	BYTE				byIndex;			//索引值

	bool				bLock;
	bool				bFlag;
	FLOAT				fSuspend;			//载具停留时间 add xtian 2009-4-14
	BYTE 				byReserve[32-4];	//-4载具停留时间

	tagMapWayPoint() { memset(this, 0 ,sizeof(*this)); }
};

//--NPC---------------------------------------------------------------------
struct tagMapNPC
{
	DWORD 				dwObjID;
	DWORD 				dwTypeID;			//属性ID
	Vector3				vPos;				//初始坐标
	FLOAT 				fYaw;				//初始朝向
	TCHAR				szName[X_SHORT_NAME];//巡逻导航点名称
	bool				bCollide;			//使用碰撞？

	bool				bLock;
	bool				bFlag;
	FLOAT				fRadius;			//巢穴半径
	BYTE				byReserve[32-5];	//-5为巢穴半径

	tagMapNPC() { memset(this, 0 ,sizeof(*this)); }
};

//--寻路导航点--------------------------------------------------------------
struct tagMapPathPoint
{
	DWORD 				dwObjID;
	Vector3 			vPos;				//坐标
	//FLOAT 				fRange;				//范围
	BYTE				byReserve1[4];

	bool				bLock;
	bool				bFlag;
	BYTE				byReserve[32];

	tagMapPathPoint() { memset(this, 0 ,sizeof(*this)); }
};


//--刷怪点------------------------------------------------------------------
struct tagMapSpawnPoint
{
	DWORD 				dwObjID;
	Vector3				vPos;				//坐标

	bool				bLock;				
	bool				bFlag;
	bool				bCollide;			//是否碰撞 add by xtian 2008-12-29
	DWORD				dwGroupID;			//怪物组ID add by xtian 2008-12-29
	INT					nLevelInc;			//随机副本怪物等级增加量
	BYTE				byReserve[7];

	tagMapSpawnPoint() { memset(this, 0 ,sizeof(*this)); }
};


//--动态障碍物--------------------------------------------------------------
struct tagMapDynamicBlock
{
	DWORD				dwObjID;
	Vector3				vPos;				//坐标
	FLOAT				fYaw;				//朝向	
	FLOAT				fScale[3];			//缩放
	TCHAR				szModelPath[X_LONG_NAME];//模型文件路径

	bool				bLock;				
	bool				bFlag;
	BYTE				byReserve[128];

	tagMapDynamicBlock() { memset(this, 0 ,sizeof(*this)); }
};

//--点光源------------------------------------------------------------------
struct tagMapPointLight
{
	DWORD				dwID;
	FLOAT				fPos[3];
	FLOAT				ambient[4];			//环境光
	FLOAT				diffuse[4];			//漫反射
	FLOAT				specular[4];		//镜面光
	FLOAT				range;				//影响范围
	FLOAT				attenuation[3];		//1, D, D^2;
	bool				bLock;
	bool				bFlag;
	bool				bDynamicPL;			//动态光源 bool 默认是动态光源
	FLOAT				fIncModulus;		//增强系数 float 默认1.0，可以在大于0的范围内调整
	FLOAT				fnoBlockRange;		//无遮挡范围
	BYTE				byReserve[20-5-4];

	tagMapPointLight() { memset(this, 0 ,sizeof(*this)); }
};

//--显示区域额外信息--------------------------------------------------------
struct tagMapAreaEx
{
	DWORD				dwObjID;
	tstring				strTitle;			//进入显示图片名称
	tstring				strMusic[3];		//播放歌曲名称
	WORD				wInterval;			//播放歌曲时间间隔
	BYTE				byVol;				//播放歌曲音量系数
	BYTE				byPriority;			//优先级
};

//--3D音效------------------------------------------------------------------
struct tagMapSound
{
	DWORD				dwObjID;
	FLOAT				fPos[3];				//位置
	FLOAT				fRange;					//范围
	TCHAR				szFileName[X_LONG_NAME];//音效文件路径
	FLOAT				fVolume;				//音量
	FLOAT				fNoAtteRange;			//没有衰减的范围

	bool				bLock;
	bool				bFlag;

	DWORD				dwInterval;				//多长时间播放一次音效

	BYTE				byReserve[28];

	tagMapSound() { memset(this, 0 ,sizeof(*this)); }
};

//--NPC格子导航图生成起点-----------------------------------------------------
struct tagNPCNavMapBuildStartPoint
{
	DWORD 				dwObjID;
	Vector3 			vPos;				//坐标

	bool				bLock;
	bool				bFlag;
	BYTE				byReserve[32];

	tagNPCNavMapBuildStartPoint() { memset(this, 0 ,sizeof(*this)); }
};

//--玩家寻路导航图生成起点-----------------------------------------------------
struct tagPlayerNavMapBuildStartPoint
{
	DWORD 				dwObjID;
	Vector3 			vPos;				//坐标

	bool				bLock;
	bool				bFlag;
	BYTE				byReserve[32];

	tagPlayerNavMapBuildStartPoint() { memset(this, 0 ,sizeof(*this)); }
};

//-------------------------------------------------------------------------------
// 副本信息
//-------------------------------------------------------------------------------
struct tagInstance
{
	DWORD					dwMapID;						// 地图ID

	DWORD					dwEndTime;						// 等待关闭时间
	DWORD					dwTimeLimit;					// 时间限制

	EInstanceMapType		eInstanceMapType;				// 副本类型
	EInstanceCreateMode		eInstanceCreateMode;			// 生成内容规则
	
	bool					bAskEnter;						// 进入是否寻问
	bool					bSelectHard;					// 是否选择难度
	bool					bSelectNormal;					// 是否可选普通
	bool					bSelectElite;					// 是否可选精英
	bool					bSelectDevil;					// 是否可选魔王
	bool					bNoticeTeamate;					// 是否通知队友

	INT						nNumDownLimit;					// 人数下限	
	INT						nNumUpLimit;					// 人数上限
	INT						nLevelDownLimit;				// 等级下限
	INT						nLevelUpLimit;					// 等级上限

	EExportMode				eExportMode;					// 出口类型
	DWORD					dwExportMapID;					// 出口地图ID
	Vector3					vExportPos;						// 出口坐标
	Vector3					vEnterPos;						// 副本内入口坐标

	DWORD					dwTargetLimit;					// 可攻击目标类型
	bool					bCombat;						// 是否可战斗
	bool					bPK;							// 是否可PK
	bool					bLoseSafeguard;					// 是否PK保护失效
	bool					bPKPenalty;						// 是否有PK惩罚
	bool					bDeadPenalty;					// 是否有死亡惩罚
	bool					bMount;							// 是否可骑乘
	bool					bTransmit;						// 是否可传送
	ERebornMode				eRebornMode;					// 复活方式

	ECompleteConditionNor	eCompleteNor;					// 普通完成条件
	DWORD					dwCompleteNorVal1;
	DWORD					dwCompleteNorVal2;	

	ECompleteConditionEli	eCompleteEli;					// 精英完成条件
	DWORD					dwCompleteEliVal1;
	DWORD					dwCompleteEliVal2;

	ECompleteConditionDev	eCompleteDev;					// 魔王完成条件
	DWORD					dwCompleteDevVal1;
	DWORD					dwCompleteDevVal2;

	ECompleteEvent			eCompleteEvent;					// 达成条件后事件
};


//--门--------------------------------------------------------------
struct tagMapDoor
{
	DWORD				dwObjID;
	Vector3				vPos;				//坐标
	FLOAT				fYaw;				//朝向	
	FLOAT				fScale[3];			//缩放
	TCHAR				szModelPath[X_LONG_NAME];//模型文件路径
	BOOL				bInitState;			//初始状态：TRUE打开；FALSE关闭		
	BOOL				bBlockingTileNPC;	//是否阻挡格子导航NPC
	bool				bLock;				
	bool				bFlag;
	DWORD				dwTypeID;			//门的typeid
	BYTE				byReserve[124];

	tagMapDoor() { memset(this, 0 ,sizeof(*this)); }
};


//--载具-------------------------------------------------------------
struct tagMapCarrier
{
	DWORD				dwObjID;
	TCHAR				szModelPath[X_LONG_NAME];	//模型文件路径
	TCHAR				szWayPoint[X_SHORT_NAME];	//导航点名称
	FLOAT				fYaw;						//朝向	
	BYTE				bSurfaceType;				//表面类型，现用于播放音效用
	FLOAT				fSpeed;						//移动速度

	bool				bLock;				
	bool				bFlag;
	BYTE				byReserve[128];

	tagMapCarrier() { memset(this, 0 ,sizeof(*this)); }
};

//--触发特效----------------------------------------------------------
struct tagMapTriggerEffect
{
	DWORD				dwObjID;				//此物体在map中的id
	TCHAR				szMdlPath[MAX_PATH];	//特效路径和文件
	FLOAT				fPos[3];
	FLOAT				fRotate[3];
	FLOAT				fScale[3];

	bool				bLock;				
	bool				bFlag;
	BYTE				byReserve[64];			//保留字节

	tagMapTriggerEffect()
	{
		memset(this,0,sizeof(*this));
		fScale[0]=fScale[1]=fScale[2]=1.0f;
	}
};

