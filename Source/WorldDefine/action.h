#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

//----------------------------------------------------------------------------
// 行走
//----------------------------------------------------------------------------
CMD_START(NC_MouseWalk)			//鼠标行走
	Vector3	srcPos;
	Vector3	dstPos;
	bool	bSwim;				//游泳状态标识
CMD_END

CMD_START(NC_KeyboardWalk)		//键盘行走
	Vector3	srcPos;
	Vector3	dstPos;
	bool	bSwim;				//游泳状态标识
CMD_END

CMD_START(NC_StopWalk)			//停止行走
	Vector3	curPos;
	bool	bSwim;				//游泳状态标识
CMD_END

CMD_START(NS_StopWalk)			//服务器收到NC_StopWalk后，向周围玩家广播
	Vector3	curPos;
	bool	bSwim;				//游泳状态标识
CMD_END

CMD_START(NS_SyncWalk)
	DWORD	dwRoleID;
	Vector3	srcPos;
	Vector3	dstPos;
	FLOAT	curTime;
	FLOAT	fXZSpeed;
	bool	bCollide;
	bool	bSwim;				//游泳状态标识
CMD_END

//----------------------------------------------------------------------------
// 跳跃
//----------------------------------------------------------------------------
CMD_START(NC_Jump)
	Vector3	srcPos;
	Vector3	dir;
	FLOAT   totalTime;
CMD_END

CMD_START(NS_SyncJump)
	DWORD	dwRoleID;
	Vector3	srcPos;
	Vector3	dir;
	FLOAT	curTime;
	FLOAT	fXZSpeed;
	FLOAT	fYSpeed;
CMD_END

//----------------------------------------------------------------------------
// 掉落
//----------------------------------------------------------------------------
CMD_START(NC_Drop)
	Vector3 srcPos;
	Vector3 dir;
	FLOAT   totalTime;
CMD_END

CMD_START(NS_SyncDrop)
	DWORD 	dwRoleID;
	Vector3	srcPos;
	Vector3	dir;
	FLOAT	curTime;
	FLOAT	fXZSpeed;
CMD_END

//----------------------------------------------------------------------------
// 垂直掉落
//----------------------------------------------------------------------------
CMD_START(NC_VDrop)
	Vector3	srcPos;
	FLOAT   totalTime;
CMD_END

CMD_START(NS_SyncVDrop)
	DWORD	dwRoleID;
	Vector3	srcPos;
	FLOAT	curTime;
CMD_END

//----------------------------------------------------------------------------
// 滑落
//----------------------------------------------------------------------------
CMD_START(NC_Slide)
	Vector3	srcPos;
	Vector3 dstPos;				//客户端计算的滑落终点
CMD_END

CMD_START(NS_SyncSlide)
	DWORD	dwRoleID;
	Vector3	srcPos;
	FLOAT	curTime;
	FLOAT	fXZSpeed;
CMD_END

//----------------------------------------------------------------------------
// 站立同步，远程角色进入视野时发送
//----------------------------------------------------------------------------
CMD_START(NS_SyncStand)
	DWORD	dwRoleID;
	Vector3	curPos;
	Vector3	faceTo;
	bool	bSwim;				//游泳状态标识
CMD_END

//----------------------------------------------------------------------------
// 移动失败
//----------------------------------------------------------------------------
CMD_START(NS_MoveFailed)
	Vector3 curPos;
	Vector3 faceTo;
	bool	bSwim;				//游泳状态标识
CMD_END

//----------------------------------------------------------------------------
// 击飞
//----------------------------------------------------------------------------
CMD_START(NS_HitFly)
	DWORD	dwRoleID;
	Vector3 curPos;
	Vector3 destPos;
	Vector3 faceTo;	
	DWORD	dwSkillID;			//技能ID
	DWORD   dwSrcRoleID;		//攻击者ID
	DWORD	dwChannelIndex;		//管道编号
	bool	bCollide;			//使用碰撞
CMD_END

//----------------------------------------------------------------------------
// 移动速度改变
//----------------------------------------------------------------------------
CMD_START(NS_MoveSpeedChange)
	DWORD	dwRoleID;
	Vector3 curPos;
	Vector3 faceTo;
CMD_END

#pragma pack(pop)
