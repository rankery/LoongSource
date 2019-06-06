#pragma once

#include ".\BaseData.h"

/** \特效数据
*/
struct tagSFXData
{
	bool bCanMove;   // 是否可移动
};

/** \class SFXData
	\brief 特效数据
*/
class SFXData
	: public BaseData
{
public:
	SFXData(void);
	~SFXData(void);

	static SFXData* Inst();
	/** \载入属性文件
	*/
	virtual void LoadFromFile();
	/** \查找特效数据
	*/
	const tagSFXData* FindData(const TCHAR* szSFXName);
private:
	map<DWORD, tagSFXData>		m_mapSFXData;
};