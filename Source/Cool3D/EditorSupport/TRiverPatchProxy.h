#pragma once
#include ".\TerrainEditor.h"

namespace Cool3D
{
	class TRiverEditor;
	class Frustum;
	class SceneNode;
	class TRiverRealPatch;

	/** \class TRiverPatchProxy
		\brief TRiverRealPatch的代理
	*/
	class TRiverPatchProxy
	{
	public:
		TRiverPatchProxy(int height);
		virtual ~TRiverPatchProxy();

		void Create(RECT rc,UINT riverID,UINT patchID);

		//--根据块的中心点是否在编辑区域内为条件加载realPatch
		void Update(TRiverEditor *pEditor,Heightmap *pHMap,const RECT* pDrawRect);
		void Draw(const Frustum& frustum);

		void BrushMtl(const tagTEditBrush& brush);
		void SmoothMtl(const tagTEditBrush& brush);
		void NoiseMtl(const tagTEditBrush& brush,float noiseFreq);

		static void SetCachePath(const TCHAR *szPath);
		void LoadCache(TRiverEditor *pEditor,Heightmap *pHMap);
		void SaveCache();
		void ClearCache();
		void DeleteCache();
		bool Build(TRiverEditor *pEditor,Heightmap *pHMap,const TCHAR* szBasePath,Heightmap* pTerrainHmap,IUICallBack* pUI);

		const RECT& GetRect() const;
		int GetID() const;

		void ApplyMtl(TRiverEditor *pEditor);

		TRiverRealPatch* GetRealPatch(void);
		bool HaveWater(TRiverEditor *pEditor,Heightmap *pHMap,bool bLoad=true);
		/** 生成水面区域bitmap
		*/
		bool BuildWaterBitmap(BitMap& bitmap,TRiverEditor *pEditor,Heightmap *pRvrHMap,IUICallBack* pUI);

	private:	
		void GetRealPatch(TRiverEditor *pEditor,Heightmap *pHMap);
		static tstring s_cachePath;

	protected:
		class Member;
		Member	*m_p;
	};
}//namespace Cool3D
