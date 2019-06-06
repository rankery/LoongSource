#pragma once
#include "scenenode.h"

namespace Cool3D
{
	class IMaterial;
	/**	\class SceneProjector
		\brief project a texture to scene
	*/
	class Cool3D_API SceneProjector :	public SceneNode
	{
	public:
		SceneProjector(void);
		virtual ~SceneProjector(void);


	protected:
		void AddDefaultMtlMod(IMaterial *pMtl);
	protected:
		bool	m_bTerrain;		//是否影响地形
		bool	m_bStaticMesh;	//是否影响static mesh

		DECL_RTTI(SceneProjector);
	};
}//namespace Cool3D