/********************************************************************
	created:	2008/04/28
	created:	28:4:2008   18:03
	filename: 	d:\loong_client\Client\QuestMgr.h
	file path:	d:\loong_client\Client
	file base:	QuestMgr
	file ext:	h
	author:		leili
	
	purpose:	任务管理器声明
*********************************************************************/
#pragma once

#include "QuestData.h"
#include "QuestQuery.h"
#include "QuestScript.h"
#include "QuestEvent.h"
#include "MapEvent.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\quest.h"
#include "..\WorldDefine\msg_map.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\msg_script.h"
#include "..\WorldDefine\msg_gm_cmd.h"

class NetSession;
class QuestActiveFrame;
class QuestNPCFrame;
class QuestTrackFrame;
class QuestHelpFrame;

struct tagIDLink 
{
	tstring	mapName;
	DWORD	typeID;
};

struct tagPosLink 
{
	tstring	mapName;
	Vector3	pos;
};

enum ESpeRewType
{
	ESRT_REP			= 0,	//声望
	ESRT_CONTRIBUTION,			//贡献
	ESRT_ATT,					//属性
	ESRT_SKILL,					//技能
	ESRT_BUFF					//buff
};
struct tagSpeRew 
{
	ESpeRewType	rew;
	DWORD		type;
	INT			value;
};

class QuestMgr
{
public:
	QuestMgr(void);
	~QuestMgr(void);

	static QuestMgr*	Inst(void);

	BOOL				LoadFromFile();
	BOOL				Init(void);
	VOID				Destroy(void);
	VOID				Update(void);

	QuestData*			GetData(void)						{ return m_pData; }
	QuestQuery*			GetQuery(void)						{ return m_pQuery; }
	QuestScript*		GetScript(void)						{ return m_pScript; }

	VOID				HandleAcceptQuestHelp( UINT16 questID );
	VOID				HandleCompleteQuestHelp( UINT16 questID );
	VOID				AcceptQuest(UINT16 questID);
	VOID				CompleteQuest(UINT16 questID);
	VOID				DeleteQuest(UINT16 questID);
	VOID				SendAcceptQuest(UINT16 questID);
	VOID				SendCompleteQuest(UINT16 questID,int index);
	VOID				SendDeleteQuest(UINT16 questID);
	VOID				SendUpdateQuest(UINT16 questID);
	VOID				SendAcceptTriggerQuest( UINT16 questID );

	DWORD				GetTalkNPCGlobalID(void) const;
	DWORD				GetTalkNPCTypeID(void)	 const;
	VOID				ClearTalkNPC(void);
	VOID				OnNPCTalk(DWORD globalID, DWORD typeID);

	BOOL				TrackQuest(UINT16 questID,bool bShowTrack=true);

	VOID				OpenActiveFrame(void);
	VOID				CloseActiveFrame(void);
	BOOL                IsActiveFrameOpened(void);
	VOID				OpenNPCFrame(LPCTSTR szFather, LPCTSTR szFrame, LPCSTR szAlign, LPCSTR szControlName = NULL );
	VOID				CloseNPCFrame(LPCTSTR szFrame, bool bHide=false);
	QuestNPCFrame*		GetNPCFrame(LPCTSTR szFrame);
	VOID				OpenTrackFrame(void);
	VOID				CloseTrackFrame(void);
	BOOL                IsTrackFrameOpened(void);
	BOOL                IsQuestHelpFrameOpened(void);

	VOID				OpenQuestHelpFrame(void);
	VOID				CloseQuestHelpFrame(void);

	BOOL				MoveToMap(LPCTSTR szDstMap,Vector3& pos);

	tstring&			ParseQuestView(tstring& strView,vector<tagIDLink>& idLinks,vector<tagPosLink>& posLinks);
	VOID				SetSpeRew(const tagQuestProto* pQuestProto, vector<GUIStatic*>& bonusSpecT, vector<GUIStatic*>& bonusSpecV);
	
	static VOID			RegisterFrames(GameFrameMgr* pMgr);

private:
	DWORD				NetRecvMessage_IncompleteQuest(tagNS_GetRoleInitState_IncompleteQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_GetCompleteQuest(tagNS_GetRoleInitState_CompleteQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_AcceptQuest(tagNS_AcceptQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_CompleteQuest(tagNS_CompleteQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_DeleteQuest(tagNS_DeleteQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_UpdateQuestNPCTalk(tagNS_QuestUpdateNPCTalk* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_UpdateQuestTrigger(tagNS_QuestUpdateTrigger* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_UpdateKillCreature(tagNS_QuestUpdateKillCreature* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_UpdateItem(tagNS_QuestUpdateItem* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_UpdateInves(tagNS_QuestUpdateInveset* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_AddQuest(tagNS_AddQuest* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_SyncWalk(tagNS_SyncWalk* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_NS_BindRebornMap(tagNS_BindRebornMap* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_NS_MsgBlob(tagNS_MsgBlob* pMsg, DWORD pParam);
	DWORD				NetRecvMessage_NS_GMQuestState(tagNS_GMQuestState* pMsg, DWORD pParam);

	DWORD				OnEvent_NPCTalk(tagStartNPCTalkEvent* pEvent);
	DWORD				OnEvent_OpenTask(tagGameEvent* pEvent);
	DWORD				OnEvent_CloseTask(tagGameEvent* pEvent);
	DWORD				OnEvent_LoadNewMap(tagGameEvent* pEvent);
	DWORD				OnEvent_LPAttIsOK(tagGameEvent* pEvent);
	DWORD				OnEvent_LPLevelChanged(tagGameEvent* pEvent);
	DWORD				OnEvent_LPSkillChanged(tagGameEvent* pEvent);
	DWORD				OnEvent_LPItemChanged(tagGameEvent* pEvent);
	DWORD				OnEvent_LPMoneyChanged(tagGameEvent* pEvent);
	DWORD				OnEvent_LPReputeChanged(tagGameEvent* pEvent);
	DWORD				OnEvent_RoleEnterTrigger(tagEnterTriggerEvent* pEvent);
	DWORD				OnEvent_UseQuest(tagUseQuestEvent* pEvent);
	DWORD				OnSetRebornEvent(tagMsgBoxEvent* pEvent);

	VOID				CloseAllQuestWnd(void);
	VOID				UpdateQuest(void);

	VOID				CleanUpTrackQuest(float curTime);

private:
	TSFPTrunk<QuestMgr>		m_Trunk;
	TObjRef<VarContainer>	m_pVar;
	Util					*m_pUtil;
	GameScriptMgr			*m_pGSMgr;

	QuestData				*m_pData;
	QuestQuery				*m_pQuery;
	QuestScript				*m_pScript;
	QuestActiveFrame		*m_pActiveFrame;
	QuestTrackFrame			*m_pTrackFrame;
	QuestHelpFrame			*m_pQuestHelpFrame;

	typedef std::list<QuestNPCFrame*> NPCFrameList;
	NPCFrameList			m_NPCFrameList;

	struct tagNPCInfo 
	{
		DWORD	globalID;
		DWORD	typeID;
	};
	tagNPCInfo				m_curTalkNPC;

	enum
	{
		EF_Main		= 0,	// 主窗口
		EF_Active			// 固定活动窗口
	};

	static const int		BUTTON_COUNT = 2;
	static TCHAR*			m_szAccArray[BUTTON_COUNT];
	static BYTE				m_byAccArray[BUTTON_COUNT];

	DWORD					m_curTrigger;
	bool					m_bLPAttIsOK;

	float					m_lastUpdatetime;
};
