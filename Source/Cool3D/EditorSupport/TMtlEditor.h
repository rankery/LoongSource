#pragma once
#include "TerrainEditor.h"
#include "..\Image\Image.h"
#include ".\TEditorMtl.h"
#include ".\ResDynaAlphaMap.h"

namespace Cool3D
{
	class TMtlEditor
	{
	public:
		TMtlEditor(void);
		virtual ~TMtlEditor(void);

		bool Create(const TCHAR* szTexFile,const TCHAR* szAlphaMap,BYTE alphaInitVal,const TCHAR* szName,
			int uvSetIndex,const Color4f& diffuse,const Color4f& ambient,const Color4f& specular);
		void Destroy();
		TEditorMtl *GetMtl()			{	return m_pMtl;}
		const TCHAR* GetName() const	{	return m_szName.c_str();}
		const ResDynaAlphaMap* GetAlphaMap() const	{	return m_pMtl->m_pAlphaMap;}

		void BrushAlpha(const tagTEditBrush& brush);
		void SmoothAlpha(const tagTEditBrush& brush);
		void NoiseAlpha(const tagTEditBrush& brush,float noiseFreq);

		void SaveAlphaMapToFile();
		void Blend2FinalImage(Image *pDestImg,bool bForceCopy);
		void CopyAlphaMap2Img(Image* pImage,int w,int h);
		bool IsEmptyAlpha();//alpha是否完全透空,如果完全透空则相当于没有这个layer
		bool IsFullOpacity();//判断是否完全不透明
		void DeleteCacheFile();
	protected:
		void PostEditChange(const tagTEditBrush& brush);
		void UpdateTexture();
		void UpdateAlpha2Img(RECT *pRc=NULL);
	protected:
		tstring			m_szName;
		TEditorMtl*		m_pMtl;
		bool			m_bNeedSave;
	};
}//namespace Cool3D