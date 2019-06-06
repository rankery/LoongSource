#include "stdafx.h"
#include "InstanceProtoData.h"

BOOL InstanceProtoData::LoadFromFile(LPCTSTR szPath)
{
	// Â·¾¶Îª¿Õ
	if (_tcscmp(szPath, _T("")) == 0)
	{
		return FALSE;
	}
	
	list<tstring> listProto;

	TObjRef<VarContainer> varInstanceProto = CreateObj("InstanceProto", "VarContainer");
	if (!varInstanceProto->Load("VFS_System", szPath, "id", &listProto))
	{
		IMSG(_T("Load Instance proto file failed"));
		return FALSE;
	}
	TObjRef<Util> pUtil;

	list<tstring>::iterator end = listProto.end();
	for (list<tstring>::iterator begin = listProto.begin(); begin != end; ++begin)
	{
		tagInstanceProtoData data;
		data.strName = varInstanceProto->GetString(_T("id"), begin->c_str());
		data.dwInstanceID = pUtil->Crc32(data.strName.c_str());
		data.eType = static_cast<EInstanceMapType>(varInstanceProto->GetInt(_T("InstanceMode"), begin->c_str()));
		data.bAskEnter = static_cast<BOOL>(varInstanceProto->GetInt(_T("AskEnter"), begin->c_str()));
		data.bSelectHard = static_cast<BOOL>(varInstanceProto->GetInt(_T("SelectHard"), begin->c_str()));
		data.bSelectNormal = static_cast<BOOL>(varInstanceProto->GetInt(_T("SelectNormal"), begin->c_str()));
		data.bSelectElite = static_cast<BOOL>(varInstanceProto->GetInt(_T("SelectElite"), begin->c_str()));
		data.bSelectDevil = static_cast<BOOL>(varInstanceProto->GetInt(_T("SelectDevil"), begin->c_str()));
		data.nNumLowerLimit = varInstanceProto->GetInt(_T("NumDownLimit"), begin->c_str());
		data.nNumUpperLimit = varInstanceProto->GetInt(_T("NumUpLimit"), begin->c_str());
		data.nLevelLowerLimit = varInstanceProto->GetInt(_T("LevelDownLimit"), begin->c_str());
		data.nLevelUpperLimit = varInstanceProto->GetInt(_T("LevelUpLimit"), begin->c_str());
		data.bCombat = static_cast<BOOL>(varInstanceProto->GetInt(_T("IsCombat"), begin->c_str()));
		data.bPK = static_cast<BOOL>(varInstanceProto->GetInt(_T("IsPK"), begin->c_str()));
		data.bLoseSafeGuard = static_cast<BOOL>(varInstanceProto->GetInt(_T("IsloseSafeGuard"), begin->c_str()));

		data.Condition[EIHM_Normal].eCondition = static_cast<EInstanceComplteCondition>(varInstanceProto->GetInt(_T("CompleteConditionNor"), begin->c_str()));
		data.Condition[EIHM_Normal].dwCompleteVal1 = varInstanceProto->GetDword(_T("CompleteValNor1"), begin->c_str(), GT_INVALID);
		data.Condition[EIHM_Normal].dwCompleteVal2 = varInstanceProto->GetDword(_T("CompleteValNor2"), begin->c_str(), GT_INVALID);
		
		data.Condition[EIHM_Elite].eCondition = static_cast<EInstanceComplteCondition>(varInstanceProto->GetInt(_T("CompleteConditionEli"), begin->c_str()));
		data.Condition[EIHM_Elite].dwCompleteVal1 = varInstanceProto->GetDword(_T("CompleteValEli1"), begin->c_str(), GT_INVALID);
		data.Condition[EIHM_Elite].dwCompleteVal2 = varInstanceProto->GetDword(_T("CompleteValEli2"), begin->c_str(), GT_INVALID);

		data.Condition[EIHM_Devil].eCondition = static_cast<EInstanceComplteCondition>(varInstanceProto->GetInt(_T("CompleteConditionDev"), begin->c_str()));
		data.Condition[EIHM_Devil].dwCompleteVal1 = varInstanceProto->GetDword(_T("CompleteValDev1"), begin->c_str(), GT_INVALID);
		data.Condition[EIHM_Devil].dwCompleteVal2 = varInstanceProto->GetDword(_T("CompleteValDev2"), begin->c_str(), GT_INVALID);

		// TODO: more
		m_mapInstanceProtoData.insert(make_pair(data.dwInstanceID, data));
	}
	KillObj("InstanceProto");
	
	return TRUE;
}

const tagInstanceProtoData *InstanceProtoData::FindInstanceProtoData(DWORD dwInstanceID)
{
	InstanceProtoDataMap::iterator find = m_mapInstanceProtoData.find(dwInstanceID);
	if (find != m_mapInstanceProtoData.end())
		return &(find->second);
	return static_cast<const tagInstanceProtoData*>(NULL);
}