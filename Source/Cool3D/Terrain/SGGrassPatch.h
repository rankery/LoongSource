#pragma once
#include "SGTrnDecoPatch.h"

namespace Cool3D
{
	class TerrainPatch;
	class EditorDecoFile;
	/**	\class SGGrassPatch 
		\brief 处理一个TerrainPatch上的草
		\remarks 必须作为TerrainPatch的子节点使用,因为要从Terrain取得地形高度
	*/
	class SGGrassPatch : public SGTrnDecoPatch
	{	
	public:
		SGGrassPatch(void);
		virtual ~SGGrassPatch(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--SGTrnDecoPatch
		virtual int GetDecoType() const;
		virtual void Build(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect=NULL,bool bEditorMode=false,bool bShadowEffect=false);
		virtual void BuildMesh(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false);
		virtual void DelMesh(Heightmap *pDensityMap,Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false);
		virtual void GetMesh(Heightmap *pTrnHMap,EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,
			RECT *pSubRect,bool bEditorMode,bool bShadowEffect=false);
		virtual void FadeEffect(bool bFadeIn);
		virtual bool GetVertStream(const Heightmap *pTrnHMap,EditorDecoFile& decoFile,
			const RECT *pSubRect,const tagTDecoLayer& layerDesc,
			VertStream * pVertStream, vector<WORD>& indexArray);

	private:
		void CreateVertsAndIndexes(Heightmap *pDensityMap,const Heightmap *pTrnHMap,
			EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,RECT *pSubRect,
			vector<Vert_PNT>& vertArray,vector<WORD>& indexArray);
		void DelVertsAndIndexes(Heightmap *pDensityMap,const Heightmap *pTrnHMap,
			EditorDecoFile& decoFile,const tagTDecoLayer& layerDesc,RECT *pSubRect,
			vector<Vert_PNT>& vertArray,vector<WORD>& indexArray);
		void GetVertsAndIndexes(const Heightmap *pTrnHMap,EditorDecoFile& decoFile,
			const tagTDecoLayer& layerDesc,RECT *pSubRect,
			vector<Vert_PNT>& vertArray,vector<WORD>& indexArray);
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGGrassPatch);
	};
}//namespace Cool3D