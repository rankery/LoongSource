#pragma once

//-------------------------------------------------
//  Game World 的 server 类型
//-------------------------------------------------
enum EServerType {
	EST_World,		//游戏世界服务器
	EST_DB,			//游戏世界数据库服务器
	EST_Login,		//游戏大区login服务器	
};

//--------------------------------------------------
// 客户端权限
//--------------------------------------------------
enum EPrivilege
{
	EP_Null		=	-1,		// 未初始化
	EP_Pri_GM_D	=	0,		// 1级权限
	EP_Pri_GM_C	=	1,		// 2级权限
	EP_Pri_GM_B	=	2,		// 3级权限
	EP_Pri_GM_A	=	3,		// 4级权限
	EP_Pri_GM_S	=	4,		// 5级权限

	// (6级以上为远程监控使用)
	EP_Pri_Rt_B	=	5,		// 6级权限
	EP_Pri_Rt_A	=	6,		// 7级权限
	EP_Pri_Rt_C	=	7,		// 8级权限(调整服务器人数上限用)
	EP_Pri_Rt_D	=	8,		// 9级权限(更新商城用)
	EP_End		=	9
};

#define MIS_GMTOOL_PRI(ePri)	((ePri) >= EP_Pri_GM_D && (ePri) <= EP_Pri_GM_S)
#define MIS_RTCLIENT_PRI(ePri)	((ePri) >= EP_Pri_Rt_B && (ePri) <= EP_Pri_Rt_D)

#define MIS_GMOPGM(root, gm)		(MIS_GMTOOL_PRI(root)		&& MIS_GMTOOL_PRI(gm) )
#define MIS_RTOPRT(root, rt)		(MIS_RTCLIENT_PRI(root)		&& MIS_RTCLIENT_PRI(rt) )