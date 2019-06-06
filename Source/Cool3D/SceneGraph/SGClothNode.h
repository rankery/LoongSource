#pragma once
#include "..\Cool3D.h"
#include "SceneNode.h"
#include "..\Public\VertexDefine.h"

namespace Cool3D
{
	class RSkin;
	class IMaterial;
	class AniCloth_SW;
	class IVertBuffer;
	class ResCloth;
	/**	\class SGClothNode
		\brief cloth在scene graph中的node,一般用做avatar node的子节点
		\remarks
		\par 软件计算:在Update时将Cloth的VB计算出来并更新之
	*/
	class Cool3D_API SGClothNode : public SceneNode
	{
	public:
		SGClothNode(void);
		virtual ~SGClothNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//--SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat)	{}
		virtual void GetBox(AABBox& out) const;
		/**只能是.FCLOTH文件*/
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumMtl() const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		virtual TResult RayCollide(const Ray& ray);
		/**	替换指定的Mtl,pMtl生存期归this管理*/
		void ReplaceMtl(int mtlID,IMaterial *pMtl);

		//--
		const ResCloth* GetResCloth() const	{	return m_pRes;}
		EVertType GetVertType() const;
		bool IsCreated()					{	return m_pAniCloth!=NULL && m_pVB!=NULL; }
		IVertBuffer *GetVB()				{	return m_pVB;  }
		int GetLod()						{	return m_nLod; }

		//! 可以暂时隐藏此节点
		void Enable(bool bEnable)			{	m_bEnable=bEnable;}
		//! 设置物体半透明
		void BeginTransparent();
		void SetTransparent(float alpha);
		void EndTransparent();

		//--BoneMatrixArray
		const tagBoneMatrixArray* const GetBoneMatrixArray( const RSkin* const pSkin );

	protected:
		void UpdateSkinVB(int lod);
		void UpdateClothVB(float deltaTime);
		void CalcBoneMatrix(int lod);

	protected:
		bool				m_bEnable;
		ResCloth			*m_pRes;
		AniCloth_SW			*m_pAniCloth;
		/** 每个SGClothNode拥有自身的dynamic vb,而不使用RenderSys的vb pool,
		是为了在渲染多个sub mesh时不用重复从vb pool中分配并且拷贝vb*/
		IVertBuffer			*m_pVB;
		vector<IMaterial*>	m_mtls;
		int					m_nLod;

		vector<tagBoneMatrixArray>  m_vecBoneMatrixArray;

		DECL_RTTI_DYNC(SGClothNode);
	};
}//namespace Cool3D