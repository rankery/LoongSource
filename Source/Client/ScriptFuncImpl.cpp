/********************************************************************
	created:	2008/04/29
	created:	29:4:2008   13:48
	filename: 	d:\loong_client\Client\ScriptFuncImpl.cpp
	file path:	d:\loong_client\Client
	file base:	ScriptFuncImpl
	file ext:	cpp
	author:		leili
	
	purpose:	脚本函数接口实现，供策划调用
*********************************************************************/
#include "StdAfx.h"
#include "NPCTalkFrame.h"
#include "QuestTalkFrame.h"
#include "ShopShelfFrame.h"
#include "QuestMgr.h"
#include "RoleMgr.h"
#include "Role.h"
#include "Item.h"
#include "PlayerNameTab.h"
#include "LocalPlayer.h"
#include "SkillMgr.h"
#include "ItemMgr.h"
#include "CurrencyMgr.h"
#include "Container.h"
#include "MapMgr.h"
#include "CombatEvent.h"
#include "EffectMgr.h"
#include "TeamSys.h"
#include "..\WorldDefine\ScriptMsgInfo.h"
#include "BroadCastMgr.h"
#include "ItemProtoData.h"
#include "CreatureData.h"
#include "MapLogicData.h"
#include "CombatSysUtil.h"
#include "PersonalityActData.h"
#include "ItemEvent.h"
#include "ChatFrame.h"
#include "QuizFrame.h"
#include "ActivityPvPMgr.h"
#include "StyleActionFrame.h"

namespace
{
	NPCTalkFrame *GetNPCTalkFrame(BOOL bCreate = TRUE)
	{
		NPCTalkFrame *pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame( _T("NPCTalk") );
		if( !P_VALID(pFrame) && bCreate==TRUE )
		{
			pFrame = (NPCTalkFrame*)TObjRef<GameFrameMgr>()->CreateFrame( _T("World"), _T("NPCTalk"), _T("NPCTalkFrame"), 0 );
		}
		if( P_VALID(pFrame) )
		{
			Role *pCurRole = RoleMgr::Inst()->FindRole( QuestMgr::Inst()->GetTalkNPCGlobalID() );
			if( P_VALID(pCurRole) )
			{
				pFrame->SetCaption( pCurRole->GetRoleName().c_str() );
			}
			else
			{
				pFrame->SetCaption( _T("") );
			}
		}
		return pFrame;
	}

	QuestTalkFrame *GetQuestTalkFrame(BOOL bCreate = TRUE)
	{
		QuestTalkFrame *pFrame = (QuestTalkFrame*)TObjRef<GameFrameMgr>()->GetFrame( _T("QuestTalk") );
		if( !P_VALID(pFrame) && bCreate==TRUE )
		{
			pFrame = (QuestTalkFrame*)TObjRef<GameFrameMgr>()->CreateFrame( _T("World"), _T("QuestTalk"), _T("QuestTalkFrame"), 0 );
		}

		return pFrame;
	}

	QuizFrame *GetQuizFrame(BOOL bCreate = TRUE)
	{
		QuizFrame *pFrame = (QuizFrame*)TObjRef<GameFrameMgr>()->GetFrame( _T("QuizFrame") );
		if( !P_VALID(pFrame) && bCreate==TRUE )
		{
			pFrame = (QuizFrame*)TObjRef<GameFrameMgr>()->CreateFrame( _T("World"), _T("QuizFrame"), _T("QuizFrame"), 0 );
		}

		return pFrame;
	}
}

//-----------------------------------------------------------------------------
// GET SCRIPT OBJECT
// Retrieves the address of the C++ object managing the given lua_State.
//-----------------------------------------------------------------------------
GameScript* GetScriptObject(lua_State* l)
{
	lua_pushlightuserdata(l, l);
	lua_gettable(l, LUA_GLOBALSINDEX);
	return((GameScript*)lua_touserdata(l, -1));
}

//-----------------------------------------------------------------------------
// LuaConsolePrint
// Print a console string
// the stack.
//-----------------------------------------------------------------------------
int LuaConsolePrint(lua_State* l)
{
	LPCSTR szText = (char*)(lua_tostring(l, -1));	// UTF-8
	std::string str;
	if( !P_VALID(szText) )
	{
		str = luaL_typename(l, -1);
		str.append("[?]");
		szText = str.c_str();
	}

	TObjRef<Console>()->Print(TObjRef<Util>()->Unicode8ToUnicode(szText));
	return(0);
}

//-----------------------------------------------------------------------------
// LUA WAIT SEC
// Causes a script to wait for the given number of seconds.
//-----------------------------------------------------------------------------
int LuaWaitSec(lua_State* l)
{
	GameScript* s = GetScriptObject(l);
	FLOAT waitTime = (float)luaL_checknumber(l, 1);
	if (::abs(waitTime)>1.0e-5f)
	{
		s->SetWaitTime(waitTime);
		s->SetState(EGSS_WaitTime);

		return(lua_yield(l, 1));
	}
	return(0);
}

//-----------------------------------------------------------------------------
// 普通NPC对话框
//-----------------------------------------------------------------------------
int SC_NPCTalk(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	// 这两个参数备用
	INT p1 = (INT)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	switch( p1 )
	{
	case EMUT_DlgTarget_Item:
		{
			QuestMgr::Inst()->ClearTalkNPC();
		}
		break;
	}
	pFrame->SetTarget(p1,p2);

	const char *szText = "";
	if( lua_isnil(L,-1) == 0 )//内容不为nil
	{
		szText = static_cast<const char*>(lua_tostring(L, -1));
	}
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->SetContent(szContent);
#else
	pFrame->SetContent(szText);
#endif

	pFrame->Show();

	return 0;
}

//-----------------------------------------------------------------------------
// 清除普通NPC对话框所有选项
//-----------------------------------------------------------------------------
int SC_ClearNPCTalkOptions(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	pFrame->ClearOptions();

	return 0;
}

//-----------------------------------------------------------------------------
// 关闭普通NPC对话框
//-----------------------------------------------------------------------------
int SC_CloseNPCTalk(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame(FALSE);
	if( !P_VALID(pFrame) )
		return 0;

	pFrame->Hide();
	TObjRef<GameFrameMgr>()->AddToDestroyList(pFrame);

	return 0;
}

//-----------------------------------------------------------------------------
// 设置普通NPC对话框可接任务选项
//-----------------------------------------------------------------------------
int SC_SetAcceptOptions(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD npcID = (DWORD)(lua_tonumber(L,-1));
	pFrame->AddAcceptOptions(npcID);

	return 0;
}

