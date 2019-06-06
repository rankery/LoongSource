#pragma once
#include "gameframe.h"
#include "..\WorldDefine\msg_combat.h"
#include "..\WorldDefine\role_att.h"
struct tagUpdateLPLevelExp;
#include "ItemEvent.h"
#include "ItemProtoData.h"
#include "CurrencyMgr.h"
/** \class CombatFrame_MsgShow
	\brief 战斗信息显示
*/

class CombatFrame_MsgShow :
	public GameFrame
{
public:
	CombatFrame_MsgShow(void);
	virtual ~CombatFrame_MsgShow(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

private:
	//--接收战斗消息
	DWORD NetRecvMsgNS_RoleHPChange(tagNS_RoleHPChange* pMsg, DWORD dwParam);
	DWORD NetRecvMsgNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);
	DWORD NetRecvMsgNS_RoleDead(tagNS_RoleDead* pMsg, DWORD dwParam);
	DWORD NetRecvMsgNS_UseItem(tagNS_UseItem* pMsg, DWORD dwParam);

	DWORD OnUpdateLPLevelExp( tagUpdateLPLevelExp* pEvent );
	DWORD OnCombatMsg_ItemAdd( tagCombatMsgForItemChange* pEvent );
	DWORD OnCombatMsg_ItemRemove( tagCombatMsgForItemChange* pEvent );
	DWORD OnBag_Silver(tagRoleSilverChange* pEvent);

private:
	TSFPTrunk<CombatFrame_MsgShow>		m_trunk;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<Log>						m_pLog;
};
