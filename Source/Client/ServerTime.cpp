#include "stdafx.h"
#include "ServerTime.h"

#include "..\WorldDefine\msg_time.h"

ServerTime::ServerTime()
: m_Trunk(this)
, m_NetCmdMgr(NULL)
, m_bSynchronized(false)
{
}

ServerTime* ServerTime::Inst()
{
	static ServerTime inst;
	return &inst;
}

void ServerTime::Init()
{
	m_NetCmdMgr = TObjRef<NetCmdMgr>();
	this->Destroy();
	m_NetCmdMgr->Register("NS_SynchronismTime", 
		(NETMSGPROC)(m_Trunk.sfp2(&ServerTime::OnNS_SynchronismTime)), 
		_T("NS_SynchronismTime"));
}

void ServerTime::Destroy()
{
	m_NetCmdMgr->UnRegister("NS_SynchronismTime", 
		(NETMSGPROC)(m_Trunk.sfp2(&ServerTime::OnNS_SynchronismTime)));
}

DWORD ServerTime::OnNS_SynchronismTime(tagNS_SynchronismTime *pCmd, DWORD)
{
	m_ServerTime = pCmd->dwTime;
	m_ClientTime = GetCurrentDWORDTime();
	m_bSynchronized = true;
	return 0;
}

EWeek ServerTime::GetCurrentWeekDay() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();

	return GetWeekDay(dwServerTime);
}

EWeek ServerTime::GetWeekDay( tagDWORDTime& time ) const
{
	DWORD dwWeek = WhichWeekday(time);
	return (EWeek)dwWeek;
}

WORD ServerTime::GetCurrentYear() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return (2000 + static_cast<WORD>(dwServerTime.year));
}

BYTE ServerTime::GetCurrentMonth() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return static_cast<BYTE>(dwServerTime.month);
}

BYTE ServerTime::GetCurrentDay() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return static_cast<BYTE>(dwServerTime.day);
}

BYTE ServerTime::GetCurrentHour() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return static_cast<BYTE>(dwServerTime.hour);
}

BYTE ServerTime::GetCurrentMinute() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return static_cast<BYTE>(dwServerTime.min);
}

BYTE ServerTime::GetCurrentSecond() const
{
	tagDWORDTime dwServerTime = CalCurrentServerDwordTime();
	return static_cast<BYTE>(dwServerTime.sec);
}

tagDWORDTime ServerTime::CalCurrentServerDwordTime() const
{
	if( !m_bSynchronized ) 
		return GetCurrentDWORDTime();
	DWORD dwDiff = CalcTimeDiff(GetCurrentDWORDTime(), m_ClientTime);
	tagDWORDTime dwServerTime = IncreaseTime(m_ServerTime, dwDiff);
	return dwServerTime;
}