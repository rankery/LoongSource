#pragma once
#include "..\rendersys\rendernode.h"

namespace Cool3D
{
	class TerrainPatchRes;
	class TerrainPatch;
	/**	\class TerrainPatchRender 
		\brief 地形块渲染器
	*/
	class TerrainPatchRender :	public RenderNode
	{
	public:
		enum ERenderMode
		{
			ERM_MultiLayers,	//多纹理
			ERM_PreCombine,		//预混合贴图
			ERM_TwoLayers,		//双层贴图
			ERM_ReflectMap,		//水面反射效果
			ERM_WithOutMtl,     //不使用任何材质
			ERM_Editor,			//编辑器模式
		};
	public:
		TerrainPatchRender();
		virtual ~TerrainPatchRender(void);

		//--RenderNode
		virtual bool SupportLod()	{ return true; }
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);	
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const;
		virtual const AABBox& GetAABBox() const;

		//--
		void SetTerrainPatchRes(TerrainPatchRes* pRes) { m_pTerrainPatchRes=pRes;}

		static ERenderMode m_renderMode;//渲染模式
	private:
		TerrainPatchRes* m_pTerrainPatchRes;
	private:
		void GetAroundLod(int& myLod,int& leftLod,int& topLod,int& rightLod,int& bottomLod);
		bool RenderMultiLayers(RenderSys *pSys,TerrainPatch *pSGNode);
		bool RenderTwoLayers(RenderSys *pSys,TerrainPatch *pSGNode);
		bool RenderPreCombine(RenderSys *pSys,TerrainPatch *pSGNode);
		bool RenderEditor(RenderSys *pSys,TerrainPatch *pSGNode);
		bool RenderReflectMap(RenderSys *pSys,TerrainPatch *pSGNode);
		bool RenderWithOutMtl(RenderSys *pSys,TerrainPatch *pSGNode);

		DECL_RTTI_DYNC(TerrainPatchRender);
	};
}//namespace Cool3D