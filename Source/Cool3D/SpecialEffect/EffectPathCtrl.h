#pragma once
#include "effectbase.h"

namespace Cool3D
{
	class Cool3D_API EffectPathCtrl : public EffectBase
	{
	public:
		EffectPathCtrl(void);
		virtual ~EffectPathCtrl(void);

		//--effect base
		virtual int GetNumRenderNodes()	{	return 1;}
		virtual RenderNode* GetRenderNode(int i)	{	return NULL;}

		virtual const Vector3& GetCurPos() const =0;
		virtual const Vector3& GetCurVel() const =0;
		virtual const Vector3& GetCurRot() const =0;
		virtual void SetGroupID(int id)	{}

	protected:
		DECL_RTTI(EffectPathCtrl);
	};
}//namespace Cool3D