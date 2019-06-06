#include "stdafx.h"
#include "ConsolidateData.h"
#include "..\WorldDefine\compose_define.h"
#include "..\WorldDefine\container_define.h"
ConsolidateData::ConsolidateData()
{

}

ConsolidateData::~ConsolidateData()
{

}

ConsolidateData g_Consolidate;
ConsolidateData* ConsolidateData::Inst()
{
	return &g_Consolidate;
}

void ConsolidateData::LoadFromFile()
{
	const TCHAR* szPosyProto = _T("data\\system\\attdata\\posy_proto.xml");
	const TCHAR* szPosyPos = _T("data\\system\\attdata\\posy_pos.xml");
	tstring path1 = g_strLocalPath +  _T("\\attdata\\posy_name.xml");
	const TCHAR* szPosyName = path1.c_str();

	const TCHAR* szEngraveProto = _T("data\\system\\attdata\\engrave_proto.xml");
	const TCHAR* szEngravePos = _T("data\\system\\attdata\\engrave_pos.xml");
	tstring path2 = g_strLocalPath +  _T("\\attdata\\engrave_name.xml");
	const TCHAR* szEngraveName = path2.c_str();

	const TCHAR* szConItemProto = _T("data\\system\\attdata\\consolidate_item.xml");

	const TCHAR* szQuenchProto = _T("data\\system\\attdata\\quench_proto.xml");
	tstring path3 = g_strLocalPath +  _T("\\attdata\\quench_name.xml"); 
	const TCHAR* szQuenchName = path3.c_str();

	list<tstring> posyList;
	list<tstring> posyPosdList;
	list<tstring> posyNameList;
	list<tstring> engraveList;
	list<tstring> engravePosList;
	list<tstring> engraveNameList;
	list<tstring> conItemList;
	list<tstring> quenchList;
	list<tstring> quenchNameList;
	list<tstring>::iterator iter;

	//∂¡»°√˙Œ∆
	TObjRef<VarContainer> varPosy = CreateObj( "PosyProto", "VarContainer" );
	if(!varPosy->Load("VFS_System", szPosyProto, "id", &posyList))
		IMSG(_T("Load file posy.xml failed\r\n"));

	TObjRef<VarContainer> varPosyPos = CreateObj( "PosyPos", "VarContainer" );
	if(!varPosyPos->Load("VFS_System", szPosyPos, "id", &posyPosdList))
		IMSG(_T("Load file posy_pos.xml failed\r\n"));

	for(iter=posyList.begin(); iter!=posyList.end(); ++iter)
	{
		tagPosyProto posy;
		posy.dwID = varPosy->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		//_tcscpy( posy.szName, varPosy->GetString(_T("name"), (*iter).c_str(), _T("")) );
		posy.ePosyAtt = (EPosyAtt)varPosy->GetDword(_T("att"), (*iter).c_str(), EPosyAtt_NULL);
		posy.byPosyTimes = (BYTE)varPosy->GetDword(_T("times"), (*iter).c_str(), GT_INVALID);
		posy.nSuccessRate = varPosy->GetDword(_T("success_rate"), (*iter).c_str(), GT_INVALID);
		posy.dwProduceTime = varPosy->GetDword(_T("produce_time"), (*iter).c_str(), GT_INVALID);
		posy.dwPotValConsume = varPosy->GetDword(_T("potval_consume"), (*iter).c_str(), GT_INVALID);
		posy.dwMoneyConsume = varPosy->GetDword(_T("money_consume"), (*iter).c_str(), GT_INVALID);
		posy.fcoefficientA = varPosy->GetFloat( _T("coefficientA"), (*iter).c_str(), 0.0f );
		posy.fcoefficientB = varPosy->GetFloat( _T("coefficientB"), (*iter).c_str(), 0.0f );
		posy.fcoefficientC = varPosy->GetFloat( _T("coefficientC"), (*iter).c_str(), 1.0f );
		for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
		{
			TCHAR szBuff[64];
			_stprintf( szBuff, _T("%s%d%s"),  _T("stuff"), i+1, _T("_id") );
			posy.ConsolidateStuff[i].dwStuffID = varPosy->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_type") );
			posy.ConsolidateStuff[i].eStuffType = (EStuffType)varPosy->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_num") );
			posy.ConsolidateStuff[i].dwStuffNum = (EStuffType)varPosy->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}

		for(list<tstring>::iterator it=posyPosdList.begin(); it!=posyPosdList.end(); ++it)
		{
			EPosyAtt ePosy = (EPosyAtt)varPosyPos->GetDword(_T("id"), (*it).c_str());
			if( posy.ePosyAtt == ePosy )
			{
				posy.ConsolidatePos[0].byConsolidate = (BYTE)varPosyPos->GetDword(_T("right_hand"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[0].ePos = EEP_RightHand;
				posy.ConsolidatePos[1].byConsolidate = (BYTE)varPosyPos->GetDword(_T("head"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[1].ePos = EEP_Head;
				posy.ConsolidatePos[2].byConsolidate = (BYTE)varPosyPos->GetDword(_T("body"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[2].ePos = EEP_Body;
				posy.ConsolidatePos[3].byConsolidate = (BYTE)varPosyPos->GetDword(_T("leg"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[3].ePos = EEP_Legs;
				posy.ConsolidatePos[4].byConsolidate = (BYTE)varPosyPos->GetDword(_T("wrist"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[4].ePos = EEP_Wrist;
				posy.ConsolidatePos[5].byConsolidate = (BYTE)varPosyPos->GetDword(_T("feet"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[5].ePos = EEP_Feet;
				posy.ConsolidatePos[6].byConsolidate = (BYTE)varPosyPos->GetDword(_T("back"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[6].ePos = EEP_Back;
				posy.ConsolidatePos[7].byConsolidate = (BYTE)varPosyPos->GetDword(_T("face"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[7].ePos = EEP_Face;
				posy.ConsolidatePos[8].byConsolidate = (BYTE)varPosyPos->GetDword(_T("neck"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[8].ePos = EEP_Neck;
				posy.ConsolidatePos[9].byConsolidate = (BYTE)varPosyPos->GetDword(_T("finger1"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[9].ePos = EEP_Finger1;
				posy.ConsolidatePos[10].byConsolidate = (BYTE)varPosyPos->GetDword(_T("waist"), (*it).c_str(), GT_INVALID);
				posy.ConsolidatePos[10].ePos = EEP_Waist;
				break;
			}
		}
		m_mapPosy.insert(make_pair(posy.dwID, posy));
	}
	
	KillObj("PosyPos");
	KillObj("PosyProto");

	TObjRef<VarContainer> varPosyName = CreateObj( "PosyName", "VarContainer" );
	if(!varPosyName->Load("VFS_System", szPosyName, "id", &posyNameList))
		IMSG(_T("Load file posy_name.xml failed\r\n"));
	for(iter=posyNameList.begin(); iter!=posyNameList.end(); iter++)
	{
		DWORD dwID = varPosyName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagPosyProto *pPosy = FindPosyProto(dwID);
		if( P_VALID(pPosy) )
		{
			_tcscpy( pPosy->szName, varPosyName->GetString(_T("name"), (*iter).c_str(), _T("")) );	
		}
	}
	KillObj("PosyName");

	//∂¡»°Ô‘øÃ
	TObjRef<VarContainer> varEngrave = CreateObj( "EngraveProto", "VarContainer" );
	if(!varEngrave->Load("VFS_System", szEngraveProto, "id", &engraveList))
		IMSG(_T("Load file engrave.xml failed"));

	TObjRef<VarContainer> varEngravePos = CreateObj( "EngravePos", "VarContainer" );
	if(!varEngravePos->Load("VFS_System", szEngravePos, "id", &engravePosList))
		IMSG(_T("Load file engrave_pos.xml failed\r\n"));

	for(iter=engraveList.begin(); iter!=engraveList.end(); ++iter)
	{
		tagEngraveProto engrave;
		engrave.dwID = varEngrave->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		//_tcscpy( engrave.szName, varEngrave->GetString(_T("name"), (*iter).c_str(), _T("")) );
		engrave.eEngraveAtt = (EEngraveAtt)varEngrave->GetDword(_T("att"), (*iter).c_str(), EEngraveAtt_NULL);
		engrave.byEngraveTimes = (BYTE)varEngrave->GetDword(_T("times"), (*iter).c_str(), GT_INVALID);
		engrave.nSuccessRate = varEngrave->GetDword(_T("success_rate"), (*iter).c_str(), GT_INVALID);
		engrave.dwProduceTime = varEngrave->GetDword(_T("produce_time"), (*iter).c_str(), GT_INVALID);
		engrave.dwPotValConsume = varEngrave->GetDword(_T("potval_consume"), (*iter).c_str(), GT_INVALID);
		engrave.dwMoneyConsume = varEngrave->GetDword(_T("money_consume"), (*iter).c_str(), GT_INVALID);
		engrave.fcoefficientA = varEngrave->GetFloat( _T("coefficientA"), (*iter).c_str(), 0.0f );
		engrave.fcoefficientB = varEngrave->GetFloat( _T("coefficientB"), (*iter).c_str(), 0.0f );
		engrave.fcoefficientC = varEngrave->GetFloat( _T("coefficientC"), (*iter).c_str(), 1.0f );
		for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
		{
			TCHAR szBuff[64];
			_stprintf( szBuff, _T("%s%d%s"),  _T("stuff"), i+1, _T("_id") );
			engrave.ConsolidateStuff[i].dwStuffID = varEngrave->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_type") );
			engrave.ConsolidateStuff[i].eStuffType = (EStuffType)varEngrave->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_num") );
			engrave.ConsolidateStuff[i].dwStuffNum = (EStuffType)varEngrave->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}

		for(list<tstring>::iterator it=engravePosList.begin(); it!=engravePosList.end(); ++it)
		{
			EPosyAtt ePosy = (EPosyAtt)varEngravePos->GetDword(_T("id"), (*it).c_str());
			if( engrave.eEngraveAtt == ePosy )
			{
				engrave.ConsolidatePos[0].byConsolidate = (BYTE)varEngravePos->GetDword(_T("right_hand"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[0].ePos = EEP_RightHand;
				engrave.ConsolidatePos[1].byConsolidate = (BYTE)varEngravePos->GetDword(_T("head"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[1].ePos = EEP_Head;
				engrave.ConsolidatePos[2].byConsolidate = (BYTE)varEngravePos->GetDword(_T("body"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[2].ePos = EEP_Body;
				engrave.ConsolidatePos[3].byConsolidate = (BYTE)varEngravePos->GetDword(_T("leg"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[3].ePos = EEP_Legs;
				engrave.ConsolidatePos[4].byConsolidate = (BYTE)varEngravePos->GetDword(_T("wrist"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[4].ePos = EEP_Wrist;
				engrave.ConsolidatePos[5].byConsolidate = (BYTE)varEngravePos->GetDword(_T("feet"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[5].ePos = EEP_Feet;
				engrave.ConsolidatePos[6].byConsolidate = (BYTE)varEngravePos->GetDword(_T("back"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[6].ePos = EEP_Back;
				engrave.ConsolidatePos[7].byConsolidate = (BYTE)varEngravePos->GetDword(_T("face"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[7].ePos = EEP_Face;
				engrave.ConsolidatePos[8].byConsolidate = (BYTE)varEngravePos->GetDword(_T("neck"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[8].ePos = EEP_Neck;
				engrave.ConsolidatePos[9].byConsolidate = (BYTE)varEngravePos->GetDword(_T("finger1"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[9].ePos = EEP_Finger1;
				engrave.ConsolidatePos[10].byConsolidate = (BYTE)varEngravePos->GetDword(_T("waist"), (*it).c_str(), GT_INVALID);
				engrave.ConsolidatePos[10].ePos = EEP_Waist;
				break;
			}
		}

		m_mapEngrave.insert(make_pair(engrave.dwID, engrave));
	}
	
	KillObj("EngravePos");
	KillObj("EngraveProto");

	TObjRef<VarContainer> varEngraveName = CreateObj( "EngraveName", "VarContainer" );
	if(!varEngraveName->Load("VFS_System", szEngraveName, "id", &engraveNameList))
		IMSG(_T("Load file engrave_name.xml failed\r\n"));
	for(iter=engraveNameList.begin(); iter!=engraveNameList.end(); iter++)
	{
		DWORD dwID = varEngraveName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagEngraveProto *pEngrave= FindEngraveProto(dwID);
		if( P_VALID(pEngrave) )
		{
			_tcscpy( pEngrave->szName, varEngraveName->GetString(_T("name"), (*iter).c_str(), _T("")) );	
		}
	}
	KillObj("EngraveName");

	//µ¿æﬂ
	TObjRef<VarContainer> varConItem = CreateObj( "ConsolidateItem", "VarContainer" );
	if(!varConItem->Load("VFS_System", szConItemProto, "id", &conItemList))
		IMSG(_T("Load file consolidate_item.xml failed\r\n"));

	for(iter=conItemList.begin(); iter!=conItemList.end(); ++iter)
	{
		tagConsolidateItem item;
		item.dwID = varConItem->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		item.dwPotValConsume = varConItem->GetDword(_T("potval_consume"), (*iter).c_str(), GT_INVALID);
		item.nSuccessRate = varConItem->GetDword(_T("success_rate"), (*iter).c_str(), GT_INVALID);
		item.eWuXing = (EWuXing)varConItem->GetDword(_T("loongsoul_att"), (*iter).c_str(), GT_INVALID);
		for(int i=0; i<MAX_CONSOLIDATE_ROLEATT; i++)
		{
			TCHAR szBuff[64];
			_stprintf( szBuff, _T("role_att%d"),  i);
			item.tagRoleAtt[i].eRoleAtt = (ERoleAttribute)varConItem->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("att_val%d"),  i);
			item.tagRoleAtt[i].nAttVal = varConItem->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}
		
		item.ConsolidatePos[0].byConsolidate = (BYTE)varConItem->GetDword(_T("right_hand"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[0].ePos = EEP_RightHand;
		item.ConsolidatePos[1].byConsolidate = (BYTE)varConItem->GetDword(_T("head"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[1].ePos = EEP_Head;
		item.ConsolidatePos[2].byConsolidate = (BYTE)varConItem->GetDword(_T("body"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[2].ePos = EEP_Body;
		item.ConsolidatePos[3].byConsolidate = (BYTE)varConItem->GetDword(_T("leg"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[3].ePos = EEP_Legs;
		item.ConsolidatePos[4].byConsolidate = (BYTE)varConItem->GetDword(_T("wrist"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[4].ePos = EEP_Wrist;
		item.ConsolidatePos[5].byConsolidate = (BYTE)varConItem->GetDword(_T("feet"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[5].ePos = EEP_Feet;
		item.ConsolidatePos[6].byConsolidate = (BYTE)varConItem->GetDword(_T("back"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[6].ePos = EEP_Back;
		item.ConsolidatePos[7].byConsolidate = (BYTE)varConItem->GetDword(_T("face"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[7].ePos = EEP_Face;
		item.ConsolidatePos[8].byConsolidate = (BYTE)varConItem->GetDword(_T("neck"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[8].ePos = EEP_Neck;
		item.ConsolidatePos[9].byConsolidate = (BYTE)varConItem->GetDword(_T("finger1"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[9].ePos = EEP_Finger1;
		item.ConsolidatePos[10].byConsolidate = (BYTE)varConItem->GetDword(_T("waist"), (*iter).c_str(), GT_INVALID);
		item.ConsolidatePos[10].ePos = EEP_Waist;

		m_mapConsolidateItem.insert(make_pair(item.dwID, item));

	}	

	KillObj("ConsolidateItem");

	//∂¡»°¥„ª
	TObjRef<VarContainer> varQuench = CreateObj( "QuenchProto", "VarContainer" );
	if(!varQuench->Load("VFS_System", szQuenchProto, "id", &quenchList))
		IMSG(_T("Load file quench.xml failed\r\n"));
	for(iter=quenchList.begin(); iter!=quenchList.end(); ++iter)
	{
		tagQuenchProto quench;
		quench.dwID = varQuench->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		//_tcscpy( quench.szName, varQuench->GetString(_T("name"), (*iter).c_str(), _T("")) );
		quench.eQuenchType = (EQuenchType)varQuench->GetDword(_T("type"), (*iter).c_str(), GT_INVALID);
		quench.srcQuenchAtt.eWuXing = (EWuXing)varQuench->GetDword(_T("src_att"), (*iter).c_str(), GT_INVALID);
		quench.srcQuenchAtt.nWuXingValue = varQuench->GetDword(_T("src_att_value"), (*iter).c_str(), GT_INVALID);
		quench.dstQuenchAtt.eWuXing = (EWuXing)varQuench->GetDword(_T("dst_att"), (*iter).c_str(), GT_INVALID);
		quench.dstQuenchAtt.nWuXingValue = varQuench->GetDword(_T("dst_att_value"), (*iter).c_str(), GT_INVALID);
		quench.dwPotValConsume = varQuench->GetDword(_T("potval_consume"), (*iter).c_str(), GT_INVALID);
		quench.dwMoneyConsume = varQuench->GetDword(_T("money_consume"), (*iter).c_str(), GT_INVALID);
		quench.nSuccessRate = varQuench->GetDword(_T("success_rate"), (*iter).c_str(), GT_INVALID);
		quench.dwProduceTime = varQuench->GetDword(_T("produce_time"), (*iter).c_str(), GT_INVALID);
		for(int i=0; i<MAX_CONSOLIDATE_STUFF_QUANTITY; i++)
		{
			TCHAR szBuff[64];
			_stprintf( szBuff, _T("%s%d%s"),  _T("stuff"), i+1, _T("_id") );
			quench.ConsolidateStuff[i].dwStuffID = varQuench->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_type") );
			quench.ConsolidateStuff[i].eStuffType = (EStuffType)varQuench->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_num") );
			quench.ConsolidateStuff[i].dwStuffNum = (EStuffType)varQuench->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}

		m_mapQuench.insert(make_pair(quench.dwID, quench));
	}

	KillObj("QuenchProto");

	TObjRef<VarContainer> varQuenchName = CreateObj( "QuenchName", "VarContainer" );
	if(!varQuenchName->Load("VFS_System", szQuenchName, "id", &quenchNameList))
		IMSG(_T("Load file quench_name.xml failed\r\n"));
	for(iter=quenchNameList.begin(); iter!=quenchNameList.end(); iter++)
	{
		DWORD dwID = varQuenchName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagQuenchProto *pQuench= FindQuenchProto(dwID);
		if( P_VALID(pQuench) )
		{
			_tcscpy( pQuench->szName, varQuenchName->GetString(_T("name"), (*iter).c_str(), _T("")) );	
		}
	}
	KillObj("QuenchName");
}