#pragma once
#include "sgeffectnode.h"

using namespace Cool3D;

namespace Cool3D
{
	/**	\class SGAttachableEffectNode
		\brief 将一个特效节点绑定到动画的tagNode上
	*/
	class Cool3D_API SGAttachableEffectNode : public SGEffectNode
	{
	public:
		SGAttachableEffectNode(void);
		virtual ~SGAttachableEffectNode(void);

		virtual void Update(float deltaTime,SceneGraph *pSG);
		void BindToTagNode(const TCHAR* szTagName);
		bool UnbindToTagNode(void);
		tstring GetTagNodeName() { return m_szTagName; }
	protected:
		void UpdateWorldMatrix();
	protected:
		int			m_tagIndex;//绑定到哪个tag ndoe
		Matrix4		m_finalToWorld;
		tstring		m_szTagName;

		DECL_RTTI_DYNC(SGAttachableEffectNode);
	};
}//end of namespace
