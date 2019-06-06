#pragma once
#include "..\Cool3D.h"
#include ".\ISGVisitorCallBack.h"
#include "..\Math\Color.h"

#include "safe_list.h"
namespace Cool3D
{
	class IUICallBack;
	struct tagLight;
	/** \class LightMapping
		\brief 计算LightMap光照和阴影
	*/
	class Cool3D_API LightMapping : public ISGVisitorCallBack
	{
	public:
		LightMapping(IUICallBack* pUI);
		virtual ~LightMapping(void);

		//--ISGVisitorCallBack
		virtual bool OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param);

		//--
		bool Start(const TCHAR* szSGPath,const tagLight* pSunLight,Color4f& skyCol,BOOL bCalcAO);

		struct tagThreadData
		{
			IDirect3DTexture9* pTexture;
			tstring strName;
		};
	private:
		class Member;
		Member* m_p;

		BOOL m_bCalcAO;	//是否计算天光

		IUICallBack* m_pUI;
		TSafeList<tagThreadData*>	m_listRT;
	};
}//namespace Cool3D