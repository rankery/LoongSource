#pragma once

namespace Cool3D
{
	class TerrainPatch;
	class SceneGraph;
	/** \class TerrainMultiLayersRender
		\brief 地形多纹理渲染器
		\remarks
	*/
	class TerrainMultiLayersRender
	{
	public:
		void Create();
		void Destroy();

		bool IsHWSurpport();

		void Begin();
		bool AttachPatch(TerrainPatch* pPatch);
		void End(SceneGraph* pSG);

		bool IsCreated() { return m_bCreated; }
		vector<TerrainPatch*>& GetTrnArray();

		static TerrainMultiLayersRender* Inst();
	private:
		TerrainMultiLayersRender(void);
		virtual ~TerrainMultiLayersRender(void);

		bool m_bCreated;
		
		class Member;
		Member* m_p;
	};
}//namespace Cool3D