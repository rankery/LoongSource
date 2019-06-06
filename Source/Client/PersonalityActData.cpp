#include "stdafx.h"
#include "PersonalityActData.h"

PersonalityActData::PersonalityActData( void )
{

}

PersonalityActData::~PersonalityActData( void )
{

}

PersonalityActData g_PersonalityAct;
PersonalityActData* PersonalityActData::Inst()
{
	return &g_PersonalityAct;
}

void PersonalityActData::LoadFromFile()
{
	tstring strActProto = _T("data\\system\\attdata\\motion_proto.xml");
	tstring strAct = g_strLocalPath + _T("\\attdata\\personality_act.xml");

	list<tstring> actProtoList;
	list<tstring> actList;
	list<tstring>::iterator iter;

	//读取个性动作
	TObjRef<VarContainer> varActProto = CreateObj( "motion_proto", "VarContainer" );
	if(!varActProto->Load("VFS_System", strActProto.c_str(), "id", &actProtoList))
		IMSG(_T("Load file motion_proto.xml failed\r\n"));

	TObjRef<VarContainer> varAct = CreateObj( "personality_act", "VarContainer" );
	if(!varAct->Load("VFS_System", strAct.c_str(), "id", &actList))
		IMSG(_T("Load file personality_act.xml failed\r\n"));


	for(iter=actProtoList.begin(); iter!=actProtoList.end(); ++iter)
	{
		tagPersonAct act;
		act.dwID			= varActProto->GetDword(_T("id"),		(*iter).c_str(),	GT_INVALID	);
		act.eMotionType		= (EMotionType1)varActProto->GetDword(_T("type"),	(*iter).c_str(),	EMT1_Self	);
		act.dwFriendVal		= varActProto->GetDword(_T("friendval"),	(*iter).c_str(),	0	);
		
		m_mapPersonAct.insert(make_pair(act.dwID, act));
	}

	for(iter=actList.begin(); iter!=actList.end(); ++iter)
	{
		const DWORD dwID = varAct->GetDword( _T("id"), (*iter).c_str(), GT_INVALID );
		map<DWORD, tagPersonAct>::iterator itrFind = m_mapPersonAct.find( dwID );
		if( itrFind != m_mapPersonAct.end() )
		{
			itrFind->second.strCommand1  = varAct->GetString(_T("command1"),		(*iter).c_str(),	_T("")		);
			itrFind->second.strCommand2  = varAct->GetString(_T("command2"),		(*iter).c_str(),	_T("")		);
			itrFind->second.strAct1	     = varAct->GetString(_T("action1"),			(*iter).c_str(),	_T("")		);
			itrFind->second.strAct2		 = varAct->GetString(_T("action2"),			(*iter).c_str(),	_T("")		);
			itrFind->second.strAct3		 = varAct->GetString(_T("action3"),			(*iter).c_str(),	_T("")		);
			itrFind->second.eType        = (tagPersonAct::EType)varAct->GetInt(_T("type"), (*iter).c_str(), tagPersonAct::T_All);
		}
	}

	KillObj("motion_proto");
	KillObj("personality_act");
}