#include "StdAfx.h"

#include "wx/wx.h"
#include "servermergetoolapp.h"

#include "Loong_LoongDBCtrl.h"
#include "../WorldDefine/time.h"


VOID CLoongLoongDBCtrl::ProcResQueryAccountCommon(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pAccountCommon, pData, std::vector <tagTWrap_Loong_AccountCommon*>);

	do{
		tagTWrap_Loong_AccountCommon *p = new tagTWrap_Loong_AccountCommon;

		p->dwAccountID				= (*pRes)[0].GetDword();
		memcpy(p->szAccountName,(*pRes)[1].GetString(),(*pRes)[1].GetLen()+1);
		p->stOthers.sSafeCode.dwSafeCodeCrc	= (*pRes)[2].GetDword();
		p->stOthers.sSafeCode.dwTimeReset	= (*pRes)[3].GetDword();
		p->stOthers.dwBagPsdCrc				= (*pRes)[4].GetDword();
		p->stOthers.nBaiBaoYuanBao			= (*pRes)[5].GetInt();
		p->stOthers.n16WareSize				= (*pRes)[6].GetInt();
		p->stOthers.n64WareSilver			= (*pRes)[7].GetInt();


		pAccountCommon->push_back(p);
	}while(pRes->NextRow());
}
VOID CLoongLoongDBCtrl::ProcResQueryBlackList(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pBlackList, pData, std::vector <tagTWrap_Loong_BlackList*>);
	do{
		tagTWrap_Loong_BlackList *p = new tagTWrap_Loong_BlackList;

		p->dwRoleID		= (*pRes)[0].GetDword();
		p->dwBlackID	= (*pRes)[1].GetDword();

		pBlackList->push_back(p);
	}while(pRes->NextRow());
}
VOID CLoongLoongDBCtrl::ProcResQueryBuff(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pBuff, pData, std::vector <tagTWrap_Loong_Buff*>);
	do{
		tagTWrap_Loong_Buff *p = new tagTWrap_Loong_Buff;

		p->dwRoleID					=	(*pRes)[0].GetDword();
		p->stOthers.dwSrcUnitID		=	(*pRes)[1].GetDword();
		p->stOthers.dwSrcSkillID	=	(*pRes)[2].GetDword();
		p->stOthers.dwItemTypeID	=	(*pRes)[3].GetDword();
		p->stOthers.n64Serial		=	(*pRes)[4].GetLong();

		p->stOthers.dwBuffID		= (*pRes)[5].GetDword();
		p->stOthers.nPersistTick	= (*pRes)[6].GetInt();
		p->stOthers.n8Level			= (*pRes)[7].GetInt();
		p->stOthers.n8CurLapTimes	= (*pRes)[8].GetInt();


		p->stOthers.n8ModifierNum	= (INT8)((*pRes)[9].GetLen()/sizeof(DWORD));

		if(p->stOthers.n8ModifierNum > 0)
		{
			p->pcBlob	= new CHAR[(*pRes)[9].GetLen()];

			(*pRes)[9].GetBlob(p->pcBlob, (*pRes)[9].GetLen());
		}

		//推进去
		pBuff->push_back(p);

	}while(pRes->NextRow());
}
VOID CLoongLoongDBCtrl::ProcResQueryClanData(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pClanData, pData, std::vector <tagTWrap_Loong_ClanData*>);
	do{
		tagTWrap_Loong_ClanData *p = new tagTWrap_Loong_ClanData;

		for(INT nClanType = ECLT_BEGIN; nClanType < ECLT_END; ++nClanType)
		{
			p->stOthers.m_nReputation[nClanType]		= (*pRes)[nClanType * 3].GetInt();
			p->stOthers.m_nContribution[nClanType]	= (*pRes)[nClanType * 3 + 1].GetInt();
			p->stOthers.m_n8ActCount[nClanType]		= (*pRes)[nClanType * 3 + 2].GetByte();

		}
		p->stOthers.m_u16FameMask	= (*pRes)[ECLT_NUM * 3].GetShort();
		p->stOthers.m_dwtLastResetTime= (*pRes)[ECLT_NUM * 3 + 1].GetDword();

		p->dwRoleID = (*pRes)[ECLT_NUM * 3 + 2].GetDword();

		//推进去
		pClanData->push_back(p);

	}while(pRes->NextRow());
}
//enemy
VOID CLoongLoongDBCtrl::ProcResQueryEnemy(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pEnemy, pData, std::vector <tagTWrap_Loong_Enemy*>);
	do{
		tagTWrap_Loong_Enemy *p = new tagTWrap_Loong_Enemy;

		p->dwRoleID		= (*pRes)[0].GetDword();
		p->dwEnemyID	= (*pRes)[1].GetDword();

		pEnemy->push_back(p);

	}while(pRes->NextRow());
}

