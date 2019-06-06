#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	class IUICallBack;
	class IRenderTexture;
	/** \class LightMapBlur
		\brief 光照图模糊工具
	*/
	class Cool3D_API LightMapBlur
	{
	public:
		LightMapBlur(IUICallBack* pUI);
		virtual ~LightMapBlur(void);
		
		bool Start(const TCHAR* szPath,int blurTimes,UINT mapWidth,UINT mapHeight);
		void Diffuse(IRenderTexture* pTex,int diffuseTimes);

		struct tagThreadData
		{
			IDirect3DTexture9* pTexture;
			tstring strName;
		};

	private:
		class Member;
		Member* m_p;

		IUICallBack* m_pUI;
	};
}//namespace Cool3D