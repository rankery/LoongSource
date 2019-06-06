/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   18:01
	filename: 	d:\loong_client\Client\QuestScript.cpp
	file path:	d:\loong_client\Client
	file base:	QuestScript
	file ext:	cpp
	author:		leili
	
	purpose:	执行任务系统的脚本
*********************************************************************/
#include "StdAfx.h"
#include "QuestScript.h"
#include "QuestMgr.h"

QuestScript::QuestScript( void )
{

}

QuestScript::~QuestScript( void )
{

}

VOID QuestScript::OnTalk( DWORD npcID )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnTalk()";
				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}

VOID QuestScript::OnTrigger( LPCTSTR scriptFile, DWORD questSerial )
{
	GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile);
	if( P_VALID(pScript) )
	{
		char szScript[512];
		sprintf( szScript, "OnTrigger(%d)", questSerial );
		pScript->RunString(szScript);
	}
}

VOID QuestScript::OnUse( LPCTSTR scriptFile, DWORD questID )
{
	GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile);
	if( P_VALID(pScript) )
	{
		char szScript[512];
		sprintf( szScript, "OnUse(%d)", questID );
		pScript->RunString(szScript);
	}
}

VOID QuestScript::OnAcceptQuestTalk( DWORD questID, DWORD step )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		DWORD npcID = QuestMgr::Inst()->GetTalkNPCTypeID();
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnAcceptQuestTalk(" << questID << "," << step << ")";

				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}

VOID QuestScript::OnCompleteQuestTalk( DWORD questID, DWORD step )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		DWORD npcID = QuestMgr::Inst()->GetTalkNPCTypeID();
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnCompleteQuestTalk(" << questID << "," << step << ")";

				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}

VOID QuestScript::OnScenarioTalk( DWORD id, DWORD step )
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		DWORD npcID = QuestMgr::Inst()->GetTalkNPCTypeID();
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnScenarioTalk(" << id << "," << step << ")";

				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}


VOID QuestScript::OnProduceSkillTalk(DWORD id, DWORD step)
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		DWORD npcID = QuestMgr::Inst()->GetTalkNPCTypeID();
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnProduceSkillTalk(" << id << "," << step << ")";

				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}

VOID QuestScript::OnGuildCommerceTalk(DWORD id, DWORD step)
{
	QuestData *pData = QuestMgr::Inst()->GetData();
	if( P_VALID(pData) )
	{
		tstring scriptFile;
		DWORD npcID = QuestMgr::Inst()->GetTalkNPCTypeID();
		if( pData->GetNPCScriptFile(npcID, scriptFile) )
		{
			GameScript *pScript = m_pGSMgr->CreateScript("VFS_System", scriptFile.c_str());
			if( P_VALID(pScript) )
			{
				string strTable;
				this->GetTable(scriptFile.c_str(), strTable);
				stringstream strFunc;
				strFunc << strTable << ".OnGuildCommerceTalk(" << id << "," << step << ")";

				pScript->RunString( strFunc.str().c_str() );
			}
		}
	}
}



VOID QuestScript::OnParse( void )
{
	GameScript *pScript = m_pGSMgr->GetParseScript();
	if( P_VALID(pScript) )
	{
		pScript->RunString( "OnParse()" );
	}
}

VOID QuestScript::GetTable( LPCTSTR szPath, string& strTable )
{
	TCHAR szFile[256];
	_tsplitpath( szPath, NULL, NULL, szFile, NULL );

	CHAR szTable[256];
	m_pUtil->UnicodeToUnicode8( szFile, szTable );
	strTable = szTable;
}

