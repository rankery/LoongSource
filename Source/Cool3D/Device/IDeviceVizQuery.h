#pragma once

namespace Cool3D
{
	/**	\class IDeviceVizQuery
		\brief 硬件的可见性查询
		\remarks 返回begin(),end()之间所调用的Draw函数通过Z testing的象素个数
	*/
	class IDeviceVizQuery
	{
	public:
		IDeviceVizQuery(void);
		virtual ~IDeviceVizQuery(void);

		virtual void Create() =0;
		virtual bool Begin() =0;
		virtual bool End() =0;
		virtual DWORD GetResultSync() =0;
		virtual bool GetResultAsync(DWORD& out) =0;
	};
}//namespace Cool3D