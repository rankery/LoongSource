#pragma once
#include "rendernode.h"

	class CSpeedTreeRT;
namespace F3D
{
	class ResTree;
	class CameraBase;
	/**	\class TreeNode 
		\biref Tree渲染所需的资源
		\remarks 必须为多个ResTree所共享
	*/
	class TreeNode : public RenderNode
	{
	public:
		TreeNode(void);
		virtual ~TreeNode(void);

		//--RenderNode
		virtual bool SupportLod()	{ return true; }
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod =0);
		virtual ETransparentType GetTransparentType(const SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		//--
		void Create(CSpeedTreeRT *pTree,const char* szOwnerName);
		void UpdateBuffers();
		void BeginBranch();
		void DrawBranch(int lod);
		void EndBranch();

		bool BeginFrond();
		void DrawFrond(int lod);
		void EndFrond();

		void DrawLeafs(int lod);
		void EndLeafsTreeType();

	private:
		class Member;
		Member *m_p;

		DECL_RTTI_DYNC(TreeNode);
	};
}//namespace F3D