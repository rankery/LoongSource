#pragma once
#include "sgskynode.h"

namespace Cool3D
{
	class Cool3D_API SGSkyBox : public SGSkyNode
	{
	public:
		SGSkyBox(void);
		virtual ~SGSkyBox(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//--SceneNode接口
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual IMaterial*	GetMtl(int mtlID);
		virtual void Update(float deltaTime,SceneGraph *pSG);
		virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);
		virtual const Matrix4& GetWorldMat() const;

		//--
		void Create(const TCHAR* szTopTex,const TCHAR* szFrontRightTex,const TCHAR* szBackLeftTex,
			const Vector3& drawSize);
		void SetOffset(const Vector3& offset);
		void SetRotate(float yaw,float pitch,float roll);
		void EnableRenderShade( const bool bEnable );
		void SetShadeColor( const DWORD dwColor );
	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(SGSkyBox);
	};
}//namespace Cool3D