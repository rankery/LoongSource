#pragma once
#include "mmuvtrans.h"

namespace Cool3D
{
	/**	\class MMUVPage 
		\brief 一张贴图包含多个动画帧,使用uv矩阵来播放这些帧
	*/
	class MMUVPage : public MMUVTrans
	{
	public:
		MMUVPage(void);
		virtual ~MMUVPage(void);

		void SetParam(int uDim,int vDim,float fps,bool bPlayOnce);
		virtual void Update(float deltaTime,IMaterial *pOwner);
		virtual void ResetAni(IMaterial *pOwner);

	private:
		void SetFrame(int frame,IMaterial *pOwner);
	private:
		int		m_uDim,
				m_vDim;	//这张贴图中有uDim*vDim个动画帧
		float	m_fps;	//动画帧播放速度
		bool	m_bPlayOnce;	//是否只播放一遍

		int		m_curFrm;
		float	m_curFrmTime;

		DECL_RTTI_DYNC(MMUVPage);
	};
}//namespace Cool3D