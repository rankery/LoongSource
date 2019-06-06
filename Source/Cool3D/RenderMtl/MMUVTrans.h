#include "..\Math\Math.h"
#pragma once
#include "imtlmodifier.h"

namespace Cool3D
{
	/**	\class MMUVTrans 
		\brief ²Ù×Ýtexture matrix
	*/
	class Cool3D_API MMUVTrans : public IMtlModifier
	{
	public:
		MMUVTrans(void);
		virtual ~MMUVTrans(void);

		const Matrix4* GetMatrix() const	{return &m_mat;}
		void SetMatrix( const Matrix4& mat ) { m_mat = mat; }
		void SetScale(float uScale,float vScale);
		void SetOffset(float uOffset,float vOffset);
		void SetRotate(float rot);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
	private:
		Matrix4 m_mat;

		DECL_RTTI_DYNC(MMUVTrans);
	};
}//namespace Cool3D