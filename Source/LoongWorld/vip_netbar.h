//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: vip_netbar.h
// author: xlguo
// actor:
// data: 2009-8-3
// last:
// brief: ½ðÅÆÍø°É
//-------------------------------------------------------------------------------
#pragma once
#include <vector>

//-------------------------------------------------------------------------
// ip¶Î
//-------------------------------------------------------------------------
class IpRange
{
public:
	IpRange(DWORD dwIpMin, DWORD dwIpMax, DWORD dwVNBId)
		:m_IPMin(dwIpMin), m_IPMax(dwIpMax), m_dwVNBId(dwVNBId){}
	BOOL	Fit(DWORD dwIP)			const	{	return dwIP >= m_IPMin && dwIP <= m_IPMax;	}
	BOOL	OnLeftOf(DWORD dwIP)	const	{	return dwIP > m_IPMax;	}
	BOOL	OnRightOf(DWORD dwIP)	const	{	return dwIP < m_IPMin;	}
	DWORD	GetVNBId()				const	{	return m_dwVNBId;		}
	DWORD	GetIpMin()				const	{	return m_IPMin;			}
	DWORD	GetIpMax()				const	{	return m_IPMax;			}
private:
	DWORD	m_IPMin;
	DWORD	m_IPMax;
	DWORD	m_dwVNBId;
};


struct tagDBVNBPlayers;
struct tagVipNetBar;

//-------------------------------------------------------------------------
// ½ðÅÆÍø°É¹ÜÀíÆ÷
//-------------------------------------------------------------------------
class VipNerBarMgr
{
	typedef TMap<DWORD, tagVipNetBar*>	VipNetBarMap;
	typedef std::vector<IpRange>		IpRangeVector;
	typedef std::set<DWORD>				AccountIDSet;
	typedef TMap<DWORD, DWORD>			IP2VNBIdMap;
	typedef std::set<DWORD>				NotifySet;

public:
	BOOL	Init();
	VOID	Destroy();
	VOID	InitData(tagDBVNBPlayers* pInitData);
	VOID	PlayerLogin(DWORD dwAccountID, DWORD dwIP);
	VOID	PlayerLogout(DWORD dwIP);
	VOID	PlayerNotify(DWORD dwAccountID);
	INT		GetRate(DWORD dwIP, INT nType);
	LPCTSTR	GetVNBName(DWORD dwIP);
	

private:
	DWORD	TransTSIp2DWORD(LPCTSTR szIP);
	tagVipNetBar* GetVipNetBar(DWORD dwIP);
	DWORD	GetVNBId(DWORD dwIp);
	DWORD	FitNetBar(DWORD dwIp);
	VOID	UpdateDbPlayerLogin(DWORD dwAccountID, DWORD dwTime);
	VOID	GeneralzeIP(DWORD &dwIP);

private:
	IP2VNBIdMap			m_mapIp2VNBId;
	VipNetBarMap		m_mapVipNetBars;
	IpRangeVector		m_vecIpRanges;				// ÓÐÐò
	AccountIDSet		m_setHistoryAccountID;
	AccountIDSet		m_setTodayAccountID;
	NotifySet			m_setNotify;				
public:
	BOOL DynamicTest(DWORD dwTestNo, DWORD dwArg1, LPCTSTR szArg2);
};

extern VipNerBarMgr g_VipNetBarMgr;