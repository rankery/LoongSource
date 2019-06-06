#pragma once
#include "..\Cool3D.h"
#include "resrenderable.h"
#define VFS_READ
#include "..\Public\SkinBoneDefine.h"
#include "..\Public\SkinSubMesh.h"
#include "..\Math\Ray.h"
#include "VertStream.h"

namespace Cool3D
{
	/**	\class ResSkin 
		\brief 人物的Skin(蒙皮)数据
		\remarks 一个人物可能有多个Skin对象加一个Skeleton组成
		\see ResSkeleton
	*/
	class Cool3D_API ResSkin : public ResRenderable
	{
	public:
		ResSkin(const TCHAR* szName);
		virtual ~ResSkin(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const;
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const;

		//--
		const vector<SkinVert>&	GetSkinVerts() const;
		const vector<tagBoneOffsetMatrix>& GetBoneOffsetMatrix(int subMeshID) const;
		const vector<LodSkinSubMesh*>& GetLodSubMesh() const;
		TResult RayCollide(const Ray& ray,const Matrix4* pObj2World,const VertStream* pVS);

		//--获取材质数据
		const tagMtlDesc& GetMtlDesc( const int i ) const;

	private:
		class Member;
		Member*	m_p;
		DECL_RTTI(ResSkin);
	};
}//namespace Cool3D