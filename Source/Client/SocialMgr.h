/********************************************************************
	created:	2008/12/10
	created:	10:12:2008   11:24
	filename: 	d:\loong_client\Client\SocialMgr.h
	file path:	d:\loong_client\Client
	file base:	SocialMgr
	file ext:	h
	author:		leili
	
	purpose:	好友管理器声明，处理网络消息
*********************************************************************/
#pragma once

#include "SocialData.h"
#include "..\WorldDefine\msg_social.h"
#include "..\WorldDefine\role_att.h"

class SocialReply;
class SocialListFrame;
struct tagAddFriendEvent;
struct tagAddBlackEvent;
struct tagUseGiftEvent;
struct tagRoleGetNameEvent;
struct tagNS_MountOnInvite;

class SocialMgr : public GameFrame
{
public:
	SocialMgr(void);
	virtual ~SocialMgr(void);

	// GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	static void  RegisterFrames(GameFrameMgr* pMgr);

	SocialData*  GetData(void) { return m_pData; }

	VOID		 AddFriend(DWORD roleID);
	VOID		 AddBlack(DWORD roleID);

	VOID		 SendMoveFriend(DWORD roleID, BYTE group);
	VOID		 SendAddFriend(DWORD roleID);
	VOID		 SendAddBlack(DWORD roleID);
	VOID		 SendDeleteFriend(DWORD roleID);
	VOID		 SendDeleteBlack(DWORD roleID);
	VOID		 SendGiftReply(DWORD giftID,BOOL bReceive);
	VOID		 SendGiveGiftTo(DWORD roleID,INT64 giftID);
	VOID		 SendMountReply(BOOL bReceive);

	VOID		 SetAutoReply(bool bEnable) { m_autoReply = bEnable; }
	bool		 IsAutoReply(void)	const	{ return m_autoReply; }
	LPCTSTR		 GetReply(void)		const;
	VOID		 SetReply(LPCTSTR szReply);
	VOID		 SetShowOutline(bool bEnable)	{ m_showOutline = bEnable; }

private:
	DWORD		 NetRecvMessage_SendFriendBlackList(tagNS_SendFriendBlackList* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_MoveToFriend(tagNS_UpdateFriGroup* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_MoveToBlackList(tagNS_MoveToBlackList* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_AddFriend(tagNS_RoleMakeFriend* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_DeleteFriend(tagNS_RoleCancelFriend* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_DeleteBlack(tagNS_DeleteBlackList* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_GetGift(tagNS_SendGiftToFri* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_SendGiftBroadcast(tagNS_SendGiftBroadcast* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_FriendLogIn(tagNS_LoginToFriend* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_FriendLogOut(tagNS_LogoutToFriend* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_UpdateFriendState(tagNS_UpdateFriState* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_SendGiftError(tagNS_SendGiftToSender* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_UpdateFriendValue(tagNS_UpdateFriValue* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_FriendNotice(tagNS_MakeFriNotice* pMsg, DWORD pParam);
	DWORD		 NetRecvMessage_MountOnInvite(tagNS_MountOnInvite* pMsg, DWORD pParam);

	DWORD		 OnEvent_OpenList(tagGameEvent* pEvent);
	DWORD		 OnEvent_CloseList(tagGameEvent* pEvent);
	DWORD		 OnEvent_AddFriend(tagAddFriendEvent* pEvent);
	DWORD		 OnEvent_AddBlack(tagAddBlackEvent* pEvent);
	DWORD		 OnEvent_UseGift(tagUseGiftEvent* pEvent);
	DWORD		 OnEvent_GiveGiftTo(tagMsgInputBoxEvent* pEvent);
	DWORD		 OnEvent_GetGift(tagMsgBoxEvent* pEvent);
	DWORD		 OnEvent_GetRoleName(tagRoleGetNameEvent* pEvent);
	DWORD		 OnEvent_MsgAddBlack(tagMsgBoxEvent* pEvent);
	DWORD		 OnEvent_Mount(tagMsgBoxEvent* pEvent);
	DWORD		 OnEvent_LPAttIsOK(tagGameEvent* pEvent);
	DWORD		 OnEvent_GetRoleIDError(tagGameEvent* pEvent);

	VOID		 ShowFriendLogInInfo(DWORD roleID, LPCTSTR szRoleName);
	VOID		 ShowFriendLogOutInfo(DWORD roleID, LPCTSTR szRoleName);
	VOID		 ShowGiftBroadcast(tagNS_SendGiftBroadcast* pMsg);
	VOID		 ShowFriendNotice(LPCTSTR szRoleName);
	VOID		 GiveGiftTo(DWORD roleID);
	VOID		 ShowGetGift(tagNS_SendGiftToFri* pMsg);
	VOID		 ShowMountInvite(tagNS_MountOnInvite* pMsg);

private:
	TSFPTrunk<SocialMgr>		m_Trunk;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pZoneSession;
	TObjRef<MsgInputBox>		m_pMsgInputBox;
	TObjRef<MsgBox>				m_pMsgBox;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	SocialListFrame				*m_pListFrame;
	SocialData					*m_pData;
	SocialReply					*m_pReply;

	INT64						m_giftGlobalID;
	DWORD						m_giftTypeID;
	bool						m_giftTiming;
	float						m_timeSelStart;
	DWORD						m_sendRoleID;
	bool						m_isUsingGift;

	typedef std::list<tagNS_SendGiftToFri> GiftList;
	GiftList					m_giftList;

	DWORD						m_mountRoleID;
	bool						m_inviteTiming;
	float						m_timeInvStart;
	typedef std::list<tagNS_MountOnInvite> MountList;
	MountList					m_mountList;

	bool						m_autoReply;
	bool						m_showOutline;

	DWORD						m_blackID;

	typedef std::list<DWORD> DelayName;
	DelayName					m_logInDelayNames;
	DelayName					m_logOutDelayNames;
	DelayName					m_noticeDelayNames;

	struct tagBC
	{
		DWORD	srcRoleID;
		DWORD	destRoleID;
		tagNS_SendGiftBroadcast	msg;
	};
	typedef std::list<tagBC> DelayBC;
	DelayBC						m_giftDelayBCs;

	typedef std::list<tstring>  DelayID;
	DelayID						m_giveGiftDelayIDs;

	typedef std::list<tagNS_SendGiftToFri> GetGiftName;
	GetGiftName					m_getGiftDelayNames;

	typedef std::list<tagNS_MountOnInvite> GetInviteName;
	GetInviteName				m_getMountDelayNames;
};