#pragma once
#include ".\BaseData.h"

struct tagItem2GM
{
	DWORD				dwTypeID;					//物品TypeID
	TCHAR				szName[X_SHORT_STRING];		// 物品名称
};

/** \class	ItemProtoData
	\brief	物品、装备原始属性结构存储
*/
class ItemProtoData
	: public BaseData
{
public:
	ItemProtoData(void);
	~ItemProtoData(void);

	static ItemProtoData* Inst();

	/** 载入属性文件
	*/
	virtual void LoadFromFile();

	int GetItemNum() { return (int)m_mapItemProto.size(); }
	std::map<DWORD, tagItem2GM>& GetItem() { return m_mapItemProto; }

	const tagItem2GM* FindItemProto(const tstring& strName) 
	{
		std::map<DWORD, tagItem2GM>::iterator it=m_mapItemProto.begin();
		for( ; it!=m_mapItemProto.end(); ++it )
		{
			if( it->second.szName == strName )
			{
				return &(it->second);
			}
		}
		return NULL;
	}

private:
	std::map<DWORD, tagItem2GM>				m_mapItemProto;		// 物品原始结构
};