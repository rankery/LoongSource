#pragma once
#include "..\ressys\resrenderable.h"
#include "..\ressys\vertstream.h"

namespace Cool3D
{
	class IVertBuffer;
	class TerrainPatchRenderGroupsRes;
	class TerrainEditor;
	/**	\class TerrainRes
		\brief 地形资源
	*/
	class TerrainRes :	public ResRenderable
	{
		struct tagHeader
		{
			TCHAR			magic[8];
			UINT			numVerts;
			DWORD			uv2Offset;
			DWORD			uv4Offset;
			UINT			numLayers;
			DWORD			mtlOffset;
			Vector3			max,min;
			Color4f			diffuse;
			Color4f			ambient;
			Color4f			specular;
			float			power;
		};

		struct tagLayerDesc
		{
			tstring			szTileTex;
			BYTE			uvSetIndex;

			tagLayerDesc()	
			{
				uvSetIndex=0;
			}
			tagLayerDesc(const tagLayerDesc& other)
			{
				szTileTex=other.szTileTex;
				uvSetIndex=other.uvSetIndex;
			}
			const tagLayerDesc& operator = (const tagLayerDesc& other)
			{
				if(this==&other)
					return *this;
				szTileTex=other.szTileTex;
				uvSetIndex=other.uvSetIndex;
				return *this;
			}
			~tagLayerDesc()
			{
			}
		};
	public:
		TerrainRes(const TCHAR* szName);
		virtual ~TerrainRes(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const	{	return 1;}
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const{}

		//--
		IVertBuffer* GetTexCoord2VB() { return m_pTexCoord2VB;}
		IVertBuffer* GetTexCoord4VB() { return m_pTexCoord4VB;}
		const VertStream& GetTexCoord2VS() { return m_texCoord2VS;}
		const VertStream& GetTexCoord4VS() { return m_texCoord4VS;}
		void CreateRenderGroupsMtls(TerrainPatchRenderGroupsRes* pRes,const TCHAR* szLightMap,vector<IMaterial*>& mtls);

		//--editor support
		static bool EditorSave(const TCHAR* szPath,RECT myRect,TerrainEditor *pEditor);
		static void FillVertStream_T2(VertStream& vertStream);
		static void FillVertStream_T4(VertStream& vertStream,TerrainEditor *pEditor);

		//--
		const Color4f& GetMtlDiffuse() const;
		const Color4f& GetMtlAmbient() const;
		const Color4f& GetMtlSpecular() const;
		float GetMtlPower()	const;

	protected:
		vector<tagLayerDesc>	m_layerDescs;
		IVertBuffer*			m_pTexCoord2VB;
		IVertBuffer*			m_pTexCoord4VB;
		VertStream				m_texCoord2VS;
		VertStream				m_texCoord4VS;
		AABBox					m_box;
		Color4f					m_mtlDiffuse;
		Color4f					m_mtlAmbient;
		Color4f					m_mtlSpecular;
		float					m_mtlPower;
        
		DECL_RTTI(TerrainRes);
	};
}//namespace Cool3D