//-----------------------------------------------------------------------------
//!\file msg_motion.h
//!
//!\date 2009-09-24
//!
//!\brief 客户端与服务器之间关于动作的消息
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push, 1)

enum 
{
	E_Motion_Success			= 0,
	E_Motion_NotValid			= 1,	// 动作无效
	E_Motion_CanotActive		= 2,	// 无法进行交互
	E_Motion_FriendValNotEnough	= 3,	// 友好度不满足
	E_Motion_DistanceNotEnough	= 4,	// 距离不够
	E_Motion_RoleNotFround		= 5,	// 人物没找到
	E_Motion_DstRoleRefuse		= 6,	// 目标玩家拒绝
	E_Motion_WaitRemotePesponses= 7,	// 正在等待目标玩家回应
	E_Motion_CannotStartWhenWait= 8,	// 正在等待目标玩家回应时，不能开始新的交互
	E_Motion_Wimming			= 9,	// 游泳时不能开始新的交互
};

/*******************************************************************************
交互流程：
	验证阶段：
		1.客户端发NC_DuetMotionInvite请求服务器
		2.服务器进行判断，若通过，则发NS_DuetMotionOnInvite给被请求玩家
		3.被请求玩家选择是否接受发送NC_DuetMotionOnInvite给服务器
		4.服务器发送NS_DuetMotionInvite给请求玩家结果
	开始动作：
		5.请求玩家收到NS_DuetMotionInvite，跑到被请求玩家处
		6.请求玩家向服务器发送NC_DuetMotionStart开始双人动作
		7.服务器广播，双人动作开始	
*******************************************************************************/

//------------------------------------------------------------------------------
// 最大距离
//------------------------------------------------------------------------------
CONST INT MAX_MOTION_DIST	= 50 * 10;

//----------------------------------------------------------------------------
// 客户端表演个性动作
//----------------------------------------------------------------------------
CMD_START(NC_RoleStyleAction)
	DWORD			dwActionID;
CMD_END

CMD_START(NS_RoleStyleAction)
	DWORD			dwRoleID;
	DWORD			dwDstRoleID;
	DWORD			dwActionID;
CMD_END

//------------------------------------------------------------------------------
// 双人动作 主动邀请的消息
//------------------------------------------------------------------------------
CMD_START(NC_DuetMotionInvite)	
	DWORD	dwDstRoleID;	// 目标玩家id
	DWORD	dwActionID;		// 动作id
CMD_END

CMD_START(NS_DuetMotionInvite)	
	DWORD	dwErrCode;		// 错误码
CMD_END

//------------------------------------------------------------------------------
// 双人动作 被邀请的消息
//------------------------------------------------------------------------------
CMD_START(NS_DuetMotionOnInvite)	
	DWORD	dwSrcRoleID;	// 发起玩家id
	DWORD	dwActionID;		// 动作id
CMD_END

CMD_START(NC_DuetMotionOnInvite)
	DWORD	dwSrcRoleID;	// 发起玩家id
	DWORD	dwActionID;		// 动作id
	BOOL	bAccpet;		// 是否同意
CMD_END

//------------------------------------------------------------------------------
// 双人动作 开始动作
//------------------------------------------------------------------------------
CMD_START(NC_DuetMotionStart)	
	DWORD	dwDstRoleID;	// 目标玩家id
	DWORD	dwActionID;		// 动作id
CMD_END


#pragma pack(pop)