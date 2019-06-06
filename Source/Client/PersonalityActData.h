#pragma once

#include ".\BaseData.h"
#include "..\WorldDefine\motion_define.h"

struct tagPersonAct : public tagMotionProto
{
	// 个性动作类型
	enum EType
	{
		T_All,       // 全身动作（只播一次）
		T_Upper,     // 上身动作（只播一次）
		T_3Act,      // 3个动作
	};
	DWORD			dwID;
	tstring			strCommand1;		// 指令1
	tstring			strCommand2;		// 指令2
	tstring			strAct1;			// 进入动作或T_All，T_Upper播放的动作
	tstring			strAct2;			// 保持动作
	tstring			strAct3;			// 离开动作
	EType			eType;				// 类型

	tagPersonAct() : dwID( GT_INVALID ), eType( T_All ){}
};

class PersonalityActData : 
	public BaseData
{
public:
	PersonalityActData(void);
	~PersonalityActData(void);

	static PersonalityActData* Inst();

	/** 载入个性动作属性文件
	*/
	virtual void LoadFromFile();

	const tagPersonAct* FindPersonAct(DWORD id)
	{
		return FindInMap(m_mapPersonAct, id);
	}

	map<DWORD, tagPersonAct>& GetPersonActMap()
	{
		return m_mapPersonAct;
	}

private:
	map<DWORD, tagPersonAct>			m_mapPersonAct;
};