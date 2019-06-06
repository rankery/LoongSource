#include "StdAfx.h"
#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_vip_netbar.h"
DWORD CLoong::UpdateVNBPlayer( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_UpdateVNBPlayer);
	m_pDB->UpdateVNBPlayer(pRecv->dwAccountID, pRecv->dwLoginTime);

	return E_Success;
}

DWORD CLoong::LoadVNBData( DWORD pDBMsg, DWORD dwDummy )
{
	MTRANS_POINTER(pRecv, pDBMsg, tagNDBC_GetVNBData);

	char buffer[1024 * 10] = {0};
	tagNDBS_GetVNBData* pSend = (tagNDBS_GetVNBData*)buffer;
	tagNDBS_GetVNBData send;
	*pSend = send;

	m_pDB->LoadVNBData(&pSend->players, pRecv->dwDate);
	INT nAllNum = pSend->players.nHisPlayers + pSend->players.nTodaysPlayers;
	pSend->dwSize = nAllNum == 0 ? sizeof(tagNDBS_GetVNBData) : sizeof(tagNDBS_GetVNBData)  - 1 + nAllNum * sizeof(DWORD);
	SendMsg( pSend, pSend->dwSize);

	return E_Success;
}

BOOL CLoongDB::UpdateVNBPlayer(DWORD dwAccountID, DWORD dwLoginTime)
{
	return Update(dwAccountID, &dwLoginTime, 1, sizeof(DWORD), &CLoongDB::FormatUpdateVNBPlayer);
}

VOID CLoongDB::FormatUpdateVNBPlayer(Beton::MyStream *pStream, DWORD dwAccountID, PVOID pData)
{
	pStream->SetReplace("vip_netbar");
	pStream->FillString("accountid=") << dwAccountID;
	pStream->FillString(",login_time=") << *((DWORD*)pData);
}

BOOL CLoongDB::LoadVNBData(OUT PVOID pData, DWORD dwDate)
{
	MTRANS_POINTER(pVNBData, pData, tagDBVNBPlayers);
	PVOID pTmp = pVNBData->dwAccountIDs;

	Load(pTmp, &pVNBData->nHisPlayers, GT_INVALID, GT_INVALID, &CLoongDB::FormatLoadHistroyVNBPlayers, &CLoongDB::ProcResLoadVNBPlayers);
	Load(pTmp, &pVNBData->nTodaysPlayers, dwDate, GT_INVALID, &CLoongDB::FormatLoadTodayVNBPlayers, &CLoongDB::ProcResLoadVNBPlayers);

	return TRUE;
}

VOID CLoongDB::FormatLoadHistroyVNBPlayers(Beton::MyStream *pStream, DWORD dwDate, DWORD dwData)
{
	pStream->SetSelect("vip_netbar", "accountid, login_time");
}

VOID CLoongDB::FormatLoadTodayVNBPlayers(Beton::MyStream *pStream, DWORD dwDate, DWORD dwData)
{
	pStream->SetSelect("vip_netbar", "accountid, login_time");
	pStream->SetWhere();
	pStream->FillString("login_time=")<<dwDate;
}

VOID CLoongDB::ProcResLoadVNBPlayers(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	ASSERT(pNum != NULL);

	MTRANS_POINTER(pAccountIDs, pData, DWORD);
	*pNum = pRes->GetRowCount();

	for(INT16 i=0; i<*pNum; ++i)
	{
		pAccountIDs[i]		= (*pRes)[0].GetDword();
		pRes->NextRow();
	}

	pData = &pAccountIDs[*pNum];
}
