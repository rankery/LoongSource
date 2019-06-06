/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestMgr.h
// Created:     2008-11-12
/////////////////////////////////////////////////////////////////////////////

#include  <windows.h>

#ifndef QUEST_MGR_H_
#define QUEST_MGR_H_
#include <string>
#include <map>
#include <list>
#include <vector>
using namespace std;

#include "Singleton.h"
#include "tinyxml/tinyxml.h"

struct tagQuestProto;

struct tagQuest 
{
	tagQuestProto *pQuestProto;
	wstring file_path;
	wstring local_path;
	wstring file_name;
	wstring file_dir;
	tagQuest()
	{
		pQuestProto = NULL;
		file_path = _T("");
		local_path = _T("");
		file_name = _T("");
		file_dir = _T("");
	}
};

typedef map<string,string> XmlNodeMap;
typedef vector<pair<string,string>> XmlNodeArray;
typedef map<UINT16, tagQuest*> QuestMap;
typedef std::vector<pair<INT16,wstring>> QuestTypeArray;

class QuestMgr : public Singleton<QuestMgr>
{
public:
	QuestMgr();
	~QuestMgr();

	//从xml任务文件里加载任务数据
	BOOL LoadQuest(LPCTSTR pPath);

	//将任务数据保存到xml文件中
	BOOL SaveQuest(UINT16 id);

	//删除任务文件
	void DeleteQuest(UINT16 id);

	//
	void SaveAcceptQuestRelation(LPCTSTR pPath);

	void SaveCompleteQuestRelation(LPCTSTR pPath);
	
	void SaveQuestNPCRelation(LPCTSTR pPath);

	QuestMap& GetQuestMap() { return m_mapQuest; }

	QuestTypeArray& GetQuestType() { return m_questType; }
	
	list<wstring>& GetQuestDir() { return m_listQuestDir; }

	//任务ID是否存在
	BOOL IsExist(INT16 questID)
	{
		return m_mapQuest.end() != m_mapQuest.find(questID);
	}

	//
	void AddQuestToMap();

	//wstring-->string
	static string ToAnsi(const wstring& widestring);

	//utf8
	static string ToUtf8(const wstring& widestring);

	//用于保存临时新建的Quest；
	tagQuest *m_pQuestTmp;

private:
	
	//遍历制定目录，将指定任务后缀名称文件加入集合
	void FindFile(LPCTSTR pFolder, list<wstring>& xmlMap, list<wstring>& localMap);
	//加载任务文件
	void LoadQuestFile(LPCTSTR pPath);
	//从xml任务文件里加载任务Local数据
	void LoadQuestLocal(LPCTSTR pPath);
	//读取任务类型数据
	BOOL LoadQuestType(LPCTSTR szPath);

	//设置任务数据
	void LoadXmlNode(TiXmlElement* pNode, XmlNodeMap& xmlNodeMap);
	void SetQuestProto(tagQuestProto* pQuestProto, XmlNodeMap& xmlNodeMap);

	void LoadXmlNode(TiXmlElement* pNode, XmlNodeArray& xmlNodeArray);
	
	void SetQuestDword(string str, DWORD& dwQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestDwordFlags(string str, DWORD& dwQuest, INT32 nMask, XmlNodeMap& xmlNodeMap);
	void SetQuestUINT16(string str, UINT16& unQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestBYTE(string str, BYTE& byQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestINT16(string str, INT16& n16Quest,XmlNodeMap& xmlNodeMap);
	void SetQuestINT32(string str, INT32& nQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestBool(string str, BOOL& bQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestFloat(string str, FLOAT& fQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestString(string str, TCHAR* szQuest, INT32 nLen, XmlNodeMap& xmlNodeMap);

	//保存任务
	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, LPCTSTR szValue );
	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue );
	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue1, DWORD dwValue2 );
	void SaveItem( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue1, DWORD dwValue2, INT nValue3 );
	void SaveMaxEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue, DWORD dwMax, DWORD dwMin );
	void SaveCreatureEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue, DWORD dwID2, DWORD dwID3, DWORD dwNum );

	//string-->wstring
	wstring ToGBKW(const char* utf8);

	//检查文件扩展名是否匹配
	bool IsExtensionOK(LPCTSTR pFileName, LPCTSTR pExt);

private:

	//所有任务数据,以任务ID为索引
	QuestMap m_mapQuest;

	QuestTypeArray	m_questType;
	
	//所有任务文件集合
	list<wstring> m_listQuestFile;
	//所有任务Local文件集合
	list<wstring> m_listQuestLocalFile;
	//所有目录集合
	list<wstring> m_listQuestDir;
};

#define sQuestMgr QuestMgr::getSingleton()

#endif /* QUEST_MGR_H_ */


