#include "stdafx.h"
#include "vip_netbar.h"

#include "../WorldDefine/msg_vip_netbar.h"

#include "../ServerDefine/vip_netbar_define.h"
#include "../ServerDefine/msg_vip_netbar.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "world_session.h"
#include "att_res.h"
#include "player_session.h"
#include "db_session.h"
#include "item_creator.h"
#include "item_mgr.h"

VipNerBarMgr g_VipNetBarMgr;

class IPComp
{
public:
	bool operator()(const IpRange& left, const IpRange& right)
	{
		return left.GetIpMin() < right.GetIpMin();
	}
};


// tbc:
BOOL VipNerBarMgr::Init()
{
	m_setNotify.clear();
	m_mapVipNetBars.Clear();
	m_vecIpRanges.clear();
	m_mapIp2VNBId.Clear();

	// 生成掉落容器
	TObjRef<VarContainer> pVar;
	CreateObj("VipNetBar", "VarContainer");

	// 从配置文件 world.ini 中得到掉落文件路径
	pVar = "VarContainer";

	// 得到配置文件中怪物掉落表的路径
	tstring strVNBTablePath	= pVar->GetString(_T("path"),	_T("vip_netbar"));

	// 加载怪物掉落文件
	pVar = "VipNetBar";
	std::list<tstring> listField;
	if (!pVar->Load(NULL, strVNBTablePath.c_str(), "id", &listField))
	{
		pVar->Clear();
		KillObj("VipNetBar");
		return FALSE;
	}
	;

	// 一个一个的加载怪物掉落文件
	for(std::list<tstring>::iterator it = listField.begin(); it != listField.end(); ++it)
	{
		tagVipNetBar* pVNB = new tagVipNetBar;
		ZeroMemory(pVNB, sizeof(tagVipNetBar));

		pVNB->dwID = _tcstol(it->c_str(), NULL, 10);
		_tcsncpy(pVNB->szName, pVar->GetString(_T("name"), it->c_str(), NULL), X_SHORT_NAME);

		if (P_VALID(m_mapVipNetBars.Peek(pVNB->dwID)))
			continue;

		m_mapVipNetBars.Add(pVNB->dwID, pVNB);
		
		LPCTSTR tszIpMin = NULL;
		LPCTSTR tszIpMax = NULL;
		DWORD dwIpMin = GT_INVALID;
		DWORD dwIpMax = GT_INVALID;

		for(INT i = 0; i < MAX_VNB_IP_NUM; ++i)
		{
			tstringstream ss;
			ss << _T("ipmin") << i+1;
			tszIpMin = pVar->GetString(ss.str().c_str(), it->c_str(), 0);
			if (!P_VALID(tszIpMin))
				break;
			
			ss.str(_T(""));
			ss << _T("ipmax") << i+1;
			tszIpMax = pVar->GetString(ss.str().c_str(), it->c_str(), 0);
			if (!P_VALID(tszIpMax))
				break;

			dwIpMin = TransTSIp2DWORD(tszIpMin);
			GeneralzeIP(dwIpMin);

			dwIpMax = TransTSIp2DWORD(tszIpMax);
			GeneralzeIP(dwIpMax);

			ASSERT(dwIpMin <= dwIpMax);

			m_vecIpRanges.push_back(IpRange(dwIpMin, dwIpMax, pVNB->dwID));
		}
	}

	std::sort(m_vecIpRanges.begin(), m_vecIpRanges.end(), IPComp());
	pVar->Clear();
	KillObj("VipNetBar");

	return TRUE;
}

VOID VipNerBarMgr::UpdateDbPlayerLogin(DWORD dwAccountID, DWORD dwTime)
{
	tagDWORDTime dwLoginDate = dwTime;
	dwLoginDate.min = 0;
	dwLoginDate.sec = 0;
	dwLoginDate.hour = 0;
	tagNDBC_UpdateVNBPlayer send;
	send.dwAccountID = dwAccountID;
	send.dwLoginTime = dwLoginDate;
	g_dbSession.Send(&send, send.dwSize);
}

