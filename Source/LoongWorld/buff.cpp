#include "stdafx.h"

#include "unit.h"
#include "role.h"
#include "title_mgr.h"
#include "buff.h"
#include "buff_effect.h"

//----------------------------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------------------------
Buff::Buff()
: m_pOwner(NULL), m_nIndex(GT_INVALID), m_dwSrcUnitID(GT_INVALID), m_dwSrcSkillID(GT_INVALID), m_n64ItemID(GT_INVALID),
  m_dwItemTypeID(GT_INVALID), m_dwID(GT_INVALID), m_nLevel(0), m_nPersistTick(0), m_nCurTick(0),
  m_nWrapTimes(0), m_pProto(NULL), m_pMod(NULL), m_eState(EBS_Idle)
{

}

//----------------------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------------------
Buff::~Buff()
{
	SAFE_DEL(m_pMod);
}

//----------------------------------------------------------------------------------------------------
// 初始化，用于在游戏运行时添加Buff
//----------------------------------------------------------------------------------------------------
VOID Buff::Init(const tagBuffProto* pBuffProto, Unit* pTarget, Unit* pSrc, DWORD dwSrcSkillID, const tagItem* pItem, INT nIndex, TList<DWORD>* listModifier)
{
	ASSERT( P_VALID(pBuffProto) );
	ASSERT( P_VALID(pTarget) );

	BeginInit();				// 设置目前正在初始化

	// 设置静态属性
	m_pProto = pBuffProto;

	// 查找触发器
	m_pTrigger = g_attRes.GetTriggerProto(pBuffProto->dwOPTrigger);

	// 设置其它属性
	m_pOwner = pTarget;
	m_nIndex = nIndex;

	m_dwSrcUnitID = (P_VALID(pSrc) ? pSrc->GetID() : GT_INVALID);
	m_dwSrcSkillID = dwSrcSkillID;

	if( P_VALID(pItem) )
	{
		m_n64ItemID = pItem->n64Serial;
		m_dwItemTypeID = pItem->dwTypeID;
	}
	else
	{
		m_n64ItemID = GT_INVALID;
		m_dwItemTypeID = GT_INVALID;
	}

	m_dwID = GetIDFromTypeID(pBuffProto->dwID);
	m_nLevel = GetLevelFromTypeID(pBuffProto->dwID);

	m_nPersistTick = 0;
	m_nCurTick = 0;
	m_nWrapTimes = 1;

	// 设置Buff影响
	if( P_VALID(listModifier) && FALSE == listModifier->Empty() )
	{
		if( P_VALID(m_pMod) )	m_pMod->Clear();
		else					m_pMod = new tagBuffMod;

		// Buff施放者身上所有能影响该buff的技能
		TList<DWORD>::TListIterator it = listModifier->Begin();
		DWORD dwSkillTypeID = GT_INVALID;

		while( listModifier->PeekNext(it, dwSkillTypeID) )
		{
			const tagSkillProto* pSkillProto = g_attRes.GetSkillProto(dwSkillTypeID);
			if( !P_VALID(pSkillProto) ) continue;

			m_pMod->SetMod(pSkillProto);
		}
	}

	// 计算Buff的初始瞬时效果
	if( !P_VALID(m_pTrigger) )
	{
		m_pOwner->CalBuffInstantEffect(pSrc, EBEM_Instant, m_pProto, m_pMod);
	}

	// 计算Buf的持续性效果
	m_pOwner->CalBuffPersistEffect(pSrc, m_pProto, m_pMod);

	EndInit();				// 初始化结束
}

