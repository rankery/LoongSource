#pragma once
#include "..\ResSys\resbase.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	class Heightmap;
	class Image;
	class IDeviceTex;
	/**	\class ResDynaAlphaMap 
		\brief ¿É±à¼­µÄalphamap
	*/
	class Cool3D_API ResDynaAlphaMap : public ResBase
	{
	public:
		ResDynaAlphaMap(const TCHAR* szName);
		virtual ~ResDynaAlphaMap(void);

		virtual UINT CreateContent(DWORD param);

		Heightmap*  GetHeightmap() const;
		IDeviceTex* GetDeviceTex() const;

		void UpdateTexture(const RECT *pRc=NULL);
		void SaveToFile();
		void SaveToImage(const TCHAR* szPath,UINT size);

	protected:
		void Update2Image(const RECT *pRc=NULL);
		void Update2Texture();

	protected:
		Heightmap	*m_pHeightmap;
		Image		*m_pImage;
		IDeviceTex  *m_pTex;

		DECL_RTTI(ResDynaAlphaMap);
	};
}//namespace Cool3D