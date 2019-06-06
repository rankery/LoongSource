#pragma once
#include "..\F3D.h"

	class IFS;
namespace F3D
{
	class Material;
	struct tagMtlDesc;
	/**	\class MaterialMgr
		\brief 材质管理器,需要根据不同的RenerSys来创建多层次的才智
	*/
	class F3D_API MaterialMgr
	{
	public:
		MaterialMgr(void);
		virtual ~MaterialMgr(void);

		static MaterialMgr *Inst();

		Material *CreateMtl(tagMtlDesc *pDesc);
		void DestroyMtl(Material *pMtl);

	};
}//namespace F3D