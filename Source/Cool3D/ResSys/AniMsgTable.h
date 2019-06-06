#pragma once
#include "..\Cool3D.h"

#pragma warning(disable:4251)

class IFS;

namespace Cool3D
{
	struct tagAniMsg
	{
		//DWORD	trackName;//这个消息绑定到那个Track
		float	time;
		DWORD	msgCode;//four cc
	};

	typedef multimap<DWORD,tagAniMsg>		MultiMsgMap;

	class Cool3D_API AniMsgTable
	{
	public:
		AniMsgTable(void);
		~AniMsgTable(void);

		//! 读盘,存盘格式为CSV,例如"atck,2.1,atck"
		bool LoadFromFile(IFS *pFS,const TCHAR* szFileName);
		//! 存盘
		bool SaveToFile(const TCHAR* szFileName); 
		//! 得到一个Track包含的msg列表
		void CheckMsgForTime( DWORD trackName, float lastTime, float curTime, list<DWORD>& msgCodes ) const;

		MultiMsgMap&  GetMsgMap() { return m_msgMap; }

		void Clear() { m_msgMap.clear();}
	private:
		MultiMsgMap		m_msgMap;		/// 采用map可以节省查找的时间
	};
}//namespace Cool3D