//-----------------------------------------------------------------------------
// 设置普通NPC对话框可交任务选项
//-----------------------------------------------------------------------------
int SC_SetCompleteOptions(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD npcID = (DWORD)(lua_tonumber(L,-1));
	pFrame->AddCompleteOptions(npcID);

	return 0;
}

//-----------------------------------------------------------------------------
// 设置普通NPC对话框选项	通用版
//-----------------------------------------------------------------------------
int SC_SetNPCTalkOption(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	NPCTalkFrame::EOptionType optionType = (NPCTalkFrame::EOptionType)(lua_tointeger(L,-4));
	DWORD p1 = (DWORD)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	tstringstream strContent;
	if( optionType == NPCTalkFrame::EOT_ServerCtrl )
	{
		switch( p2 )
		{
		case EDO_A:
			strContent << _T("A.");
			break;
		case EDO_B:
			strContent << _T("B.");
			break;
		case EDO_C:
			strContent << _T("C.");
			break;
		case EDO_D:
			strContent << _T("D.");
			break;
		}
	}
	strContent << TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(strContent.str().c_str(), p1, p2, optionType);
#else
	pFrame->AddTalkOption(szText, p1, p2, optionType);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
// 设置接任务对话框内容
//-----------------------------------------------------------------------------
int SC_AcceptQuestTalk(lua_State *L)
{
	QuestTalkFrame *pFrame = GetQuestTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	UINT16 questID = (UINT16)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->SetContent(questID,szContent,true);
#else
	pFrame->SetContent(questID,szText,true);
#endif

	pFrame->SetCanSelect(false);
	pFrame->SetTrigger(false);

	pFrame->Show();

	return 0;
}

//-----------------------------------------------------------------------------
// 设置由触发器引起的接任务对话框内容
//-----------------------------------------------------------------------------
int SC_TriggerAcceptQuestTalk(lua_State *L)
{
	QuestTalkFrame *pFrame = GetQuestTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	UINT16 questID = (UINT16)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->SetContent(questID,szContent,true);
#else
	pFrame->SetContent(questID,szText,true);
#endif

	pFrame->SetCanSelect(false);
	pFrame->SetTrigger(true);

	pFrame->Show();

	return 0;
}

//-----------------------------------------------------------------------------
// 显示屏显
//-----------------------------------------------------------------------------
int SC_ShowScreenInfo(lua_State *L)
{
	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	CombatSysUtil::Inst()->ShowScreenCenterMsg( szContent );
#else
	CombatSysUtil::Inst()->ShowScreenCenterMsg( szText );
#endif

	return 0;
}

//-----------------------------------------------------------------------------
// 直接接取任务
//-----------------------------------------------------------------------------
int SC_AcceptQuest(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));
	QuestMgr::Inst()->AcceptQuest( questID );

	return 0;
}

//-----------------------------------------------------------------------------
// 设置交任务对话框内容
//-----------------------------------------------------------------------------
int SC_CompleteQuestTalk(lua_State *L)
{
	QuestTalkFrame *pFrame = GetQuestTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	UINT16 questID = (UINT16)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->SetContent(questID,szContent,false);
#else
	pFrame->SetContent(questID,szText,false);
#endif

	pFrame->SetCanSelect(true);
	pFrame->SetTrigger(false);

	pFrame->Show();

	return 0;
}

//-----------------------------------------------------------------------------
// 设置由触发器引起的交任务对话框内容
//-----------------------------------------------------------------------------
int SC_TriggerCompleteQuestTalk(lua_State *L)
{
	QuestTalkFrame *pFrame = GetQuestTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	UINT16 questID = (UINT16)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->SetContent(questID,szContent,false);
#else
	pFrame->SetContent(questID,szText,false);
#endif

	pFrame->SetCanSelect(true);
	pFrame->SetTrigger(true);

	pFrame->Show();

	return 0;
}

//-----------------------------------------------------------------------------
// 直接完成任务
//-----------------------------------------------------------------------------
int SC_CompleteQuest(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));
	QuestMgr::Inst()->CompleteQuest( questID );

	return 0;
}

//-----------------------------------------------------------------------------
// 直接更新任务
//-----------------------------------------------------------------------------
int SC_UpdateQuest(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));
	QuestMgr::Inst()->SendUpdateQuest( questID );

	return 0;
}

//-----------------------------------------------------------------------------
// 获得角色名称
//-----------------------------------------------------------------------------
int SC_GetPlayerName(lua_State *L)
{
	std::string szRet = "";
	LocalPlayer *pLP=RoleMgr::Inst()->GetLocalPlayer();
	if(pLP!=NULL)
	{
#ifdef UNICODE
		szRet = TObjRef<Util>()->UnicodeToUnicode8(pLP->GetRoleName().c_str());
#else
		szRet = pLP->GetRoleName().c_str();
#endif
	}

	lua_pushstring(L,szRet.c_str());

	return 1;
}

