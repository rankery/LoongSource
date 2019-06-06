#include "..\device\IDevice.h"
#pragma once
#include "imaterial.h"

namespace Cool3D
{
	/**	\class MtlCombiner 
		\brief 将几个材质以特定的方式组合起来, 这个可以用来制作一棵材质树结构,其合并方式类似于D3D的Texture Stage
	*/
	class MtlCombiner : public IMaterial
	{
	public:
		MtlCombiner(void);
		virtual ~MtlCombiner(void);

		void SetOperator(ETextureOp colorOp,ETextureOp alphaOp)	{	m_colorOp=colorOp; m_alphaOp=alphaOp;}
		virtual bool IsResourceReady() const;

		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			if(m_pMtl1)
				return m_pMtl1->GetRenderTrait2();
			else
				return IMaterial::GetRenderTrait2();
		}
		
		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
	public:
		IMaterial	*m_pMtl1;
		IMaterial	*m_pMtl2;
		ETextureOp	m_colorOp,
					m_alphaOp;

		DECL_RTTI_DYNC(MtlCombiner);
	};
}//namespace Cool3D