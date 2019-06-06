#pragma once
#include "..\ressys\resrenderable.h"

namespace Cool3D
{
	class IIndexBuffer;
	class TerrainEditor;
	/**	\class TerrainPatchRenderGroupsRes
		\brief 地形块渲染分组资源
	*/
	class TerrainPatchRenderGroupsRes :	public ResRenderable
	{
		struct tagHeader
		{
			TCHAR	magic[4];
			UINT	numRenderGroup;
			DWORD   renderGroupOffset;
			DWORD	indexOffset;
		};
	public:
		struct tagRenderGroup
		{
			vector<int>	layers;
			vector<int> grids;
			tstring		szAlphaMap;
			int		startIndex;
			int		numFace;
			tagRenderGroup()
			{
				memset(this,0,sizeof(tagRenderGroup));
			}
		};
	public:
		TerrainPatchRenderGroupsRes(const TCHAR* szName);
		virtual ~TerrainPatchRenderGroupsRes(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox	*GetAABBox() const;
		virtual int GetNumMtl() const	{	return 1;}
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const{}
		
		//--
		IIndexBuffer* GetIB() const;
		int GetNumRenderGroups() const;
		const tagRenderGroup& GetRenderGroup(int group) const;

		//--editor support
		static bool EditorSave(const TCHAR* szPath,RECT myRect,
			vector<tagRenderGroup>& renderGroups,TerrainEditor *pEditor);

	protected:
		static void BuildIndices(vector<tagRenderGroup>& renderGroups,vector<WORD>& indices);
	protected:
		vector<tagRenderGroup>  m_renderGroups;
		IIndexBuffer*			m_pIB;
        
		DECL_RTTI(TerrainPatchRenderGroupsRes);
	};
}//namespace Cool3D