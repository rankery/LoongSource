#include "stdafx.h"
#include "ItemProtoData.h"


ItemProtoData::ItemProtoData()
{
}

ItemProtoData::~ItemProtoData()
{
	map<DWORD, tagItemProto>::iterator iterItem;
	for(iterItem = m_mapItemProto.begin(); iterItem != m_mapItemProto.end(); ++iterItem)
	{
		tagItemProto item = iterItem->second;
		if(P_VALID(item.pDisplayInfo))
		{
			SAFE_DEL(item.pDisplayInfo);
		}
	}
	m_mapItemProto.clear();
	
	map<DWORD, tagEquipProto>::iterator iterEquip;
	for(iterEquip = m_mapEquipProto.begin(); iterEquip != m_mapEquipProto.end(); ++iterEquip)
	{
		tagEquipProto equip = iterEquip->second;
		if(P_VALID(equip.pDisplayInfo))
		{
			SAFE_DEL(equip.pDisplayInfo);
		}
	}
	m_mapEquipProto.clear();
}

ItemProtoData g_itemproto;
ItemProtoData* ItemProtoData::Inst()
{
	return &g_itemproto;
}

void ItemProtoData::LoadFromFile()
{
	
	LPCTSTR szItemProto = _T("data\\system\\attdata\\item_proto.xml");
	tstring strPath1 = g_strLocalPath + _T("\\attdata\\item_name.xml");
	LPCTSTR szItemDisplay = strPath1.c_str(); 
	LPCTSTR szEquipProto = _T("data\\system\\attdata\\equip_proto.xml");
	tstring strPath2 = g_strLocalPath + _T("\\attdata\\equip_name.xml");
	LPCTSTR szEquipDisplay = strPath2.c_str();
	LPCTSTR szSuitProto = _T("data\\system\\attdata\\equip_suit.xml");
	tstring strPath3 = g_strLocalPath + _T("\\attdata\\equip_suit_name.xml");
	LPCTSTR szSuitDisplay = strPath3.c_str();
	LPCTSTR szTreasureProto = _T("data\\system\\attdata\\clantreasure_proto.xml");

    LPCTSTR szPetEquipProto = _T("data\\system\\attdata\\pet_equip_proto.xml");

	LPCTSTR szEquipQlty = _T("data\\system\\attdata\\equip_qlty_effect.xml");

	list<tstring> ItemProtoFieldList;
	list<tstring> ItemDisplayFieldList;
	list<tstring> EquipProtoFieldList;
	list<tstring> EquipDisplayFieldList;
	list<tstring> SuitProtoFieldList;
	list<tstring> SuitDisplayFieldList;
	list<tstring> TreasureProtoFieldList;
    list<tstring> PetEquipProtoList;
	list<tstring> EquipQltyList;
	list<tstring>::iterator iter;

	
	//---------------------------------------------------------------------------------
	//读取item_proto
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarItemProto = CreateObj("ItemProto", "VarContainer");
	if(!VarItemProto->Load("VFS_System", szItemProto, "id", &ItemProtoFieldList))
		IMSG(_T("Load file item_proto.xml failed\r\n"));

	for(iter = ItemProtoFieldList.begin(); iter != ItemProtoFieldList.end(); ++iter)
	{
		tagItemProto item;
		item.dwTypeID			 =						VarItemProto->GetDword( _T("id"),			(*iter).c_str(), GT_INVALID );
		item.eType				 =	(EItemType)			VarItemProto->GetDword( _T("Type"),			(*iter).c_str(), EIT_Null );
		item.eTypeEx			 =	(EItemTypeEx)		VarItemProto->GetDword( _T("TypeEx"),		(*iter).c_str(), EITE_Null );
		item.eTypeReserved		 =	(EItemTypeReserved)	VarItemProto->GetDword( _T("TypeReserved"),	(*iter).c_str(), EITR_Null );
		item.eStuffType			 =	(EStuffType)		VarItemProto->GetDword( _T("StuffType"),	(*iter).c_str(), EST_Null );
		item.eSpecFunc			 =	(EItemSpecFunc)		VarItemProto->GetDword( _T("SpecFunc"),		(*iter).c_str(), EISF_Null );
		item.nSpecFuncVal1		 =	(INT)				VarItemProto->GetDword( _T("SpecFuncVal1"),	(*iter).c_str(), GT_INVALID );
		item.nSpecFuncVal2		 =	(INT)				VarItemProto->GetDword( _T("SpecFuncVal2"),	(*iter).c_str(), GT_INVALID );
		item.byLevel			 =	(BYTE)				VarItemProto->GetDword( _T("Level"),		(*iter).c_str(), 0 );
		item.byQuality			 =	(BYTE)				VarItemProto->GetDword( _T("Quality"),		(*iter).c_str(), 0 );
		item.byBindType			 =	(BYTE)				VarItemProto->GetDword( _T("BindType"),		(*iter).c_str(), 0 );
		item.dwQuestID			 =						VarItemProto->GetDword( _T("QuestID"),		(*iter).c_str(), GT_INVALID );
		item.nBasePrice			 =	(INT)				VarItemProto->GetDword( _T("BasePrice"),	(*iter).c_str(), 0 );
		item.dwTimeLimit		 =						VarItemProto->GetDword( _T("TimeLimit"),	(*iter).c_str(), GT_INVALID );
		item.nMaxUseTimes		 =	(INT)				VarItemProto->GetDword( _T("MaxUseTimes"),	(*iter).c_str(), GT_INVALID );
		item.n16MaxLapNum		 =	(INT16)				VarItemProto->GetDword( _T("MaxLapNum"),	(*iter).c_str(), GT_INVALID	);
		item.n16MaxHoldNum		 =	(INT16)				VarItemProto->GetDword( _T("MaxHoldNum"),	(*iter).c_str(), GT_INVALID );
		item.n16Enmity			 =	(INT16)				VarItemProto->GetDword( _T("Enmity"),		(*iter).c_str(), GT_INVALID );
		item.bNeedBroadcast		 =	(BOOL)				VarItemProto->GetDword( _T("NeedBroadcast"),(*iter).c_str(), false ) ? true : false;
		item.bCanSell			 =	(BOOL)				VarItemProto->GetDword( _T("CanSell"),		(*iter).c_str(), false ) ? true : false;
		item.eClanRepute		 =	(EReputationType)	VarItemProto->GetDword( _T("ZoneRepType"),	(*iter).c_str(), ERT_NULL );
		item.nClanReputeVal		 =	(INT32)				VarItemProto->GetDword( _T("ZoneRepVal"),	(*iter).c_str(), 0 );
		item.eOtherClanRepute	 =	(EReputationType)	VarItemProto->GetDword( _T("OtherRepType"),	(*iter).c_str(), ERT_NULL );
		item.nOtherClanReputeVal =	(INT32)				VarItemProto->GetDword( _T("OtherRepVal"),	(*iter).c_str(), 0 );
		item.dwBuffID0			 =						VarItemProto->GetDword( _T("BuffID0"),		(*iter).c_str(), GT_INVALID );
		item.dwTriggerID0		 =						VarItemProto->GetDword( _T("TriggerID0"),	(*iter).c_str(), GT_INVALID );
		item.dwBuffID1			 =						VarItemProto->GetDword( _T("BuffID1"),		(*iter).c_str(), GT_INVALID );
		item.dwTriggerID1		 =						VarItemProto->GetDword( _T("TriggerID1"),	(*iter).c_str(), GT_INVALID );
		item.dwCooldownTime		 =						VarItemProto->GetDword( _T("CooldownTime"),	(*iter).c_str(), GT_INVALID );
		item.eOPType			 =	(ESkillOPType)		VarItemProto->GetDword( _T("OpType"),		(*iter).c_str(), ESOPT_NUll );
		item.fOPDist			 =						VarItemProto->GetFloat( _T("OpDist"),		(*iter).c_str(), 0.0f );
		item.fOPRadius			 =						VarItemProto->GetFloat( _T("OpRaduis"),		(*iter).c_str(), 0.0f );
		item.bInterruptMove		 =	(BOOL)				VarItemProto->GetDword( _T("InterruptMove"),(*iter).c_str(), false ) ? true : false;
		item.nPrepareTime		 =	(INT)				VarItemProto->GetDword( _T("PrepareTime"),	(*iter).c_str(), 0 );
		item.eSexLimit			 =	(ESexLimit)			VarItemProto->GetDword( _T("SexLimit"),		(*iter).c_str(), ESL_Null );
		item.byMinUseLevel		 =	(BYTE)				VarItemProto->GetDword( _T("MinUseLevel"),	(*iter).c_str(), 1 );
		item.byMaxUseLevel		 =	(BYTE)				VarItemProto->GetDword( _T("MaxUseLevel"),	(*iter).c_str(), GT_INVALID);
		item.dwTargetLimit		 =						VarItemProto->GetDword( _T("TargetLimit"),	(*iter).c_str(), GT_INVALID);
		item.dwSpecBuffLimitID	 =						VarItemProto->GetDword( _T("BuffLimitID"),	(*iter).c_str(), GT_INVALID);
		//item.bFriendly		 =	(BOOL)				VarItemProto->GetDword( _T("Friendly"),		(*iter).c_str(), false ) ? true : false;
		//item.bHostile			 =	(BOOL)				VarItemProto->GetDword( _T("Hostile"),		(*iter).c_str(), false ) ? true : false;
		//item.bIndependent		 =	(BOOL)				VarItemProto->GetDword( _T("Independent"),	(*iter).c_str(), false ) ? true : false;
		item.dwStateLimit		 =						VarItemProto->GetDword(  _T("StateLimit"),	(*iter).c_str(), 0 );
		item.bMoveable			 =	(BOOL)				VarItemProto->GetDword( _T("moveable"),		(*iter).c_str(), FALSE );
		//more

		m_mapItemProto.insert(make_pair(item.dwTypeID, item));
	}

	KillObj("ItemProto");


	
	//---------------------------------------------------------------------------------
	//读取item_name
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarItemDisplay = CreateObj("ItemDisplay", "VarContainer");
	if(!VarItemDisplay->Load("VFS_System", szItemDisplay, "id", &ItemDisplayFieldList))
		IMSG(_T("Load file item_name.xml failed\r\n"));

	for(iter = ItemDisplayFieldList.begin(); iter != ItemDisplayFieldList.end(); ++iter)
	{
		DWORD dwID = VarItemDisplay->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagItemDisplayInfo* pDisplay = new tagItemDisplayInfo;
		_tcsncpy( pDisplay->szName,			VarItemDisplay->GetString(_T("name"),			(*iter).c_str(), _T("")), sizeof(pDisplay->szName)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szType,			VarItemDisplay->GetString(_T("display_type"),	(*iter).c_str(), _T("")), sizeof(pDisplay->szType)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDesc,			VarItemDisplay->GetString(_T("desc"),			(*iter).c_str(), _T("")), sizeof(pDisplay->szDesc)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szMaleIcon,		VarItemDisplay->GetString(_T("male_icon"),		(*iter).c_str(), _T("")), sizeof(pDisplay->szMaleIcon)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szActionMod,	VarItemDisplay->GetString(_T("action_model"),	(*iter).c_str(), _T("")), sizeof(pDisplay->szActionMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szSourceEffect, VarItemDisplay->GetString(_T("source_effect"),	(*iter).c_str(), _T("")), sizeof(pDisplay->szSourceEffect)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDestEffect,	VarItemDisplay->GetString(_T("dest_effect"),	(*iter).c_str(), _T("")), sizeof(pDisplay->szDestEffect)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szPrepareMod,	VarItemDisplay->GetString(_T("prepare_action"),	(*iter).c_str(), _T("")), sizeof(pDisplay->szPrepareMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDropMod,		VarItemDisplay->GetString(_T("drop_model"),		(*iter).c_str(), _T("")), sizeof(pDisplay->szDropMod)/sizeof(TCHAR));
		
		pDisplay->bActionBlend		= (BOOL)VarItemDisplay->GetDword( _T("action_blend"),		(*iter).c_str(),	FALSE );

		//more

		tagItemProto* pItem = FindInMap(m_mapItemProto, dwID);
		if(P_VALID(pItem))
		{
			pItem->pDisplayInfo = pDisplay;
		}
		else
		{
			SAFE_DEL(pDisplay);
		}
	}
	
	KillObj("ItemDisplay");



	//---------------------------------------------------------------------------------
	//读取equip_proto
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarEquipProto = CreateObj("EquipProto", "VarContainer");
	if(!VarEquipProto->Load("VFS_System", szEquipProto, "id", &EquipProtoFieldList))
		IMSG(_T("Load file equip_proto.xml failed\r\n"));

	for(iter = EquipProtoFieldList.begin(); iter != EquipProtoFieldList.end(); ++iter)
	{
		tagEquipProto equip;
		equip.dwTypeID			  =						VarEquipProto->GetDword( _T("id"),			(*iter).c_str(), GT_INVALID );
		equip.eType				  =	(EItemType)			VarEquipProto->GetDword( _T("Type"),		(*iter).c_str(), EIT_Null );
		equip.eTypeEx			  =	(EItemTypeEx)		VarEquipProto->GetDword( _T("TypeEx"),		(*iter).c_str(), EITE_Null );
		equip.eTypeReserved		  =	(EItemTypeReserved)	VarEquipProto->GetDword( _T("TypeReserved"),(*iter).c_str(), EITR_Null );
		equip.byLevel			  =	(BYTE)				VarEquipProto->GetDword( _T("Level"),		(*iter).c_str(), 0 );
		equip.byQuality			  =	(BYTE)				VarEquipProto->GetDword( _T("Quality"),		(*iter).c_str(), 0 );
		equip.byBindType		  =	(BYTE)				VarEquipProto->GetDword( _T("BindType"),	(*iter).c_str(), 0 );
		equip.nBasePrice		  =	(INT)				VarEquipProto->GetDword( _T("BasePrice"),	(*iter).c_str(), GT_INVALID );
		equip.dwTimeLimit		  =						VarEquipProto->GetDword( _T("TimeLimit"),	(*iter).c_str(), GT_INVALID );
		equip.nMaxUseTimes		  =	(INT)				VarEquipProto->GetDword( _T("MaxUseTimes"),	(*iter).c_str(), GT_INVALID );
		equip.n16MaxLapNum		  =	(INT16)				VarEquipProto->GetDword( _T("MaxLapNum"),	(*iter).c_str(), GT_INVALID	);
		equip.n16MaxHoldNum		  =	(INT16)				VarEquipProto->GetDword( _T("MaxHoldNum"),	(*iter).c_str(), GT_INVALID );
		equip.eSexLimit			  =	(ESexLimit)			VarEquipProto->GetDword( _T("SexLimit"),	(*iter).c_str(), ESL_Null );
		equip.byMinUseLevel		  =	(BYTE)				VarEquipProto->GetDword( _T("MinUseLevel"),	(*iter).c_str(), 1 );
		equip.byMaxUseLevel		  =	(BYTE)				VarEquipProto->GetDword( _T("MaxUseLevel"),	(*iter).c_str(), GT_INVALID);

		equip.eEquipPos			  =	(EEquipPos)			VarEquipProto->GetDword( _T("EquipPos"),	(*iter).c_str(), EEP_Start );
		equip.eWeaponPos		  =	(EWeaponPos)		VarEquipProto->GetDword( _T("WeaponPos"),	(*iter).c_str(), EWP_NULL );
		equip.n16WuHun			  =	(INT16)				VarEquipProto->GetDword( _T("WuHun"),		(*iter).c_str(), 0 );
		equip.n16MinDmg			  =	(INT16)				VarEquipProto->GetDword( _T("MinDmg"),		(*iter).c_str(), 0 );
		equip.n16MaxDmg			  =	(INT16)				VarEquipProto->GetDword( _T("MaxDmg"),		(*iter).c_str(), 0 );
		equip.n16Armor			  =	(INT16)				VarEquipProto->GetDword( _T("Armor"),		(*iter).c_str(), 0 );
		equip.nMaxPotVal		  =	(INT)				VarEquipProto->GetDword( _T("MaxPotval"),	(*iter).c_str(), 0 );
		equip.nPotVal			  =	(INT)				VarEquipProto->GetDword( _T("Potval"),		(*iter).c_str(), 0 );
		equip.nPotIncTimes		  =	(INT)				VarEquipProto->GetDword( _T("PotIncTimes"),		(*iter).c_str(), 0 );

		equip.n16Newness		  =	(INT16)				VarEquipProto->GetDword( _T("MaxNewness"),	(*iter).c_str(), 0 );
		equip.bNeedBroadcast	  =	(BOOL)				VarEquipProto->GetDword( _T("NeedBroadcast"),(*iter).c_str(), false ) ? true : false;
		equip.bCanSell			  =	(BOOL)				VarEquipProto->GetDword( _T("CanSell"),		(*iter).c_str(), false ) ? true : false;
		equip.eClanRepute		  =	(EReputationType)	VarEquipProto->GetDword( _T("ZoneRepType"),	(*iter).c_str(), ERT_NULL );
		equip.nClanReputeVal	  =	(INT32)				VarEquipProto->GetDword( _T("ZoneRepVal"),	(*iter).c_str(), 0 );
		equip.eOtherClanRepute	  =	(EReputationType)	VarEquipProto->GetDword( _T("OtherRepType"),(*iter).c_str(), ERT_NULL );
		equip.nOtherClanReputeVal =	(INT32)				VarEquipProto->GetDword( _T("OtherRepVal"),	(*iter).c_str(), 0 );
		equip.dwBuffID0			  =						VarEquipProto->GetDword( _T("BuffID0"),		(*iter).c_str(), GT_INVALID );
		equip.dwTriggerID0		  =						VarEquipProto->GetDword( _T("TriggerID0"),	(*iter).c_str(), GT_INVALID );
		equip.dwBuffID1			  =						VarEquipProto->GetDword( _T("BuffID1"),		(*iter).c_str(), GT_INVALID );
		equip.dwTriggerID1		  =						VarEquipProto->GetDword( _T("TriggerID1"),	(*iter).c_str(), GT_INVALID );
		
		TCHAR szTmp[X_SHORT_NAME]={0};
		for(int i=0; i<MAX_ROLEATT_BASE_EFFECT; ++i)
		{
			_stprintf(szTmp, _T("RoleAttType%d"), i);
			equip.BaseEffect[i].eRoleAtt =	(ERoleAttribute) VarEquipProto->GetDword( szTmp,		(*iter).c_str(), ERA_Null );
			_stprintf(szTmp, _T("RoleAttVal%d"), i);
			equip.BaseEffect[i].nValue	 =	(INT)			 VarEquipProto->GetDword( szTmp,		(*iter).c_str(), 0 );
		}

		for(int i=0; i<MAX_PEREQUIP_SUIT_NUM; ++i)
		{
			_stprintf(szTmp, _T("SuitID%d"),		i+1);
			equip.dwSuitID[i]		 =					VarEquipProto->GetDword( szTmp, (*iter).c_str(), GT_INVALID );
			_stprintf(szTmp, _T("SuitMinQlty%d"),	i+1);
			equip.bySuitMinQlty[i]	 = (BYTE)			VarEquipProto->GetInt(	 szTmp, (*iter).c_str(), 0 );
		}

		equip.byLongfuAtt[EWX_Metal] = (BYTE)			VarEquipProto->GetInt(	 _T("Metal"),		(*iter).c_str(), 0 );
		equip.byLongfuAtt[EWX_Wood]  = (BYTE)			VarEquipProto->GetInt(	 _T("Wood"),		(*iter).c_str(), 0 );
		equip.byLongfuAtt[EWX_Water] = (BYTE)			VarEquipProto->GetInt(	 _T("Water"),		(*iter).c_str(), 0 );
		equip.byLongfuAtt[EWX_Fire]  = (BYTE)			VarEquipProto->GetInt(	 _T("Fire"),		(*iter).c_str(), 0 );
		equip.byLongfuAtt[EWX_Soil]  = (BYTE)			VarEquipProto->GetInt(	 _T("Soil"),		(*iter).c_str(), 0 );
		equip.dwColor				 =					VarEquipProto->GetDword( _T("Corlor"),		(*iter).c_str(), 0 );
		equip.bCanDye				 = (BOOL)			VarEquipProto->GetInt(	 _T("CanDye"),		(*iter).c_str(), false ) ? true : false;

		equip.n16AttALimit[ERA_Physique]	= (INT16)	VarEquipProto->GetInt( _T("PhysiqueLim"),	(*iter).c_str(), 0 );		
		equip.n16AttALimit[ERA_Strength]	= (INT16)	VarEquipProto->GetInt( _T("StrengthLim"),	(*iter).c_str(), 0 );
		equip.n16AttALimit[ERA_Pneuma]		= (INT16)	VarEquipProto->GetInt( _T("PneumaLim"),		(*iter).c_str(), 0 );
		equip.n16AttALimit[ERA_InnerForce]	= (INT16)	VarEquipProto->GetInt( _T("InnerForceLim"),	(*iter).c_str(), 0 );
		equip.n16AttALimit[ERA_Technique]	= (INT16)	VarEquipProto->GetInt( _T("TechniqueLim"),	(*iter).c_str(), 0 );
		equip.n16AttALimit[ERA_Agility]		= (INT16)	VarEquipProto->GetInt( _T("AgilityLim"),	(*iter).c_str(), 0 );
		//more

		m_mapEquipProto.insert(make_pair(equip.dwTypeID, equip));
	}

	KillObj("EquipProto");

	
	//---------------------------------------------------------------------------------
	//读取equip_name
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarEquipDisplay = CreateObj("EquipDisplay", "VarContainer");
	if(!VarEquipDisplay->Load("VFS_System", szEquipDisplay, "id", &EquipDisplayFieldList))
		IMSG(_T("Load file equip_name.xml failed\r\n"));

	for(iter = EquipDisplayFieldList.begin(); iter != EquipDisplayFieldList.end(); ++iter)
	{
		DWORD dwID = VarEquipDisplay->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagItemDisplayInfo* pDisplay = new tagItemDisplayInfo;
		_tcsncpy( pDisplay->szName,				VarEquipDisplay->GetString( _T("name"),					(*iter).c_str(), _T("") ), sizeof(pDisplay->szName)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szType,				VarEquipDisplay->GetString( _T("display_type"),			(*iter).c_str(), _T("") ), sizeof(pDisplay->szType)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDesc,				VarEquipDisplay->GetString( _T("desc2"),				(*iter).c_str(), _T("") ), sizeof(pDisplay->szDesc)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDescEx,			VarEquipDisplay->GetString( _T("desc"),					(*iter).c_str(), _T("") ), sizeof(pDisplay->szDesc)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szMaleIcon,			VarEquipDisplay->GetString( _T("male_icon"),			(*iter).c_str(), _T("") ), sizeof(pDisplay->szMaleIcon)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szFemaleIcon,		VarEquipDisplay->GetString( _T("female_icon"),			(*iter).c_str(), _T("") ), sizeof(pDisplay->szFemaleIcon)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szMaleRightMod,		VarEquipDisplay->GetString( _T("male_right_model"),		(*iter).c_str(), _T("") ), sizeof(pDisplay->szMaleRightMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szFemaleRightMod,	VarEquipDisplay->GetString( _T("female_right_model"),	(*iter).c_str(), _T("") ), sizeof(pDisplay->szFemaleRightMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szMaleLeftMod,		VarEquipDisplay->GetString( _T("male_left_model"),		(*iter).c_str(), _T("") ), sizeof(pDisplay->szMaleLeftMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szFemaleLeftMod,	VarEquipDisplay->GetString( _T("female_left_model"),	(*iter).c_str(), _T("") ), sizeof(pDisplay->szFemaleLeftMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szDropMod,			VarEquipDisplay->GetString( _T("drop_model"),			(*iter).c_str(), _T("") ), sizeof(pDisplay->szDropMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szPertainMod,		VarEquipDisplay->GetString( _T("pertain_model"),		(*iter).c_str(), _T("") ), sizeof(pDisplay->szPertainMod)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szBladeFlareSFX,	VarEquipDisplay->GetString( _T("blade_flare_sfx"),		(*iter).c_str(), _T("") ), sizeof(pDisplay->szBladeFlareSFX)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szBladeGlowSFX,	    VarEquipDisplay->GetString( _T("blade_glow_sfx"),		(*iter).c_str(), _T("") ), sizeof(pDisplay->szBladeGlowSFX)/sizeof(TCHAR));
		_tcsncpy( pDisplay->szAttachSFX,	    VarEquipDisplay->GetString( _T("attach_sfx"),			(*iter).c_str(), _T("") ), sizeof(pDisplay->szAttachSFX)/sizeof(TCHAR));

		pDisplay->ePertainModPos	= (EWeaponPos)	 VarEquipDisplay->GetDword( _T("pertain_model_pos"),(*iter).c_str(), EWP_NULL );
		pDisplay->eModelType		= (EEquipMdlType)VarEquipDisplay->GetDword( _T("model_type"),		(*iter).c_str(), EEMT_NULL );

		TCHAR szBuff[X_SHORT_NAME];
		for( int i = 0; i < MAX_EQUIP_ATTACH_NUM; i++ )
		{
			_stprintf( szBuff, _T("attach%d_pos"), i+1 );
			pDisplay->eAttachModelPos[i] = (EEquipAttachPos)VarEquipDisplay->GetDword( szBuff,	(*iter).c_str(), EEAP_NULL );
		}

		//more

		tagEquipProto* pEquip = FindInMap(m_mapEquipProto, dwID);
		if(P_VALID(pEquip))
		{
			pEquip->pDisplayInfo = pDisplay;
		}
		else
		{
			SAFE_DEL(pDisplay);
		}
		
	}

	KillObj("EquipDisplay");

	//---------------------------------------------------------------------------------
	//读取equip_suit
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarSuitProto = CreateObj("SuitProto", "VarContainer");
	if(!VarSuitProto->Load("VFS_System", szSuitProto, "id", &SuitProtoFieldList))
		IMSG(_T("Load file equip_suit.xml failed\r\n"));

	for(iter = SuitProtoFieldList.begin(); iter != SuitProtoFieldList.end(); ++iter)
	{
		tagSuitProtoClient suit;
		suit.dwID				=		VarSuitProto->GetDword(	_T("id"),			 (*iter).c_str(), GT_INVALID );
		suit.n8SpecEffectNum	= (INT8)VarSuitProto->GetInt(	_T("SpecEffectNum"), (*iter).c_str(), GT_INVALID );
		TCHAR szTemp[X_SHORT_NAME] = {0};
		for( int i = 0; i < MAX_SUIT_ATT_NUM; i++ )
		{
			_stprintf( szTemp, _T("TriggerID%d"),	i+1 );
			suit.dwTriggerID[i] =		VarSuitProto->GetDword( szTemp, (*iter).c_str(), GT_INVALID );
			_stprintf( szTemp, _T("BuffID%d"),		i+1 );
			suit.dwBuffID[i]	=		VarSuitProto->GetDword( szTemp, (*iter).c_str(), GT_INVALID );
			_stprintf( szTemp, _T("ActiveNum%d"),	i+1 );
			suit.n8ActiveNum[i]	= (INT8)VarSuitProto->GetInt(	szTemp, (*iter).c_str(), GT_INVALID );
		}

		m_mapSuitProto.insert(make_pair(suit.dwID, suit));
	}

	KillObj("SuitProto");

	//---------------------------------------------------------------------------------
	//读取equip_suit_name
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarSuitDisplay = CreateObj("SuitDisplay", "VarContainer");
	if(!VarSuitDisplay->Load("VFS_System", szSuitDisplay, "id", &SuitDisplayFieldList))
		IMSG(_T("Load file equip_suit_name.xml failed\r\n"));

	for(iter = SuitDisplayFieldList.begin(); iter != SuitDisplayFieldList.end(); ++iter)
	{
		DWORD dwID = VarSuitDisplay->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagSuitProtoClient* pSuit = FindInMap(m_mapSuitProto, dwID);
		if (P_VALID(pSuit))
		{
			_tcsncpy( pSuit->szSuitName,		VarSuitDisplay->GetString( _T("name"),				(*iter).c_str(), _T("") ), sizeof(pSuit->szSuitName)/sizeof(TCHAR));
			_tcsncpy( pSuit->szSpecEffectMod,	VarSuitDisplay->GetString( _T("spec_effect_mod"),	(*iter).c_str(), _T("") ), sizeof(pSuit->szSpecEffectMod)/sizeof(TCHAR));
		}
	}

	KillObj("SuitDisplay");

	//---------------------------------------------------------------------------------
	//读取treasure_proto
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarTreasureProto = CreateObj("TreasureProto", "VarContainer");
	if(!VarTreasureProto->Load("VFS_System", szTreasureProto, "id", &TreasureProtoFieldList))
		IMSG(_T("Load file clantreasure_proto.xml failed\r\n"));

	for (iter = TreasureProtoFieldList.begin(); iter != TreasureProtoFieldList.end(); ++iter)
	{
		tagTreasureProto treasure;

		treasure.dwTreasureID	=	VarTreasureProto->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		treasure.dwItemID		=	VarTreasureProto->GetDword(_T("itemid"), (*iter).c_str(), GT_INVALID);
		treasure.nConNeed		=	VarTreasureProto->GetInt(_T("act_clancon_need"), (*iter).c_str(), 0);
		treasure.eCtype			=	(ECLanType)VarTreasureProto->GetInt(_T("clantype"), (*iter).c_str(), GT_INVALID);
		treasure.dwShopID		=	VarTreasureProto->GetDword(_T("shopid"), (*iter).c_str(), GT_INVALID);

		m_mapTreasureProto.insert(make_pair(treasure.dwTreasureID, treasure));
		m_mapTreasureProtoByItemID.insert(make_pair(treasure.dwItemID, treasure));
	}

	KillObj("TreasureProto");

    //---------------------------------------------------------------------------------
    //读取PetEquip_proto
    //---------------------------------------------------------------------------------
    TObjRef<VarContainer> VarPetEquipProto = CreateObj("PetEquipProto", "VarContainer");
    if(!VarPetEquipProto->Load("VFS_System", szPetEquipProto, "id", &PetEquipProtoList))
        IMSG(_T("Load file pet_equip_proto.xml failed\r\n"));

    for (iter = PetEquipProtoList.begin(); iter != PetEquipProtoList.end(); ++iter)
    {
        tagPetEquipProto petEquip;

        petEquip.dwTypeID       =   VarPetEquipProto->GetDword(_T("id"), iter->c_str(), GT_INVALID);
        petEquip.nStep          =   VarPetEquipProto->GetInt(_T("step"), iter->c_str(), GT_INVALID);
        petEquip.nGrade         =   VarPetEquipProto->GetInt(_T("grade"), iter->c_str(), GT_INVALID);
        petEquip.nType          =   VarPetEquipProto->GetInt(_T("type"), iter->c_str(), GT_INVALID);
        petEquip.bTypeUnique    =   VarPetEquipProto->GetInt(_T("unique_same_type"), iter->c_str(), GT_INVALID) == 0 ? FALSE : TRUE;
        petEquip.bUnique        =   VarPetEquipProto->GetInt(_T("unique"), iter->c_str(), GT_INVALID) == 0 ? FALSE : TRUE;
        petEquip.nPetAtt[0]     =   VarPetEquipProto->GetInt(_T("att1"), iter->c_str(), GT_INVALID);
        petEquip.nPetAttMod[0]  =   VarPetEquipProto->GetInt(_T("mod1"), iter->c_str(), GT_INVALID);
        petEquip.nPetAtt[1]     =   VarPetEquipProto->GetInt(_T("att2"), iter->c_str(), GT_INVALID);
        petEquip.nPetAttMod[1]  =   VarPetEquipProto->GetInt(_T("mod2"), iter->c_str(), GT_INVALID);
        petEquip.nPetAtt[2]     =   VarPetEquipProto->GetInt(_T("att3"), iter->c_str(), GT_INVALID);

        m_mapPetEquipProtoByID.insert(make_pair(petEquip.dwTypeID, petEquip));
    }

    KillObj("PetEquipProto");

	//---------------------------------------------------------------------------------
	//读取equip_qlty_effect
	//---------------------------------------------------------------------------------
	TObjRef<VarContainer> VarEquipQlty = CreateObj("EquipQlty", "VarContainer");
	if(!VarEquipQlty->Load("VFS_System", szEquipQlty, "id", &EquipQltyList))
		IMSG(_T("Load file equip_qlty_effect.xml failed\r\n"));

	for (iter = EquipQltyList.begin(); iter != EquipQltyList.end(); ++iter)
	{	
		INT qlty			= VarEquipQlty->GetInt(_T("id"), iter->c_str(), 0);	
		tagEquipQltyEffect m;
		m.fWeaponFactor		= VarEquipQlty->GetFloat(_T("WeaponFactor"), iter->c_str(), 1.0);
		m.fArmorFactor		= VarEquipQlty->GetFloat(_T("ArmorFactor"),	 iter->c_str(), 1.0);
		m.fPotFactor		= VarEquipQlty->GetFloat(_T("PotFactor"),	 iter->c_str(), 1.0);
		//more
		
		m_mapEquipQlty.insert(make_pair(qlty, m));
	}

	KillObj("EquipQlty");
}

const tagPetEquipProto* ItemProtoData::FindPetEquipByItemID( DWORD dwID )
{
    const tagItemProto* pItem = FindItemProto(dwID);
    if (P_VALID(pItem))
    {
        return FindPetEquipByEquipID(pItem->nSpecFuncVal1);
    }
    else
    {
        return NULL;
    }
}

const tagPetEquipProto* ItemProtoData::FindPetEquipByEquipID( DWORD dwID )
{
    map<DWORD, tagPetEquipProto>::const_iterator it;
    it = m_mapPetEquipProtoByID.find(dwID);
    if (it != m_mapPetEquipProtoByID.end())
    {
        return &it->second;
    }
    else
    {
        return NULL;
    }
}