//-----------------------------------------------------------------------------
// 根据类型和索引从stringtable获得值
//-----------------------------------------------------------------------------
int SC_GetMsgValue(lua_State *L)
{
	std::string szRet = "";
	LPCTSTR		szMsg = NULL;

	EMsgUnitType msgType = (EMsgUnitType)(lua_tointeger(L,-2));
	DWORD msgID = (DWORD)(lua_tointeger(L,-1));

	switch( msgType )
	{
	case EMUT_DlgWithSelOpt:
	case EMUT_DlgQuiz:
	case EMUT_DlgOption:
	case EMUT_MsgID:
	case EMUT_Boardcast:
	case EMUT_DlgTarget_Activity:
	case EMUT_ErrorCode:
		{
			szMsg = TObjRef<GameScriptMgr>()->GetMsg(msgID);
			if( szMsg != NULL && _tcslen(szMsg)>0 )
			{
#ifdef UNICODE
				szRet = TObjRef<Util>()->UnicodeToUnicode8(g_StrTable[szMsg]);
#else
				szRet = g_StrTable[szMsg];
#endif
			}
		}
		break;
	case EMUT_QuestID:
		{
			szMsg = TObjRef<GameScriptMgr>()->GetQuestDesc(msgID);
			if( szMsg != NULL )
			{
#ifdef UNICODE
				szRet = TObjRef<Util>()->UnicodeToUnicode8(szMsg);
#else
				szRet = szMsg;
#endif
			}
		}
		break;
	case EMUT_RoleID:
		{
			szMsg = PlayerNameTab::Inst()->FindNameByID(msgID);
			if( szMsg==NULL || _tcslen(szMsg)==0 )
			{
				char szBuff[X_SHORT_NAME] = {0};
				sprintf( szBuff, "<R=%u>", msgID );
				lua_pushstring(L, szBuff);
				return 1;
			}
#ifdef UNICODE
			szRet = TObjRef<Util>()->UnicodeToUnicode8(szMsg);
#else
			szRet = szMsg;
#endif
		}
		break;
	case EMUT_GuildID:
		{
			//tstring strGuildName = GuildMgr::Inst()->GetGuildName(msgID);
			//szMsg = strGuildName.c_str();
			//bool bGet = GuildMgr::Inst()->IsGuildNameExist(msgID);
			//if (bGet == false)
			//{
				char szBuff[X_SHORT_NAME] = {0};
				sprintf(szBuff, "<G=%u>", msgID);
				lua_pushstring(L, szBuff);
				return 1;
			//}
#ifdef UNICODE
			szRet = TObjRef<Util>()->UnicodeToUnicode8(szMsg);
#else
			szRet = szMsg;
#endif
		}
		break;
	case EMUT_ItemTypeID:
		{
			const tagItemDisplayInfo* pItem = ItemProtoData::Inst()->FindItemDisplay(msgID);
			if( !P_VALID(pItem) ) 
				return 0;
#ifdef UNICODE
			szRet = TObjRef<Util>()->UnicodeToUnicode8(pItem->szName);
#else
			szRet = pItem->szName;
#endif
		}
		break;
	case EMUT_NPCTypeID:
		{
			const tagCreatureProto* pCreature = CreatureData::Inst()->FindNpcAtt(msgID);
			if( !P_VALID(pCreature) )
				return 0;
#ifdef UNICODE
			szRet = TObjRef<Util>()->UnicodeToUnicode8(pCreature->szName);
#else
			szRet = pCreature->szName;
#endif
		}
		break;
	case EMUT_MapCrc:		
		{
			szMsg = MapLogicData::Inst()->FindMapShowName(msgID);
			if( szMsg != NULL && _tcslen(szMsg)>0 )
			{
#ifdef UNICODE
				szRet = TObjRef<Util>()->UnicodeToUnicode8(szMsg);
#else
				szRet = szMsg;
#endif
			}	
		}
		break;

	case EMUT_Integer:
	case EMUT_Num:
		{
			CHAR szInt[X_SHORT_NAME] = {0};
			sprintf( szInt, "%d", msgID );
			szRet = szInt;
		}
		break;
	}
	lua_pushstring(L,szRet.c_str());

	return 1;
}

//-----------------------------------------------------------------------------
// 获得一个伪随机数
//-----------------------------------------------------------------------------
int SC_GetRandom(lua_State *L)
{
	DWORD maxNum = (DWORD)(lua_tonumber(L,-1));

	srand( (unsigned)time( NULL ) );
	int random = rand() % (maxNum+1);

	lua_pushinteger(L,random);

	return 1;
}

//-----------------------------------------------------------------------------
// 某个任务是否已完成
//-----------------------------------------------------------------------------
int SC_QuestIsCompleted(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) );

	lua_pushboolean(L,pQuery->QuestIsCompleted(questID));

	return 1;
}

