#pragma once
#include "..\ressys\resrenderable.h"
#include "..\EditorSupport\TerrainEditor.h"

namespace Cool3D
{
	/**	\class ResGrass 
		\brief ²Ý×ÊÔ´
	*/
	class ResGrass : public ResRenderable
	{
		struct tagGrassHeader
		{
			BYTE		magic[7];
			TCHAR		szTexPath[256];
			BYTE		alphaRef;
			EVertType	vertType;
			UINT		numVerts;
			DWORD		vertOffset;
		};
	public:
		ResGrass(const TCHAR* szName);
		virtual ~ResGrass(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);
		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const;
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const;

		//--editor support
		static bool EditorSave(const TCHAR* szPath,const RECT& rect,TerrainEditor *pEditor);
		static bool ApplyLightMap(Image* pLightMap,TCHAR* szDecoFile,RECT& rcTPatch,TerrainEditor *pEditor);

	protected:
		void Destroy();
	private:
		class Member;
		Member* m_p;

		DECL_RTTI(ResGrass);
	};
}//namespace Cool3D