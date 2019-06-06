#pragma once

#include "..\WorldDefine\group_define.h"

struct tagNS_InviteToLeader;
struct tagNS_InviteJoinTeam;
struct tagNS_ApplyToJoinTeam;
struct tagNS_ApplyReplyRefuse;
struct tagNS_InviteReply;
struct tagNS_KickMember;
struct tagNS_LeaveTeam;
struct tagNS_SetPickMode;
struct tagNS_ChangeLeader;
struct tagRoleGetNameEvent;
struct tagNS_RoleStateToTeam;
struct tagNS_RolePosToTeam;
struct tagNS_RoleLevelChange;
struct tagNS_RemoteAttChangeSingle;
struct tagNS_RemoteAttChangeMutiple;
struct tagNS_GetTeamID;
struct tagInviteJoinTeamEvent;
struct tagApplyJoinTeamEvent;
struct tagTeammateData
{
	DWORD			dwRoleID;
	tstring			strName;
	DWORD			dwMapID;
	EClassTypeEx	eClassEx;
	INT				nLevel;
	INT				nMaxHP;
	INT				nHP;
	INT				nMaxMP;
	INT				nMP;
	FLOAT			fX;
	FLOAT			fZ;
	DWORD			dwEquipTypeID[TEAM_DISPLAY_NUM];
	tagAvatarAtt	AvatarAtt;
	tagTeammateData() : dwRoleID( GT_INVALID ) {}
};

/** \class TeamSys
	\brief 组队系统，单件模式（方便外部调用查询接口）
*/
class TeamSys
{
public:
	TeamSys(void);
	~TeamSys(void);
	static TeamSys* Inst();

	BOOL Init(void);
	VOID Destroy(void);
	VOID Updata();

	/** \本地玩家是否在队伍中
	*/
	bool IsInTeam() const { return m_vecTeammates.size() > 0; }
	/** \获得队友数量
	*/
	int GetTeammateNum() const { return m_vecTeammates.size(); }
	/** \队伍是否已满
	*/
	bool IsTeamFull() { return m_vecTeammates.size() == MAX_TEAM_NUM - 1; }
	/** \获得队友列表
	*/
	const vector<tagTeammateData>& GetTeammateList() { return m_vecTeammates; }
	/** \是否队长
	*/
	bool IsLeader( const DWORD dwRoleID ) const { ASSERT(P_VALID(dwRoleID)); return dwRoleID == m_dwLeaderRoleID; }
	/** \是否队友（包括队长）
	*/
	bool IsTeammate( const DWORD dwRoleID );
	/** \获得当前拾取模式
	*/
	EPickMode GetPickMode() const { return m_ePickMode; }
	/** \设置拾取模式
	*/
	void SetPickMode( const EPickMode ePickMode );

	/**获得入队申请模式
	*/
	ERefuseApplyMode GetRefuseApplyMode(){ return m_eRefuseApplyMode; }
	/**设置如对申请模式
	*/
	void SetRefuseApplyMode( ERefuseApplyMode mode ){ m_eRefuseApplyMode = mode; }

	/** \移除队员
	*/
	void KickTeammate( const DWORD dwTeammateID );
	/** \移交队长
	*/
	void HandOutLeader( const DWORD dwNewLeaderID );
	/** \获取队友信息
	*/
	tstring GetTeammateNameByID( const DWORD dwRoleID ) const;
	DWORD GetTeammateIDByName( const TCHAR* szName ) const;
	/** \离开队伍
	*/
	void LeaveTeam();
	/** \组队邀请
	*/
	void InviteJoinTeam( const DWORD dwRoleID );
	
	/**\申请加入队伍
	   \param dwRoleID 目标玩家ID, 此玩家在要加入的队伍中
	*/
	void ApplyJoinTeam( const DWORD dwRoleID );
	
