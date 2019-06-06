#pragma once
#include "..\Cool3D.h"
#include "irendermtl.h"
#include "..\ResSys\ResTexture.h"

namespace Cool3D
{
	class ResTexture;

	/**	\class MBitmap
		\brief Î»Í¼²ÄÖÊ
	*/
	class Cool3D_API MBitmap :	public IRenderMtl
	{
	public:
		MBitmap(void);
		virtual ~MBitmap(void);

		virtual void SetTexture(UINT index,const TCHAR* szPath,UINT numLevels=0);
		void SetMipFilter(ETextrueFilter filter)	{ m_mipFilter=filter; }
		virtual bool IsResourceReady() const
		{
			return m_pBitmap!=NULL && m_pBitmap->IsCreated();
		}

		virtual DWORD GetRenderTrait1() const
		{
			return IMaterial::GetRenderTrait1();
		}
		virtual DWORD GetRenderTrait2() const
		{
			return DWORD(m_pBitmap);
		}

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		void SetCube(bool bIsCube) { m_bIsCube=bIsCube; }
	public:
		ResTexture		*m_pBitmap;
		ETextrueFilter	m_magFilter;
		ETextrueFilter	m_minFilter;
		ETextrueFilter	m_mipFilter;
		ETextureAddress	m_addressU,
						m_addressV,
						m_addressW;
		UINT			m_numLevels;
	private:
		bool			m_bIsCube;

		DECL_RTTI_DYNC(MBitmap);
	};
}//namespace Cool3D