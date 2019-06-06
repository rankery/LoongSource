#pragma once
#include "rendernode.h"
#include "..\Device\IDevice.h"

namespace Cool3D
{
	class VertStream;
	class IVertBuffer;
	/**	\class RStaticMesh 
		\brief 使用Vert buffer但是不使用index的几何体
	*/
	class RStaticMesh :	public RenderNode
	{
	public:
		RStaticMesh(void);
		virtual ~RStaticMesh(void);

		//--RenderNode
		virtual bool SupportLod();
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;
		//--
		void Create(int mtlID,EDrawType drawType,VertStream *pVertStream);
		void Destroy();

	protected:
		EVertType			m_vertType;
		EDrawType			m_drawType;
		int					m_mtlID;
		int					m_numPrimitive;

		AABBox				m_box;

		IVertBuffer			*m_pVB;

		DECL_RTTI_DYNC(RStaticMesh);
	};
}//namespace Cool3D