//equip
VOID CLoongLoongDBCtrl::ProcResQueryEquip(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pEquipList, pData, std::vector <tagTWrap_Loong_Equip*>);

	do{
		tagTWrap_Loong_Equip *pEquip = new tagTWrap_Loong_Equip;

		pEquip->n64SerialNum								= (*pRes)[0].GetLong();
		// 装备属性
		pEquip->stOthers.equipSpec.byQuality 				= (*pRes)[1].GetByte();
		pEquip->stOthers.equipSpec.nPotValUsed 				= (*pRes)[2].GetInt();
		pEquip->stOthers.equipSpec.byMinUseLevel 			= (*pRes)[3].GetByte();
		pEquip->stOthers.equipSpec.byMaxUseLevel 			= (*pRes)[4].GetByte();
		pEquip->stOthers.equipSpec.n16WuHun 				= (*pRes)[5].GetShort();
		pEquip->stOthers.equipSpec.n16MinDmg 				= (*pRes)[6].GetShort();
		pEquip->stOthers.equipSpec.n16MaxDmg 				= (*pRes)[7].GetShort();
		pEquip->stOthers.equipSpec.n16Armor 				= (*pRes)[8].GetShort();
		pEquip->stOthers.equipSpec.nPotVal 					= (*pRes)[9].GetInt();

		pEquip->stOthers.equipSpec.byPosyTimes 				= (*pRes)[11].GetByte();

		pEquip->stOthers.equipSpec.byEngraveTimes 			= (*pRes)[13].GetByte();

		pEquip->stOthers.equipSpec.byHoleNum 				= (*pRes)[15].GetByte();

		pEquip->stOthers.equipSpec.byBrandLevel 			= (*pRes)[17].GetByte();

		pEquip->stOthers.equipSpec.byLongfuLevel			= (*pRes)[19].GetByte();

		pEquip->stOthers.equipSpec.bCanCut 					= (*pRes)[21].GetBool();
		pEquip->stOthers.equipSpec.dwLongHunInnerID			= (*pRes)[22].GetDword();
		pEquip->stOthers.equipSpec.dwLongHunOuterID			= (*pRes)[23].GetDword();
		pEquip->stOthers.equipSpec.bySpecAtt				= (*pRes)[24].GetInt();
		// 时装属性
		pEquip->stOthers.equipSpec.n16Appearance			= (*pRes)[25].GetInt();
		pEquip->stOthers.equipSpec.byRein					= (*pRes)[26].GetByte();
		pEquip->stOthers.equipSpec.bySavvy					= (*pRes)[27].GetByte();
		pEquip->stOthers.equipSpec.byFortune				= (*pRes)[28].GetByte();
		pEquip->stOthers.equipSpec.n8ColorID				= (INT8)(*pRes)[29].GetInt();

		pEquip->stOthers.equipSpec.n16QltyModPct			= (*pRes)[30].GetShort();
		pEquip->stOthers.equipSpec.n16QltyModPctEx			= (*pRes)[31].GetShort();

		pEquip->stOthers.equipSpec.n16AttALimMod			= (*pRes)[32].GetInt();
		pEquip->stOthers.equipSpec.n16AttALimModPct			= (*pRes)[33].GetInt();
		pEquip->stOthers.equipSpec.byFlareVal				= (*pRes)[34].GetByte();
		pEquip->stOthers.equipSpec.n16PotValModPct			= (*pRes)[35].GetByte();

		(*pRes)[10].GetBlob(pEquip->stOthers.equipSpec.nRoleAttEffect, min((*pRes)[10].GetLen(), sizeof(pEquip->stOthers.equipSpec.nRoleAttEffect)));
		(*pRes)[12].GetBlob(pEquip->stOthers.equipSpec.PosyEffect, min((*pRes)[12].GetLen(), sizeof(pEquip->stOthers.equipSpec.PosyEffect)));
		(*pRes)[14].GetBlob(pEquip->stOthers.equipSpec.nEngraveAtt, min((*pRes)[14].GetLen(), sizeof(pEquip->stOthers.equipSpec.nEngraveAtt)));
		(*pRes)[16].GetBlob(pEquip->stOthers.equipSpec.dwHoleGemID, min((*pRes)[16].GetLen(), sizeof(pEquip->stOthers.equipSpec.dwHoleGemID)));
		(*pRes)[18].GetBlob(pEquip->stOthers.equipSpec.nBrandAtt, min((*pRes)[18].GetLen(), sizeof(pEquip->stOthers.equipSpec.nBrandAtt)));
		(*pRes)[20].GetBlob(pEquip->stOthers.equipSpec.byLongfuAtt, min((*pRes)[20].GetLen(), sizeof(pEquip->stOthers.equipSpec.byLongfuAtt)));

		//1.3.0新增
		pEquip->stOthers.equipSpec.nPotIncTimes				= (*pRes)[36].GetInt();
		pEquip->stOthers.equipSpec.dwPurpleQltyIdfPct		= (*pRes)[37].GetDword();

		pEquipList->push_back(pEquip);

	}while(pRes->NextRow());
}

