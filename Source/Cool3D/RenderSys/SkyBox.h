#pragma once
#include "rendernode.h"
#include "..\ResSys\VertStream.h"
#include "..\Math\CameraBase.h"

namespace F3D
{
	class ResTexture;

	/**	\class SkyBox 
		\brief 简单的天空盒子
		\remarks 因为天空的绘制有特殊的顺序要求,所以从RenderNode派生,而没有放在SceneGraph中去管理
	*/
	class SkyBox : public RenderNode
	{
	public:
		SkyBox(void);
		~SkyBox(void);

		//--RenderNode
		void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod =0);	//不支持Lod的
		ETransparentType GetTransparentType(const SceneNode *pSGNode) const	{return ETrans_None;}
		DWORD GetMtlTrait(SceneNode *pSGNode) const	{return 0;}
		const AABBox& GetAABBox() const;

		//--
		void Create(const char* szTopTex,const char* szWarpTex);
		void SetDrawSize(const Vector3& size);
		EVertType GetVertType()		{	return EVType_PNT; }

	private:
		ResTexture			*m_pTopTex;
		ResTexture			*m_pWarpTex;
		CameraBase			m_camera;
		
		Vert_PNT			m_vert[5*4];//top+warp共5个Quad,每个Quad四个顶点
	};
}//namespace F3D