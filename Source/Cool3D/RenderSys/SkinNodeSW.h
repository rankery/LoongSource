#pragma once
#include "skinnode.h"
#include "..\ResSys\VertStream.h"

namespace F3D
{
	/**	\class SkinNodeSW
		\brief 使用软件计算Skin的skin node
	*/
	class SkinNodeSW : public SkinNode
	{
	public:
		SkinNodeSW(void);
		virtual ~SkinNodeSW(void);

		//--render node
		virtual bool SupportLod();	//是否支持LOD,默认为不支持
		virtual void Draw(int nLod =0);	//支持Lod的
		virtual bool IsTransparent() const;
		virtual DWORD GetMtlTrait() const;
		virtual const AABBox& GetAABBox() const;

		//--skin node
		virtual void Create(ResSkin *pRes);

	private:
		//VertStream	m_
		int			m_numLod;
		AABBox		m_box;
	};
}//namespace F3D