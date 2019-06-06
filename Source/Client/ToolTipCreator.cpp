#include "StdAfx.h"
#include "ToolTipCreator.h"
#include "Item.h"
#include "ItemMgr.h"
#include "ItemProtoData.h"
#include "SkillProtoData.h"
#include "SkillMgr.h"
#include "gamescript.h"
#include "Role.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "..\WorldDefine\shop_define.h"
#include "TooltipFrame.h"
#include "PetProtoData.h"
#include "GuildMgr.h"
#include "GuildDefine.h"

ToolTipCreator::ToolTipCreator(void)
{
}

ToolTipCreator::~ToolTipCreator(void)
{
}

ToolTipCreator m_tooltipcreator;
ToolTipCreator* ToolTipCreator::Inst()
{
	return &m_tooltipcreator;
}


tstring ToolTipCreator::GetItemTips(Item* pItem)
{
	tstring scriptFile;
	if(MIsEquipment(pItem->GetItemTypeID()))
	{
		scriptFile = g_strLocalPath + _T("\\tip\\equip_tip.lua");
	}
	else
	{
		scriptFile = g_strLocalPath + _T("\\tip\\item_tip.lua");
	}	
	
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pItem, (void*)pItem->GetProto());
		tstring ret = TObjRef<Util>()->Unicode8ToUnicode(szTmp);
        return ParseDynamicTips(ret.c_str());
	}

	return _T("");
}

tstring ToolTipCreator::GetItemTips(DWORD dwID, BYTE byQuality, DWORD dwNameId)
{
	const tagItemProto* pAtt = ItemProtoData::Inst()->FindItemProto(dwID);
	if(!P_VALID(pAtt))
		return _T("");
	
	tstring scriptFile;
	if(MIsEquipment(dwID))
	{
		scriptFile = g_strLocalPath + _T("\\tip\\goods_equip_tip.lua");
	}
	else
	{
		scriptFile = g_strLocalPath + _T("\\tip\\goods_item_tip.lua");
	}	
	
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pAtt, (DOUBLE)byQuality, (DOUBLE)dwNameId);
		tstring ret = TObjRef<Util>()->Unicode8ToUnicode(szTmp);
        return ParseDynamicTips(ret.c_str());
	}
	
	return _T("");
}

tstring ToolTipCreator::GetTalentTips( DWORD dwID )
{
	const tagSkillProtoClient* pAtt = SkillProtoData::Inst()->FindSkillProto(dwID);
	if(!P_VALID(pAtt))
		return _T("");
	
	const tagSkillData* pData = SkillMgr::Inst()->GetSkillDataByTypeID(dwID);
	

	tstring scriptFile = g_strLocalPath + _T("\\tip\\talent_tip.lua");
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pData, (void*)pAtt);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}

	return _T("");
}

tstring ToolTipCreator::GetGuildSkillTips(DWORD dwID)
{
	const tagSkillProtoClient* pAtt = SkillProtoData::Inst()->FindSkillProto(dwID);
	if(!P_VALID(pAtt))
	{
		//return _T("");
		// 特殊处理
		dwID = dwID - 1;
		const tagSkillProtoClient* pAtt = SkillProtoData::Inst()->FindSkillProto(dwID);
		const tagSkillData* pData = SkillMgr::Inst()->GetSkillDataByTypeID(dwID);
		tstring scriptFile = g_strLocalPath + _T("\\tip\\guild_skill_max_level_tip.lua");
		GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
		if( P_VALID(pScript) )
		{
			LPCSTR szTmp = pScript->GetObjString((void*)pData, (void*)pAtt);
			return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
		}
	}
	const tagSkillData* pData = SkillMgr::Inst()->GetSkillDataByTypeID(dwID);


	tstring scriptFile = g_strLocalPath + _T("\\tip\\guild_skill_tip.lua");
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pData, (void*)pAtt);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}

	return _T("");	
}


tstring ToolTipCreator::GetSkillTips( DWORD dwID )
{
	const tagSkillData* pData = SkillMgr::Inst()->GetSkillDataByTypeID(dwID);
	if(!P_VALID(pData))
		return _T("");

	tstring scriptFile = g_strLocalPath + _T("\\tip\\skill_tip.lua");
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pData, (void*)pData->pProto);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}

	return _T("");
}

tstring ToolTipCreator::GetBuffTips(const tagRoleBuff* pBuff)
{
	if(!P_VALID(pBuff))
		return _T("");

	tstring scriptFile = g_strLocalPath + _T("\\tip\\buff_tip.lua");
	GameScript * pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if(P_VALID(pScript))
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pBuff, (void*)pBuff->pBuffProto);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}
	return _T("");
}

tstring ToolTipCreator::GetPriceTips(const tagItemCost* pCost, BOOL bSingle)
{
	if(!P_VALID(pCost))
		return _T("");

	tstring scriptFile = g_strLocalPath + _T("\\tip\\price_tip.lua");
	GameScript * pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if(P_VALID(pScript))
	{
		LPCSTR szTmp = pScript->GetObjString((void*)pCost, (DOUBLE)(bSingle), NULL);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}
	return _T("");
}

