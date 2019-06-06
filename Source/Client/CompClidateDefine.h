#pragma once
#include "stdafx.h"
#include  "..\Worlddefine\ItemDefine.h"

#define CTRL_QUANTITY 7						//材料控件个数
#define FORMULA_QUANTITY 5					//子配方控件个数
#define STUFF_QUANTITY 6					//材料数量显示个数
#define MAX_ASSIST_STUFF_QUANTITY 6			//辅料的数量


//合成、强化主料(物品或装备)
struct tagMainStuff
{
	INT64			n64ID;					//64ID
	DWORD			dwTypeID;				//typeid
	EEquipPos		euqipPos;				//装备的位置
	tagMainStuff()
	{
		memset(this, 0x0, sizeof(*this));
	}
};

//生产合成、强化辅料(保存配方需要的材料和玩家放入的材料)
struct tagAssistStuff
{
	INT64			n64ID;					//64ID材料的全局ID
	DWORD			dwTypeID;				//材料TypeID(从表合成强化表中读取)
	EStuffType		stuffType;				//材料类型(从表合成强化表中读取)
	INT				nQuantity;				//材料需要的数量
	INT				nBagQuan;				//背包中该材料的数量
	tagAssistStuff()
	{
		memset(this, 0x0, sizeof(*this));
	}
};

//IM道具
struct tagIMStuff
{
	INT64			n64ID;					//IMid
	tagIMStuff()
	{
		n64ID = GT_INVALID;
	}
};

/**class CompConsoContainer
   brief:	生产合成、强化容器
   purpose:	保存主料 、辅料的数据。
*/
class CompConsoContainer					
{
public:
	CompConsoContainer(void){ ClearAllStuff(); }
	~CompConsoContainer(void){ ClearAllStuff(); }

	void Add(const tagMainStuff& value)
	{
		m_mainStuff = value;
	}
	void Add(const tagAssistStuff& value)
	{
		m_assStuff.push_back(value);
	}
	void Add(const tagIMStuff& value)
	{
		m_imStuff = value;
	}

	void ClearAllStuff()
	{
		memset(&m_mainStuff, 0x0, sizeof(m_mainStuff));
		memset(&m_imStuff, 0x0, sizeof(m_imStuff));
		m_assStuff.clear();
	}
	
	void ClearAssistStuff()
	{
		m_assStuff.clear();
		memset(&m_imStuff, 0x0, sizeof(m_imStuff));
	}
	
	EStuffType GetAssistStuffType(DWORD dwPos)
	{
		if(dwPos>=0 && dwPos<(DWORD)m_assStuff.size())
			return EST_Null;
		return m_assStuff[dwPos].stuffType;
	}

	EEquipPos GetMainStuffEquipPos()
	{
		return m_mainStuff.euqipPos;
	}

	void GetAssistStuffMap(vector<tagAssistStuff>& assMap)
	{
		assMap = m_assStuff;
	}

	void GetMainStuff(tagMainStuff& value)
	{
		value = m_mainStuff;
	}

	void GetIMStuff(tagIMStuff& value)
	{
		value = m_imStuff;
	}

	tagAssistStuff* GetAssistStuff(int nPos)
	{
		if(nPos<0 && nPos>=(int)m_assStuff.size())
			return NULL;
		return& m_assStuff[nPos];
	}

	tagAssistStuff* GetAssistStuff(INT64 n64ID)
	{
		for(int i=0; i<(int)m_assStuff.size(); i++)
		{
			if(m_assStuff[i].n64ID==n64ID)
				return &m_assStuff[i];
		}
		return NULL;
	}

	int GetAssStuffSize() { return (int)m_assStuff.size(); }

private:
	tagMainStuff				m_mainStuff;		//主料
	vector<tagAssistStuff>		m_assStuff;			//辅料
	tagIMStuff					m_imStuff;			//IM材料s
};
