#pragma once
#include "..\rendermtl\imaterial.h"
#include "..\rendermtl\MConstColor.h"

namespace Cool3D
{
	class MBitmap;
	class ResPixelShader;
	/**	\class TerrainMtlMultiLayers 
		\brief 地形多层纹理融合材质
	*/
	class TerrainMtlMultiLayers : public IMaterial
	{
	public:
		struct tagLayer
		{
			MBitmap	*m_pTileTex;	//图素贴图
			int		uvSetIndex;
		};
	public:
		TerrainMtlMultiLayers(void);
		virtual ~TerrainMtlMultiLayers(void);

		void BeginLayers(int numLayers);
		void AddLayer(const TCHAR* szDiffuseTexPath,BYTE uvSetIndex);
		void EndLayers();
		void LoadAlphaMap(const TCHAR* szPath);
		void LoadLightMap(const TCHAR* szPath);
		void LoadShaders(int numLayers);
				
		virtual bool IsResourceReady() const;
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

	public:
		int					m_numLayers;
		vector<tagLayer>	m_layers;
		MConstColor			m_color;
		MBitmap				*m_pAlphaMap;
		MBitmap				*m_pLightMap;
		ResPixelShader		*m_pPS;

		DECL_RTTI_DYNC(TerrainMtlMultiLayers);
	};
}//namespace Cool3D