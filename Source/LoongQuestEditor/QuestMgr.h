#ifndef QUESTMGR_H_
#define QUESTMGR_H_

#include <map>
#include "Singleton.h"
#include "xml\tinyxml.h"

struct tagQuestProto;

class QuestMgr : public Singleton< QuestMgr >
{
public:

	QuestMgr();
	~QuestMgr();

	//
	BOOL LoadQuestFromXml( tagQuestProto* pQuest, LPCTSTR szFileName );

	//
	void SaveQuestToXml( tagQuestProto* pQuest, LPCTSTR szFileName );

private:

	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, LPCTSTR szValue );
	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue );
	void SaveEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue1, DWORD dwValue2 );
	void SaveMaxEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue, DWORD dwMax, DWORD dwMin );
	typedef map<string,string> XmlNodeMap;
	BOOL LoadXmlNode(TiXmlElement* pNode, XmlNodeMap& xmlNodeMap);
	BOOL SetQuestProto(tagQuestProto* pQuestProto, XmlNodeMap& xmlNodeMap);
	void SetQuestDword(string str, DWORD& dwQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestBool(string str, BOOL& bQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestFloat(string str, FLOAT& fQuest,XmlNodeMap& xmlNodeMap);
	void SetQuestString(string str, TCHAR* szQuest, XmlNodeMap& xmlNodeMap);
private:
	std::map< DWORD, tagQuestProto* > m_mapQuests;
};

#define sQuestMgr QuestMgr::getSingleton()

#endif /* QUESTMGR_H_ */