VOID VipNerBarMgr::InitData(tagDBVNBPlayers* pInitData)
{
	m_setHistoryAccountID.clear();
	for (INT i=0; i<pInitData->nHisPlayers; ++i)
	{
		DWORD dwHisPlayerId = pInitData->dwAccountIDs[i];
		if (m_setHistoryAccountID.find(dwHisPlayerId) == m_setHistoryAccountID.end())
		{
			m_setHistoryAccountID.insert(dwHisPlayerId);
		}
	}

	m_setTodayAccountID.clear();
	for (INT i=0; i<pInitData->nTodaysPlayers; ++i)
	{
		DWORD dwTodayPlayerId = pInitData->dwAccountIDs[i + pInitData->nHisPlayers];
		if (m_setTodayAccountID.find(dwTodayPlayerId) == m_setTodayAccountID.end())
		{
			m_setTodayAccountID.insert(dwTodayPlayerId);
		}
	}
}


DWORD VipNerBarMgr::FitNetBar(DWORD dwIp)
{
	INT nLeft = 0;
	INT nRight = m_vecIpRanges.size() - 1;
	INT	nMiddle = 0;

	while (nLeft <= nRight)
	{
		nMiddle = (nLeft + nRight) / 2;
		if (m_vecIpRanges[nMiddle].Fit(dwIp))
		{
			return m_vecIpRanges[nMiddle].GetVNBId();
		}
		if (m_vecIpRanges[nMiddle].OnRightOf(dwIp))
		{
			nRight = nMiddle - 1;
		}
		else if (m_vecIpRanges[nMiddle].OnLeftOf(dwIp))
		{
			nLeft = nMiddle + 1;
		}
	}

	return GT_INVALID;
}

VOID VipNerBarMgr::PlayerLogin( DWORD dwAccountID, DWORD dwIP )
{
	GeneralzeIP(dwIP);
	// 网吧在线人数
	tagVipNetBar* pVNB = GetVipNetBar(dwIP);
	if (!P_VALID(pVNB))
	{
		return;
	}

	pVNB->OnPlayerLogin();
	UpdateDbPlayerLogin(dwAccountID, GetCurrentDWORDTime());

	if (m_setTodayAccountID.find(dwAccountID) == m_setTodayAccountID.end())
	{
		const tagVNBGiftProto* pGiftProto = g_attRes.GetRandVNBGiftProto();
		if (P_VALID(pGiftProto))
		{
			tagItem* pItem = ItemCreator::Create(EICM_VipNetBar, GT_INVALID, pGiftProto->dwItemTypeID, pGiftProto->nNum);
			if(P_VALID(pItem))
			{
				// 存储到item_baibao表中
				ItemMgr::InsertBaiBao2DBEx(pItem, dwAccountID, ELCLD_VIP_NetBar);
				ItemMgr::ProcBaiBaoRecord(pItem->dwTypeID, 
					GT_INVALID, GT_INVALID, EBBRT_VipNetBarGift);
				// 删除物品
				::Destroy(pItem);

				// 今日礼物
				m_setTodayAccountID.insert(dwAccountID);

				m_setNotify.insert(dwAccountID);
			}
			else
			{
				ASSERT(0);
			}
		}
	}
	if (m_setHistoryAccountID.find(dwAccountID) == m_setHistoryAccountID.end())
	{
		std::list<tagVNBEquipProto*> listEquips;
		g_attRes.GetRandVNBEquipProto(listEquips);
		while (!listEquips.empty())
		{
			const tagVNBEquipProto* pEquipProto = listEquips.front();
			listEquips.pop_front();

			if (P_VALID(pEquipProto))
			{
				tagItem* pEquip = ItemCreator::CreateEx(EICM_VipNetBar, GT_INVALID, pEquipProto->dwEquipTypeID, 1, (EItemQuality)pEquipProto->nQuality);
				if (P_VALID(pEquip))
				{
					ItemMgr::InsertBaiBao2DBEx(pEquip , dwAccountID, ELCLD_VIP_NetBar);
					// 删除物品
					::Destroy(pEquip );

					// 网吧装备
					m_setHistoryAccountID.insert(dwAccountID);			
				}
				else
				{
					ASSERT(0);
				}
			}
		}
	}
}

VOID VipNerBarMgr::PlayerLogout(DWORD dwIP)
{
	GeneralzeIP(dwIP);
	// 网吧在线人数
	tagVipNetBar* pVNB = GetVipNetBar(dwIP);
	if (P_VALID(pVNB))
	{
		pVNB->OnPlayerLogout();
	}
}


