//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: stall.h
// author: Sxg
// actor:
// data: 2008-12-17
// last:
// brief: 摆摊
//-------------------------------------------------------------------------------
#pragma once

class Role;

struct tagRoleDataSave;
struct tagStallGoods;
//-------------------------------------------------------------------------------
// 常量
//-------------------------------------------------------------------------------
const INT STALL_BROADCAST_INTERVAL_TICK	= 2 * 60 * TICK_PER_SECOND;	// 2分钟
const INT STALL_GAINEXP_INTERVAL_TIME	= 60;						// 60秒
const INT STALL_MAX_DAILY_EXP			= 1000;
const BYTE STALL_MAX_LEVEL				= 10;
const BYTE STALL_EXP_LEVEL_LIMIT[STALL_MAX_LEVEL]	= {1, 20, 20, 40, 40, 60, 60, 80, 80, 100};

//-------------------------------------------------------------------------------
class Stall
{
public:
	Stall(Role *pRole, const tagRoleDataSave* pRoleData);
	~Stall();

public:
	DWORD Init(FLOAT fCityTaxRate);
	DWORD Destroy();
	VOID  Update();

	DWORD SetTitle(LPCTSTR strTitle);
	DWORD SetAdText(LPCTSTR strAd);
	DWORD SetAdFlag(bool bBroadcast);
	DWORD SetGoods(const INT64 n64Serial, const INT64 n64UnitPrice, const BYTE byIndex);
	DWORD UnSetGoods(const BYTE byIndex);
	DWORD SetFinish();

	BOOL  IsEmpty()			const { return 0 >= m_n8GoodsNum; }
	BYTE  GetModeLevel()	const { return m_byLevel; }
	//INT8  GetGoodsNum()		const { return m_n8GoodsNum ; }

public:
	DWORD GetStallTitle(OUT LPTSTR pSzTitle);
	DWORD GetSpecGoods(BYTE byIndex, OUT LPVOID pData, OUT INT &nGoodsSz);
	DWORD GetGoods(OUT LPVOID pData, OUT BYTE &byGoodsNum, OUT INT &nGoodsSz);
	DWORD Sell(Role *pRole, INT64 n64UnitSilver, INT64 n64Serial, BYTE byIndex, INT16 n16Num, OUT INT16 &n16RemainNum);
	INT32 CalMemSzGoodsUsed() const;

public:
	DWORD GainExp(INT32 nExp);	// GM命令用

public:
	VOID  Save2DB(tagRoleDataSave* pRoleData);

private:
	DWORD CanStall();
	VOID  UnSet(const BYTE byIndex);

private:
	VOID  StallUpgrade(BOOL bNextDay);
	BOOL  IsSameDay(DWORD dwTime);

private:
	// 角色初始化时确定的属性
	Role			*m_pRole;
	FLOAT			m_fDeTaxRate;		// 税收减免
	INT				m_nTotalTax;		// 缴税总额(单位：金)	-- 仅该值需保存到数据库中
	BYTE			m_byLevel;			// 摊位模型等级
	INT32			m_nDailyExp;		// 当天摆摊经验
	INT32			m_nCurExp;			// 当前等级经验
	bool			m_bBroadcast;		// 是否需要广播广告

	// 摆摊时才能确定的属性
	INT8			m_n8GoodsNum;		// 当前摊位上的待售商品
	FLOAT			m_fCityTaxRate;		// 城镇税率
	TCHAR			*m_strTitle;		// 标题
	TCHAR			*m_strAd;			// 广告语
	tagStallGoods	*m_pStallGoods;		// 摊位商品

	// 向客户端传输时的物品结构大小
	static INT		m_SzStallItem;		// tagMsgStallGoods结构中数据为Item时的大小
	static INT		m_SzStallEquip;		// tagMsgStallGoods结构中数据为Equip时的大小

	// 更新计时
	DWORD			m_dwLastAdBroadcastTick;	// 上一次播放广告的tick
	DWORD			m_dwLastUpgradeTime;		// 上一次获得经验的时间
};