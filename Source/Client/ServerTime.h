#pragma once;

#include "..\WorldDefine\time.h"
#include "..\WorldDefine\QuestDef.h"

struct tagNS_SynchronismTime;

class ServerTime
{
public:
	static ServerTime *Inst();
	void Init();
	void Destroy();
	
	// 取得当前是星期几
	EWeek GetCurrentWeekDay() const;

	// 获得指定日期是星期几
	EWeek GetWeekDay(tagDWORDTime& time) const;
	
	// 取得当前是哪一年
	WORD GetCurrentYear() const;

	// 取得当前是哪一月
	BYTE GetCurrentMonth() const;

	// 取得当前是哪一日
	BYTE GetCurrentDay() const;

	// 取得当前是哪一个小时
	BYTE GetCurrentHour() const;

	// 取得当前是哪一分钟
	BYTE GetCurrentMinute() const;

	// 取得当前是哪一秒
	BYTE GetCurrentSecond() const;
	
	// 取得当前服务器tagDWORDTime格式时间
	tagDWORDTime CalCurrentServerDwordTime() const;
	
private:
	ServerTime();
	
	DWORD OnNS_SynchronismTime(tagNS_SynchronismTime *pCmd, DWORD);
	

private:
	TSFPTrunk<ServerTime> m_Trunk;
	TObjRef<NetCmdMgr> m_NetCmdMgr;

	tagDWORDTime m_ServerTime;	// 同步发生时服务器的时间
	tagDWORDTime m_ClientTime;	// 同步发生时客户端的时间
	bool m_bSynchronized;		// 是否已经同步		
};

