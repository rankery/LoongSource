#pragma once
#include "..\scenegraph\scenenode.h"
#include "..\EditorSupport\TerrainEditor.h"

namespace Cool3D
{
	class Heightmap;
	class VertStream;
	class EditorDecoFile;
	/**	\class SGTrnDecoPatch 
		\biref Terrain patch×°ÊÎ²ã
	*/
	class SGTrnDecoPatch : public SceneNode
	{
	public:
		SGTrnDecoPatch(void);
		virtual ~SGTrnDecoPatch(void);

		virtual int GetDecoType() const =0;
		virtual void Build(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect=NULL,bool bEditorMode=false,bool bShadowEffect=false) =0;
		virtual void BuildMesh(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false) =0;
		virtual void DelMesh(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false) =0;
		virtual void GetMesh(Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false) =0;
		virtual void FadeEffect(bool bFadeIn) =0;
		virtual bool GetVertStream(const Heightmap *pTrnHMap,EditorDecoFile& decoFile,
			const RECT *pSubRect,const tagTDecoLayer& layerDesc,
			VertStream * pVertStream, vector<WORD>& indexArray)=0;

	private:
		DECL_RTTI(SGTrnDecoPatch);
	};
}//namespace Cool3D