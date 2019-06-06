#pragma once
#include "..\SceneGraph\scenenode.h"
#include ".\ResGrass.h"

namespace Cool3D
{
	/**	\class GrassPatch 
		\brief 草块
	*/
	class GrassPatch : public SceneNode
	{
	public:
		GrassPatch(void);
		virtual ~GrassPatch(void);

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
		/**	\todo res创建好之后创建mtl
		*/
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

		//--
		const TCHAR* GetResName() const;
		ResGrass* GetRes(){ return m_pRes;}

	protected:
		void Destroy();

	protected:
		ResGrass			*m_pRes;
		vector<IMaterial*>	m_mtls;
		DWORD				m_lastForceUpdateTime;

		DECL_RTTI_DYNC(GrassPatch);
	};
}//namespace Cool3D