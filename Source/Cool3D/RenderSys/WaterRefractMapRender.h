#pragma once
#include "..\Cool3D.h"
#include ".\RenderSys.h"

namespace Cool3D
{
	class TerrainPatch;

	/** \class WaterRefractMapRender
		\brief 水面折射效果
	*/
	class Cool3D_API WaterRefractMapRender
	{
	public:
		static WaterRefractMapRender* Inst();
		void Init();
		void Destroy();

		bool IsHWSurpport();

		void RenderRefractMap( map<DWORD,vector<RenderItem>>& mapRiver, vector<RenderItem>& trnArray, SceneGraph* pSG, const tagViewport& viewPort );
		inline IRenderTexture* GetRefractRTex();
	private:
		WaterRefractMapRender(void);
		virtual ~WaterRefractMapRender(void);
	
		void RenderTerrainPatchFog( TerrainPatch* pPatch );
		void RenderUnderWaterFog( vector<RenderItem>& trnArray, SceneGraph* pSG );
		void RenderRefractMask( map<DWORD,vector<RenderItem>>& mapRiver, SceneGraph* pSG );

		class Member;
		Member* m_p;
	};
}//end namespace Cool3D