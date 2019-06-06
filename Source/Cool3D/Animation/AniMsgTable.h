#pragma once
#include "..\F3D.h"

class IFS;
namespace F3D
{
	struct tagAniMsg
	{
		DWORD	trackName;//这个消息绑定到那个Track
		float	time;
		DWORD	msgCode;//four cc
	};

	class F3D_API AniMsgTable
	{
	public:
		AniMsgTable(void);
		~AniMsgTable(void);

		//! 读盘,存盘格式为CSV,例如"atck,2.1,atck"
		bool LoadFromFile(IFS *pFS,const char* szFileName);
		//! 存盘
		bool SaveToFile(const char* szFileName); 
		//! 得到一个Track包含的msg列表
		vector<tagAniMsg> GetTrackMsgArray(DWORD trackName);

		vector<tagAniMsg>& GetMsgArray() { return m_msgArray; }

		void Clear() { m_msgArray.clear(); }
	private:
		vector<tagAniMsg>	m_msgArray;
	};
}//namespace F3D