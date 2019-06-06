//-----------------------------------------------------------------------------
//!\file vcard.cpp
//!\author xlguo
//!
//!\date 2009-02-03
//!\last 2009-02-04
//!
//!\brief 角色名贴 loongworld用结构
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "vcard.h"
#include "role.h"
#include "role_mgr.h"
#include "../ServerDefine/msg_vcard.h"

//-----------------------------------------------------------------------------
// 更新名帖基本信息	
//-----------------------------------------------------------------------------
#define CHGANDSAVE(a1, a2)	if((a1) != (a2)){a1 = (a2);bSave = TRUE;	}
BOOL tagRoleVCardBase::Refresh()
{
	if (!P_VALID(m_pRole))
	{
		return FALSE;
	}

	ASSERT( dwRoleID == m_pRole->GetID());
	
	BOOL bSave = FALSE;
	CHGANDSAVE(dwLevel, m_pRole->GetLevel());

	// 	dwJob			= pRole->GetJob();
	// 	dwMateRoleID	= pRole->GetMate();
	// 	dwFactionID		= pRole->GetFaction();
	// 	dwPosition		= pRole->GetPosition();

	return bSave;
}
#undef CHGANDSAVE

tagRoleCustomVCard::tagRoleCustomVCard()
{

}
tagRoleCustomVCard::~tagRoleCustomVCard()
{
}

//-----------------------------------------------------------------------------
// 初始化自定义数据
//-----------------------------------------------------------------------------
void tagRoleCustomVCard::Init(IN const tagCustomVCardData* pVCardData, OUT const BYTE* &pData)
{
 	ASSERT( P_VALID(pVCardData) );

	DWORD dwSize = sizeof(tagCustomVCardData) - 1 + pVCardData->byHeadUrlSize + pVCardData->bySignatureSize;

	bVisibility		= pVCardData->bVisibility;
	bySex			= pVCardData->bySex;
	byConstellation	= pVCardData->byConstellation;
	byChineseZodiac = pVCardData->byChineseZodiac;
	byProvince		= pVCardData->byProvince;
	byArea			= pVCardData->byArea;

	IFASTCODE->MemCpy(tchCity, pVCardData->chCity, sizeof(TCHAR) * LEN_CITY_NAME);


	HeadUrl(pVCardData->byData, pVCardData->byHeadUrlSize);
	Signature(pVCardData->byData + pVCardData->byHeadUrlSize, pVCardData->bySignatureSize);

	pData = reinterpret_cast<const BYTE *>(pVCardData) + dwSize;
}

//-----------------------------------------------------------------------------
// 设置头像URL
//-----------------------------------------------------------------------------
void tagRoleCustomVCard::HeadUrl(const BYTE* szUrl, BYTE bySize)
{
	// 客户端不可直接更改
	if (bySize <= 2)
	{
		return;
	}
	strHeadUrl = (TCHAR*)szUrl;
}


//-----------------------------------------------------------------------------
// 设置签名
//-----------------------------------------------------------------------------
void tagRoleCustomVCard::Signature(const BYTE* szSignature, BYTE bySize)
{
	strSignature = (TCHAR*)szSignature;
}


//-----------------------------------------------------------------------------
// 序列化 为tagCustomVCardData
//-----------------------------------------------------------------------------
void tagRoleCustomVCard::Serialize(IN tagCustomVCardData* pCustomVCard, OUT BYTE*& pData)
{
	DWORD dwSize = sizeof(tagCustomVCardData) - 1 + (strHeadUrl.size() + strSignature.size() + 2) * sizeof(TCHAR);

	pCustomVCard->bVisibility		= bVisibility;
	pCustomVCard->byChineseZodiac	= byChineseZodiac;
	pCustomVCard->byConstellation	= byConstellation;
	pCustomVCard->bySex				= bySex;
	pCustomVCard->byProvince		= byProvince;
	pCustomVCard->byArea			= byArea;
	IFASTCODE->MemCpy(pCustomVCard->chCity, tchCity, sizeof(TCHAR) * LEN_CITY_NAME);

	pCustomVCard->byHeadUrlSize		= (strHeadUrl.size() + 1) * sizeof(TCHAR);
	pCustomVCard->bySignatureSize	= (strSignature.size() + 1) * sizeof(TCHAR);
	IFASTCODE->MemCpy(pCustomVCard->byData, strHeadUrl.c_str(), pCustomVCard->byHeadUrlSize);
	IFASTCODE->MemCpy(pCustomVCard->byData + pCustomVCard->byHeadUrlSize, strSignature.c_str(), pCustomVCard->bySignatureSize);

	pData = reinterpret_cast<BYTE *>(pCustomVCard) + dwSize;
}


DWORD tagRoleCustomVCard::GetCustomVCardDataSize() const
{
	return sizeof(tagCustomVCardData) - 1 + (strHeadUrl.size() + strSignature.size() + 2) * sizeof(TCHAR);
}

