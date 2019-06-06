#pragma once
#include "..\Cool3D.h"
#include ".\ISGVisitorCallBack.h"


#include "safe_list.h"
namespace Cool3D
{
	class IUICallBack;
	/** \class Radiosity
		\brief º∆À„LightMap∑¯…‰∂»
	*/
	class Cool3D_API Radiosity : public ISGVisitorCallBack
	{
	public:
		Radiosity(IUICallBack* pUI);
		virtual ~Radiosity(void);

		//--ISGVisitorCallBack
		virtual void OnSceneGraphLoad(HugeQuadTreeSG* pSG);
		virtual bool OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param);

		//--
		bool Start(const TCHAR* szSGPath);

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