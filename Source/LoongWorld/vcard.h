//-----------------------------------------------------------------------------
//!\file vcard.h
//!\author xlguo
//!
//!\date 2009-02-03
//!\last 2009-02-04
//!
//!\brief 角色名贴 loongworld用结构
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "..\WorldDefine\vcard_define.h"



class Role;
//-----------------------------------------------------------------------------
// 用户自定义信息
//-----------------------------------------------------------------------------
struct tagRoleCustomVCard
{
	BOOL	bVisibility;		// 可见性
	BYTE	bySex;				// 性别：默认为角色性别，有3项可选：男、女、保密。
	BYTE	byConstellation;	// 星座：下拉菜单选择，有13个可选，分别为12星座和保密，默认为保密。
	BYTE	byChineseZodiac;	// 生肖：下拉菜单选择，有13个可选	
	BYTE	byArea;				// 区域
	BYTE	byProvince;			// 省份

	TCHAR	tchCity[LEN_CITY_NAME];			// 城市

	tstring	strHeadUrl;		// 头像：默认为角色性别头像。
	tstring	strSignature;		// 个性签名：最多100个汉字。200个字符。

	tagRoleCustomVCard();
	~tagRoleCustomVCard();
	//-----------------------------------------------------------------------------
	// 初始化自定义数据
	//-----------------------------------------------------------------------------
	void Init(IN const tagCustomVCardData* pVCardData, OUT const BYTE*& pData);

	//-----------------------------------------------------------------------------
	// 设置头像URL
	//-----------------------------------------------------------------------------
	void HeadUrl(const BYTE* szUrl, BYTE bySize);

	//-----------------------------------------------------------------------------
	// 设置签名
	//-----------------------------------------------------------------------------
	void Signature(const BYTE* szSignature, BYTE bySize);

	//-----------------------------------------------------------------------------
	// 取头像
	//-----------------------------------------------------------------------------
	const TCHAR* HeadUrl()	const {	return strHeadUrl.c_str();		}

	//-----------------------------------------------------------------------------
	// 取签名
	//-----------------------------------------------------------------------------
	const TCHAR* Signature()const {	return strSignature.c_str();	}

	//-----------------------------------------------------------------------------
	// 序列化 为tagCustomVCardData
	//-----------------------------------------------------------------------------
	void Serialize(IN tagCustomVCardData* pCustomVCard, OUT BYTE*& pData);

	DWORD GetCustomVCardDataSize() const;

};

//-----------------------------------------------------------------------------
// 角色基本信息
//-----------------------------------------------------------------------------
struct tagRoleVCardBase
{
	DWORD	dwRoleID;			//角色ID：显示玩家的角色名称。
	DWORD	dwLevel;			//角色等级：显示玩家的角色等级。
	DWORD	dwJob;				//角色职业：显示玩家的角色职业，未转职角色显示为“浪人”
	DWORD	dwMateRoleID;		//角色配偶：显示玩家的角色配偶，未婚角色显示为空
	DWORD	dwFactionID;		//所属帮派：显示玩家的所属帮派名称，无帮派玩家显示为空。
	DWORD	dwPosition;			//帮派职位：显示玩家的帮派职位，无帮派玩家显示为空。

	tagRoleVCardBase(){m_pRole = NULL;}

	//-----------------------------------------------------------------------------
	// 序列化基本信息
	//-----------------------------------------------------------------------------
	void Serialize(tagVCardData* pVCardData);
	
protected:
	//-----------------------------------------------------------------------------
	// 初始化角色基本信息
	//-----------------------------------------------------------------------------
	BOOL InitUpdate(Role *pRole, const tagVCardData* pVCardData)
	{
		BOOL bNeedSave = FALSE;
		Init(pVCardData);
		if (P_VALID(pRole))
		{
			ASSERT( P_VALID(pRole) );
			m_pRole = pRole;
			bNeedSave = Refresh();
		}
		return bNeedSave;
	}
	void Init(const tagVCardData* pVCardData )
	{
		dwRoleID	= pVCardData->dwRoleID;	
		dwLevel		= pVCardData->dwLevel;
		dwJob		= pVCardData->dwJob;
		dwMateRoleID= pVCardData->dwMateRoleID;	
		dwFactionID	= pVCardData->dwFactionID;
		dwPosition	= pVCardData->dwPosition;
	}
public:
	//-----------------------------------------------------------------------------
	// 更新名帖基本信息	
	//-----------------------------------------------------------------------------
	BOOL Refresh();

protected:
	Role*	m_pRole;
};

//-----------------------------------------------------------------------------
// 角色名帖
//-----------------------------------------------------------------------------
struct tagRoleVCard:public tagRoleVCardBase
{
	//-----------------------------------------------------------------------------
	// 用户自定义卡片
	//-----------------------------------------------------------------------------
	tagRoleCustomVCard	customVCard;

	//-----------------------------------------------------------------------------
	// 初始化
	//-----------------------------------------------------------------------------
//	void Init(const BYTE*& pData, Role *pRole);

	//-----------------------------------------------------------------------------
	// 序列化 为tagVCardData
	//-----------------------------------------------------------------------------
	void Serialize(IN tagVCardData* pVCardData, OUT BYTE*& pData);

	//-----------------------------------------------------------------------------
	// 得到对应的tagVCardData尺寸
	//-----------------------------------------------------------------------------
	DWORD GetVCardDataSize() const;
	DWORD GetCustomDataSize() const;

	//-----------------------------------------------------------------------------
	// 测试用
	//-----------------------------------------------------------------------------
	static tagCustomVCardData* GetInitData();
	static VOID Test();

public:
	void Init(const BYTE*& pData, Role *pRole);
	void SetCustomData(tagCustomVCardData* pData);
	void SendVCard2Client(DWORD dwDstRoleID);
	void SendSaveDB();
	void SendLoadOffLineVCard(DWORD dwSrcRoleID, DWORD dwDstRoleID);
	void SendHeadUrlTo(DWORD dwDstRoleID);
	void SendNullUrlToMe(DWORD dwWhosHead);

	VOID NotifyClientGetVCard(DWORD dwRoleID, DWORD dwErrCode);
	VOID NotifyClientSetVCard(DWORD dwRoleID, DWORD dwErrCode);

};