DWORD VipNerBarMgr::GetVNBId(DWORD dwIp)
{
	DWORD dwVNBId = m_mapIp2VNBId.Peek(dwIp);
	if (!GT_VALID(dwVNBId))
	{
		dwVNBId = FitNetBar(dwIp);
		if (P_VALID(dwVNBId))
		{
			m_mapIp2VNBId.Add(dwIp, dwVNBId);
		}
	}		
	return dwVNBId;
}

VOID VipNerBarMgr::Destroy()
{
	//tbc:
}

struct RateItem
{
	INT		nMinNum;
	INT		nRate;
};
const INT NUM_VNB_EXP_RATE	= 5;
const INT NUM_VNB_LOOT_RATE	= 5;
RateItem ExpRate[NUM_VNB_EXP_RATE] =
{
	{	1,		500		},
	{	5,		1000	},
	{	10,		1500	},
	{	15,		2000	},
	{	20,		2500	},
};

RateItem LootRate[NUM_VNB_LOOT_RATE] =
{
	{	1,		200		},
	{	5,		400	},
	{	10,		800	},
	{	15,		1200	},
	{	20,		1500	},
};



INT VipNerBarMgr::GetRate( DWORD dwIP, INT nType)
{
	GeneralzeIP(dwIP);
	const tagVipNetBar* pVnb = GetVipNetBar(dwIP);
	if (!P_VALID(pVnb))
	{
		return 0;
	}
	
	RateItem* pArr = (nType == 0 ? ExpRate : (nType == 1 ? LootRate : NULL));
	if(!P_VALID(pArr))
	{
		return 0;
	}

	for (INT i=NUM_VNB_EXP_RATE - 1; i>=0; --i)
	{
		if (pVnb->nPlayerNum > pArr[i].nMinNum )
		{
			return pArr[i].nRate;
		}
	}
	
	return 0;
}

LPCTSTR VipNerBarMgr::GetVNBName( DWORD dwIP )
{
	GeneralzeIP(dwIP);
	const tagVipNetBar* pVnb = GetVipNetBar(dwIP);
	if (!P_VALID(pVnb))
	{
		return 0;
	}

	return pVnb->szName;
}

tagVipNetBar* VipNerBarMgr::GetVipNetBar( DWORD dwIP )
{
	DWORD dwVNBId = GetVNBId(dwIP);
	return m_mapVipNetBars.Peek(dwVNBId);
}

BOOL VipNerBarMgr::DynamicTest(DWORD dwTestNo, DWORD dwArg1, LPCTSTR szArg2)
{
	if (!P_VALID(szArg2))
	{
		return FALSE;
	}

	DWORD dwIp = TransTSIp2DWORD(szArg2);
	switch(dwTestNo)
	{
	case 0:
		PlayerLogin(dwArg1, dwIp);
		break;
	case 1:
		PlayerLogout(dwIp);
		break;
	case 2:
		INT nRate0 = GetRate(dwIp, 0);
		INT nRate1 = GetRate(dwIp, 1);
		LPCTSTR szName = GetVNBName(dwIp);
		break;
	}
	return TRUE;
}

VOID VipNerBarMgr::GeneralzeIP( DWORD &dwIP )
{
	DWORD dwTemp = 0;

	dwTemp |= (dwIP >> 24) & 0x000000ff;
	dwTemp |= (dwIP >> 8) & 0x0000ff00;
	dwTemp |= (dwIP << 8) & 0x00ff0000;
	dwTemp |= (dwIP << 24) & 0xff000000;

	dwIP = dwTemp;
}

DWORD VipNerBarMgr::TransTSIp2DWORD( LPCTSTR tszIP )
{
	TObjRef<StreamTransport> pTrans = "StreamTransport";
	TObjRef<Util> pUtil = "Util";
	
	LPSTR szIp = (LPSTR)pUtil->UnicodeToAnsi(tszIP);
	DWORD dwIp = pTrans->StringIP2IP(szIp);
	
	return dwIp;
}

VOID VipNerBarMgr::PlayerNotify( DWORD dwAccountID )
{
	if (m_setNotify.find(dwAccountID) != m_setNotify.end())
	{
		m_setNotify.erase(dwAccountID);
		
		PlayerSession* pSession = g_worldSession.FindSession(dwAccountID);
		if (P_VALID(pSession))
		{
//			tagNS_VipNetBarGift send;
//			pSession->SendMessage(&send, send.dwSize);
		}
	}
}