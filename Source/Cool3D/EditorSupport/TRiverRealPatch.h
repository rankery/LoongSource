#pragma once
#include ".\TerrainEditor.h"

namespace Cool3D
{
	class TRiverEditor;
	class IUICallBack;
	class AABBox;
	class BitMap;

	/** \class TRiverRealPatch
		\brief 河流中的一个河流块
	*/
	class TRiverRealPatch
	{
	public:
		enum ELoadState
		{
			LS_INVALID		=-1,
			LS_NOTLOAD		=0,			//未加载
			LS_LOADING,					//加载中
			LS_LOADED,					//已加载
		};
	public:
		TRiverRealPatch(UINT riverID,int height);
		virtual ~TRiverRealPatch();

		bool CreateRenderBuffer(TRiverEditor *pEditor,Heightmap *pHMap,const RECT *pRect);
		bool CreateMtl(const TCHAR *szTexFile,const TCHAR *szMapFile,BYTE initVal,float alpha);

		//--渲染
		void Draw();

		//--编辑
		void UpdateVB(Heightmap *pHMap);
		void BrushMtl(const tagTEditBrush& brush);
		void SmoothMtl(const tagTEditBrush& brush);
		void NoiseMtl(const tagTEditBrush& brush,float noiseFreq);

		//--存盘
		bool SaveToFile() const;
		//--读盘
		void LoadFromFile(TRiverEditor *pEditor,const TCHAR *szFileName);

		bool CacheIsLoaded() const;
		void ClearCache();
		bool Build(TRiverEditor *pEditor,const TCHAR* szSavePath,int id,Heightmap* pTerrainHmap,IUICallBack* pUI);
		void ApplyMtl(TRiverEditor *pEditor);
		void DeleteCacheFile();

		const AABBox& GetWorldBox() const;

		bool HaveWater();
		/** 生成水面区域bitmap
		*/
		bool BuildWaterBitmap(BitMap& bitmap,RECT rc,IUICallBack* pUI);

	private:
		void DrawPass(bool useMtl,int numFace);
		void FillVertStream(Heightmap* pHMap);
		bool ConvLocalMtlBrush(tagTEditBrush& brush);

	protected:
		class Member;
		Member	*m_p;
	};
}//namespace Cool3D