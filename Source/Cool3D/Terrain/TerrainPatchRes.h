#pragma once
#include "..\ressys\resrenderable.h"
#include "..\ressys\vertstream.h"

namespace Cool3D
{
	class IVertBuffer;
	class TerrainEditor;
	/**	\class TerrainPatchRes
		\brief 地形块资源
	*/
	class TerrainPatchRes :	public ResRenderable
	{
	public:
		struct tagHeader
		{
			TCHAR	magic[8];
			AABBox	box;
			int		patchX;
			int		patchZ;
			TCHAR	szPrecombineImage[256];
			UINT	numVerts;
			DWORD	vertsOffset;
			UINT    riverID;
			float 	underWaterFogStart;
			float 	underWaterFogEnd;
			Vector3 underWaterFogColor;
		};

		TerrainPatchRes(const TCHAR* szName);
		virtual ~TerrainPatchRes(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const	{	return 1;}
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const{}
		
		//--
		IVertBuffer* GetPosNormalVB()		{ return m_pPosNormalVB;}
		const VertStream& GetPosNormalVS()	{ return m_posNormalVS;  }

		IMaterial* CreateTwoLayersMtl();
		IMaterial* CreatePrecombineMtl();
		//! 编辑器模式不需要lightmap
		IMaterial* CreateEditorMtl(const Color4f& diffuse,const Color4f& ambient,const Color4f& specular,float power);

		int GetPatchX() { return m_patchX;}
		int GetPatchZ()	{ return m_patchZ;}

		const UINT     GetRiverID()            const { return m_riverID; }
		const float    GetUnderWaterFogStart() const { return m_underWaterFogStart; }
		const float    GetUnderWaterFogEnd()   const { return m_underWaterFogEnd; }
		const Vector3& GetUnderWaterFogColor() const { return m_underWaterFogColor; }

		//--editor support
		static bool EditorSave(const TCHAR* szPath,RECT myRect,
			const TCHAR* szPrecombineImage,TerrainEditor *pEditor);

	protected:
		AABBox					m_box;//包裹盒
		tstring					m_szPrecombineImage;//预混合贴图
		IVertBuffer*			m_pPosNormalVB;
		VertStream				m_posNormalVS;
		int						m_patchX;
		int						m_patchZ;

		UINT                    m_riverID;
		float 					m_underWaterFogStart;
		float 					m_underWaterFogEnd;
		Vector3					m_underWaterFogColor;
        
		DECL_RTTI(TerrainPatchRes);
	};
}//namespace Cool3D