//-----------------------------------------------------------------------------
// 序列化基本信息
//-----------------------------------------------------------------------------
void tagRoleVCardBase::Serialize(tagVCardData* pVCardData)
{
	pVCardData->dwRoleID	= dwRoleID;
	pVCardData->dwLevel		= dwLevel;
	pVCardData->dwJob		= dwJob;
	pVCardData->dwMateRoleID= dwMateRoleID;
	pVCardData->dwFactionID	= dwFactionID;
	pVCardData->dwPosition	= dwPosition;
}




tagCustomVCardData* tagRoleVCard::GetInitData()
{
	TCHAR* pTchHead = _T("");
	TCHAR* pTchSign = _T("");
	DWORD dwHeadSize = (_tcslen(pTchHead) + 1) * sizeof(TCHAR);
	DWORD dwSignSize = (_tcslen(pTchSign) + 1) * sizeof(TCHAR);

	DWORD dwCustomSize = sizeof(tagCustomVCardData) - 1 + dwHeadSize + dwSignSize;

	tagCustomVCardData* pCustom = reinterpret_cast<tagCustomVCardData * >(new BYTE[dwCustomSize]);


	pCustom->bVisibility = FALSE;
	pCustom->byArea = 0;
	pCustom->byChineseZodiac = 0;
	pCustom->byConstellation = 0;
	pCustom->byProvince = 0;
	pCustom->bySex = 0;
	pCustom->byHeadUrlSize = (BYTE)dwHeadSize;
	pCustom->bySignatureSize = (BYTE)dwSignSize;
	memcpy(pCustom->chCity, _T(""), LEN_CITY_NAME* sizeof(TCHAR));
	memcpy(pCustom->byData, pTchHead, dwHeadSize);		
	memcpy(pCustom->byData + dwHeadSize, pTchSign, dwSignSize);		
	return pCustom;
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
void tagRoleVCard::Init(const BYTE*& pData, Role *pRole)
{
	const tagVCardData* pVCData = (const tagVCardData*)pData;
	BOOL bSave = tagRoleVCardBase::InitUpdate(pRole, pVCData);

	// 初始化自定义信息
	const BYTE* pCustomData = (const BYTE*)&pVCData->customVCardData;
	customVCard.Init(&pVCData->customVCardData, pCustomData);
	pData = pCustomData;

	if (bSave)
	{
		// 更新数据库
		DWORD dwSaveDB = sizeof(tagNDBC_VCardChg) - sizeof(tagVCardData) + GetVCardDataSize();

		MCREATE_MSG(pSendDB, dwSaveDB, NDBC_VCardChg);
		pSendDB->dwRoleID = dwRoleID;

		BYTE* pByte = NULL;
		Serialize(&(pSendDB->VCardData), pByte);

		ASSERT(pByte- (BYTE *)(&(pSendDB->VCardData)) == GetVCardDataSize());

		// 发送
		g_dbSession.Send(pSendDB, pSendDB->dwSize);
		MDEL_MSG(pSendDB);
	}
}

//-----------------------------------------------------------------------------
// 序列化 为tagVCardData
//-----------------------------------------------------------------------------
void tagRoleVCard::Serialize(IN tagVCardData* pVCardData, OUT BYTE*& pData)
{

	// 序列化基本信息
	tagRoleVCardBase::Serialize(pVCardData);

	// 序列化自定义信息
	customVCard.Serialize(&pVCardData->customVCardData, pData);
}

//-----------------------------------------------------------------------------
// 得到对应的tagVCardData尺寸
//-----------------------------------------------------------------------------
DWORD tagRoleVCard::GetVCardDataSize() const
{
	return sizeof(tagVCardData) - sizeof(tagCustomVCardData) + GetCustomDataSize();
}
DWORD tagRoleVCard::GetCustomDataSize() const
{
	return customVCard.GetCustomVCardDataSize();
}

#include "../ServerDefine/base_define.h"
#include "../WorldDefine/msg_role_vcard.h"
VOID tagRoleVCard::Test()
{
	tagRoleVCard testvcard;
// 	testvcard.Init(/*tagVCardData**/ pData);
// 	testvcard.SetCustomData(/*tagCustomVCardData**/ pData);
// 	testvcard.SendVCard2Client(/*DWORD */dwDstRoleID);
// 	testvcard.SendSaveDB();
// 	testvcard.SendLoadOffLineVCard(/*DWORD */dwDstRoleID);
	
// 	tagRoleVCard vcard;
// 
// 	tagCustomVCardData* pvcData = tagRoleVCard::GetInitData();
// 	const BYTE* pVCData = (BYTE *)pvcData;
// 	vcard.Init(pVCData, NULL);
// 	delete pvcData;
// 
// 	DWORD dwSize = sizeof(tagNS_GetRoleVCard) - 1 + vcard.GetVCardDataSize();
// 
// 	MCREATE_MSG(pSend, dwSize, NS_GetRoleVCard);
// 	pSend->dwRoleID = 1;
// 	pSend->dwErrorCode = E_VCard_Success;
// //	vcard.Refresh();
// 	BYTE* pByte = NULL;
// 	vcard.Serialize(reinterpret_cast<tagVCardData *>( pSend->pData ), pByte);
// 
// 	ASSERT(pByte - pSend->pData == vcard.GetVCardDataSize());
// 
// 	tagVCardData* pcustomvcdata = reinterpret_cast<tagVCardData *>( pSend->pData );
// 	const BYTE* pDFADF = (BYTE*)(&(pcustomvcdata->customVCardData));
// 	vcard.Init(pDFADF, NULL);
// 
// 	MDEL_MSG(pSend);
// 
// 
// 	BYTE vcdata[1000];
// 	LPVOID pVdCData = vcdata;
// 	vcard.Save2DB(pVdCData, pVdCData);
}

void tagRoleVCard::SetCustomData( tagCustomVCardData* pData )
{
	const BYTE* pByte = NULL;
	customVCard.Init( pData, pByte);
}

void tagRoleVCard::SendVCard2Client( DWORD dwDstRoleID )
{
	Role* pDst = g_roleMgr.GetRolePtrByID(dwDstRoleID);
	if (!P_VALID(pDst))
		return;

	DWORD dwSize = sizeof(tagNS_GetRoleVCard) - 1 + GetVCardDataSize();
	MCREATE_MSG(pSend, dwSize, NS_GetRoleVCard);

	pSend->dwRoleID = dwRoleID;
	pSend->dwErrorCode = E_VCard_Success;
	Refresh();
	BYTE* pByte = NULL;
	Serialize(reinterpret_cast<tagVCardData *>( pSend->pData ), pByte);
	ASSERT(pByte - pSend->pData == GetVCardDataSize());
	pDst->SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

void tagRoleVCard::SendSaveDB()
{
	DWORD dwSaveDB = sizeof(tagNDBC_VCardChg) - sizeof(tagVCardData) + GetVCardDataSize();
	MCREATE_MSG(pSendDB, dwSaveDB, NDBC_VCardChg);

	pSendDB->dwRoleID = dwRoleID;
	BYTE* pByte = NULL;
	Serialize(&pSendDB->VCardData, pByte);
	ASSERT(pByte - (BYTE *)(&(pSendDB->VCardData)) == GetVCardDataSize());
	g_dbSession.Send(pSendDB, pSendDB->dwSize);

	MDEL_MSG(pSendDB);
}

void tagRoleVCard::SendLoadOffLineVCard( DWORD dwSrcRoleID, DWORD dwDstRoleID )
{
	tagNDBC_GetOffLineVCard send;
	send.dwRoleID = dwSrcRoleID;
	send.dwQueryID = dwDstRoleID;
	g_dbSession.Send(&send, send.dwSize);
}

void tagRoleVCard::SendHeadUrlTo( DWORD dwDstRoleID )
{
	Role* pDst = g_roleMgr.GetRolePtrByID(dwDstRoleID);
	if (!P_VALID(pDst))
		return;

	const TCHAR* pUrl = customVCard.HeadUrl();
	DWORD dwUrlSize = (_tcslen(pUrl) + 1) * sizeof(TCHAR);
	DWORD dwMsgSize = sizeof(tagNS_GetRoleHeadPicURL) - 1 + dwUrlSize;

	MCREATE_MSG(pSend, dwMsgSize, NS_GetRoleHeadPicURL);
	pSend->dwRoleID = dwRoleID;
	pSend->byHeadUrlSize = BYTE(dwUrlSize);
	IFASTCODE->MemCpy(pSend->byData, pUrl, dwUrlSize);
	pDst->SendMessage(pSend, pSend->dwSize);
	MDEL_MSG(pSend);
}

void tagRoleVCard::SendNullUrlToMe( DWORD dwWhosHead )
{
	tagNS_GetRoleHeadPicURL send;
	send.dwRoleID = dwWhosHead;
	send.byHeadUrlSize = 0;
	ASSERT(P_VALID(m_pRole));
	m_pRole->SendMessage(&send, send.dwSize);
}

VOID tagRoleVCard::NotifyClientGetVCard( DWORD dwRoleID, DWORD dwErrCode )
{
	ASSERT(dwErrCode != E_VCard_Success);
	tagNS_GetRoleVCard send;
	send.dwRoleID = dwRoleID;
	send.dwErrorCode = dwErrCode;
	ASSERT(P_VALID(m_pRole));
	m_pRole->SendMessage(&send, send.dwSize);
}

VOID tagRoleVCard::NotifyClientSetVCard( DWORD dwRoleID, DWORD dwErrCode )
{
	tagNS_SetRoleVCard send;
	send.dwRoleID = dwRoleID;
	send.dwErrorCode = dwErrCode;
	ASSERT(P_VALID(m_pRole));
	m_pRole->SendMessage(&send, send.dwSize);
}