//-----------------------------------------------------------------------------
//!\file vcard_define.h
//!\author client/server hyu/xlguo
//!
//!\date 2009-02-03
//! last 
//!
//!\brief 角色名贴通信用结构 LoongWorld<->LoongDB  LoongWorld<->client
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "msg_common_errorcode.h"

#pragma pack(push,1)

//----------------------------------------------------------------------------
// 12星座
//----------------------------------------------------------------------------
enum EnumVCardConstellation
{
	EVC_Aries			=		1,		//白羊座(3.21-4.20)
	EVC_Taurus			=		2,		//金牛座(4.21-5.21)
	EVC_Gemini			=		3,		//双子座(5.22-6.21)
	EVC_Cancer			=		4,		//巨蟹座(6.22-7.22)
	EVC_Leo				=		5,		//狮子座(7.23-8.23)
	EVC_Virgo			=		6,		//处女座(8.24-9.23)
	EVC_Libra			=		7,		//天秤座(9.24-10.23)
	EVC_Scorpio			=		8,		//天蟹座(10.24-11.22)
	EVC_Sagittarius		=		9,		//射手座(11.23-12.21)
	EVC_Capricorn		=		10,		//摩羯座(12.22-1.20)
	EVC_Aquarius		=		11,		//水瓶座(1.21-2.19)
	EVC_Pisces			=		12,		//双鱼座(2.20-3.20)

	EVC_Default			=		13		//默认保密
};

//----------------------------------------------------------------------------
// 12生肖
//----------------------------------------------------------------------------
enum EnumVCardChineseZodiac
{
	EVCZ_Rat				=		1,		//子鼠
	EVCZ_Ox					=		2,		//丑牛
	EVCZ_Tiger				=		3,		//寅虎
	EVCZ_Rabbit				=		4,		//卯兔
	EVCZ_Loong				=		5,		//辰龙
	EVCZ_Snake				=		6,		//巳蛇
	EVCZ_Horse				=		7,		//午马
	EVCZ_Goat				=		8,		//未羊
	EVCZ_Monkey				=		9,		//申猴
	EVCZ_Rooster			=		10,		//酉鸡
	EVCZ_Dog				=		11,		//戌狗
	EVCZ_Boar				=		12,		//亥猪

	EVCZ_Default			=		13		//默认保密
};

//----------------------------------------------------------------------------
// 姓别
//----------------------------------------------------------------------------
enum EnumVCardSex
{
	EVS_Male				=		1,		//男
	EVS_Female				=		2,		//女
	EVS_Default				=		3,		//默认保密
};

//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	E_VCard_Success					= E_Success,	// 成功
	E_VCard_NotVisible				= 1,			// 名帖不可见
	E_VCard_NoPrivilege				= 2,			// 无改变权限
	E_VCard_Failed					= 3,			// 发生错误
};

enum
{
	LEN_CITY_NAME					= 6,
	LEN_MAX_URL						= 200,
	LEN_MAX_SIGNATURE				= 200,
};

//-----------------------------------------------------------------------------
// 用户自定义信息数据(LoongWorld<->LoongDB，client->LoongWorld)
//-----------------------------------------------------------------------------
struct tagCustomVCardData
{
	BOOL	bVisibility;			// 可见性
	BYTE	bySex;					// 性别：默认为角色性别，有3项可选：男、女、保密。
	BYTE	byConstellation;		// 星座：下拉菜单选择，有13个可选，分别为12星座和保密，默认为保密。
	BYTE	byChineseZodiac;		// 生肖：下拉菜单选择，有13个可选
	BYTE	byArea;					// 区域（0为空）
	BYTE	byProvince;				// 省份（0为空）

	TCHAR	chCity[LEN_CITY_NAME];	// 城市(以'\0'结束)
	BYTE	byHeadUrlSize;			// 头像尺寸(byte长度)
	BYTE	bySignatureSize;		// 签名尺寸 最大200(byte长度)
	BYTE	byNameHistory;			// 改名记录
	BYTE	byData[1];				// 头像：默认为角色性别头像。个性签名：最多100个汉字。200个字符。(以'\0'结束); 更名记录
};

//-----------------------------------------------------------------------------
// 名帖数据(LoongWorld->client)
//-----------------------------------------------------------------------------
struct tagVCardData
{
	DWORD				dwRoleID;			// 角色ID：显示玩家的角色名称。
	DWORD				dwLevel;			// 角色等级：显示玩家的角色等级。
	DWORD				dwJob;				// 角色职业：显示玩家的角色职业，未转职角色显示为“浪人”
	DWORD				dwMateRoleID;		// 角色配偶：显示玩家的角色配偶，未婚角色显示为空
	DWORD				dwFactionID;		// 所属帮派：显示玩家的所属帮派名称，无帮派玩家显示为空。
	DWORD				dwPosition;			// 帮派职位：显示玩家的帮派职位，无帮派玩家显示为空。
	
	tagCustomVCardData 	customVCardData;	// 用户自定义数据
};
#pragma pack(pop)