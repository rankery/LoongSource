#pragma once
#include "..\WorldDefine\compose_define.h"
#include ".\BaseData.h"

class ConsolidateData
	: public BaseData
{
public:
	ConsolidateData(void);
	~ConsolidateData(void);

	static ConsolidateData* Inst();

	/** 载入强化属性文件
	*/
	virtual void LoadFromFile();

	/** 获取铭纹配方基础结构
	*/
	tagPosyProto* FindPosyProto(DWORD dwID)
	{
		return FindInMap(m_mapPosy, dwID);
	}
	/**获取镌刻配方基础结构
	*/
	tagEngraveProto* FindEngraveProto(DWORD dwID)
	{
		return FindInMap(m_mapEngrave, dwID);
	}
	/**获取强化道具基础结构
	*/
	tagConsolidateItem* FindConsolidateItemProto(DWORD dwID)
	{
		return FindInMap(m_mapConsolidateItem, dwID);
	}
	/**获取淬火西方基础结构
	*/
	tagQuenchProto* FindQuenchProto(DWORD dwID)
	{
		return FindInMap(m_mapQuench, dwID);
	}

	/**获取铭纹map
	*/
	const map<DWORD, tagPosyProto>& GetPosyMap( )
	{
		return m_mapPosy;
	}

	/**获取镌刻map
	*/
	const map<DWORD, tagEngraveProto>& GetEngraveMap( )
	{
		return m_mapEngrave;
	}

	/**获取强化道具map
	*/
	const map<DWORD, tagConsolidateItem>& GetConsolidateItemMap( )
	{
		return  m_mapConsolidateItem;
	}
	/**获取淬火map
	*/
	const map<DWORD, tagQuenchProto>& GetQuenchMap()
	{
		return m_mapQuench;
	}

private:
	map<DWORD, tagPosyProto>		m_mapPosy;			//铭纹
	map<DWORD, tagEngraveProto>		m_mapEngrave;		//镌刻
	map<DWORD, tagQuenchProto>		m_mapQuench;		//淬火
	map<DWORD, tagConsolidateItem>  m_mapConsolidateItem;	//强化道具(镶嵌,烙印,龙附)
};