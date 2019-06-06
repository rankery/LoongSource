#pragma once
#include "..\Cool3D.h"

#pragma warning(disable:4251)

class IFS;

namespace Cool3D
{
	class ActTimeTable
	{
	public:
		ActTimeTable(void);
		~ActTimeTable(void);

		//! 读盘,存盘格式为CSV,例如"atck,2000"
		bool LoadFromFile(IFS *pFS,const TCHAR* szFileName);
		float operator[](DWORD dwTrackName);
	private:
		map<DWORD,float>	m_actTimeMap;// 采用map可以节省查找的时间
	};
}//namespace Cool3D
