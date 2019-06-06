#pragma once
#include "imaterial.h"

namespace F3D
{
	class CSpeedTreeRT;
	class MConstColor;
	class MBitmap;
	/**	\class MSpeedTree 
		\brief speed tree系统的材质包装
		\todo 一个Speedtree材质包括两个材质对象,一个是树干材质,一个是树叶树枝的材质
	*/
	class MSpeedTree :	public IMaterial
	{
	public:
		MSpeedTree(void);
		~MSpeedTree(void);

		void CreateFromTree(const CSpeedTreeRT* pTree);

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
	public:
		MConstColor	*m_pColor;
		MBitmap		*m_pDiffuse;
		MBitmap		*m_pShadow;
		MBitmap		*m_pBump;
	};
}//namespace F3D