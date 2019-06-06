#include "StdAfx.h"
#include ".\rbillboards.h"
#include "..\Kernel.h"

namespace Cool3D
{
	RBillboards::RBillboards(void)
	{
	}

	RBillboards::~RBillboards(void)
	{
	}

	void RBillboards::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)
	{
		Matrix4 matView;
		IDevice *pDev=Device();
		pDev->MatPush(EMS_View);
		matView=*(pDev->GetMatrix(EMS_View));

		//--x
		matView._11=1;
		matView._12=0;
		matView._13=0;
		//--y
		matView._21=0;
		matView._22=1;
		matView._23=0;
		//--z
		matView._31=0;
		matView._32=0;
		matView._33=1;

		pDev->MatLoad(EMS_View,&matView);
		pDev->FFlushMatrix(false,true,false,false);
		DynamicMesh::Draw(pSys,pSGNode,nLod,pItem,pSG);
		pDev->MatPop(EMS_View);
	}
}//namespace Cool3D