//-----------------------------------------------------------------------------
// 某个任务是否能完成
//-----------------------------------------------------------------------------
int SC_QuestCanComplete(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) );

	lua_pushboolean(L,pQuery->QuestCanComplete(questID));

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家性别
//-----------------------------------------------------------------------------
int SC_GetPlayerSex(lua_State *L)
{
	LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();

	lua_pushinteger(L,pPlayer->GetPlayerSex());

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家背包某物品的数量 普通+任务+装备
//-----------------------------------------------------------------------------
int SC_GetItemNum(lua_State *L)
{
	DWORD typeID = (DWORD)(lua_tonumber(L,-1));

	ItemContainer *pPocket = ItemMgr::Inst()->GetPocket();
	ASSERT( P_VALID(pPocket) );

	ItemContainer *pQuest = ItemMgr::Inst()->GetQuest();
	ASSERT( P_VALID(pQuest) );

	ItemContainer *pEquipment = ItemMgr::Inst()->GetEquipment();
	ASSERT( P_VALID(pEquipment) );

	int num = pPocket->GetItemQuantity(typeID) + pQuest->GetItemQuantity(typeID) + pEquipment->GetItemQuantity(typeID);

	lua_pushinteger(L,num);

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家背包金钱
//-----------------------------------------------------------------------------
int SC_GetPlayerMoney(lua_State *L)
{
	int silver = (int)CurrencyMgr::Inst()->GetBagSilver();

	lua_pushinteger(L,silver);

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家等级
//-----------------------------------------------------------------------------
int SC_GetPlayerLevel(lua_State *L)
{
	LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();
	ASSERT( P_VALID(pPlayer) );

	lua_pushinteger(L,pPlayer->GetRoleLevel());

	return 1;
}

//-----------------------------------------------------------------------------
// 获得当前地图名称
//-----------------------------------------------------------------------------
int SC_GetMapName(lua_State *L)
{
	std::string szRet = "";
	
#ifdef UNICODE
	szRet = TObjRef<Util>()->UnicodeToUnicode8(MapMgr::Inst()->GetCurMapName().c_str());
#else
	szRet = MapMgr::Inst()->GetCurMapName().c_str();
#endif

	lua_pushstring(L,szRet.c_str());

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家属性
//-----------------------------------------------------------------------------
int SC_GetPlayerAttribute(lua_State *L)
{
	int typeID = (int)(lua_tonumber(L,-1));

	LocalPlayer *pPlayer = RoleMgr::Inst()->GetLocalPlayer();

	lua_pushinteger(L,pPlayer->GetAttribute((ERoleAttribute)typeID));

	return 1;
}

//-----------------------------------------------------------------------------
// 判断是否有足够的金钱
//-----------------------------------------------------------------------------
int SC_MoneyIsEnough(lua_State *L)
{
	int requireMoney = (int)lua_tointeger(L,-1);

	BOOL bIsEnough = FALSE;
	if( CurrencyMgr::Inst()->GetBagSilver() >= requireMoney )
	{
		bIsEnough = TRUE;
	}

	lua_pushboolean( L, bIsEnough );

	return 1;
}

//-----------------------------------------------------------------------------
// 某个任务是否已接取
//-----------------------------------------------------------------------------
int SC_QuestIsAccepted(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-1));

	QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
	ASSERT( P_VALID(pQuery) );

	lua_pushboolean(L,pQuery->QuestIsAccepted(questID));

	return 1;
}

//-----------------------------------------------------------------------------
// 本地玩家拥有指定 并且 大于等于指定等级 的技能
//-----------------------------------------------------------------------------
int SC_PlayerHasSkill(lua_State *L)
{
	DWORD skillID = (DWORD)(lua_tonumber(L,-2));
	INT level = (INT)(lua_tonumber(L,-1));

	const tagSkillData *pSkill = SkillMgr::Inst()->GetSkillDataByTypeID(skillID);

	BOOL ret = FALSE;
	if( P_VALID(pSkill) )
	{
		if( pSkill->nLevel >= level )
			ret = TRUE;
	}

	lua_pushboolean(L,ret);

	return 1;
}

//-----------------------------------------------------------------------------
// 播放指定个性动作
//-----------------------------------------------------------------------------
int SC_PlayEmotionAct(lua_State *L)
{
	DWORD actID = (DWORD)(lua_tonumber(L,-1));

	const tagPersonAct* pAct = PersonalityActData::Inst()->FindPersonAct(actID);
	if( !P_VALID( pAct ) )
		return 0;

	tagTryToPlayPersonalityActEvent act(_T("tagTryToPlayPersonalityActEvent"), NULL, pAct);
	TObjRef<GameFrameMgr>()->SendEvent(&act);

	return 0;
}

//-----------------------------------------------------------------------------
// 在指定NPC身上播放特效
//-----------------------------------------------------------------------------
int SC_PlayEffect(lua_State *L)
{
	DWORD npcID = (DWORD)(lua_tonumber(L,-2));

	if( !P_VALID(npcID) )//在本地玩家身上播放
	{
		npcID = RoleMgr::Inst()->GetLocalPlayerID();
	}

	DWORD ret = GT_INVALID;

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szEffect = TObjRef<Util>()->Unicode8ToUnicode(szText);
	ret = EffectMgr::Inst()->PlayRoleEffect( npcID, szEffect );
#else
	ret = EffectMgr::Inst()->PlayRoleEffect( npcID, szText );
#endif

	lua_pushnumber(L,ret);

	return 1;
}

//-----------------------------------------------------------------------------
// 在指定NPC身上停止播放特效（用于循环特效）
//-----------------------------------------------------------------------------
int SC_StopEffect(lua_State *L)
{
	DWORD effectID = (DWORD)(lua_tonumber(L,-1));

	EffectMgr::Inst()->StopEffect( effectID );

	return 0;
}

//-----------------------------------------------------------------------------
// 获得当前服务器时间
//-----------------------------------------------------------------------------
int SC_GetServerTime(lua_State *L)
{
	// todo 返回字符串？

	return 1;
}

//-----------------------------------------------------------------------------
// 获得玩家当前位置所在触发器ID
//-----------------------------------------------------------------------------
int SC_GetTriggerID(lua_State *L)
{
	DWORD triggerID = MapMgr::Inst()->GetCurTriggerID();

	lua_pushnumber(L,triggerID);

	return 1;
}

//-----------------------------------------------------------------------------
// 获得当前队伍人数
//-----------------------------------------------------------------------------
int SC_GetTeamNum(lua_State *L)
{
	INT teamNum = 1;

	if( TeamSys::Inst()->IsInTeam() )
	{
		teamNum += TeamSys::Inst()->GetTeammateNum();
	}

	lua_pushinteger(L,teamNum);

	return 1;
}

//-----------------------------------------------------------------------------
// 开始倒计时
//-----------------------------------------------------------------------------
int SC_StartCountDown(lua_State *L)
{
	// todo 

	return 0;
}

//-----------------------------------------------------------------------------
// 获得倒计时的剩余时间
//-----------------------------------------------------------------------------
int SC_GetRemanentTime(lua_State *L)
{
	// todo

	return 1;
}

//-----------------------------------------------------------------------------
// 获得服务器端随机数的值
//-----------------------------------------------------------------------------
int SC_GetServerRandom(lua_State *L)
{
	// todo 

	return 1;
}

//-----------------------------------------------------------------------------
// 获得指定任务的当前可接次数
//-----------------------------------------------------------------------------
int SC_GetTaskDoneTime(lua_State *L)
{
	// todo

	return 1;
}


//-----------------------------------------------------------------------------
// 聊天频道显示
//-----------------------------------------------------------------------------
int SC_ShowChatChannelInfo(lua_State *L)
{
	const char *szText = "";
	szText = static_cast<const char*>(lua_tostring(L, -3));
	DWORD param1 = (DWORD)(lua_tonumber(L, -2));
	ESendChatChannel channel;
	switch (param1)
	{
	case 1:
		channel = ESCC_Affiche;
		break;
	default:
		channel = ESCC_Current;
		break;
	}
	DWORD param2 = (DWORD)(lua_tonumber(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	BroadCastMgr::Inst()->ToChatChannel(szContent, channel, param2 == 1);
#else
	BroadCastMgr::Inst()->ToChatChannel(szText, channel, param2 == 1);
#endif
	return 0;
}


//-----------------------------------------------------------------------------
// 客户端显示信息
//-----------------------------------------------------------------------------
int SC_ShowInfo(lua_State *L)
{
	DWORD infoType = (DWORD)(lua_tonumber(L,-3));

	const char *szText = "";
	if( lua_isnil(L,-1) == 0 )//内容不为nil
	{
		szText = static_cast<const char*>(lua_tostring(L, -2));
	}

	DWORD param1 = (DWORD)(lua_tonumber(L,-1));

	switch( infoType )
	{
	case EMUT_ChatInfo://聊天频道信息
		{
			ChatFrame* pFrame = (ChatFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Chat"));

			ESendChatChannel channel;
			switch( param1 )
			{
			case 1:
				channel = ESCC_Affiche;
				break;
			default:
				channel = ESCC_Current;
			}
#ifdef UNICODE
			const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
			pFrame->PushInfo( szContent, channel );
#endif
		}
		break;

	case EMUT_Boardcast://广播信息
		{
#ifdef UNICODE
			const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
			BroadCastMgr::Inst()->BroadCast(szContent,param1==1);
#else
			BroadCastMgr::Inst()->BroadCast(szContent,param1==1);
#endif
		}
		break;
	}

	return 0;
}

int SC_GetTriggerPos(lua_State *L)
{
	DWORD triggerID = (DWORD)(lua_tonumber(L,-1));

	Vector3 triggerPos(0.0f,0.0f,0.0f);
	MapMgr::Inst()->GetTriggerPos(triggerID, triggerPos);

	lua_pushnumber(L,triggerPos.x);
	lua_pushnumber(L,triggerPos.z);

	return 2;
}

int SC_SetQuestDesc(lua_State *L)
{
	UINT16 questID = (UINT16)(lua_tonumber(L,-2));

	const char *szText = "";
	if( lua_isnil(L,-1) == 0 )//内容不为nil
	{
		szText = static_cast<const char*>(lua_tostring(L, -1));
	}
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	QuestMgr::Inst()->GetData()->SetQuestDesc( questID, szContent );
#endif

	return 0;
}

//-----------------------------------------------------------------------------
// 解析数据类型
//-----------------------------------------------------------------------------
int SC_ParseType(lua_State *L)
{
	const BYTE *pData = TObjRef<GameScriptMgr>()->GetData();
	DWORD parsePos = TObjRef<GameScriptMgr>()->GetParsePos();
	DWORD dataSize = TObjRef<GameScriptMgr>()->GetDataSize();

	const BYTE *pTemp = pData + parsePos;
	if( parsePos < dataSize )
	{
		EMsgUnitType eMsgUnitType = *(EMsgUnitType*)pTemp;
		TObjRef<GameScriptMgr>()->AddParsePos(4);
		lua_pushnumber(L,(lua_Integer)eMsgUnitType);
	}
	else
		lua_pushnil(L);

	return 1;
}

//-----------------------------------------------------------------------------
// 解析DWORD数据
//-----------------------------------------------------------------------------
int SC_ParseDWORD(lua_State *L)
{
	const BYTE *pData = TObjRef<GameScriptMgr>()->GetData();
	DWORD parsePos = TObjRef<GameScriptMgr>()->GetParsePos();
	DWORD dataSize = TObjRef<GameScriptMgr>()->GetDataSize();

	const BYTE *pTemp = pData + parsePos;
	if( parsePos < dataSize )
	{
		DWORD val = *(DWORD*)pTemp;
		TObjRef<GameScriptMgr>()->AddParsePos(4);
		lua_pushinteger(L,(lua_Integer)val);
	}
	else
		lua_pushnil(L);

	return 1;
}

int SC_ParseINT(lua_State *L)
{
	const BYTE *pData = TObjRef<GameScriptMgr>()->GetData();
	DWORD parsePos = TObjRef<GameScriptMgr>()->GetParsePos();
	DWORD dataSize = TObjRef<GameScriptMgr>()->GetDataSize();

	const BYTE *pTemp = pData + parsePos;
	if( parsePos < dataSize )
	{
		INT val = *(INT*)pTemp;
		TObjRef<GameScriptMgr>()->AddParsePos(4);
		lua_pushinteger(L,(lua_Integer)val);
	}
	else
		lua_pushnil(L);

	return 1;
}

//-----------------------------------------------------------------------------
// 解析string数据
//-----------------------------------------------------------------------------
int SC_ParseString(lua_State *L)
{
	const BYTE *pData = TObjRef<GameScriptMgr>()->GetData();
	DWORD parsePos = TObjRef<GameScriptMgr>()->GetParsePos();
	DWORD dataSize = TObjRef<GameScriptMgr>()->GetDataSize();

	const BYTE *pTemp = pData + parsePos;
	if( parsePos < dataSize )
	{
		INT strSize = _tcsclen((TCHAR*)pData);
		TCHAR *szVal = new TCHAR[strSize + 1];
		_tcsncpy( szVal, (TCHAR*)pTemp, strSize * sizeof(TCHAR) );
		szVal[strSize] = _T('\0');
		std::string strVal = TObjRef<Util>()->UnicodeToUnicode8( szVal );
		lua_pushstring(L,strVal.c_str());
		SAFE_DEL_ARRAY(szVal);
		TObjRef<GameScriptMgr>()->AddParsePos( strSize+1 );
	}
	else
		lua_pushnil(L);

	return 1;
}

// 设置复活点
int SC_SetReborn(lua_State *L)
{
	const char *szText = static_cast<const char*>(lua_tostring(L, -3));
	const char *szText3 = static_cast<const char*>(lua_tostring(L, -1));

	if( P_VALID(GetObj("SetRebornMsgBox")) )
		KillObj("SetRebornMsgBox");
	CreateObj("SetRebornMsgBox", "MsgBox");

	tstring strCaption = TObjRef<Util>()->Unicode8ToUnicode(szText);
	tstring strContent = TObjRef<Util>()->Unicode8ToUnicode(szText3);	
	
	TObjRef<MsgBox>("SetRebornMsgBox")->Init(strCaption.c_str(), strContent.c_str(), 
		_T("SetRebornEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);


	return 0;
}

//------------------------------------------------------------------------------
// 打开商店界面
//------------------------------------------------------------------------------
int SC_OpenShop(lua_State *L)
{
	tagGameEvent event(_T("Open_Shop"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//------------------------------------------------------------------------------
// 打开仓库界面
//------------------------------------------------------------------------------
int SC_OpenWare(lua_State *L)
{
	tagGameEvent event(_T("Open_Ware"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 行囊扩容
//-----------------------------------------------------------------------------
int SC_BagExtend(lua_State *L)
{
	tagItemContainerExtend event(_T("Bag_Extend"), NULL);
	event.bUseSilver = lua_toboolean(L, -1);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 仓库扩容
//-----------------------------------------------------------------------------
int SC_WareExtend(lua_State *L)
{
	tagItemContainerExtend event(_T("Ware_Extend"), NULL);
	event.bUseSilver = lua_toboolean(L, -1);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 帮派创建
//-----------------------------------------------------------------------------
int SC_GuildCreate(lua_State *L)
{
	tagGameEvent event(_T("GuildEvent_Create"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 帮派解散
//-----------------------------------------------------------------------------
int SC_GuildDismiss(lua_State *L)
{
	tagGameEvent event(_T("GuildEvent_Dismiss"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 打开帮派事务界面
//-----------------------------------------------------------------------------
int SC_OpenGuildAffair(lua_State *L)
{
	tagGameEvent event(_T("Open_GuildAffair"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
};

//-----------------------------------------------------------------------------
// 打开接取商银界面
//-----------------------------------------------------------------------------
int SC_OpenPickUpSilver(lua_State *L)
{
	tagGameEvent event(_T("Open_PickUpSilver"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 打开帮派仓库界面
//-----------------------------------------------------------------------------
int SC_OpenGuildStorage(lua_State *L)
{
    tagGameEvent event(_T("OpenGuildStorage"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

//-----------------------------------------------------------------------------
// 打开帮派仓库设置权限界面
//-----------------------------------------------------------------------------
int SC_OpenGuildStoragePermission(lua_State *L)
{
    tagGameEvent event(_T("OpenGuildStoragePermission"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

//-----------------------------------------------------------------------------
// 打开上缴商银界面
//-----------------------------------------------------------------------------
int SC_OpenContributeSilver(lua_State *L)
{
	tagGameEvent event(_T("Open_ContributeSilver"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 放弃跑商
//-----------------------------------------------------------------------------
int SC_QuitCommerce(lua_State *L)
{
	tagGameEvent event(_T("QuitCommerce"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 打开商会界面
//-----------------------------------------------------------------------------
int SC_OpenCOC(lua_State *L)
{
	tagGameEvent event(_T("Open_COC"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}

//-----------------------------------------------------------------------------
// 打开跑商排行榜
//-----------------------------------------------------------------------------
int SC_OpenCommerceRank(lua_State *L)
{
	tagGameEvent event(_T("Open_CommerceRank"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
}


//-----------------------------------------------------------------------------
// 帮派升级设施
//-----------------------------------------------------------------------------
int SC_GuildUpgrade(lua_State *L)
{
	tagGameEvent event(_T("GuildEvent_Upgrade"), NULL);
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	return 0;
};

//-----------------------------------------------------------------------------
// 本地玩家是否是帮派成员
//-----------------------------------------------------------------------------
int SC_IsInGuild(lua_State *L)
{
	lua_pushboolean(L, RoleMgr::Inst()->GetLocalPlayer()->IsInGuild());
	if (RoleMgr::Inst()->GetLocalPlayer()->IsInGuild() == false)
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("GuildError_Code10357")]);
	return 1;
}



int SC_SetCommerceOption(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if (!P_VALID(pFrame))
		return 0;
	DWORD p1 = (DWORD)(lua_tonumber(L, -3));
	DWORD p2 = (DWORD)(lua_tonumber(L, -2));
	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(szContent, p1, p2, NPCTalkFrame::EOT_GuildCommerce);
#else
	pFrame->AddTalkOption(szText, p1, p2, NPCTalkFrame::EOT_GuildCommerce);
#endif
	return 0;
}

//-----------------------------------------------------------------------------
// 设置生产大师NPC对话框选项
//-----------------------------------------------------------------------------
int SC_SetProduceOptions(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD p1 = (DWORD)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(szContent, p1, p2, NPCTalkFrame::EOT_ProduceSkillTalk);
#else
	pFrame->AddTalkOption(szText, p1, p2, NPCTalkFrame::EOT_ProduceSkillTalk);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//	学习生产技能
//-----------------------------------------------------------------------------
int SC_LearnProduceSkill(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD p1 = (DWORD)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(szContent, p1, p2, NPCTalkFrame::EOT_LearnProduceSkill);
#else
	pFrame->AddTalkOption(szText, p1, p2, NPCTalkFrame::EOT_LearnProduceSkill);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//	遗忘生产技能
//-----------------------------------------------------------------------------
int SC_LeaveProduceSkill(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD p1 = (DWORD)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(szContent, p1, p2, NPCTalkFrame::EOT_LeaveProduceSkill);
#else
	pFrame->AddTalkOption(szText, p1, p2, NPCTalkFrame::EOT_LeaveProduceSkill);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//	设置学习\遗忘时返回
//-----------------------------------------------------------------------------
int SC_SetCancelProduceSkill(lua_State *L)
{
	NPCTalkFrame *pFrame = GetNPCTalkFrame();
	if( !P_VALID(pFrame) )
		return 0;

	DWORD p1 = (DWORD)(lua_tonumber(L,-3));
	DWORD p2 = (DWORD)(lua_tonumber(L,-2));

	const char *szText = static_cast<const char*>(lua_tostring(L, -1));
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szText);
	pFrame->AddTalkOption(szContent, p1, p2, NPCTalkFrame::EOT_CancelProduceSkill);
#else
	pFrame->AddTalkOption(szText, p1, p2, NPCTalkFrame::EOT_CancelProduceSkill);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//	打印整型信息
//-----------------------------------------------------------------------------
int SC_Print(lua_State *L)
{
	INT info = static_cast<INT>(lua_tointeger(L, -1));
	IMSG(_T("%d\r\n"), info);

	return 0;
}

//-----------------------------------------------------------------------------
//	发送服务器控制消息
//-----------------------------------------------------------------------------
int SC_ServerCtrlTalk(lua_State *L)
{
	tagNC_NPCTalk msg;
	msg.dwNPCId	= QuestMgr::Inst()->GetTalkNPCGlobalID();
	TObjRef<NetSession>()->Send(&msg);

	return 0;
}

//-----------------------------------------------------------------------------
//	清空答题界面
//-----------------------------------------------------------------------------
int SC_QuizClear(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame();
	if( P_VALID(pFrame) )
	{
		pFrame->Clear();
	}

	return 0;
}

//-----------------------------------------------------------------------------
//	设置答题倒计时
//-----------------------------------------------------------------------------
int SC_QuizSetCD(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame();
	if( P_VALID(pFrame) )
	{
		DWORD cd = (DWORD)(lua_tonumber(L, -1));
		pFrame->SetCD(cd);
	}

	return 0;
}

//-----------------------------------------------------------------------------
//	设置答题数量
//-----------------------------------------------------------------------------
int SC_QuizSetNum(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame();
	if( P_VALID(pFrame) )
	{
		DWORD num = (DWORD)(lua_tonumber(L, -1));
		pFrame->SetNum(num);
	}

	return 0;
}

//-----------------------------------------------------------------------------
//	设置答题选项
//-----------------------------------------------------------------------------
int SC_QuizSetOption(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame();
	if( !P_VALID(pFrame) )
		return 0;

	const char *szText = static_cast<const char*>(lua_tostring(L, -3));
	DWORD p1 = (DWORD)(lua_tonumber(L,-2));
	DWORD p2 = (DWORD)(lua_tonumber(L,-1));

#ifdef UNICODE
	pFrame->SetOption(TObjRef<Util>()->Unicode8ToUnicode(szText), p1, p2);
#else
	pFrame->SetOption(szText, p1, p2);
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//	显示答题界面
//-----------------------------------------------------------------------------
int SC_QuizShow(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame();
	if( P_VALID(pFrame) )
	{
		DWORD p1 = (DWORD)(lua_tonumber(L,-3));
		DWORD p2 = (DWORD)(lua_tonumber(L,-2));
		const char *szText = static_cast<const char*>(lua_tostring(L, -1));

		pFrame->SetTarget(p1,p2);

#ifdef UNICODE
		pFrame->SetContent(TObjRef<Util>()->Unicode8ToUnicode(szText));
#else
		pFrame->SetContent(szText);
#endif

		pFrame->Show();
	}

	return 0;
}

//-----------------------------------------------------------------------------
//	关闭答题界面
//-----------------------------------------------------------------------------
int SC_QuizClose(lua_State *L)
{
	QuizFrame *pFrame = GetQuizFrame(FALSE);
	if( P_VALID(pFrame) )
	{
		pFrame->Hide();
		TObjRef<GameFrameMgr>()->AddToDestroyList(pFrame);
	}

	return 0;
}

//-----------------------------------------------------------------------------
//以下为PvP活动
//-----------------------------------------------------------------------------
int SC_PvPSetActivityCD(lua_State *L)
{
	DWORD pvpCD = (DWORD)lua_tonumber(L,-1);
	ActivityPvPMgr::Inst()->SetActivityCD( pvpCD );
	return 0;
}

int SC_PvPSetActivityID(lua_State *L)
{
	DWORD pvpID = (DWORD)lua_tonumber(L,-1);

	const char *szPvPName = "";
	if( lua_isnil(L,-2) == 0 )	//内容不为nil
	{
		szPvPName = static_cast<const char*>(lua_tostring(L, -2));
	}
#ifdef UNICODE
	const TCHAR *szContent = TObjRef<Util>()->Unicode8ToUnicode(szPvPName);
	ActivityPvPMgr::Inst()->SetActivityName( szContent, pvpID );
#endif

	return 0;
}

int SC_PvPSetActivityScore(lua_State *L)
{
	DWORD pvpScore = (DWORD)lua_tonumber(L,-1);
	ActivityPvPMgr::Inst()->SetActivityScore( pvpScore );
	return 0;
}

int SC_PvPSetActivityRebornTime(lua_State *L)
{
	DWORD pvpReborn = (DWORD)lua_tonumber(L,-1);
	ActivityPvPMgr::Inst()->SetActivityRebornTime(pvpReborn);
	return 0;
}

int SC_PvPSetActivityTeamID(lua_State *L)
{
	DWORD pvpTeamID = (DWORD)lua_tonumber(L,-1);
	ActivityPvPMgr::Inst()->SetActivityTeamID(pvpTeamID);
	return 0;
}

int SC_PvPSetActivityType(lua_State *L)
{
	DWORD pvpType = (DWORD)lua_tonumber(L, -1);
	ActivityPvPMgr::Inst()->SetActivityType(pvpType);
	return 0;
}

int SC_PvPScoreSort(lua_State *L)
{
	DWORD dwRank = (DWORD)lua_tonumber(L, -3);
	DWORD dwRoleID = (DWORD)lua_tonumber(L, -2);
	DWORD dwScore = (DWORD)lua_tonumber(L, -1);
	ActivityPvPMgr::Inst()->SetPvPActivityScoreSort(dwRank, dwRoleID, dwScore);
	return 0;
}

int SC_PvPMultiScoreSort(lua_State *L)
{
	DWORD dwRank = (DWORD)lua_tonumber(L, -4);
	DWORD dwRoleID = (DWORD)lua_tonumber(L, -3);
	DWORD dwScore = (DWORD)lua_tonumber(L, -2);
	DWORD dwTeamID = (DWORD)lua_tonumber(L, -1);
	ActivityPvPMgr::Inst()->SetPvPMultiActivityScoreSort(dwRank, dwRoleID, dwScore, dwTeamID);
	return 0;
}

int SC_PvPScoreSortInfo(lua_State *L)
{
	ActivityPvPMgr::Inst()->RefreshScoreSort();
	return 0;
}

int SC_SetClass(lua_State *L)
{
    tagGameEvent event(_T("OpenSetClass"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

int SC_ChangeClass(lua_State *L)
{
    tagGameEvent event(_T("OpenChangeClass"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

int SC_ChangeClassEx(lua_State *L)
{
    tagGameEvent event(_T("OpenChangeClassEx"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

// 结婚说明
int SC_MarriageNote(lua_State *L)
{
    tagGameEvent event(_T("OpenMarriageNote"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

// 申请结婚
int SC_MarriageApply(lua_State *L)
{
    tagGameEvent event(_T("OpenMarriageApply"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

// 举行婚礼
int SC_WeddingCeremony(lua_State *L)
{
    tagGameEvent event(_T("OpenWeddingCeremony"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}

// 申请离婚
int SC_DivorceApply(lua_State *L)
{
    tagGameEvent event(_T("OpenDivorceApply"), NULL);
    TObjRef<GameFrameMgr>()->SendEvent(&event);
    return 0;
}



//-----------------------------------------------------------------------------
// table of function names and entry points that will be registered with Lua
//-----------------------------------------------------------------------------
static const luaL_reg questLib[] =
{
	{"Print",						LuaConsolePrint},
	{"WaitSec",						LuaWaitSec},
	{"NPCTalk",						SC_NPCTalk},
	{"ClearNPCTalkOptions",			SC_ClearNPCTalkOptions},
	{"SetAcceptOptions",			SC_SetAcceptOptions},
	{"SetCompleteOptions",			SC_SetCompleteOptions},
	{"SetNPCTalkOption",			SC_SetNPCTalkOption},
	{"AcceptQuestTalk",				SC_AcceptQuestTalk},
	{"CompleteQuestTalk",			SC_CompleteQuestTalk},
	{"CloseNPCTalk",				SC_CloseNPCTalk},
	{"AcceptQuest",					SC_AcceptQuest},
	{"CompleteQuest",				SC_CompleteQuest},
	{"UpdateQuest",					SC_UpdateQuest},
	{"GetPlayerName",				SC_GetPlayerName},
	{"GetRandom",					SC_GetRandom},
	{"QuestIsCompleted",			SC_QuestIsCompleted},
	{"GetPlayerSex",				SC_GetPlayerSex},
	{"GetItemNum",					SC_GetItemNum},
	{"GetPlayerMoney",				SC_GetPlayerMoney},
	{"GetPlayerLevel",				SC_GetPlayerLevel},
	{"GetMapName",					SC_GetMapName},
	{"GetPlayerAttribute",			SC_GetPlayerAttribute},
	{"MoneyIsEnough",				SC_MoneyIsEnough},
	{"QuestIsAccepted",				SC_QuestIsAccepted},
	{"TriggerCompleteQuestTalk",	SC_TriggerCompleteQuestTalk},
	{"TriggerAcceptQuestTalk",		SC_TriggerAcceptQuestTalk},
	{"ShowScreenInfo",				SC_ShowScreenInfo},
	{"QuestCanComplete",			SC_QuestCanComplete},
	{"PlayerHasSkill",				SC_PlayerHasSkill},
	{"PlayEmotionAct",				SC_PlayEmotionAct},
	{"PlayEffect",					SC_PlayEffect},
	{"StopEffect",					SC_StopEffect},
	{"GetServerTime",				SC_GetServerTime},
	{"GetTriggerID",				SC_GetTriggerID},
	{"GetTeamNum",					SC_GetTeamNum},
	{"StartCountDown",				SC_StartCountDown},
	{"GetRemanentTime",				SC_GetRemanentTime},
	{"GetServerRandom",				SC_GetServerRandom},
	{"GetTaskDoneTime",				SC_GetTaskDoneTime},
	{"SetQuestDesc",				SC_SetQuestDesc},
	{"OpenShop",					SC_OpenShop},
	{"OpenWare",					SC_OpenWare},
	{"BagExtend",					SC_BagExtend},
	{"WareExtend",					SC_WareExtend},
	{"GuildCreate",					SC_GuildCreate},
    {"GuildStorage",			    SC_OpenGuildStorage},
    {"GuildStorPermit",			    SC_OpenGuildStoragePermission},
	{"GuildDismiss",				SC_GuildDismiss},
	{"OpenGuildAffair",				SC_OpenGuildAffair},
	{"GuildUpgrade",				SC_GuildUpgrade},
	{"SetCommerceOption",			SC_SetCommerceOption},
	{"OpenPickUpSilver",			SC_OpenPickUpSilver},
	{"OpenContributeSilver",		SC_OpenContributeSilver},
	{"QuitCommerce",				SC_QuitCommerce},
	{"OpenCOC",						SC_OpenCOC},
	{"OpenCommerceRank",			SC_OpenCommerceRank},
	{"IsInGuild",					SC_IsInGuild},
    {"SetClass",					SC_SetClass},
    {"ChangeClass",					SC_ChangeClass},
    {"ChangeClassEx",               SC_ChangeClassEx},
    {"MarriageNote",                SC_MarriageNote},
    {"MarriageApply",               SC_MarriageApply},
    {"WeddingCeremony",             SC_WeddingCeremony},
    {"DivorceApply",                SC_DivorceApply},
	{NULL, NULL}
};

static const luaL_reg reviveLib[] = 
{
	{"SetReborn",					SC_SetReborn},
	{NULL, NULL}
};


static const luaL_reg produceLib[] =
{
	{"NPCTalk",						SC_NPCTalk},
	{"ClearNPCTalkOptions",			SC_ClearNPCTalkOptions},
	{"SetProduceOptions",			SC_SetProduceOptions},
	{"LearnProduceSkill",			SC_LearnProduceSkill},
	{"LeaveProduceSkill",			SC_LeaveProduceSkill},
	{"SetCancelProduceSkill",		SC_SetCancelProduceSkill},
	{NULL, NULL}
};

static const luaL_reg parseLib[] = 
{
	{"ParseType",					SC_ParseType},
	{"ParseDWORD",					SC_ParseDWORD},
	{"ParseINT",					SC_ParseINT},
	{"ParseString",					SC_ParseString},
	{"GetMsgValue",					SC_GetMsgValue},
	{NULL, NULL}
};

static const luaL_reg logLib[] = 
{
	{"Print",						SC_Print},
	{NULL, NULL}
};

static const luaL_reg commonLib[] =
{
	{"ServerCtrlTalk",				SC_ServerCtrlTalk},
	{"ShowChatChannelInfo",			SC_ShowChatChannelInfo},
	{"ShowInfo",					SC_ShowInfo},
	{"GetTriggerPos",				SC_GetTriggerPos},
	{NULL, NULL}
};

static const luaL_reg quizLib[] =
{
	{"Clear",						SC_QuizClear},
	{"SetCD",						SC_QuizSetCD},
	{"SetNum",						SC_QuizSetNum},
	{"SetOption",					SC_QuizSetOption},
	{"Show",						SC_QuizShow},
	{"Close",						SC_QuizClose},
	{NULL, NULL}
};

static const luaL_reg pvpLib[] = 
{
	{"PvPSetActivityCD",			SC_PvPSetActivityCD},
	{"PvPSetActivityID",			SC_PvPSetActivityID},
	{"PvPSetActivityScore",			SC_PvPSetActivityScore},
	{"PvPSetActivityRebornTime",	SC_PvPSetActivityRebornTime},
	{"PvPSetActivityTeamID",		SC_PvPSetActivityTeamID},
	{"PvPSetActivityType",			SC_PvPSetActivityType},
	{"PvPScoreSort"	,				SC_PvPScoreSort},
	{"PvPMultiScoreSort",			SC_PvPMultiScoreSort},
	{"PvPScoreSortInfo",			SC_PvPScoreSortInfo},
	{NULL, NULL}
};

//-----------------------------------------------------------------------------
// LUA OPEN SCRIPT LIB
// Registers all the basic script support functions with Lua.
//-----------------------------------------------------------------------------
VOID LuaOpenCommonLib(lua_State*  l)
{
	// register our library of routines under the table name Script

	luaL_openlib(l,	"Quest",	questLib,	0);
	luaL_openlib(l, "Revive",	reviveLib,	0);
	luaL_openlib(l, "Produce",	produceLib,	0);
	luaL_openlib(l, "Parse",	parseLib,	0);
	luaL_openlib(l, "Common",	commonLib,	0);
	luaL_openlib(l, "Log",		logLib,		0);
	luaL_openlib(l, "Quiz",		quizLib,	0);
	luaL_openlib(l, "PvP",		pvpLib,		0);
}