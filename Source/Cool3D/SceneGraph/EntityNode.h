#pragma once
#include "..\Cool3D.h"
#include "scenenode.h"
#include "..\Math\Math.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\RTTIObj.h"

namespace Cool3D
{
	class DynamicMesh;
	class ResRenderable;
	class AnimationCtrl;
	class VertStream;
	/**	\class EntityNode
		\brief SceneGraph中Actor等活动的实体
		\par
		它的渲染数据可以是Actor或者ResRenderable
		\todo 用户定义的AniCtrl如何支持???
		
	*/
	class Cool3D_API EntityNode : public SceneNode
	{
	public:
		EntityNode(void);
		virtual ~EntityNode(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		//-- SceneNode
		virtual bool IsInFrustum(const Frustum *pFrumstum) const;


		void EnableMipMap(bool bEnable)	{	m_bMipMap=bEnable;}
		/**	(从磁盘)创建新的资源
			\remarks 先从已知的文件扩展名匹配,如果找到则从ResMgr中获得,否则将从UserResMgr中获得
		*/
		virtual void LoadRes(const TCHAR* szName);
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const Matrix4& GetWorldMat() const;
		virtual void SetWorldMat(const Matrix4 *pMat);
		virtual void GetBox(AABBox& out) const;
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		virtual UINT GetNumMtl() const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual TResult RayCollide(const Ray& ray);

		/**	替换指定的Mtl,pMtl生存期归this管理*/
		void ReplaceMtl(int mtlID,IMaterial *pMtl);
		
		//--
		virtual ResRenderable* GetResObj() const;
		virtual AnimationCtrl* GetAniCtrl() const;
		const AABBox& GetAABBox() const;

		TResult RayCollideForDynaNode(const Ray& ray);

		//! 可以暂时隐藏此节点
		void Hide(bool bHide){ m_bHide=bHide; }
		bool IsHide()		 { return m_bHide;}

		//! 设置物体半透明
		void SetTransparent(float alpha);
		float GetTransparent() const { return m_fTransparent; }
		//! 设置相乘颜色
		void SetMulColor(const float r, const float g, const float b);
		//! 设置阴影效果
		void SetShadowEffect(bool bInShadow);
		//! 设置漫反射材质增强系数
		void SetMtlDiffuseFactor(float factor);

		//--editor support
		void GetVertexBuffer(VertStream* vb);
		void GetIndexBuffer(vector<WORD>& ib);

		//--用于重新生成nav，只适用于fscn
		void GetPosBuffer(VertStream* vb);

		//-- 哪些场景模型不进行碰撞检测
		static void LoadCDIgnoreList(const TCHAR* szFileName);

		//--
		void BuildDecalMesh( const AABBox& box, DynamicMesh *pMesh, const Vector3* pNormal );

		virtual bool IsLoaded();

		void UpdateBoundingBox();
		const AABBox& GetLocalBox() const;
	private:
		static set<tstring>	m_cdIgnoreList;

	private:
		bool m_bHide;
		float m_fTransparent;

		bool m_bMipMap;//是否使用mip map(例如场景中的动画可以使用mip map,而怪物则不希望使用mip map)

		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(EntityNode);
	};
}//namespace Cool3D