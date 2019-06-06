#include "stdafx.h"
#include "ActivityMgr.h"
#include "ActivityBroadcastHandler.h"
#include "ServerTime.h"
#include "../WorldDefine/msg_activity.h"
#include "FestivalActivityPromptFrame.h"

namespace SortObj 
{
	bool CompareTime(int SrcHour, int SrcMinute, int DestHour, int DestMinute)
	{
		if (((SrcHour * 60 + SrcMinute) - (DestHour * 60 + DestMinute)) > 0)
			return true;
		return false;
	}

	bool IsActive(const tagActivityProtoData &activity)
	{
		if( (activity.week == ServerTime::Inst()->GetCurrentWeekDay())
			&& (CompareTime(activity.startHour, activity.startMinute, ServerTime::Inst()->GetCurrentHour(), ServerTime::Inst()->GetCurrentMinute()) == false)
			&& (CompareTime(activity.endHour, activity.endMinute, ServerTime::Inst()->GetCurrentHour(), ServerTime::Inst()->GetCurrentMinute()) == false) )
		{
			return true;
		}

		return false;
	}

	class SortByTime
	{
	public:
		bool operator()(const tagActivityProtoData &op1, const tagActivityProtoData &op2)
		{

			if (op1.week < op2.week)
			{
				return true;
			}
			else if (op1.week > op2.week)
			{
				return false;
			}
			else
			{
				if (op1.startHour < op2.startHour)
				{
					return true;
				}
				else if (op1.startHour > op2.startHour)
				{
					return false;
				}
				else
				{
					return op1.startMinute < op2.startMinute;
				}
			}

		}
	};

	class SortByQuestID
	{
	public:
		bool operator()(const tagActivityProtoData &op1, const tagActivityProtoData &op2)
		{
			return op1.id < op2.id;
		}
	};

	class SortByLevel
	{
	public:
		bool operator()(const tagActivityProtoData &op1, const tagActivityProtoData &op2)
		{
			if( op1.minLevel < op2.minLevel )
				return true;
			else if( op1.minLevel > op2.minLevel )
				return false;
			else 
				return op1.maxLevel < op2.maxLevel;
		}
	};

	class SortByNpcID
	{
	public:
		bool operator()(const tagActivityProtoData &op1, const tagActivityProtoData &op2)
		{
			return op1.acceptNpcID < op2.acceptNpcID;
		}
	};

}


ActivityMgr::ActivityMgr()
: m_trunk(this), 
m_pNetCmdMgr(NULL),
m_pGameFrameMgr(NULL),
m_pSession(NULL),
m_pFsectivalActivityFrame(NULL)
{
}

ActivityMgr::~ActivityMgr()
{

}

bool ActivityMgr::Init()
{
	m_pNetCmdMgr->Register("NS_ActivityTaskState", (NETMSGPROC)(m_trunk.sfp2(&ActivityMgr::OnNS_ActivityTaskState)), _T("NS_ActivityTaskState"));
	
	m_pNetCmdMgr->Register("NS_RoleInitOk", (NETMSGPROC)m_trunk.sfp2(&ActivityMgr::OnNS_RoleInitOk), _T("NS_RoleInitOk") );

	if (!LoadActivityProto())
		return false;
	
	ActivityBroadcastHandler::Inst()->Init();

	return true;	
}

VOID ActivityMgr::Update()
{
	if( P_VALID(m_pFsectivalActivityFrame) )
	{
		if( m_pFsectivalActivityFrame->CanDestroy() )
		{
			m_pGameFrameMgr->AddToDestroyList( m_pFsectivalActivityFrame );
			m_pFsectivalActivityFrame = NULL;
		}
	}
}

void ActivityMgr::Destroy()
{
	m_pNetCmdMgr->UnRegister("NS_ActivityTaskState", (NETMSGPROC)(m_trunk.sfp2(&ActivityMgr::OnNS_ActivityTaskState)));
	m_pNetCmdMgr->UnRegister("NS_RoleInitOk", (NETMSGPROC)(m_trunk.sfp2(&ActivityMgr::OnNS_RoleInitOk)) );

	ActivityBroadcastHandler::Inst()->Destroy();
	m_activities.clear();
}

ActivityMgr* ActivityMgr::Inst()
{
	static ActivityMgr inst;
	return &inst;
}

bool ActivityMgr::LoadActivityProto()
{
	if (!(m_data.LoadFromFile()))
		return false;
	InitActivityTable();

	return true;
}

void ActivityMgr::InitActivityTable()
{
	const ActivityProtoData::ActivityProtoMap &data = m_data.GetData();
	ActivityProtoData::ActivityProtoMap::const_iterator begin = data.begin();
	ActivityProtoData::ActivityProtoMap::const_iterator end = data.end();
	for (; begin != end; ++begin)
	{
		if (begin->second.mode == EActivityMode_Day)
		{
			for (int i = 0; i < 7; ++i)
			{
				tagActivityProtoData temp = begin->second;
				temp.week = static_cast<EWeek>(i);
				m_activities.push_back(temp);
			}
		}
		else
			m_activities.push_back(begin->second);
	}
}

DWORD ActivityMgr::OnNS_ActivityTaskState(tagNS_ActivityTaskState *pCmd, DWORD)
{
	return 0;	
}

void ActivityMgr::SortActivities(ActivityMgr::ESortType type)
{
	switch (type)
	{
	case EST_SortByTime:
		{
			SortObj::SortByTime sortobj;
			std::sort(m_activities.begin(), m_activities.end(), sortobj);
		} break;
	case EST_SortByName:
		{
			SortObj::SortByQuestID sortobj;
			std::sort(m_activities.begin(), m_activities.end(), sortobj);
		} break;
	case EST_SortByLevel:
		{
			SortObj::SortByLevel sortobj;
			std::sort(m_activities.begin(), m_activities.end(), sortobj);
		} break;
	case EST_SortByNpc:
		{
			SortObj::SortByNpcID sortobj;
			std::sort(m_activities.begin(), m_activities.end(), sortobj);
		} break;
	case EST_SortByAcceptTime:
		{
			
		} break;
	default:break;
	}
}

const ActivityMgr::ActivityVector& ActivityMgr::GetActivityVector()
{
	return m_activities;
}

UINT16 ActivityMgr::GetActivityID( DWORD index ) const
{
	if (index < m_activities.size())
		return m_activities[index].id;
	return -1;
}


//处理网络事件，服务器已经把所有的初始化消息都发完的时候，发送角色初始化完毕的消息
DWORD ActivityMgr::OnNS_RoleInitOk( tagNS_RoleInitOk* pNetCmd, DWORD dwParam )
{
	m_pFsectivalActivityFrame = (FestivalActivityPromptFrame*)TObjRef<GameFrameMgr>()->CreateFrame(_T("World"), _T("FestivalActivityPromptFrame"), _T("FestivalActivityPromptFrame"), 0);
	return 0;
}