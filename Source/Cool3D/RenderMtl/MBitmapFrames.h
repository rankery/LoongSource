#pragma once
#include "mbitmap.h"

namespace Cool3D
{
	/**	\class MBitmapFrames 
		\brief 多个贴图文件组成的贴图动画
	*/
	class MBitmapFrames : public MBitmap
	{
	public:
		MBitmapFrames(void);
		virtual ~MBitmapFrames(void);

		void SetParam(float fps)
		{
			m_fps=fps;
		}
		void LoadFrames(const TCHAR* szBaseName,bool bSysFS,int numFrm,int numLevels);
		virtual void UpdateModifiers(float deltaTime);

	private:
		vector<ResTexture*>	m_texFrames;
		float	m_fps;	//动画帧播放速度

		DECL_RTTI_DYNC(MBitmapFrames);
	};
}//namespace Cool3D