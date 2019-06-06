//-------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: move_data.h
// author: Aslan
// actor:
// data: 2009-01-13
// last:
// brief: 移动结构
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Cool3D/Util/Timer.h"

class Unit;
class Role;
class Creature;
class Map;

//------------------------------------------------------------------------------------------------------
// 格子怪物移动控制器
//------------------------------------------------------------------------------------------------------
class NavCollider_TileMove
{
public:
	NavCollider_TileMove();
	~NavCollider_TileMove();

public:
	VOID		Init(const Vector3& vStart, const Vector3& vDest, FLOAT fXZSpeed);
	ENavResult	Update(Map* pMap, Vector3& vOutPos);

private:
	Vector3			m_vStart;		// 起始坐标
	Vector3			m_vDest;		// 终点坐标
	FLOAT			m_fTotalTime;	// 总共需要移动多长时间
	FLOAT			m_fCurTime;		// 当前走了多长时间
	INT				m_nMoveTicks;	// 需要的总的心跳数
	FLOAT			m_fXZSpeed;		// xz方向的移动速度
};

class MoveData
{
public:
	//---------------------------------------------------------------------------------------------
	// 移动的结果
	//---------------------------------------------------------------------------------------------
	enum EMoveRet
	{
		EMR_Success		=	0,		// 可以移动
		EMR_SelfLimit	=	1,		// 自身限制，比如中了眩晕，定身等等
		EMR_NoArrive	=	2,		// 不能到达
		EMR_Conflict	=	3,		// 移动状态之间冲突
		EMR_Invalid		=	4,		// 非法调用
	};

public:
	//---------------------------------------------------------------------------------------------
	// 辅助函数
	//---------------------------------------------------------------------------------------------
	static BOOL		IsPosInvalid(const Vector3& vPos);
	static BOOL		IsFaceInvalid(const Vector3& vFace);

	//---------------------------------------------------------------------------------------------
	// CONSTRUCT
	//---------------------------------------------------------------------------------------------
	MoveData() : m_pOwner(NULL), m_eCurMove(EMS_Stand), m_eNextPreMove(EMS_Stand), 
				 m_nVisTileIndex(GT_INVALID), m_bWaitClientMsg(FALSE), m_bIsStopped(FALSE),
				 m_pColliderWalk(NULL), m_pColliderJump(NULL), m_pColliderSwim(NULL),
				 m_pColliderSlide(NULL), m_pColliderHitFly(NULL), m_pColliderNPCWalk(NULL), m_pColliderTileWalk(NULL) {}

	~MoveData()
	{
		SAFE_DEL(m_pColliderWalk);
		SAFE_DEL(m_pColliderJump);
		SAFE_DEL(m_pColliderSwim);
		SAFE_DEL(m_pColliderSlide);
		SAFE_DEL(m_pColliderHitFly);
		SAFE_DEL(m_pColliderNPCWalk);
		SAFE_DEL(m_pColliderTileWalk);
	}

	//-------------------------------------------------------------------------------------
	// 得到当前移动状态
	//-------------------------------------------------------------------------------------
	EMoveState	GetCurMoveState()		{ return m_eCurMove; }
	EMoveState	GetNextMoveState()		{ return m_eNextPreMove; }
	BOOL		IsStopped()				{ return m_bIsStopped; }

	//---------------------------------------------------------------------------------------------
	// 初始化移动结构，主要用于玩家和生物初始化时的移动状态
	//---------------------------------------------------------------------------------------------
	VOID Init(Unit* pOwner, const FLOAT fX, const FLOAT fY, const FLOAT fZ, const FLOAT fFaceX, const FLOAT fFaceY, const FLOAT fFaceZ);

	//----------------------------------------------------------------------------------------------
	// 重置移动结构，这主要用于玩家切换地图和怪物重生时，一般很少用
	//----------------------------------------------------------------------------------------------
	VOID Reset(FLOAT fX, const FLOAT fY, const FLOAT fZ, const FLOAT fFaceX, const FLOAT fFaceY, const FLOAT fFaceZ);

	//-----------------------------------------------------------------------------------------------
	// 开始移动
	//-----------------------------------------------------------------------------------------------
	EMoveRet	StartRoleWalk(const Vector3& vStart, const Vector3& vEnd);
	EMoveRet	StartRoleJump(const Vector3& vStart, const Vector3& vDir);
	EMoveRet	StartRoleDrop(const Vector3& vStart, const Vector3& vDir);
	EMoveRet	StartRoleVDrop(const Vector3& vStart);
	EMoveRet	StartRoleSlide(const Vector3& vStart);
	EMoveRet	StartRoleSwim(const Vector3& vStart, const Vector3& vEnd);
	EMoveRet	StartCreatureWalk(const Vector3& vDest, EMoveState eState, BOOL bCheck=TRUE);
	EMoveRet	StartHitFly(const Vector3& vDir);
	EMoveRet	StopRoleMove(const Vector3& vPos);
	EMoveRet	StopRoleSwim(const Vector3& vPos);

	//-----------------------------------------------------------------------------------------------
	// 停止移动
	//-----------------------------------------------------------------------------------------------
	EMoveRet	StopMove(BOOL bSend=TRUE);
	EMoveRet	StopMove(const Vector3& vNewFace, BOOL bSend=TRUE);
	EMoveRet	StopMoveForce(BOOL bSend=TRUE);

	//-----------------------------------------------------------------------------------------------
	// 瞬移
	//-----------------------------------------------------------------------------------------------
	VOID		ForceTeleport(const Vector3& vPos, BOOL bSend=TRUE);

	//-----------------------------------------------------------------------------------------------
	// 改变朝向
	//-----------------------------------------------------------------------------------------------
	VOID		SetFaceTo(const Vector3& vFace);

