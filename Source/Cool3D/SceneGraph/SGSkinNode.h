#pragma once
#include "..\Cool3D.h"
#include "scenenode.h"
#include "..\Public\VertexDefine.h"

namespace Cool3D
{
	class RSkin;
	class IMaterial;
	class AniSkin_SW;
	class IVertBuffer;
	class ResSkin;
	struct tagMtlDesc;
	/**	\class SGSkinNode
		\brief skin在scene graph中的node,一般用做avatar node的子节点
		\remarks
		\par 软件计算:在Update时将Skin的VB计算出来并更新之
	*/
	class Cool3D_API SGSkinNode : public SceneNode
	{
	public:
		SGSkinNode(void);
		virtual ~SGSkinNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		/**只能是.FSKIN文件*/
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumMtl() const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		virtual TResult RayCollide(const Ray& ray);
		/**	替换指定的Mtl,pMtl生存期归this管理*/
		void ReplaceMtl(int mtlID,IMaterial *pMtl);
		
		//--
		const ResSkin* GetResSkin() const	{	return m_pRes;}
		EVertType GetVertType() const;
		bool IsCreated()					{	return m_pAniSkin!=NULL && m_pVB!=NULL; }
		IVertBuffer *GetVB()				{	return m_pVB;  }
		int GetLod()						{	return m_nLod; }

		//! 可以暂时隐藏此节点
		void Enable(bool bEnable)			{	m_bEnable=bEnable;}
		//! 设置物体半透明
		void SetTransparent(float alpha);
		float GetTransparent() const { return m_fTransparent; }
		//! 设置相乘颜色
		void SetMulColor(const float r, const float g, const float b);

		//--BoneMatrixArray
		const tagBoneMatrixArray* const GetBoneMatrixArray( const RSkin* const pSkin );
		//--获取材质数据
		const tagMtlDesc& GetMtlDesc( const int i ) const;
	protected:
		void UpdateVB(int lod);
		void CalcBoneMatrix(int lod);
	protected:
		bool				m_bEnable;
		ResSkin				*m_pRes;
		AniSkin_SW			*m_pAniSkin;
		/** 每个SGSkinNode拥有自身的dynamic vb,而不使用RenderSys的vb pool,
		是为了在渲染多个sub mesh时不用重复从vb pool中分配并且拷贝vb*/
		IVertBuffer			*m_pVB;
		vector<IMaterial*>	m_mtls;
		int					m_nLod;
		float				m_fTransparent;

		vector<tagBoneMatrixArray>  m_vecBoneMatrixArray;

		DECL_RTTI_DYNC(SGSkinNode);
	};
}//namespace Cool3D