//-----------------------------------------------------------------------------
//!\file talent_handler.cpp
//!\author Aslan
//!
//!\date 2008-07-28
//! last 2008-07-28
//!
//!\brief 天资消息处理类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/talent_define.h"
#include "../WorldDefine/msg_talent.h"

#include "player_session.h"
#include "unit.h"
#include "role.h"


//-----------------------------------------------------------------------------
// 学习技能
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLearnSkill(tagNetCmd* pCmd)
{
	tagNC_LearnSkill* pRecv = (tagNC_LearnSkill*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->LearnSkill(pRecv->dwSkillID, pRecv->dwNPCID, pRecv->n64ItemID);

	// 发送返回消息
	tagNS_LearnSkill send;
	send.dwSkillID = pRecv->dwSkillID;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------
// 技能升级
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleLevelUpSkill(tagNetCmd* pCmd)
{
	tagNC_LevelUpSkill* pRecv = (tagNC_LevelUpSkill*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->LevelUpSkill(pRecv->dwSkillID, pRecv->dwNPCID, pRecv->n64ItemID);

	// 发送返回消息
	tagNS_LevelUpSkill send;
	send.dwSkillID = pRecv->dwSkillID;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------
// 遗忘技能
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleForgetSkill(tagNetCmd* pCmd)
{
	tagNC_ForgetSkill* pRecv = (tagNC_ForgetSkill*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->ForgetSkill(pRecv->dwSkillID, pRecv->dwNPCID);

	// 发送返回消息
	tagNS_ForgetSkill send;
	send.dwSkillID = pRecv->dwSkillID;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

//------------------------------------------------------------------------------
// 洗点
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleClearTalent(tagNetCmd* pCmd)
{
	tagNC_ClearTalent* pRecv = (tagNC_ClearTalent*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->ClearTalent(pRecv->n64ItemID);

	// 发送返回消息
	tagNS_ClearTalent send;
	send.eType = pRecv->eType;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