//氏族珍宝激活
VOID CLoongLoongDBCtrl::ProcResQueryActClanTreasure(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pActClanTreasure, pData, std::vector <tagTWrap_Loong_ActClanTreasure*>);
	do{
		tagTWrap_Loong_ActClanTreasure *p = new tagTWrap_Loong_ActClanTreasure;

		p->u16TreasureID	= (*pRes)[0].GetDword();
		p->dwActNameID		= (*pRes)[1].GetDword();
		p->dwRoleID			= (*pRes)[2].GetDword();
		p->byClanType		= (*pRes)[3].GetDword();
		p->dwActTime		= (*pRes)[4].GetDword();

		pActClanTreasure->push_back(p);

	}while(pRes->NextRow());
}
//氏族进入快照
VOID CLoongLoongDBCtrl::ProcResQueryFameHallEnterSnap(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pFameHallEnterSnap, pData, std::vector <tagTWrap_Loong_FameHallEnterSnap*>);
	do{
		tagTWrap_Loong_FameHallEnterSnap *p = new tagTWrap_Loong_FameHallEnterSnap;

		p->dwRoleID				= (*pRes)[0].GetDword();
		p->dwEnterNameID		= (*pRes)[1].GetDword();
		p->byClanType			= (*pRes)[2].GetByte();
		p->dwEnterTime			= (*pRes)[3].GetDword();


		pFameHallEnterSnap->push_back(p);

	}while(pRes->NextRow());
}
//氏族进入快照
VOID CLoongLoongDBCtrl::ProcResQueryRepRstTime(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pRepRstTime, pData, std::vector <tagTWrap_Loong_RepRstTime*>);
	do{
		tagTWrap_Loong_RepRstTime *p = new tagTWrap_Loong_RepRstTime;

		p->byClanType				= (ECLanType)(*pRes)[0].GetDword();
		p->dwRepRstTimeStamp		= (*pRes)[1].GetDword();

		pRepRstTime->push_back(p);

	}while(pRes->NextRow());
}
//好友列表(单向)
VOID CLoongLoongDBCtrl::ProcResQueryFriendSave(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pFriendSave, pData, std::vector <tagTWrap_Loong_Friend*>);
	do{
		tagTWrap_Loong_Friend *p = new tagTWrap_Loong_Friend;

		p->dwRoleID					= (*pRes)[0].GetDword();
		p->stOthers.dwFriendID		= (*pRes)[1].GetDword();
		p->stOthers.nGroupID		= (*pRes)[2].GetDword();


		pFriendSave->push_back(p);

	}while(pRes->NextRow());
}
//友好度
VOID CLoongLoongDBCtrl::ProcResQueryFriendshipSave(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pFriendshipSave, pData, std::vector <tagTWrap_Loong_FriendshipSave*>);
	do{
		tagTWrap_Loong_FriendshipSave *p = new tagTWrap_Loong_FriendshipSave;

		p->dwRoleID					= (*pRes)[0].GetDword();
		p->stOthers.dwFriendID		= (*pRes)[1].GetDword();
		p->stOthers.nFriVal			= (*pRes)[2].GetDword();

		pFriendshipSave->push_back(p);

	}while(pRes->NextRow());
}
//团购
VOID CLoongLoongDBCtrl::ProcResQueryGroupPurchase(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGroupPurchase, pData, std::vector <tagTWrap_Loong_GroupPurchase*>);
	do{
		tagTWrap_Loong_GroupPurchase *p = new tagTWrap_Loong_GroupPurchase;

		p->stOthers.dwGuildID			= (*pRes)[0].GetDword();
		p->stOthers.dwRoleID			= (*pRes)[1].GetDword();
		p->stOthers.dwMallID			= (*pRes)[2].GetDword();

		p->stOthers.nPrice				= (*pRes)[3].GetDword();
		p->stOthers.nParticipatorNum	= (*pRes)[4].GetDword();
		p->stOthers.nRequireNum			= (*pRes)[5].GetDword();
		p->stOthers.nRemainTime			= (*pRes)[6].GetDword();


		if((*pRes)[7].GetLen() != 0)
		{
			p->pcBlob = new CHAR[(*pRes)[7].GetLen()];
			p->dwBlobSize = (*pRes)[7].GetLen();

			(*pRes)[7].GetBlob( p->pcBlob, (*pRes)[7].GetLen() );
		}
		pGroupPurchase->push_back(p);

	}while(pRes->NextRow());
}
//帮派	
VOID CLoongLoongDBCtrl::ProcResQueryGuild(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGuild, pData, std::vector <tagTWrap_Loong_Guild*>);
	do{
		tagTWrap_Loong_Guild *p = new tagTWrap_Loong_Guild;

		p->stOthers.dwID				= (*pRes)[0].GetDword();
		p->stOthers.dwFounderNameID	= (*pRes)[2].GetDword();
		p->stOthers.dwSpecState		= (EGuildSpecState)(*pRes)[3].GetInt();
		p->stOthers.byLevel			= (*pRes)[4].GetByte();
		p->stOthers.byHoldCity[0]	= (*pRes)[5].GetByte();
		p->stOthers.byHoldCity[1]	= (*pRes)[6].GetByte();
		p->stOthers.byHoldCity[2]	= (*pRes)[7].GetByte();
		p->stOthers.nFund			= (*pRes)[8].GetInt();
		p->stOthers.nMaterial		= (*pRes)[9].GetInt();
		p->stOthers.nReputation		= (*pRes)[10].GetInt();
		p->stOthers.nDailyCost		= (*pRes)[11].GetInt();
		p->stOthers.n16Peace			= (INT16)(*pRes)[12].GetInt();
		p->stOthers.n16Rank			= (INT16)(*pRes)[13].GetInt();
		
		memcpy(p->szDate,(*pRes)[16].GetString(),(*pRes)[16].GetLen()+1);

		p->stOthers.nGroupPurchase	= (*pRes)[17].GetInt();

		p->stOthers.dwLeaderRoleID	= (*pRes)[18].GetInt();
		p->stOthers.byAffairRemainTimes	= (*pRes)[19].GetByte();
		p->stOthers.bCommendation	= (*pRes)[20].GetBool();

		memset(p->stOthers.szName,0,sizeof(p->stOthers.szName));
		memset(p->stOthers.szTenet,0,sizeof(p->stOthers.szName));
		memset(p->stOthers.szSymbolURL,0,sizeof(p->stOthers.szName));

		(*pRes)[1].GetBlob(p->stOthers.szName, (*pRes)[1].GetLen());
		(*pRes)[14].GetBlob(p->stOthers.szTenet, (*pRes)[14].GetLen());
		(*pRes)[15].GetBlob(p->stOthers.szSymbolURL, (*pRes)[15].GetLen());



		pGuild->push_back(p);

	}while(pRes->NextRow());
}
//帮派成员	
VOID CLoongLoongDBCtrl::ProcResQueryGuildMem(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGuildMem, pData, std::vector <tagTWrap_Loong_GuildMem*>);
	do{
		tagTWrap_Loong_GuildMem *p = new tagTWrap_Loong_GuildMem;

		p->stOthers.sGuildMember.dwRoleID			= (*pRes)[0].GetDword();
		p->stOthers.dwGuildID						= (*pRes)[1].GetDword();
		p->stOthers.sGuildMember.eGuildPos			= (EGuildMemberPos)(*pRes)[2].GetInt();
		p->stOthers.sGuildMember.nTotalContribution	= (*pRes)[3].GetInt();
		p->stOthers.sGuildMember.nContribution		= (*pRes)[4].GetInt();
		p->stOthers.sGuildMember.nExploit			= (*pRes)[5].GetInt();
		p->stOthers.sGuildMember.nSalary				= (*pRes)[6].GetInt();
		memcpy(p->szDate,(*pRes)[7].GetString(),(*pRes)[7].GetLen()+1);

		p->stOthers.sGuildMember.bUseGuildWare				= (*pRes)[8].GetBool();

		pGuildMem->push_back(p);

	}while(pRes->NextRow());
}
//Item
VOID CLoongLoongDBCtrl::ProcResQueryItem(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pItem, pData, std::vector <tagTWrap_Loong_Item*>);
	do{
		tagTWrap_Loong_Item *p = new tagTWrap_Loong_Item;

		p->stOthers.n64Serial 					= (*pRes)[0].GetLong();
		p->stOthers.n16Num 						= (*pRes)[1].GetShort();
		p->stOthers.dwTypeID 					= (*pRes)[2].GetDword();
		p->stOthers.byBind 						= (*pRes)[3].GetInt();
		p->stOthers.bLock 						= (*pRes)[4].GetBool();
		p->stOthers.nUseTimes 					= (*pRes)[5].GetInt();

		p->stOthers.eCreateMode					= (EItemCreateMode)(*pRes)[7].GetInt(); 
		p->stOthers.dwCreateID					= (*pRes)[8].GetDword();
		p->stOthers.dwCreatorID 					= (*pRes)[9].GetDword();

		p->stOthers.dwOwnerID 					= (*pRes)[11].GetDword();
		p->stOthers.dwAccountID 					= (*pRes)[12].GetDword();
		p->stOthers.eConType 					= (EItemConType)(*pRes)[13].GetInt();
		p->stOthers.n16Index 					= (*pRes)[14].GetShort();
		p->stOthers.dwNameID						=(*pRes)[15].GetDword();


		DataTime2DwordTime(p->stOthers.dw1stGainTime, (*pRes)[6].GetString(), (*pRes)[6].GetLen());
		DataTime2DwordTime(p->stOthers.dwCreateTime, (*pRes)[10].GetString(), (*pRes)[10].GetLen());


		pItem->push_back(p);

	}while(pRes->NextRow());
}
//item cd time
VOID CLoongLoongDBCtrl::ProcResQueryItemCDTime(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pItemCDTime, pData, std::vector <tagTWrap_Loong_ItemCDTime*>);
	do{
		tagTWrap_Loong_ItemCDTime *p = new tagTWrap_Loong_ItemCDTime;

		p->dwRoleID					= (*pRes)[0].GetDword();

		if((*pRes)[1].GetLen() >= sizeof(tagCDTime))
		{
			p->vecCDTime.resize((*pRes)[1].GetLen()/sizeof(tagCDTime));
			(*pRes)[1].GetBlob(&p->vecCDTime[0], (*pRes)[1].GetLen());	
		}

		pItemCDTime->push_back(p);

	}while(pRes->NextRow());
}
//item need log
VOID CLoongLoongDBCtrl::ProcResQueryItemNeedLog(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pItemNeedLog, pData, std::vector <tagTWrap_Loong_ItemNeedLog*>);
	do{
		tagTWrap_Loong_ItemNeedLog *p = new tagTWrap_Loong_ItemNeedLog;

		p->dwTypeID					= (*pRes)[0].GetDword();
		p->bNeedLog					= (*pRes)[1].GetBool();
		p->sMinQlty					= (*pRes)[2].GetShort();

		pItemNeedLog->push_back(p);

	}while(pRes->NextRow());
}
//item log baibao
VOID CLoongLoongDBCtrl::ProcResQueryLogBaiBao(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pLogBaiBao, pData, std::vector <tagTWrap_Loong_LogBaiBao*>);
	do{
		tagTWrap_Loong_LogBaiBao *p = new tagTWrap_Loong_LogBaiBao;

		p->dwAccountID = (*pRes)[0].GetDword();
		p->stOthers.dwRoleID = (*pRes)[1].GetDword();
		p->stOthers.dwTypeID = (*pRes)[4].GetDword();
		p->stOthers.n16Type = (*pRes)[3].GetShort();
		DataTime2DwordTime(*(tagDWORDTime*)&(p->stOthers.dwTime), (*pRes)[2].GetString(), (*pRes)[2].GetLen());
		
		if((*pRes)[5].GetLen() != 0)
		{
			p->ptcWords = new TCHAR[(*pRes)[5].GetLen()/2];
			//
			(*pRes)[5].GetBlob(p->ptcWords, (*pRes)[5].GetLen());
		}
		p->stOthers.n16Size = (INT16)(sizeof(tagBaiBaoRecord) + (*pRes)[5].GetLen() - sizeof(p->stOthers.szWords));

		pLogBaiBao->push_back(p);

	}while(pRes->NextRow());
}
//item role del
VOID CLoongLoongDBCtrl::ProcResQueryRoleDel(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pRoleDel, pData, std::vector <tagTWrap_Loong_RoleDel*>);
	do{
		tagTWrap_Loong_RoleDel *p = new tagTWrap_Loong_RoleDel;

		p->dwAccountID = (*pRes)[0].GetDword();
		p->dwRoleID = (*pRes)[1].GetDword();
		
		memcpy(p->szRoleName,(*pRes)[2].GetString(),(*pRes)[2].GetLen()+1);
		memcpy(p->szIP,(*pRes)[3].GetString(),(*pRes)[3].GetLen()+1);
		memcpy(p->szDeleteTime,(*pRes)[4].GetString(),(*pRes)[4].GetLen()+1);

		pRoleDel->push_back(p);

	}while(pRes->NextRow());

}
//item roledata
VOID CLoongLoongDBCtrl::ProcResQueryRoleData(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pRoleData, pData, std::vector <tagTWrap_Loong_RoleData*>);
	do{
		tagTWrap_Loong_RoleData *p = new tagTWrap_Loong_RoleData;
		tagRoleDataConst	*pRoleDataConst = &p->stOthers.RoleDataConst;
		tagRoleDataSave		*pRoleDataSave	= &p->stOthers.RoleDataSave;


		(*pRes)[0].GetTCHAR(pRoleDataConst->szRoleName, X_SHORT_NAME);
		pRoleDataConst->dwRoleNameCrc				= (*pRes)[1].GetDword();
		pRoleDataConst->Avatar.bySex				= (*pRes)[3].GetByte();
		pRoleDataConst->Avatar.wHairMdlID			= (*pRes)[4].GetShort();
		pRoleDataConst->Avatar.wHairTexID			= (*pRes)[5].GetShort();
		pRoleDataConst->Avatar.wFaceMdlID			= (*pRes)[6].GetShort();
		pRoleDataConst->Avatar.wFaceDetailTexID		= (*pRes)[7].GetShort();
		pRoleDataConst->Avatar.wDressMdlID			= (*pRes)[8].GetShort();

		pRoleDataSave->DisplaySet					= (*pRes)[2].GetByte();
		pRoleDataSave->dwMapID						= (*pRes)[9].GetDword();
		pRoleDataSave->fCoordinate[0]				= (*pRes)[10].GetFloat();
		pRoleDataSave->fCoordinate[1]				= (*pRes)[11].GetFloat();
		pRoleDataSave->fCoordinate[2]				= (*pRes)[12].GetFloat();
		pRoleDataSave->fFace[0]						= (*pRes)[13].GetFloat();
		pRoleDataSave->fFace[1]						= (*pRes)[14].GetFloat();
		pRoleDataSave->fFace[2]						= (*pRes)[15].GetFloat();

		pRoleDataSave->eClass						= (EClassType)(*pRes)[16].GetInt();
		pRoleDataSave->eClassEx						= (EClassTypeEx)(*pRes)[17].GetInt();
		pRoleDataSave->nLevel						= (*pRes)[18].GetShort();
		pRoleDataSave->nCurExp						= (*pRes)[19].GetInt();
		pRoleDataSave->nHP							= (*pRes)[20].GetInt();
		pRoleDataSave->nMP							= (*pRes)[21].GetInt();

		pRoleDataSave->nAttPoint					= (*pRes)[22].GetInt();
		pRoleDataSave->nTalentPoint					= (*pRes)[23].GetInt();

		pRoleDataSave->nAttPointAdd[ERA_Physique]	= (*pRes)[24].GetInt();
		pRoleDataSave->nAttPointAdd[ERA_Strength]	= (*pRes)[25].GetInt();
		pRoleDataSave->nAttPointAdd[ERA_Pneuma]		= (*pRes)[26].GetInt();
		pRoleDataSave->nAttPointAdd[ERA_InnerForce]	= (*pRes)[27].GetInt();
		pRoleDataSave->nAttPointAdd[ERA_Technique]	= (*pRes)[28].GetInt();
		pRoleDataSave->nAttPointAdd[ERA_Agility]	= (*pRes)[29].GetInt();

		pRoleDataSave->nInjury						= (*pRes)[30].GetInt();
		pRoleDataSave->nVitality					= (*pRes)[31].GetInt();
		pRoleDataSave->nKnowledge					= (*pRes)[32].GetInt();
		pRoleDataSave->nMorality					= (*pRes)[33].GetInt();
		pRoleDataSave->nCulture						= (*pRes)[34].GetInt();
		pRoleDataSave->nCredit						= (*pRes)[35].GetInt();
		pRoleDataSave->nIdentity					= (*pRes)[36].GetInt();
		pRoleDataSave->nVIPPoint					= (*pRes)[37].GetInt();

		pRoleDataSave->n16BagSize					= (*pRes)[38].GetShort();
		pRoleDataSave->nBagGold						= (*pRes)[39].GetInt();
		pRoleDataSave->nBagSilver					= (*pRes)[40].GetInt();
		pRoleDataSave->nBagYuanBao					= (*pRes)[41].GetInt();

		pRoleDataSave->dwGuildID					= (*pRes)[42].GetDword();
		//pRoleDataSave->nWareGold					= (*pRes)[43].GetInt();
		//pRoleDataSave->nWareSilver					= (*pRes)[44].GetInt();
		//pRoleDataSave->nWareYuanBao					= (*pRes)[45].GetInt();

		if(!DataTime2DwordTime(pRoleDataConst->CreateTime,	(*pRes)[46].GetString(), (*pRes)[46].GetLen()))
		{
			//songg
			//IMSG(_T("Error: Func DataTime2DwordTime() run error in CLoongDB::FormatSaveRoleData()!!!!!\r\n"));
			//IMSG(_T("Error: Role Name CRC32 is %u!!!!!\r\n"), pRoleDataConst->dwRoleNameCrc);
			//ASSERT(0);
		}

		DataTime2DwordTime(pRoleDataSave->LoginTime,	(*pRes)[47].GetString(), (*pRes)[47].GetLen());
		DataTime2DwordTime(pRoleDataSave->LogoutTime,	(*pRes)[48].GetString(), (*pRes)[48].GetLen());

		pRoleDataSave->nOnlineTime					= (*pRes)[49].GetDword();
		pRoleDataSave->nCurOnlineTime				= (*pRes)[50].GetDword();

		pRoleDataSave->nRage						= (*pRes)[51].GetInt();
		pRoleDataSave->nEndurance					= (*pRes)[52].GetInt();
		pRoleDataSave->bSafeGuard					= (*pRes)[53].GetInt();

		DataTime2DwordTime(pRoleDataSave->CloseSafeGuardTime,	(*pRes)[54].GetString(), (*pRes)[54].GetLen());

		pRoleDataSave->talent[0].eType				= (ETalentType)(*pRes)[55].GetInt();
		pRoleDataSave->talent[1].eType				= (ETalentType)(*pRes)[56].GetInt();
		pRoleDataSave->talent[2].eType				= (ETalentType)(*pRes)[57].GetInt();
		pRoleDataSave->talent[3].eType				= (ETalentType)(*pRes)[58].GetInt();

		pRoleDataSave->talent[0].nPoint				= (*pRes)[59].GetInt();
		pRoleDataSave->talent[1].nPoint				= (*pRes)[60].GetInt();
		pRoleDataSave->talent[2].nPoint				= (*pRes)[61].GetInt();
		pRoleDataSave->talent[3].nPoint				= (*pRes)[62].GetInt();

		pRoleDataSave->dwRebornMapID				= (*pRes)[63].GetDword();
		pRoleDataSave->nTotalTax					= (*pRes)[64].GetInt();
		pRoleDataSave->nMorale						= (*pRes)[65].GetInt();

		DataTime2DwordTime(*(tagDWORDTime*)&pRoleDataSave->dwTimeGetMallFree, (*pRes)[66].GetString(), (*pRes)[66].GetLen());
		pRoleDataSave->sRemoteOpenSet				= (*pRes)[67].GetDword();
		pRoleDataSave->u16ActiveTitleID				= (*pRes)[68].GetInt();
		pRoleDataSave->nExVolume					= (*pRes)[69].GetInt();


		p->dwRoleID									= (*pRes)[70].GetDword();

		ZeroMemory(&pRoleDataSave->AvatarEquip, SIZE_AVATAR_EQUIP);
		if(!(*pRes)[71].IsNull())
		{
			(*pRes)[71].GetBlob(&pRoleDataSave->AvatarEquip, min((*pRes)[71].GetLen(), SIZE_AVATAR_EQUIP));
		}
		p->bRemoveFlag								= (*pRes)[72].GetBool();
		memcpy(p->szRemoveTime,(*pRes)[73].GetString(),(*pRes)[73].GetLen()+1);
		p->dwAccountID								= (*pRes)[74].GetDword();
		//脚本数据
		if(!(*pRes)[75].IsNull())
		{
			(*pRes)[75].GetBlob(&pRoleDataSave->Data.dwData, (*pRes)[75].GetLen());
		}
		pRoleDataSave->nTreasureSum					= (*pRes)[76].GetDword();

		pRoleDataSave->byStallLevel					= (*pRes)[77].GetBool();
		pRoleDataSave->nStallDailyExp				= (*pRes)[78].GetInt();
		pRoleDataSave->nStallCurExp					= (*pRes)[79].GetInt();
		pRoleDataSave->dwLastStallTime				= (*pRes)[80].GetDword();

		//1.2.5.0新增
		pRoleDataSave->nHostility					= (*pRes)[81].GetInt();
		pRoleDataSave->nDeadUnSetSafeGuardCountDown = (*pRes)[82].GetInt();
		pRoleDataSave->dwItemTransportMapID			= (*pRes)[83].GetDword();
		pRoleDataSave->fItemTransportX			= (*pRes)[84].GetFloat();
		pRoleDataSave->fItemTransportZ			= (*pRes)[85].GetFloat();
		pRoleDataSave->fItemTransportY			= (*pRes)[86].GetFloat();

		//1.3.0 新增
		pRoleDataSave->dwStrength					= (*pRes)[87].GetDword();
		pRoleDataSave->nStrengthNum					= (*pRes)[88].GetInt();
		pRoleDataSave->dwLoverID					= (*pRes)[89].GetDword();
		pRoleDataSave->bHaveWedding					= (*pRes)[90].GetBool();
		pRoleDataSave->timeLastLessing				= (*pRes)[91].GetDword();
		pRoleDataSave->nLessingLevel				= (*pRes)[92].GetInt();
		pRoleDataSave->bNeedPrisonRevive			= (*pRes)[93].GetBool();

		pRoleData->push_back(p);

	}while(pRes->NextRow());
}

