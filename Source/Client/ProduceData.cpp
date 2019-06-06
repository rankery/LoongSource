#include "stdafx.h"
#include "ProduceData.h"
ProduceData::ProduceData( void )
{

}

ProduceData::~ProduceData( void )
{

}
ProduceData g_Produce;
ProduceData* ProduceData::Inst()
{
	return &g_Produce;
}

void ProduceData::LoadFromFile()
{
	const TCHAR* szProduceProto = _T("data\\system\\attdata\\compose_proto.xml");
	tstring strPath1 = g_strLocalPath + _T("\\attdata\\compose_name.xml");
	const TCHAR* szProduceName = strPath1.c_str();

	const TCHAR* szDeComposeProto = _T("data\\system\\attdata\\decompose_proto.xml");
	tstring strPath2 = g_strLocalPath + _T("\\attdata\\decompose_name.xml");
	const TCHAR* szDeComposeName = strPath2.c_str();

	list<tstring> produceList;
	list<tstring> proNameList;
	list<tstring> decompsoeList;
	list<tstring> deNameList;
	list<tstring>::iterator iter;

	//读取生产合成文件
	TObjRef<VarContainer> varProduce = CreateObj( "ProduceProto", "VarContainer" );
	if(!varProduce->Load("VFS_System", szProduceProto, "id", &produceList))
		IMSG(_T("Load file compose_proto.xml failed\r\n"));

	for(iter=produceList.begin(); iter!=produceList.end(); ++iter)
	{
		tagProduceProto produceProto;
		produceProto.dwID = varProduce->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		//_tcscpy( produceProto.szName, varProduce->GetString(_T("name"), (*iter).c_str(), _T("")) );
		produceProto.eProType = (EProduceType)varProduce->GetDword(_T("produce_type"), (*iter).c_str(), EPCT_NULL);
		produceProto.eComType = (EComposeType)varProduce->GetDword(_T("compose_type"), (*iter).c_str(), ECOMT_NULL);
		produceProto.eFormType = (EFormulaType)varProduce->GetDword(_T("formula_type"), (*iter).c_str(), EFORMT_NULL);
		produceProto.eFormFrom = (EFormulaFrom)varProduce->GetDword(_T("formula_from"), (*iter).c_str(), EFormulaFrom_NULL);
		produceProto.dwFormLvl = varProduce->GetDword(_T("fromula_level"), (*iter).c_str(), GT_INVALID);
		produceProto.dwProItemTypeID = varProduce->GetDword(_T("compose_item"), (*iter).c_str(), GT_INVALID);
		produceProto.dwProQuantity = varProduce->GetDword(_T("compose_num"), (*iter).c_str(), GT_INVALID);
		produceProto.nSuccessRate = (INT)varProduce->GetDword(_T("success_rate"), (*iter).c_str(), GT_INVALID);
		produceProto.dwProduceTime = varProduce->GetDword(_T("produce_time"), (*iter).c_str(), GT_INVALID);
		produceProto.dwVitalityConsume = varProduce->GetDword(_T("vitality_consume"), (*iter).c_str(), GT_INVALID);
		produceProto.dwMasterIncre = varProduce->GetDword(_T("master_incre"), (*iter).c_str(), GT_INVALID);
		produceProto.dwMoneyConsume = varProduce->GetDword(_T("money_consume"), (*iter).c_str(), 0);
		for(int i=0; i<MAX_PRODUCE_STUFF_QUANTITY; i++)
		{
			TCHAR szBuff[64];
			_stprintf( szBuff, _T("%s%d%s"),  _T("stuff"), i+1, _T("_id") );
			produceProto.ProduceStuff[i].dwStuffID = varProduce->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_type") );
			produceProto.ProduceStuff[i].eStuffType = (EStuffType)varProduce->GetDword(szBuff, (*iter).c_str(), EST_Null);
			_stprintf( szBuff, _T("%s%d%s"), _T("stuff"), i+1, _T("_num") );
			produceProto.ProduceStuff[i].dwStuffNum = varProduce->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}
		switch(produceProto.eProType)
		{
		case EPCT_Artisan:
			m_mapArtisan.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_DanTraining:
			m_mapDanTraining.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Smith:
			m_mapSmith.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Casting:
			m_mapCasting.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Dressmaker:
			m_mapDressmaker.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Aechnics:
			m_mapAechnics.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Smilt:
			m_mapSmilt.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_GodArtisan:
			m_mapGodArtisan.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_Pet:
			m_mapPet.insert(make_pair(produceProto.dwID, produceProto));
			break;
		case EPCT_EquipMaster:
			m_mapEquipMaster.insert(make_pair(produceProto.dwID, produceProto));
			break;
		}
	}

	KillObj( "ProduceProto" );

	TObjRef<VarContainer> varProduceName = CreateObj( "ProduceName", "VarContainer" );
	if(!varProduceName->Load("VFS_System", szProduceName, "id", &proNameList))
		IMSG(_T("Load file compose_proto.xml failed\r\n"));
	for(iter=proNameList.begin(); iter!=proNameList.end(); iter++)
	{
		DWORD dwID = varProduceName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		EProduceType eType = (EProduceType)varProduceName->GetDword(_T("produce_type"), (*iter).c_str(), EPCT_NULL);
		tagProduceProto* produceProto = FindProduceProto(eType, dwID);
		if( P_VALID(produceProto) )
		{
			_tcscpy( produceProto->szName, varProduceName->GetString(_T("name"), (*iter).c_str(), _T("")) );
		}
	}

	KillObj( "ProduceName" );


	//读取分解文件
	TObjRef<VarContainer> varDeCompose = CreateObj( "DeComposeProto", "VarContainer" );
	if(!varDeCompose->Load("VFS_System", szDeComposeProto, "id", &decompsoeList))
		IMSG(_T("Load file compose_proto.xml failed\r\n"));
	for(iter=decompsoeList.begin(); iter!=decompsoeList.end(); iter++)
	{
		tagDeComposeProto deCompose;
		deCompose.dwID = varDeCompose->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		deCompose.eComType = (EComposeType)varDeCompose->GetDword(_T("compose_type"), (*iter).c_str(), ECOMT_NULL);
		deCompose.byLevel = (BYTE)varDeCompose->GetDword(_T("level"), (*iter).c_str(), GT_INVALID);
		deCompose.byQuality = (BYTE)varDeCompose->GetDword(_T("quality"), (*iter).c_str(), GT_INVALID);
		deCompose.eType = (EItemType)varDeCompose->GetDword(_T("type"), (*iter).c_str(), EIT_Null);
		deCompose.eTypeEx = (EItemTypeEx)varDeCompose->GetDword(_T("type_ex"), (*iter).c_str(), EITE_Null);
		deCompose.ePos = (EEquipPos)varDeCompose->GetDword(_T("equip_pos"), (*iter).c_str(), GT_INVALID);
		deCompose.dwFormLvl = varDeCompose->GetDword(_T("formula_level"), (*iter).c_str(), GT_INVALID);
		deCompose.eFormFrom = (EFormulaFrom)varDeCompose->GetDword(_T("formula_from"), (*iter).c_str(), EFormulaFrom_NULL);
		deCompose.dwProduceTime = varDeCompose->GetDword(_T("produce_time"), (*iter).c_str(), GT_INVALID);
		deCompose.dwVitalityConsume = varDeCompose->GetDword(_T("vitality_consume"), (*iter).c_str(), GT_INVALID);
		deCompose.dwMasterIncre = varDeCompose->GetDword(_T("master_incre"), (*iter).c_str(), GT_INVALID);
		deCompose.dwMoneyConsume = varDeCompose->GetDword(_T("money_consume"), (*iter).c_str(), 0);
		for(int i=0; i<MAX_OUTPUT_QUANTITY; i++)
		{
			TCHAR szBuff[64];
			_stprintf(szBuff, _T("item%d_id"), i+1);
			deCompose.OutputStuff[i].dwStuffTypeID = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("per_min%d_val"), i+1);
			deCompose.OutputStuff[i].nPerMinVal = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("per_max%d_val"), i+1);
			deCompose.OutputStuff[i].nPerMaxVal = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("suc_min%d_val"), i+1);
			deCompose.OutputStuff[i].nSucMinVal = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("suc_max%d_val"), i+1);
			deCompose.OutputStuff[i].nSucMaxVal = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
			_stprintf(szBuff, _T("rate%d"), i+1);
			deCompose.OutputStuff[i].nRate = varDeCompose->GetDword(szBuff, (*iter).c_str(), GT_INVALID);
		}

		m_mapDeCompose.insert(make_pair(deCompose.dwID, deCompose));
	}

	KillObj( "DeComposeProto" );
	
	TObjRef<VarContainer> varDeName = CreateObj( "DeNameName", "VarContainer" );
	if(!varDeName->Load("VFS_System", szDeComposeName, "id", &deNameList))
		IMSG(_T("Load file compose_proto.xml failed\r\n"));
	for(iter=deNameList.begin(); iter!=deNameList.end(); iter++)
	{
		DWORD dwID = varDeName->GetDword(_T("id"), (*iter).c_str(), GT_INVALID);
		tagDeComposeProto* deCompose = FinDeComposeProto(dwID);
		if( P_VALID(deCompose) )
		{
			_tcscpy( deCompose->szName, varDeName->GetString(_T("name"), (*iter).c_str(), _T("")) );
		}
	}
	
	KillObj( "DeNameName" );

}