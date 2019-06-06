/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   17:59
	filename: 	d:\loong_client\Client\QuestScript.h
	file path:	d:\loong_client\Client
	file base:	QuestScript
	file ext:	h
	author:		leili
	
	purpose:	执行任务系统的脚本
*********************************************************************/
#pragma once
class GameScriptMgr;

class QuestScript : public GameScript
{
public:
	QuestScript(void);
	~QuestScript(void);

	/**	初始对话
	*/
	VOID OnTalk(DWORD npcID);

	/**	交任务对话
	*/
	VOID OnAcceptQuestTalk(DWORD questID, DWORD step);

	/**	接任务对话
	*/
	VOID OnCompleteQuestTalk(DWORD questID, DWORD step);

	/**	情景对话
	*/
	VOID OnScenarioTalk(DWORD id, DWORD step);

	/**	生产技能对话
	*/
	VOID OnProduceSkillTalk(DWORD id, DWORD step);

	/**帮跑跑商选择
	*/
	VOID OnGuildCommerceTalk(DWORD id, DWORD step);

	/**	脚本解析服务器数据
	*/
	VOID OnParse(void);

	VOID OnTrigger(LPCTSTR scriptFile, DWORD questSerial);
	VOID OnUse(LPCTSTR scriptFile, DWORD questID);

private:
	VOID GetTable(LPCTSTR szPath, string& strTable);

private:
	TObjRef<Util>			m_pUtil;
	TObjRef<GameScriptMgr>	m_pGSMgr;
};