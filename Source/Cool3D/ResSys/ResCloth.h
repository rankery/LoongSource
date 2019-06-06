#pragma once
#include "..\Cool3D.h"
#include "Resrenderable.h"
#define VFS_READ
#include "..\Public\SkinBoneDefine.h"
#include "..\Public\SkinSubMesh.h"
#include "..\Math\Ray.h"
#include "VertStream.h"

namespace Cool3D
{
	/**	\class ResCloth 
		\brief 人物的布料数据
	*/
	class Cool3D_API ResCloth : public ResRenderable
	{
	public:
		ResCloth(const TCHAR* szName);
		virtual ~ResCloth(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT			GetNumRenderNode()		const;
		virtual RenderNode*		GetRenderNode(UINT i)	const;
		virtual const AABBox*	GetAABBox()				const;
		virtual int				GetNumMtl()				const;
		virtual void			CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const;

		//--
		const vector<SkinVert>&			GetClothVerts() const;
		const vector<tagBoneOffsetMatrix>&	GetBoneOffsetMatrix(int subMeshID) const;
		const vector<LodClothSubMesh*>&		GetLodSubMesh() const;
		TResult								RayCollide(const Ray& ray,const Matrix4* pObj2World,const VertStream* pVS);

		const vector<DWORD>&	GetClothIndex() const;
		const vector<DWORD>&	GetTagIndex()	const;
		vector<tagSpring>&		GetSprings();
		vector<tagBlock>&		GetBlocks();

		UINT					GetNumSkinRenderNode()		const;
		RenderNode*				GetSkinRenderNode(UINT i)	const;

	private:
		class Member;
		Member*	m_p;
		DECL_RTTI(ResCloth);
	};
}//namespace Cool3D