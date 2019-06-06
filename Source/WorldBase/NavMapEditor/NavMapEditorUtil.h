#pragma once

namespace WorldBase
{
	class NavMapEditorUtil
	{
	public:
		NavMapEditorUtil(void){}
		~NavMapEditorUtil(void){}

		/** 将src的不可行走区域应用到target
		*/
		static void MergeCangoMap(BitMap* pSrc,BitMap* pTarget);

	};

}//namespace WorldBase