//skill
VOID CLoongLoongDBCtrl::ProcResQuerySkill(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pSkill, pData, std::vector <tagTWrap_Loong_Skill*>);
	do{

		tagTWrap_Loong_Skill *p = new tagTWrap_Loong_Skill;

		p->dwRoleID					= (*pRes)[0].GetDword();
		p->stOthers.dwID			= (*pRes)[1].GetDword();
		p->stOthers.nLearnLevel		= (*pRes)[2].GetInt();
		p->stOthers.nSelfLevel		= (*pRes)[3].GetInt();
		p->stOthers.nProficiency	= (*pRes)[4].GetInt();
		p->stOthers.nCoolDown		= (*pRes)[5].GetInt();

		pSkill->push_back(p);


	}while(pRes->NextRow());
}
//task
VOID CLoongLoongDBCtrl::ProcResQueryTask(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pTask, pData, std::vector <tagTWrap_Loong_Task*>);
	do{
		tagTWrap_Loong_Task *p = new tagTWrap_Loong_Task;

		p->stOthers.u16QuestID	= (UINT16)(*pRes)[0].GetDword();
		p->stOthers.dwAcceptTime	= (*pRes)[1].GetDword();
		p->stOthers.dwQuestFlag	= (*pRes)[2].GetDword();

		for(INT32 j=0; j<QUEST_CREATURES_COUNT; ++j)
		{
			p->stOthers.n16MonsterNum[j] = (INT16)(*pRes)[j+3].GetInt();
		}

		(*pRes)[QUEST_CREATURES_COUNT+3].GetBlob(p->stOthers.Data.dwData, (*pRes)[QUEST_CREATURES_COUNT+3].GetLen());
		if((*pRes)[QUEST_CREATURES_COUNT+4].GetLen() > 0)
			(*pRes)[QUEST_CREATURES_COUNT+4].GetBlob(&p->stOthers.DynamicTarget, (*pRes)[QUEST_CREATURES_COUNT+4].GetLen());


		p->dwRoleID = (*pRes)[11].GetDword();



		pTask->push_back(p);


	}while(pRes->NextRow());
}
//task
VOID CLoongLoongDBCtrl::ProcResQueryTaskDone(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pTaskDone, pData, std::vector <tagTWrap_Loong_TaskDone*>);
	do{
		tagTWrap_Loong_TaskDone *p = new tagTWrap_Loong_TaskDone;


		p->stOthers.u16QuestID = (UINT16)(*pRes)[0].GetDword();
		p->stOthers.dwStartTime = (*pRes)[2].GetDword();
		p->stOthers.nTimes = (*pRes)[1].GetInt();
		p->dwRoleID			= (*pRes)[3].GetDword();

		pTaskDone->push_back(p);

	}while(pRes->NextRow());
}
//title
VOID CLoongLoongDBCtrl::ProcResQueryTitle(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pTitle, pData, std::vector <tagTWrap_Loong_Title*>);
	do{
		tagTWrap_Loong_Title *p = new tagTWrap_Loong_Title;

		p->stOthers.u16TitleID		= (UINT16)(*pRes)[0].GetDword();
		p->stOthers.dwStateMark	= (*pRes)[1].GetDword();
		p->dwRoleID			= (*pRes)[2].GetDword();

		pTitle->push_back(p);

	}while(pRes->NextRow());
}
//名帖
VOID CLoongLoongDBCtrl::ProcResQueryVisitingCard(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pVisitingCard, pData, std::vector <tagTWrap_Loong_VisitingCard*>);
	do{
		tagTWrap_Loong_VisitingCard *p = new tagTWrap_Loong_VisitingCard;

		p->stOthers.dwLevel			= (*pRes)[0].GetInt();
		p->stOthers.dwJob			= (*pRes)[1].GetInt();
		p->stOthers.dwMateRoleID		= (*pRes)[2].GetInt();
		p->stOthers.dwFactionID		= (*pRes)[3].GetInt();
		p->stOthers.dwPosition		= (*pRes)[4].GetInt();

		p->stOthers.customVCardData.bVisibility		= (*pRes)[5].GetBool();
		p->stOthers.customVCardData.bySex			= (*pRes)[6].GetByte();
		p->stOthers.customVCardData.byConstellation	= (*pRes)[7].GetByte();
		p->stOthers.customVCardData.byChineseZodiac	= (*pRes)[8].GetByte();
		p->stOthers.customVCardData.byArea			= (*pRes)[9].GetByte();
		p->stOthers.customVCardData.byProvince		= (*pRes)[10].GetByte();
		(*pRes)[11].GetTCHAR(p->stOthers.customVCardData.chCity, LEN_CITY_NAME);


		(*pRes)[12].GetTCHAR(p->tcHeadUrl, LEN_MAX_URL);
		(*pRes)[13].GetTCHAR(p->tcSignature, LEN_MAX_SIGNATURE);

		p->stOthers.dwRoleID		= (*pRes)[14].GetInt();

		pVisitingCard->push_back(p);

	}while(pRes->NextRow());
}
//元宝
VOID CLoongLoongDBCtrl::ProcResQueryYBAccount(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pYBAccount, pData, std::vector <tagTWrap_Loong_YBAccount*>);
	do{
		tagTWrap_Loong_YBAccount *p = new tagTWrap_Loong_YBAccount;

		p->dwRoleID = (*pRes)[0].GetDword();
		p->nYuanBao = (*pRes)[1].GetDword();
		p->nGold = (*pRes)[2].GetInt();
		p->nSilver = (*pRes)[3].GetInt();
		p->bSellOrder = (*pRes)[4].GetBool();
		p->bBuyOrder = (*pRes)[5].GetBool();

		pYBAccount->push_back(p);

	}while(pRes->NextRow());
}
//元宝交易订单
VOID CLoongLoongDBCtrl::ProcResQueryYuanBaoOrder(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pYuanBaoOrder, pData, std::vector <tagTWrap_Loong_YuanBaoOrder*>);
	do{
		tagTWrap_Loong_YuanBaoOrder *p = new tagTWrap_Loong_YuanBaoOrder;

		p->dwID = (*pRes)[0].GetDword();
		p->dwRoleID = (*pRes)[1].GetDword();
		p->eYBOType = (EYBOTYPE)(*pRes)[2].GetInt();
		p->nPrice = (*pRes)[3].GetInt();
		p->nNum = (*pRes)[4].GetInt();
		p->eYBOMode = (EYBOMODE)(*pRes)[5].GetInt();
		p->dwStartTime = (*pRes)[6].GetDword();
		p->dwEndTime =  (*pRes)[7].GetDword();
		p->nAvgPrice = (*pRes)[8].GetInt();
		p->nDealNum = (*pRes)[9].GetInt();

		pYuanBaoOrder->push_back(p);

	}while(pRes->NextRow());
}
//bill item
VOID CLoongLoongDBCtrl::ProcResQueryBillItem(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pBillItem, pData, std::vector <tagTWrap_Loong_BillItem*>);
	do{
		tagTWrap_Loong_BillItem *p = new tagTWrap_Loong_BillItem;

		strncpy(p->szTokenID,(*pRes)[0].GetString(),(*pRes)[0].GetLen()+1);
		p->dwAccountID = (*pRes)[1].GetDword();
		p->dwItemTypeID = (*pRes)[2].GetDword();
		p->nNum = (*pRes)[3].GetInt();

		pBillItem->push_back(p);

	}while(pRes->NextRow());
}
//bill yuanbao
VOID CLoongLoongDBCtrl::ProcResQueryBillYuanbao(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pBillYuanBao, pData, std::vector <tagTWrap_Loong_BillYuanBao*>);
	do{
		tagTWrap_Loong_BillYuanBao *p = new tagTWrap_Loong_BillYuanBao;

		strncpy(p->szTokenID,(*pRes)[0].GetString(),(*pRes)[0].GetLen()+1);
		p->dwAccountID = (*pRes)[1].GetDword();
		p->nNum = (*pRes)[2].GetInt();

		pBillYuanBao->push_back(p);

	}while(pRes->NextRow());
}
//bill yuanbao log
VOID CLoongLoongDBCtrl::ProcResQueryBillYuanbaoLog(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pBillYuanBaoLog, pData, std::vector <tagTWrap_Loong_BillYuanBaoLog*>);
	do{
		tagTWrap_Loong_BillYuanBaoLog *p = new tagTWrap_Loong_BillYuanBaoLog;

		strncpy(p->szTokenID,(*pRes)[0].GetString(),(*pRes)[0].GetLen()+1);
		p->dwAccountID = (*pRes)[1].GetDword();
		p->nNum = (*pRes)[2].GetInt();
		memcpy(p->szTime,(*pRes)[3].GetString(),(*pRes)[3].GetLen()+1);

		pBillYuanBaoLog->push_back(p);

	}while(pRes->NextRow());
}
//pet skill
VOID CLoongLoongDBCtrl::ProcResQueryPetSkill(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pPetSkill, pData, std::vector <tagTWrap_Loong_PetSkill*>);
	do{
		tagTWrap_Loong_PetSkill *p = new tagTWrap_Loong_PetSkill;

		p->stOthers.dwTypeID	= (*pRes)[0].GetDword();			//'宠物技能id', 
		p->dwPetID = (*pRes)[1].GetDword();							//宠物id', 
		p->stOthers.nPara1	= (*pRes)[2].GetDword();				// '参数1', 
		p->stOthers.nPara2	= (*pRes)[3].GetDword();				// '参数2', 

		pPetSkill->push_back(p);
	}while(pRes->NextRow());
}		
//pet data
VOID CLoongLoongDBCtrl::ProcResQueryPetData(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pPetData, pData, std::vector <tagTWrap_Loong_PetData*>);
	do{
		tagTWrap_Loong_PetData *p = new tagTWrap_Loong_PetData;

		p->dwPetID	= (*pRes)[0].GetDword();
		(*pRes)[1].GetTCHAR(p->tszName, X_SHORT_NAME);

		p->dwMasterID	= (*pRes)[2].GetDword();				// 主人ID
		p->dwProtoID	= (*pRes)[3].GetDword();				// 原型ID
		p->nQuality		= (*pRes)[4].GetInt();
		p->nAptitude	= (*pRes)[5].GetInt();
		p->nPotential	= (*pRes)[6].GetInt();
		p->nSpirit		= (*pRes)[7].GetInt();
		p->nExpCur		= (*pRes)[8].GetInt();
		p->nStep		= (*pRes)[9].GetInt();
		p->nGrade		= (*pRes)[10].GetInt();
		p->nTalentCount	= (*pRes)[11].GetInt();
		p->nWuXingEnergy= (*pRes)[12].GetInt();
		p->byPetState	= (*pRes)[13].GetInt();
		p->bLocked		= (*pRes)[14].GetBool();

		pPetData->push_back(p);

	}while(pRes->NextRow());
}

