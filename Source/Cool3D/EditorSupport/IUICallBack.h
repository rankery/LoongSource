#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	/** \class IUICallBack
		\brief 界面回调接口
		\remark 用于在界面上显示消息和进度
	*/
	class Cool3D_API IUICallBack
	{
	public:
		IUICallBack(void);
		virtual ~IUICallBack(void);

		virtual void AddMsg(const TCHAR* szFormat,...) =0;
		virtual void SetProgress(int curVal,int maxVal) =0;
	};
}//namespace Cool3D