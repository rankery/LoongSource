#pragma once
#include "resrenderable.h"

namespace F3D
{
	/**	\class ResTree 
		\brief Ê÷
	*/
	class ResTree :	public ResRenderable
	{
	public:
		ResTree(const char* szName);
		virtual ~ResTree(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual void SimpleDraw();
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const	{	return 0; }
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls) const {}

	private:
		class Member;
		Member *m_p;

		DECL_RTTI(ResTree);
	};
}//namespace F3D