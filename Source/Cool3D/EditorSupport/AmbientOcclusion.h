#pragma once
#include "..\Cool3D.h"
#include "ISGVisitorCallBack.h"

#include "safe_list.h"
namespace Cool3D
{
	class IUICallBack;

	/** \class AmbientOcclusion
		\brief 计算环境光遮挡
	*/
	class Cool3D_API AmbientOcclusion 
		: public ISGVisitorCallBack
	{
	public:
		AmbientOcclusion(IUICallBack* pUI);
		virtual ~AmbientOcclusion(void);

		//--virtual
		virtual bool OnVisitSceneNode(HugeQuadTreeSG *pSG,SceneNode *pNode,int index,int totalNum,DWORD param);

		//--
		bool Start(const TCHAR *szSGPath,BOOL bSmallAO);
		void DeleteTempFiles(const TCHAR *szPath);

		static tstring MakeAOTempMapName(const TCHAR *szMapName);
		static tstring MakeAOMapName(const TCHAR *szMapName);

		struct tagThreadData
		{
			IDirect3DTexture9* pTexture;
			tstring strName;
		};
	private:
		class Member;
		Member *m_p;

		IUICallBack	*m_pUI;
		TSafeList<tagThreadData*>	m_listRT;
	};
}//namespace Cool3D