VOID ToolTipCreator::ShowEquipCompare( DWORD dwTypeID )
{
	const tagEquipProto* pProto = ItemProtoData::Inst()->FindEquipProto(dwTypeID);
	if(P_VALID(pProto))
	{
		if(EEP_RightHand == pProto->eEquipPos || EEP_Finger1 == pProto->eEquipPos)
		{
			Equipment* pEquip   = ItemMgr::Inst()->GetCurEquip(pProto->eEquipPos);
			Equipment* pEquipEx = ItemMgr::Inst()->GetCurEquip((EEquipPos)(pProto->eEquipPos+1));
			if(P_VALID(pEquip))
			{
				if(P_VALID(pEquipEx))
				{
					ShowTipCompare(GetItemTips(pEquip).c_str(), GetItemTips(pEquipEx).c_str());
				}
				else
				{
					ShowTipCompare(GetItemTips(pEquip).c_str());	
				}
			}
			else if(P_VALID(pEquipEx))
			{
				ShowTipCompare(GetItemTips(pEquipEx).c_str());
			}
		}
		else
		{
			Equipment* pEquip = ItemMgr::Inst()->GetCurEquip(pProto->eEquipPos);
			if(P_VALID(pEquip))
			{
				ShowTipCompare(GetItemTips(pEquip).c_str());
			}
		}
	}
}

tstring ToolTipCreator::GetPetSkillTips( DWORD dwID )
{
    const tagPetSkillData* pProto = PetProtoData::Inst()->FindPetSkill(dwID);
    if (!P_VALID(pProto))
        return _T("");

    tstring scriptFile = g_strLocalPath + _T("\\tip\\pet_skill_tip.lua");
    GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
    if( P_VALID(pScript) )
    {
        void* obj = NULL;
        LPCSTR szTmp = pScript->GetObjString(obj, (void*)pProto);
        return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
    }

    return _T("");
}

tstring ToolTipCreator::GetCommerceGoodsTips(DWORD dwGoodID)
{
	const tagGoodInfo *pInfo = GuildMgr::Inst()->FindGoodInfo(dwGoodID);
	if (!P_VALID(pInfo))
		return _T("");
	tstring scriptFile = g_strLocalPath + _T("\\tip\\commerce_good_tip.lua");
	GameScript *pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
	if( P_VALID(pScript) )
	{
		void* obj = NULL;
		LPCSTR szTmp = pScript->GetObjString(obj, (void*)pInfo);
		return TObjRef<Util>()->Unicode8ToUnicode(szTmp);
	}
	return _T("");
}

tstring ToolTipCreator::GetHotKeyTips( LPCTSTR szBaseTip, DWORD key1, DWORD key2, LPCTSTR szSpace/* = NULL*/, bool bParenthesis/* = true*/ )
{
	GameInputMap* pKeyMap = TObjRef<GameInputMap>();

	tstring strTips;
	if( NULL != szBaseTip )
		strTips = szBaseTip;

	if( !strTips.empty() && NULL != szSpace )
		strTips += szSpace;

	if( ( key1 || key2 ) && bParenthesis )
		strTips += _T('(');

	if( key1 )
		strTips += pKeyMap->GetKeyName( (DIK)key1 );

	if( key2 )
	{
		if( key1 )
			strTips += _T('+');
		strTips += pKeyMap->GetKeyName( (DIK)key2 );
	}

	if( ( key1 || key2 ) && bParenthesis )
		strTips += _T(')');

	return strTips;
}

tstring ToolTipCreator::ParseDynamicTips( LPCTSTR szInput )
{
    tstring strOut = _T("");

    tstring strInput = szInput;
    UINT nIndex = 0;
    while (nIndex < strInput.size())
    {
        if (strInput[nIndex] == _T('|'))
        {
            int n = strInput.find_first_of(_T('|'), nIndex + 1);
            if( n == tstring::npos )
                break;	// 语法错误

            tstring str = strInput.substr(nIndex+1, n-nIndex-1);
            n =  str.find_first_of(_T('='));
            if( n == tstring::npos || n <= 0 || n>= (INT)str.size() )
                break;	// 语法错误

            tstring strName = str.substr(0, n);
            tstring strValue = str.substr(n+1, str.size() - n);

            tstring strTemp = ParseKey(strName, strValue);
            strOut.append(strTemp);

            nIndex += str.size() + 2;
        }
        else
        {
            strOut.push_back(strInput[nIndex]);
            ++nIndex;
        }
    }

    return strOut;
}

tstring ToolTipCreator::ParseKey( tstring& strName, tstring& strValue )
{
    if (_T("DynTips_Type") == strName)
    {
        if (_T("1") == strValue)
        {
            // 当前绑定的复活点
            LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
            if (P_VALID(pLp))
            {
                tstring strReborn = pLp->GetRebornPoinName();
                return strReborn.empty() ? g_StrTable[_T("LoongLostVillige")] : strReborn;
            }
        }
    }

    return _T("");
}

tstring ToolTipCreator::GetStallPriceTips( const tagItemCost* pCost, int nCount )
{
    if(!P_VALID(pCost))
        return _T("");

    tstring scriptFile = g_strLocalPath + _T("\\tip\\FullPrice_tip.lua");
    GameScript * pScript = TObjRef<GameScriptMgr>()->CreateScript("VFS_System", scriptFile.c_str());
    if(P_VALID(pScript))
    {
        tagItemCost tempCost(*pCost);
        tempCost.n64CostSilver  = pCost->n64CostSilver * nCount;
        LPCSTR szTmp = pScript->GetObjString((void*)(&tempCost), NULL, NULL);
        tstring ret;
        ret += GetPriceTips(pCost, TRUE);
        ret += _T("\\n");
        ret += TObjRef<Util>()->Unicode8ToUnicode(szTmp);
        return ret;
    }
    return _T("");
}