#pragma once
#include "imaterial.h"

	class CSpeedTreeRT;
namespace F3D
{
	class ResTexture;
	/**	\class TreeMtl 
		\brief SpeedTree的材质的包装
		\todo 使用MSpeedTree代替我
	*/
	class TreeMtl :	public IMaterial
	{
	public:
		TreeMtl(void);
		virtual ~TreeMtl(void);

		void CreateForTree(CSpeedTreeRT *pTree);
		void ActiveBranch();
		void ActiveComposite();

	private:
		ResTexture	*m_texBranch;
		ResTexture	*m_texShadow;
		ResTexture	*m_texComposite;

		DECL_RTTI_DYNC(TreeMtl);
	};
}//namespace F3D