#pragma once
#include "HalfByteMap.h"

namespace WorldBase
{
	/** \class AreaMap
		\brief 玩家自动寻路区域图
	*/
	class AreaMap
	{
	public:
		AreaMap(void);
		~AreaMap(void);

		void ReadFile(IFS* pFS,DWORD hFile);
		int GetValue(int x,int z);
		int SafeGetValue(int x,int z);

		void Create(HalfByteMap* pSource);
		void WriteFile(FILE* fp);

	private:
		BYTE		m_type;			//类型:0一个区域,1两个区域,2多个区域
		BYTE		m_oneAreaIndex;	//只有一个区域时的区域编号
		BitMap		m_bitmap;		//有两个区域时保存为Bitmap
		HalfByteMap m_halfByteMap;	//有多个区域时保存为HalfByteMap
	};
}//namespace WorldBase
