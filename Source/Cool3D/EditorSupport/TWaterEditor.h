#pragma once

namespace F3D
{
	/**	\class TWaterEditor
		\brief 编辑n个水面区域
	*/
	class F3D_API TWaterEditor
	{
	public:
		TWaterEditor(void);
		virtual ~TWaterEditor(void);

		void AddWaterArea();
	};
}//namespace F3D