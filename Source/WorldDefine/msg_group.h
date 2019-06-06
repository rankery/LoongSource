//-----------------------------------------------------------------------------
//!\file msg_group.h
//!
//!\date 2008-11-20
//!
//!\brief 客户端与服务器之间关于队伍的消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "group_define.h"

#pragma pack(push, 1)

// 错误码
enum
{
	E_Team_Not_Leader					=	1,		// 邀请方不是队长
	E_Team_Target_Not_Exit				=	2,		// 目标不合法
	E_Team_Target_Have_Team				=	3,		// 被邀请者已在小队中
	E_Team_Member_Full					=	4,		// 小队成员以满
	E_Team_Role_Not_In_Team				=	5,		// 踢出的玩家不在本小队
	E_Team_Pick_Model_Not_Exit			=	6,		// 所设置的拾取模式不存在
	E_Team_Target_Busy					=	7,		// 被邀请者正忙
	E_Team_Target_Not_Online			=	8,		// 被邀请玩家不在线
	E_Team_Map_CanNot_Invite			=	9,		// 该地图内无法邀请队友组队
	E_Team_Map_CanNot_Leave				=	10,		// 该地图内无法离开小队
	E_Team_Map_CanNor_ChangeLeader		=	11,		// 该地图内无法移交队长
	E_Team_Map_CanNot_Kick				=	12,		// 该地图内无法踢人
	E_Team_You_Have_Team				=   13,		// 您已经在队伍中
	//Jason 2009-11-25
	E_Team_Apply_Invalid_Param			=	14,		// 申请入队参数无效
	E_Team_Hack_Tools_Prohibited		=	15,		// 外挂禁止
	E_Team_Cannot_Find_Map				=	16,		// 未找到地图
	E_Team_Already_In_Team				=	17,		// 申请人已经在该队中
	E_Team_Already_Have_Team			=	18,		// 申请人已经有其他队
	E_Team_Target_No_Team				=	19,		// 被申请人，没有队
	E_Team_Refuse						=	20,		// 拒绝
	E_Team_Peer_Not_Online				=	21,		// 被申请方不在线，（客户端判断方便些，即使前面已经有一个E_Team_Target_Not_Online）
};


CMD_START(NC_InviteJoinTeam)		// 邀请组队
	DWORD		dwDestRoleID;		// 被邀请玩家	
CMD_END;

CMD_START(NS_InviteToLeader)		// 邀请组队(返回给邀请者)
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NS_InviteJoinTeam)		// 邀请组队(返回给被邀请者)
	DWORD		dwLeaderID;
	DWORD		dwDestRoleID;
CMD_END;

CMD_START(NC_InviteReply)			// 玩家是否同意组队邀请
	DWORD		dwLeaderID;
	BOOL		bAgree;				// 是否同意
CMD_END;

CMD_START(NS_InviteReply)
	DWORD		dwTeamMemID[MAX_TEAM_NUM];
	BOOL		bAgree;
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NC_KickMember)			// 踢玩家出队
	DWORD		dwSrcRoleID;		// 队长	
	DWORD		dwDestRoleID;		// 被踢玩家
CMD_END;

CMD_START(NS_KickMember)
	DWORD		dwDestRoleID;
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NC_LeaveTeam)				// 离开队伍
CMD_END;
	
CMD_START(NS_LeaveTeam)				// 包括主动离开和掉线
	DWORD		dwRoleID;
	DWORD		dwLeaderID;			// 队长ID
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NC_SetPickMode)			// 设置小队拾取模式
	EPickMode	ePickMode;			// 拾取模式
CMD_END;

CMD_START(NS_SetPickMode)
	EPickMode	ePickMode;
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NC_ChangeLeader)
	DWORD		dwNewLeader;
CMD_END;

CMD_START(NS_ChangeLeader)
	DWORD		dwLeaderID;
	DWORD		dwNewLeaderID;
	DWORD		dwErrorCode;
CMD_END;

CMD_START(NS_RoleStateToTeam)
	DWORD			dwRoleID;		// 玩家ID
	DWORD			dwMapID;
	EClassTypeEx    eClassEx;		// 职业
	BYTE		    bySex;
	INT				nLevel;
	INT				nMaxHP;
	INT				nHP;
	INT				nMaxMP;
	INT				nMP;
	FLOAT			fX;
	FLOAT			fZ;
	DWORD			dwEquipTypeID[TEAM_DISPLAY_NUM];	
	tagAvatarAtt	AvatarAtt;
CMD_END;

CMD_START(NS_RolePosToTeam)		
	DWORD		dwRoleID;
	DWORD		dwMapID;
	FLOAT		fX;
	FLOAT		fZ;
CMD_END;

CMD_START(NS_GetTeamID)
	DWORD		dwTeamID;
	EPickMode	ePickMode;
CMD_END

// Jason 2009-11-25
// 申请入队相关协议
CMD_START(NC_ApplyToJoinTeam)	// 申请入队 ，申请者客户端发给world服务器
	//DWORD		dwApplicantID;	// 申请人id，可以去掉的
	DWORD		dwTeamMemberID;	// 团队成员id
CMD_END
CMD_START(NS_ApplyToJoinTeam)	// 申请入队，该协议由world转发给队长客户端
	DWORD		dwApplicantID;	// 申请人id
	INT			nLevel;
	//名字
	TCHAR	szName[X_SHORT_NAME];
CMD_END

CMD_START(NC_ApplyReply)			// 队长是否同意入队邀请，队长客户端发送给world服务器
	//DWORD		dwTeamLeaderID;		// 队长id，可以去掉
	DWORD		dwApplicantID;		// 申请者id
	BOOL		bTeamLeaderAgree;	// 是否同意
CMD_END

// Jason，注意，当队长同意时，复用邀请应答协议，这样客户端就不用再处理申请成功消息了。
// 也就是说，当队长同意时，不用world服务器返回的协议是tagNS_InviteReply；
// 队长不同意时，返回下面的协议
CMD_START(NS_ApplyReplyRefuse)			// 队长是否同意入队邀请。world服务器，反馈给申请者客户端
	DWORD		dwTeamLeaderID;		// 队长id
	//DWORD		dwApplicantID;		// 申请者id
	//BOOL		bTeamLeaderAgree;	// 是否同意
	DWORD		dwErrCode;
	//名字
	TCHAR	szName[X_SHORT_NAME];
CMD_END



#pragma pack(pop)