#pragma once

#include "ActivityProtoData.h"

struct tagNS_RoleInitOk;
// 前向声明
struct tagNS_ActivityTaskState;
class FestivalActivityPromptFrame;

class ActivityMgr
{

public:
	enum EActivityState
	{
		EAS_Start,		// 开始了
		EAS_End, 		// 结束了
		EAS_StartSoon,	// 即将开始
		EAS_EndSoon		// 即将结束
	};
	
	enum ESortType
	{
		EST_None,
		EST_SortByTime,
		EST_SortByName,
		EST_SortByLevel,
		EST_SortByNpc,
		EST_SortByAcceptTime
	};
	
	typedef std::vector<tagActivityProtoData> ActivityVector;

public:
	~ActivityMgr();
	bool Init();
	VOID Update();
	void Destroy();
	
	static ActivityMgr* Inst();
	const ActivityProtoData& GetData() const { return m_data; }
	const ActivityVector& GetActivityVector();
	void SortActivities(ESortType type);
	UINT16 GetActivityID(DWORD index) const;

private:
	ActivityMgr();

	bool LoadActivityProto();
	
	// 初始化活动表 
	void InitActivityTable();

	// 响应服务器活动状态网络消息 
	DWORD OnNS_ActivityTaskState(tagNS_ActivityTaskState *pCmd, DWORD);

	DWORD OnNS_RoleInitOk( tagNS_RoleInitOk* pNetCmd, DWORD dwParam );
	
private:
	TSFPTrunk<ActivityMgr> m_trunk;
	TObjRef<NetCmdMgr> m_pNetCmdMgr;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;
	TObjRef<NetSession> m_pSession;
	
	FestivalActivityPromptFrame*	m_pFsectivalActivityFrame;
	ActivityProtoData m_data;								// 活动任务属性表
	ActivityVector m_activities;
};
