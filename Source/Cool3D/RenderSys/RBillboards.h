#pragma once
#include "DynamicMesh.h"

namespace Cool3D
{
	/**	\class RBillboards 
		\brief
	*/
	class RBillboards :	public DynamicMesh
	{
	public:
		RBillboards(void);
		virtual ~RBillboards(void);

		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);

	protected:
		
	};
}//namespace Cool3D