#pragma once
#include "..\SceneGraph\SceneNode.h"
#include "ResRiver.h"
#include "ResRiverPatch.h"

namespace Cool3D
{
	class IRenderTexture;

	/**	\class SGRiverPatch
		\brief 
	*/
	class Cool3D_API SGRiverPatch : public SceneNode
	{
	public:
		SGRiverPatch();
		virtual ~SGRiverPatch();

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

		//--获得res
		ResRiver *GetRiverRes();
		ResRiverPatch *GetPatchRes();

		void LoadRiverRes(const TCHAR *szName);
		void LoadPatchRes(const TCHAR *szName);
		bool IsEnableEffect();
		void OpenEffect();
		void CloseEffect();
		float GetSurfaceHeight();
		void SetReflectMap(IRenderTexture* pReflectMap);
		void SetRefractMap(IRenderTexture* pRefractMap);
		UINT GetRiverID();

	private:
		class Member;
		Member	*m_p;

		DECL_RTTI_DYNC(SGRiverPatch);
	};
}//namespace Cool3D