//----------------------------------------------------------------------------------------------------
// 初始化，用于在人物加载时添加buff
//----------------------------------------------------------------------------------------------------
VOID Buff::Init(Unit* pTarget, const tagBuffSave* pBuffSave, INT nIndex)
{
	if( !P_VALID(pTarget) || !P_VALID(pBuffSave) )
		return;

	BeginInit();

	m_pProto = g_attRes.GetBuffProto(Buff::GetTypeIDFromIDAndLevel(pBuffSave->dwBuffID, pBuffSave->n8Level));
	if( !P_VALID(m_pProto) )
	{
		IMSG(_T("Can not find the buff proto when loading the buff: id=%u, roleid=%u\r\n"), pBuffSave->dwBuffID, pTarget->GetID());
		m_dwID = GT_INVALID;
		EndInit();
		return;
	}

	m_pTrigger = g_attRes.GetTriggerProto(m_pProto->dwOPTrigger);

	m_dwID = pBuffSave->dwBuffID;
	m_nLevel = pBuffSave->n8Level;

	m_pOwner = pTarget;
	m_nIndex = nIndex;

	m_dwSrcUnitID = pBuffSave->dwSrcUnitID;
	m_dwSrcSkillID = pBuffSave->dwSrcSkillID;
	m_n64ItemID = pBuffSave->n64Serial;
	m_dwItemTypeID = pBuffSave->dwItemTypeID;

	m_nPersistTick = pBuffSave->nPersistTick;
	m_nCurTick = 0;

	m_nWrapTimes = pBuffSave->n8CurLapTimes;

	// 设置Buff影响
	if( pBuffSave->n8ModifierNum > 0 )
	{
		if( P_VALID(m_pMod) )	m_pMod->Clear();
		else					m_pMod = new tagBuffMod;

		DWORD* pdwSkillTypeID = (DWORD*)pBuffSave->byData;

		for(INT n = 0; n < pBuffSave->n8ModifierNum; ++n)
		{
			const tagSkillProto* pSkillProto = g_attRes.GetSkillProto(pdwSkillTypeID[n]);
			if( !P_VALID(pSkillProto) ) continue;

			m_pMod->SetMod(pSkillProto);
		}
	}

	// 计算Buff的持续性效果（这里该怎么得到Buff施放者的指针？？）
	m_pOwner->CalBuffPersistEffect(NULL, m_pProto, m_pMod, m_nWrapTimes);

	EndInit();
}

//----------------------------------------------------------------------------------------------------
// 初始化tagBuffSave结构
//----------------------------------------------------------------------------------------------------
VOID Buff::InitBuffSave(OUT tagBuffSave *pBuffSave, OUT INT32 &nSize)
{
	pBuffSave->dwBuffID				= m_dwID;
	pBuffSave->nPersistTick			= m_nPersistTick;;
	pBuffSave->n8Level				= m_nLevel;

	pBuffSave->n8CurLapTimes		= m_nWrapTimes;

	pBuffSave->dwSrcUnitID			= m_dwSrcUnitID;
	pBuffSave->dwSrcSkillID			= m_dwSrcSkillID;
	pBuffSave->n64Serial			= m_n64ItemID;
	pBuffSave->dwItemTypeID			= m_dwItemTypeID;

	// Buff的技能影响
	MTRANS_POINTER(pModifier, pBuffSave->byData, DWORD);
	INT n = 0;

	if( P_VALID(m_pMod) && FALSE == m_pMod->listModifier.Empty() )
	{	
		TList<DWORD>::TListIterator it = m_pMod->listModifier.Begin();
		DWORD dwSkillTypeID = GT_INVALID;
		while( m_pMod->listModifier.PeekNext(it, dwSkillTypeID) )
		{
			pModifier[n] = dwSkillTypeID;
			++n;
		}
	}
	pBuffSave->n8ModifierNum = n;

	nSize = (INT32)((size_t)&pModifier[n] - (size_t)pBuffSave);
}

