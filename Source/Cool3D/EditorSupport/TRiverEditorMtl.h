#pragma once
#include "..\RenderMtl\IMaterial.h"
#include ".\TerrainEditor.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MBitmap.h"

namespace Cool3D
{
	class MBitmap;
	class ResDynaAlphaMap;

	/**	\class TRiverEditorMtl
		\brief 河流材质
	*/
	class TRiverEditorMtl : public IMaterial
	{
	public:
		TRiverEditorMtl();
		virtual ~TRiverEditorMtl();

		bool Create(const TCHAR *szTexFile,const TCHAR *szMapFile,BYTE initVal,float alpha);

		//--IMaterial
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		virtual bool IsResourceReady() const;

		//--
		void LoadBaseTex(const TCHAR* szBaseTex);
		void LoadMap(const TCHAR* szPath,int size,int initVal);
		void DeleteCacheFile();
		void SaveMapToFile();
		void SaveMapToImage(const TCHAR *szImagePath);
		bool IsEmptyAlpha();
		const TCHAR* GetName() { return m_name.c_str(); }
		void CopyAlphaMap2Img(Image* pImage,int w,int h);

		//--编辑河流浓度图
		void BrushMap(const tagTEditBrush& brush);
		void SmoothMap(const tagTEditBrush& brush);
		void NoiseMap(const tagTEditBrush& brush,float noiseFreq);

	protected:
		void PostEditChange(const tagTEditBrush& brush);

	public:
		MBitmap				*m_pTex;		//河流贴图
		ResDynaAlphaMap		*m_pMap;		//浓度图
		tstring				m_name;			//缓存的文件名称
		bool				m_bNeedSave;	//
		MConstColor			m_color;
		float				m_alpha;

		DECL_RTTI_DYNC(TRiverEditorMtl);
	};
}//namespace Cool3D