//vip stall
VOID CLoongLoongDBCtrl::ProcResQueryVipStall(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pVipStall, pData, std::vector <tagTWrap_Loong_VipStall*>);
	do{
		tagTWrap_Loong_VipStall *p = new tagTWrap_Loong_VipStall;

		BYTE index = (*pRes)[0].GetByte();
		p->byStallIndex	= index;
		p->dwOwnerID	= (*pRes)[1].GetDword();
		p->nRemainTime	= (*pRes)[2].GetInt();

		pVipStall->push_back(p);

	}while(pRes->NextRow());
}
//guild_commodity
VOID CLoongLoongDBCtrl::ProcResQueryGuildCommodity(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGuildCommodity, pData, std::vector <tagTWrap_Loong_GuildCommodity*>);
	do{
		tagTWrap_Loong_GuildCommodity *p = new tagTWrap_Loong_GuildCommodity;

		p->stOthers.dwRoleID = (*pRes)[0].GetDword();
		p->dwGuildID = (*pRes)[1].GetDword();
		p->stOthers.nLevel = (*pRes)[2].GetInt();
		p->stOthers.nTael = (*pRes)[3].GetInt();

		if(!(*pRes)[4].IsNull())
		{
			(*pRes)[4].GetBlob(&p->stOthers.sGoodInfo, (*pRes)[4].GetLen());
			p->dwGoodInfoSize = (*pRes)[4].GetLen();
		}

		pGuildCommodity->push_back(p);

	}while(pRes->NextRow());
}
//guild_skill
VOID CLoongLoongDBCtrl::ProcResQueryGuildSkill(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGuildSkill, pData, std::vector <tagTWrap_Loong_GuildSkill*>);
	do{
		tagTWrap_Loong_GuildSkill *p = new tagTWrap_Loong_GuildSkill;

		p->dwGuildID = (*pRes)[0].GetDword();
		p->stOthers.dwSkillID = (*pRes)[1].GetDword();
		p->stOthers.n16Progress = (*pRes)[2].GetInt();
		p->stOthers.nLevel = (*pRes)[3].GetInt();
		p->stOthers.bResearching = (*pRes)[4].GetBool();

		pGuildSkill->push_back(p);

	}while(pRes->NextRow());
}
//vip_netbar
VOID CLoongLoongDBCtrl::ProcResQueryVipBar(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pVipNetBar, pData, std::vector <tagTWrap_Loong_VipNetBar*>);
	do{
		tagTWrap_Loong_VipNetBar *p = new tagTWrap_Loong_VipNetBar;

		p->dwAccountID = (*pRes)[0].GetDword();
		p->dwLoginTime = (*pRes)[1].GetDword();

		pVipNetBar->push_back(p);

	}while(pRes->NextRow());
}
//guild_upgrade
VOID CLoongLoongDBCtrl::ProcResQueryGuildUpgrade(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pGuildFacilities, pData, std::vector <tagTWrap_Loong_GuildFacilities*>);
	do{
		tagTWrap_Loong_GuildFacilities *p = new tagTWrap_Loong_GuildFacilities;

		p->dwGuildID = (*pRes)[0].GetDword();
		p->eType = (EFacilitiesType)(*pRes)[1].GetInt();
		p->byLevel = (*pRes)[2].GetByte();
		p->n16Progress = (*pRes)[3].GetInt();

		for (int n=0; n<MAX_UPGRADE_NEED_ITEM_TYPE; n++)
		{
			p->dwItemTypeID[n]	= (*pRes)[n*2+4].GetDword();
			p->nItemNeed[n]		= (*pRes)[n*2+5].GetInt();
		}

		pGuildFacilities->push_back(p);

	}while(pRes->NextRow());
}
//commerce_rank
VOID CLoongLoongDBCtrl::ProcResQueryCommerceRank(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pCommerceRank, pData, std::vector <tagTWrap_Loong_CommerceRank*>);
	do{
		tagTWrap_Loong_CommerceRank *p = new tagTWrap_Loong_CommerceRank;

		p->stOthers.dwRoleID = (*pRes)[0].GetDword();
		p->dwGuildID = (*pRes)[1].GetDword();
		p->stOthers.nTimes = (*pRes)[2].GetInt();
		p->stOthers.nTael = (*pRes)[3].GetInt();

		pCommerceRank->push_back(p);

	}while(pRes->NextRow());
}
//activity
VOID CLoongLoongDBCtrl::ProcResQueryActivity(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pActivityData, pData, std::vector <tagTWrap_Loong_ActivityData*>);
	do{
		tagTWrap_Loong_ActivityData *p = new tagTWrap_Loong_ActivityData;

		p->dwID = (*pRes)[0].GetDword();

		if(!(*pRes)[1].IsNull())
		{
			(*pRes)[1].GetBlob(&p->Data, (*pRes)[1].GetLen());
		}

		pActivityData->push_back(p);

	}while(pRes->NextRow());
}
//left msg
VOID CLoongLoongDBCtrl::ProcResQueryLeftMsg(OUT LPVOID &pData, Beton::QueryResult *pRes)
{
	MTRANS_POINTER(pLeftMsg, pData, std::vector <tagTWrap_Loong_LeftMsg*>);
	do{
		tagTWrap_Loong_LeftMsg *p = new tagTWrap_Loong_LeftMsg;

		p->dwMsgID = (*pRes)[0].GetDword();
		p->dwRoleID = (*pRes)[1].GetDword();

		if(!(*pRes)[2].IsNull())
		{
			p->pcBlob = new CHAR[(*pRes)[2].GetLen()];
			p->dwBlobSize = (*pRes)[2].GetLen();

			(*pRes)[2].GetBlob(p->pcBlob, (*pRes)[2].GetLen());
		}

		pLeftMsg->push_back(p);

	}while(pRes->NextRow());
}