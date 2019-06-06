#pragma once
#include ".\RenderSys.h"

namespace Cool3D
{
	/**	\class SortByMtlAndZVal 
		\biref 将RenderItem排序
		\remarks
			\par 减少shader切换次数
			\par 减少贴图切换次数
			\par 减少pixelshader的调用
	*/
	class SortByMtlAndZVal
	{
		class GroupByPri;
		class GroupByTrait1;
		class GroupByTrait2;
		class Sorter;
	public:
		void Sort(vector<RenderItem>& items,const Matrix4& matView);
	};
}//namespace Cool3D