#include "stdafx.h"
#include "LevelUpData.h"

LevelUpData::LevelUpData( void )
{

}

LevelUpData::~LevelUpData( void )
{

}

LevelUpData	g_LevelUpData;
LevelUpData* LevelUpData::Inst()
{	
	return &g_LevelUpData;
}

void LevelUpData::LoadFromFile()
{
	const TCHAR* szExpProto = _T("data\\system\\attdata\\role_att_level_up.xml");

	list<tstring> expList;
	list<tstring>::iterator iter;

	//∂¡»°æ≠—È
	TObjRef<VarContainer> varExp = CreateObj( "ExpProto", "VarContainer" );
	if(!varExp->Load("VFS_System", szExpProto, "id", &expList))
		IMSG(_T("Load file role_att_level_up.xml failed\r\n"));

	for(iter=expList.begin(); iter!=expList.end(); ++iter)
	{
		tagExperience exp;
		exp.nLevel				 = varExp->GetInt(_T("id"),			(*iter).c_str(), NULL);
		exp.nExp				 = varExp->GetInt(_T("ExpLevelUp"),	(*iter).c_str(), NULL);
		exp.nAtt[ERA_Physique]	 = varExp->GetInt(_T("Physique"),	(*iter).c_str(), NULL);
		exp.nAtt[ERA_Strength]	 = varExp->GetInt(_T("Strength"),	(*iter).c_str(), NULL);
		exp.nAtt[ERA_Pneuma]	 = varExp->GetInt(_T("Pneuma"),		(*iter).c_str(), NULL);
		exp.nAtt[ERA_InnerForce] = varExp->GetInt(_T("InnerForce"),	(*iter).c_str(), NULL);
		exp.nAtt[ERA_Technique]	 = varExp->GetInt(_T("Technique"),	(*iter).c_str(), NULL);
		exp.nAtt[ERA_Agility]	 = varExp->GetInt(_T("Agility"),	(*iter).c_str(), NULL);
		exp.nCost[ESCT_HP]		 = varExp->GetInt(_T("HP"),			(*iter).c_str(), NULL);
		exp.nCost[ESCT_MP]		 = varExp->GetInt(_T("MP"),			(*iter).c_str(), NULL);
		exp.nCost[ESCT_Valicity] = varExp->GetInt(_T("Vitality"),	(*iter).c_str(), NULL);

		m_mapExp.insert(make_pair(exp.nLevel, exp));
	}

	KillObj("ExpProto");
}