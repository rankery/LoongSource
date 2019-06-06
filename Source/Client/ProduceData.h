#pragma once
#include "..\WorldDefine\compose_define.h"
#include ".\BaseData.h"
class ProduceData
	: public BaseData
{
public:
	ProduceData(void);
	~ProduceData(void);

	static ProduceData* Inst();

	/** 载入强化属性文件
	*/
	virtual void LoadFromFile();

	/** 获取合成配方基础结构
	*/
	tagProduceProto* FindProduceProto(EProduceType eType, DWORD dwID)
	{
		switch(eType)
		{
		case EPCT_Artisan:
			 return FindInMap(m_mapArtisan, dwID);
			break;
		case EPCT_DanTraining:
			return FindInMap(m_mapDanTraining, dwID);
			break;
		case EPCT_Smith:
			return FindInMap(m_mapSmith, dwID);
			break;
		case EPCT_Casting:
			return FindInMap(m_mapCasting, dwID);
			break;
		case EPCT_Dressmaker:
			return FindInMap(m_mapDressmaker, dwID);
			break;
		case EPCT_Aechnics:
			return FindInMap(m_mapAechnics, dwID);
			break;
		case EPCT_Smilt:
			return FindInMap(m_mapSmilt, dwID);
			break;
		case EPCT_GodArtisan:
			return FindInMap(m_mapGodArtisan, dwID);
			break;
		case EPCT_Pet:
			return FindInMap(m_mapPet, dwID);
			break;
		case EPCT_EquipMaster:
			return FindInMap(m_mapEquipMaster, dwID);
			break;
		default:
			return NULL;
			break;
		}
		return NULL;
	}

	/**获取分解配方基础结构
	*/
	tagDeComposeProto* FinDeComposeProto(DWORD dwID)
	{
		return FindInMap(m_mapDeCompose, dwID);
	}

	/**获取生产合成map
	*/
	void GetProduceMap(map<DWORD, tagProduceProto>& outMap, EProduceType eType=EPCT_Artisan)
	{
		switch(eType)
		{
		case EPCT_Artisan:
			outMap = m_mapArtisan;
			break;
		case EPCT_DanTraining:
			outMap = m_mapDanTraining;
			break;
		case EPCT_Smith:
			outMap = m_mapSmith;
			break;
		case EPCT_Casting:
			outMap = m_mapCasting;
			break;
		case EPCT_Dressmaker:
			outMap = m_mapDressmaker;
			break;
		case EPCT_Aechnics:
			outMap = m_mapAechnics;
			break;
		case EPCT_Smilt:
			outMap = m_mapSmilt;
			break;
		case EPCT_GodArtisan:
			outMap = m_mapGodArtisan;
			break;
		case EPCT_Pet:
			outMap = m_mapPet;
			break;
		case EPCT_EquipMaster:
			outMap = m_mapEquipMaster;
			break;
		}
	}
	const map<DWORD, tagProduceProto>* GetProduceMap(EProduceType eType=EPCT_Artisan)
	{
		switch(eType)
		{
		case EPCT_Artisan:
			return &m_mapArtisan;
			break;
		case EPCT_DanTraining:
			return &m_mapDanTraining;
			break;
		case EPCT_Smith:
			return &m_mapSmith;
			break;
		case EPCT_Casting:
			return &m_mapCasting;
			break;
		case EPCT_Dressmaker:
			return &m_mapDressmaker;
			break;
		case EPCT_Aechnics:
			return &m_mapAechnics;
			break;
		case EPCT_Smilt:
			return &m_mapSmilt;
			break;
		case EPCT_GodArtisan:
			return &m_mapGodArtisan;
			break;
		case EPCT_Pet:
			return &m_mapPet;
			break;
		case EPCT_EquipMaster:
			return &m_mapEquipMaster;
		}
		return NULL;
	}

	/**得到点化map
	*/
	void GetDeComposeMap(map<DWORD, tagDeComposeProto>& outMap)
	{
		outMap = m_mapDeCompose;
	}
	
	map<DWORD, tagDeComposeProto>& GetDeComposeMap()
	{
		return m_mapDeCompose;
	}

private:
	map<DWORD, tagProduceProto>		m_mapArtisan;				//工匠
	map<DWORD, tagProduceProto>		m_mapDanTraining;			//练丹
	map<DWORD, tagProduceProto>		m_mapSmith;					//锻造
	map<DWORD, tagProduceProto>		m_mapCasting;				//铸甲
	map<DWORD, tagProduceProto>		m_mapDressmaker;			//裁缝	
	map<DWORD, tagProduceProto>		m_mapAechnics;				//工艺
	map<DWORD, tagProduceProto>		m_mapSmilt;					//冶练
	map<DWORD, tagDeComposeProto>	m_mapDeCompose;				//分解
	map<DWORD, tagProduceProto>		m_mapGodArtisan;			//天匠
	map<DWORD, tagProduceProto>		m_mapPet;					//灵兽
	map<DWORD, tagProduceProto>		m_mapEquipMaster;			// 装备匠师
};