#include "StdAfx.h"
#include "player_session.h"
#include "role.h"
#include "creature.h"
#include "map.h"
#include "script_mgr.h"

#include "../WorldDefine/role_att.h"

DWORD PlayerSession::HandleTalkToNPC(tagNetCmd* pCmd)
{
	MTRANS_POINTER(pRecv, pCmd, tagNC_NPCTalk);

	MTRANS_ELSE_RET(pRole,		GetRole(),							Role,				GT_INVALID);
	MTRANS_ELSE_RET(pMap,		pRole->GetMap(),					Map,				GT_INVALID);
	MTRANS_ELSE_RET(pNpc,		pMap->FindCreature(pRecv->dwNPCId), Creature,			GT_INVALID);
	MTRANS_ELSE_RET(pScript,	pNpc->GetScript(),					CreatureScript,		GT_INVALID);

	// ´¥·¢Ì¸»°
	pScript->OnTalk(pNpc, pRole, -1);

	return 0;
}