//----------------------------------------------------------------------------------------------------
// 更新，如果返回True，则表示该Buff要删除了
//----------------------------------------------------------------------------------------------------
BOOL Buff::Update()
{
	if( !IsValid() ) return FALSE;

	BeginUpdate();

	// 如果是间隔作用的buff，则检测时间是否到了
	if( IsInterOP() )
	{
		if( ++m_nCurTick >= m_pProto->nInterOPTick )
		{
			// 计算间隔作用效果
			Unit* pSrc = NULL;
			if( P_VALID(m_pOwner->GetMap()) )
			{
				pSrc = m_pOwner->GetMap()->FindUnit(m_dwSrcUnitID);
			}
			m_pOwner->CalBuffInstantEffect(pSrc, EBEM_Inter, m_pProto, m_pMod, m_nWrapTimes);
			m_nCurTick = 0;
		}
	}

	// 检测buff的持续时间
	if( !IsPermanent() )
	{
		if( ++m_nPersistTick >= GetMaxPersistTick() )
		{
			// 持续时间到了
			EndUpdate();
			return TRUE;
		}
        //由于服务器端计算延迟 ，故 每407Tick 补 5 Tick
        

		if( (m_nPersistTick + 407)%407 == 0 )
		{
			m_nPersistTick+= 5;
			if( GetMaxPersistTick() >= 6000.0f)
			{
				m_nPersistTick++;
			}
		}

		

	}

	EndUpdate();

	return FALSE;
}

//----------------------------------------------------------------------------------------------------
// 销毁
//----------------------------------------------------------------------------------------------------
VOID Buff::Destroy(BOOL bSelf)
{
	BeginDestroy();

	Unit* pSrc = NULL;
	if( P_VALID(m_pOwner->GetMap()) )
	{
		pSrc = m_pOwner->GetMap()->FindUnit(m_dwSrcUnitID);
	}
	// 计算Buff的结束时效果，只有是时间到了自身结束时才计算buff结束时效果
	if( bSelf )
	{
		m_pOwner->CalBuffInstantEffect(pSrc, EBEM_Finish, m_pProto, m_pMod, m_nWrapTimes);
	}

	// 取消Buff的持续性效果
	m_pOwner->CalBuffPersistEffect(pSrc, m_pProto, m_pMod, m_nWrapTimes, FALSE);

	// 清空Buff
	m_dwID = GT_INVALID;
	m_pOwner = NULL;
	m_pProto = NULL;

	// 如果Mod有值，这里不释放掉，而是将内容清空，以备下次再使用该Buff时不用再分配内存
	if( P_VALID(m_pMod) )
	{
		m_pMod->Clear();
	}

	EndDestroy();
}

//----------------------------------------------------------------------------------------------------
// 叠加
//----------------------------------------------------------------------------------------------------
VOID Buff::Wrap()
{
	BeginUpdate();

	Unit* pSrc = NULL;
	if( P_VALID(m_pOwner->GetMap()) )
	{
		pSrc = m_pOwner->GetMap()->FindUnit(m_dwSrcUnitID);
	}

	++m_nWrapTimes;
	m_nPersistTick = 0;

	// 立即计算一次Buff的一次瞬间效果
	m_pOwner->CalBuffInstantEffect(pSrc, EBEM_Instant, m_pProto, m_pMod);

	// 查看是否需要再累加一次持续性效果
	if( m_nWrapTimes > GetMaxWrapTimes() )
	{
		m_nWrapTimes = GetMaxWrapTimes();
	}
	else
	{
		// 还没有超过最大叠加次数，则再计算一次Buff持续效果
		m_pOwner->WrapBuffPersistEffect(pSrc, m_pProto, m_pMod);
	}

	EndUpdate();
}