	//-----------------------------------------------------------------------------------------------
	// 更新移动
	//-----------------------------------------------------------------------------------------------
	VOID		Update();
	VOID		UpdateRoleWalk();
	VOID		UpdateRoleJump();
	VOID		UpdateRoleDrop();
	VOID		UpdateRoleVDrop();
	VOID		UpdateRoleSlide();
	VOID		UpdateRoleSwim();
	VOID		UpdateCreatureWalk();
	VOID		UpdateHitFly();

	//-------------------------------------------------------------------------------------
	// 取站立坐标点
	//-------------------------------------------------------------------------------------
	VOID		DropDownStandPoint(BOOL bSend=TRUE);
	VOID		DropDownStandPoint(const Vector3& vPos, BOOL bSend=TRUE);

private:
	//-------------------------------------------------------------------------------------
	// 各种检测
	//-------------------------------------------------------------------------------------
	EMoveRet	IsCanRoleWalk(const Vector3& vStart, const Vector3& vEnd);
	EMoveRet	IsCanRoleJump(const Vector3& vStart, const Vector3& vDir);
	EMoveRet	IsCanRoleDrop(const Vector3& vStart, const Vector3& vDir);
	EMoveRet	IsCanRoleVDrop(const Vector3& vStart);
	EMoveRet	IsCanRoleSlide(const Vector3& vStart);
	EMoveRet	IsCanRoleSwim(const Vector3& vStart, const Vector3& vEnd);
	EMoveRet	IsCanRoleStop(const Vector3& vPos);
	EMoveRet	IsCanHitFly(const Vector3& vEnd);

public:
	EMoveRet	IsCanCreatureWalk(const Vector3& vEnd, EMoveState eState, BOOL bCheck, Vector3* vNearPos=NULL);
	BOOL		IsInValidDistance(const Vector3& vStart);

private:

	BOOL		IsMapValid();
	FLOAT		GetCreatureMoveStateSpeed(EMoveState eState);

private:

	//-------------------------------------------------------------------------------------
	// 位置改变
	//-------------------------------------------------------------------------------------
	VOID		OnPosChange(const Vector3& vNewPos);

	//-------------------------------------------------------------------------------------
	// 开始移动
	//-------------------------------------------------------------------------------------
	VOID		OnStartMove();


	//-------------------------------------------------------------------------------------
	// 得到行走控制器
	//-------------------------------------------------------------------------------------
	NavCollider_Move*		GetColliderWalk()		{ if( !P_VALID(m_pColliderWalk) )		m_pColliderWalk = new NavCollider_Move;			return m_pColliderWalk; }
	NavCollider_Jump*		GetColliderJump()		{ if( !P_VALID(m_pColliderJump) )		m_pColliderJump = new NavCollider_Jump;			return m_pColliderJump; }
	NavCollider_Swim*		GetColliderSwim()		{ if( !P_VALID(m_pColliderSwim) )		m_pColliderSwim = new NavCollider_Swim;			return m_pColliderSwim; }
	NavCollider_Slide*		GetColliderSlide()		{ if( !P_VALID(m_pColliderSlide) )		m_pColliderSlide = new NavCollider_Slide;		return m_pColliderSlide; }
	NavCollider_HitFly*		GetColliderHitFly()		{ if( !P_VALID(m_pColliderHitFly) )		m_pColliderHitFly = new NavCollider_HitFly;		return m_pColliderHitFly; }
	NavCollider_NPCMove*	GetColliderNPCWalk()	{ if( !P_VALID(m_pColliderNPCWalk) )	m_pColliderNPCWalk = new NavCollider_NPCMove;	return m_pColliderNPCWalk; }
	NavCollider_TileMove*	GetColliderTileWalk()	{ if( !P_VALID(m_pColliderTileWalk) )	m_pColliderTileWalk = new NavCollider_TileMove;	return m_pColliderTileWalk; }

public:
	static Timer			m_Timer;							// 移动计时器

	Unit*					m_pOwner;							// 控制者
	EMoveState				m_eCurMove;							// 当前移动状态
	EMoveState				m_eNextPreMove;						// 在当前状态进行“完毕之后”，预计要切换到的状态
	Vector3					m_vPos;								// 当前位置
	Vector3					m_vPosStart;						// 当前状态开始时的初始位置
	Vector3					m_vDir;								// 当前移动方向
	Vector3					m_vDest;							// 目标点
	Vector3					m_vFace;							// 朝向

	INT						m_nVisTileIndex;					// 当前所处的可视地砖索引
	BOOL					m_bWaitClientMsg;					// 如果当前状态完毕且下一个状态不是站立，该字段用于设置是否在等待客户端消息

	FLOAT					m_fStartTime;						// 移动开始的时间

	BOOL					m_bIsStopped;						// 被外界停下

	NavCollider_Move*		m_pColliderWalk;					// 行走控制器
	NavCollider_Jump*		m_pColliderJump;					// 跳跃，掉落和垂直掉落控制器
	NavCollider_Swim*		m_pColliderSwim;					// 游泳控制器
	NavCollider_Slide*		m_pColliderSlide;					// 滑落控制器
	NavCollider_HitFly*		m_pColliderHitFly;					// 击飞控制器
	NavCollider_NPCMove*	m_pColliderNPCWalk;					// NPC移动控制器
	NavCollider_TileMove*	m_pColliderTileWalk;				// 格子移动控制器
};

//-------------------------------------------------------------------------------------------
// 改变面向
//-------------------------------------------------------------------------------------------
inline VOID MoveData::SetFaceTo(const Vector3& vFace)
{
	if( IsFaceInvalid(vFace) ) return;
	if( abs(vFace.x) < 0.001f && abs(vFace.z) < 0.001f ) return;

	m_vFace = vFace;
	m_vFace.y = 0;
}

