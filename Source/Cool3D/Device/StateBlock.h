#pragma once
#include "..\Cool3D.h"
#include "..\util\Noncopyable.h"
#include "IDevice.h"

namespace Cool3D
{
	/**	\class StateBlock 
		\brief 一个RenderState的值的数组
	*/
	class Cool3D_API StateBlock : private Noncopyable
	{
		struct tagStateVal
		{
			ERStateType state;
			DWORD val;
		};
	public:
		StateBlock(void);
		virtual ~StateBlock(void);

		void Clear();
		/**	将Device中指定的State的当前值存入数组中
		*/
		void SaveState(ERStateType state);
		/**	将所保存的State的值的数组应用到Device上
		*/
		void Apply();

	protected:
		vector<tagStateVal>	m_stateArray;
	};
}//namespace Cool3D