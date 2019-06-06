#pragma once
#include "..\f3d\specialeffect\sgeffectnode.h"
#include "..\F3D\F3DEngine.h"

using namespace F3D;

namespace F3D
{
//	class F3D_API SGAttachableEffectNode :
	class SGAttachableEffectNode :
		public SGEffectNode
	{
	public:
		SGAttachableEffectNode(void);
		~SGAttachableEffectNode(void);

		virtual void Update(float deltaTime,SceneGraph *pSG);
		void BindToTagNode(const char* szTagName);
		bool UnbindToTagNode(void);
	protected:
		int			m_tagIndex;//°ó¶¨µ½ÄÄ¸ötag ndoe
		Matrix4		m_finalToWorld;
		string		m_szTagName;
		//DECL_RTTI_DYNC(SGAttachableEffectNode);
	};
}//end of namespace