//------------------------------------------------------------------------------------------------------
// 打断
//------------------------------------------------------------------------------------------------------
BOOL Buff::Interrupt(EBuffInterruptFlag eFlag, INT nMisc)
{
	if( !IsValid() ) return FALSE;

	// 如果buff不接受flag中的打断方式，返回失败
	if( !(eFlag & m_pProto->dwInterruptFlag) ) return FALSE;

	// 对于某些特殊的判断，要单独判断
	switch(eFlag)
	{
		// 体力低于
	case EBIF_HPLower:
		{
			INT nHPLimit = m_pProto->nHPInterruptLimit;
			if( nHPLimit < 100000 )
			{
				return m_pOwner->GetAttValue(ERA_HP) < nHPLimit;
			}
			else if( m_pOwner->GetAttValue(ERA_MaxHP) > 0 )
			{
				return m_pOwner->GetAttValue(ERA_HP) * 10000 / m_pOwner->GetAttValue(ERA_MaxHP) < (nHPLimit - 100000);
			}
			else
			{
				return FALSE;
			}
		}
		break;

		// 真气低于
	case EBIF_MPLower:
		{
			INT nMPLimit = m_pProto->nMPInterruptLimit;
			if( nMPLimit < 100000 )
			{
				return m_pOwner->GetAttValue(ERA_MP) < nMPLimit;
			}
			else if( m_pOwner->GetAttValue(ERA_MaxMP) > 0 )
			{
				return m_pOwner->GetAttValue(ERA_MP) * 10000 / m_pOwner->GetAttValue(ERA_MaxMP) < (nMPLimit - 100000);
			}
			else
			{
				return FALSE;
			}
		}
		break;

		// 怒气低于
	case EBIF_RageLower:
		{
			INT nRageLimit = m_pProto->nRageInterruptLimit;
			return m_pOwner->GetAttValue(ERA_Rage) < nRageLimit;
		}
		break;

		// 持久力低于
	case EBIF_EnduranceLower:
		{
			INT nEnduranceLimit = m_pProto->nEnduranceInterruptLimit;
			if( nEnduranceLimit < 100000 )
			{
				return m_pOwner->GetAttValue(ERA_Endurance) < nEnduranceLimit;
			}
			else if( m_pOwner->GetAttValue(ERA_MaxEndurance) > 0 )
			{
				return m_pOwner->GetAttValue(ERA_Endurance) * 10000 / m_pOwner->GetAttValue(ERA_MaxEndurance) < (nEnduranceLimit - 100000);
			}
			else
			{
				return FALSE;
			}
		}
		break;

		// 活力低于
	case EBIF_VitalityLower:
		{
			INT nVitalityLimit = m_pProto->nVitalityInterruptLimit;

			if( nVitalityLimit < 100000 )
			{
				return m_pOwner->GetAttValue(ERA_Vitality) < nVitalityLimit;
			}
			else if( m_pOwner->GetAttValue(ERA_MaxVitality) > 0 )
			{
				return m_pOwner->GetAttValue(ERA_Vitality) * 10000 / m_pOwner->GetAttValue(ERA_MaxVitality) < (nVitalityLimit - 100000);
			}
			else
			{
				return FALSE;
			}
		}
		break;

		// 被攻击
	case EBIF_BeAttacked:
		{
			INT nProp = m_pProto->nAttackInterruptRate + ( P_VALID(m_pMod) ? m_pMod->nAttackInterruptRateMod : 0 );

			if( IUTIL->Rand() % 10000 > nProp ) return FALSE;
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------------
// 触发
//-----------------------------------------------------------------------------------------------------------------
BOOL Buff::OnTrigger(Unit* pTarget, ETriggerEventType eEvent, DWORD dwEventMisc1/* =GT_INVALID */, DWORD dwEventMisc2/* =GT_INVALID */)
{
	if( !IsValid() ) return FALSE;

	if( m_eState != EBS_Idle ) return FALSE;

	// 检测触发器满足条件
	if( !P_VALID(m_pTrigger) || m_pTrigger->eEventType != eEvent ) return FALSE;

	// 测试Trigger是否满足
	if( !m_pOwner->TestTrigger(pTarget, m_pTrigger, dwEventMisc1, dwEventMisc2) ) return FALSE;

	// 称号事件触发
	Role *pRole = dynamic_cast<Role *>(pTarget);
	if (NULL != pRole)
	{
		pRole->GetTitleMgr()->SigEvent(ETE_TRIGGER, m_pTrigger->dwID, GT_INVALID);
	}

	// 设置正在更新
	BeginUpdate();

	// 计算Buff瞬时效果
	Unit* pSrc = NULL;
	if( P_VALID(m_pOwner->GetMap()) )
	{
		pSrc = m_pOwner->GetMap()->FindUnit(m_dwSrcUnitID);
	}
	m_pOwner->CalBuffInstantEffect(pSrc, EBEM_Instant, m_pProto, m_pMod, m_nWrapTimes, pTarget);

	// 设置停止更新
	EndUpdate();

	return TRUE;
}

