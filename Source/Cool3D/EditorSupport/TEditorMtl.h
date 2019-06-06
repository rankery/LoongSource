#pragma once
#include "..\rendermtl\imaterial.h"
#include "..\RenderMtl\MConstColor.h"

namespace Cool3D
{
	class MBitmap;
	class ResDynaAlphaMap;
	/**	\class TEditorMtl 
		\brief Terrain Editor所使用的材质
	*/
	class TEditorMtl : public IMaterial
	{
	public:
		TEditorMtl(void);
		virtual ~TEditorMtl(void);

		void LoadBaseTex(const TCHAR* szBaseTex);
		void LoadAlphaMap(const TCHAR* szPath,int size,int initVal);
		void DeleteCacheFile();
				
		virtual bool IsResourceReady() const;
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		MBitmap			*m_pTileTex;	//图素贴图
		ResDynaAlphaMap *m_pAlphaMap;	//透明贴图
		MConstColor		m_color;
		int				m_uvSetIndex;

		DECL_RTTI_DYNC(TEditorMtl);
	};
}//namespace Cool3D