	/** \获取队友数据
	*/
	const tagTeammateData* GetTeammateData( const DWORD dwRoleID );
	/** \获取队伍ID
	*/
	DWORD GetTeamID() const;
private:
	VOID  HandleNSErrorCode( const DWORD dwErrorCode );
	VOID  ShowInviteMsgBox( const TCHAR* szInviteName );
	VOID  ShowLeaveTeamMsgBox();
	VOID  ShowKickMemberMsgBox( const TCHAR* szDestName );
	VOID  AddChatMsg( const TCHAR* szMsg );
	VOID  ShowJoinTeamMsg( const TCHAR* szPlayerName );
	VOID  ShowLeaveTeamMsg( const TCHAR* szPlayerName );
	/** \队伍解散
	*/
	VOID  OnBreakTeam( const bool bSendEvent = true );
	/** \网络消息响应
	*/
	DWORD OnNS_InviteToLeader( tagNS_InviteToLeader* pCmd, DWORD );
	DWORD OnNS_InviteJoinTeam( tagNS_InviteJoinTeam* pCmd, DWORD );
	DWORD OnNS_InviteReply( tagNS_InviteReply* pCmd, DWORD );
	DWORD OnNS_KickMember( tagNS_KickMember* pCmd, DWORD );
	DWORD OnNS_LeaveTeam( tagNS_LeaveTeam* pCmd, DWORD );
	DWORD OnNS_SetPickMode( tagNS_SetPickMode* pCmd, DWORD );
	DWORD OnNS_ChangeLeader( tagNS_ChangeLeader* pCmd, DWORD );
	DWORD OnNS_RoleStateToTeam( tagNS_RoleStateToTeam* pCmd, DWORD );
	DWORD OnNS_RolePosToTeam( tagNS_RolePosToTeam* pCmd, DWORD );
	DWORD OnNS_RoleLevelChange( tagNS_RoleLevelChange* pCmd, DWORD );
	DWORD OnNS_RemoteAttChangeSingle( tagNS_RemoteAttChangeSingle* pCmd, DWORD );
	DWORD OnNS_RemoteAttChangeMutiple( tagNS_RemoteAttChangeMutiple* pCmd, DWORD );
	DWORD OnNS_GetTeamID( tagNS_GetTeamID* pCmd, DWORD );

	/**处理其他玩家的入队申请
	*/
	DWORD OnNS_RPWantToJoinTeam( tagNS_ApplyToJoinTeam* pCmd, DWORD );
	/**队长拒绝了你的入队申请
	*/
	DWORD OnNS_ApplyReplyRefuse( tagNS_ApplyReplyRefuse* pCmd, DWORD );

	/** \游戏事件响应
	*/
	DWORD  OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent);
	DWORD  OnInviteMsgBox( tagMsgBoxEvent* pEvent );
	DWORD  OnLeaveTeamMsgBox( tagMsgBoxEvent* pEvent );
	DWORD  OnKickMemberMsgBox( tagMsgBoxEvent* pEvent );
	DWORD  OnInviteJoinTeamEvent(tagInviteJoinTeamEvent* pEvent);

	/** \申请加入其他人的队伍
	*/
	DWORD  OnApplyJoinTeamEvent( tagApplyJoinTeamEvent* pEvent );
	/**远程玩家申请加入队伍，队长看到一个提示框
	*/
	DWORD OnApplyToJoinTeamMsgBox( tagMsgBoxEvent* pEvent );

private:
	TSFPTrunk<TeamSys>			m_trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameFrameMgr>       m_pGameFrameMgr;

	DWORD						m_dwTeamID;                 // 队伍ID
	vector<tagTeammateData>		m_vecTeammates;				// 队友RoleID列表
	DWORD                       m_dwLeaderRoleID;			// 另外保存一个队长的RoleID
	EPickMode                   m_ePickMode;				// 当前的拾取模式
	DWORD                       m_dwInviteRoleID;			// 邀请者的ID（用于获取邀请者名字）
	DWORD                       m_dwKickingMemberID;        // 正在驱除的队员（用于等待确认）
	BOOL                        m_bWaitForInviteRoleName;
	
	DWORD                       m_dwInviteMsgBoxShowTime;   // 打开邀请对话框的时间
	DWORD						m_dwWantToJoinTeamTime;		// 远程玩家申请加入对话框时间倒计时15秒

	ERefuseApplyMode			m_eRefuseApplyMode;			// 如队申请模式
	DWORD						m_dwApplyTeamRPId;			// 当前正在处理的，申请加入队伍的玩家ID
	//TMap<DWORD,INT>				m_mapApplyRP;				// 远程玩家申请加入队伍，队长依次同意或者拒绝
	TMap<DWORD,tagNS_ApplyToJoinTeam> m_mapApplyRP;
};
