#pragma once
#include "..\Cool3D.h"
#include ".\ISGVisitorCallBack.h"
#include "..\RenderSys\IRenderCallBack.h"

#include "safe_list.h"
namespace Cool3D
{
	class IUICallBack;
	/** \class LightMapTempGen
		\brief 光照图临时文件生成
		\remark 包括posmap和normalmap
	*/
	class Cool3D_API LightMapTempGen : public ISGVisitorCallBack,
									public IRenderCallBack
	{
	public:
		LightMapTempGen(IUICallBack* pUI);
		virtual ~LightMapTempGen(void);
		
		//--
		virtual bool OnVisitSceneNode(HugeQuadTreeSG* pSG,SceneNode* pNode,int index,int totalNum,DWORD param); 
		virtual void OnRenderBefore(SceneNode* pSGNode,RenderNode* pRenderNode); 

		//--
		bool Start(const TCHAR* szSGPath);
		void DeleteTempFiles(const TCHAR* szPath);

		static tstring MakePosMapName(const TCHAR* szLightMapName);
		static tstring MakeNormalMapName(const TCHAR* szLightMapName);

		struct tagThreadData
		{
			IDirect3DTexture9* pTexture;
			tstring strName;
		};
	private:
		class Member;
		Member* m_p;

		IUICallBack* m_pUI;
		TSafeList<tagThreadData*>	m_listRT;
	};
}//namespace Cool3D