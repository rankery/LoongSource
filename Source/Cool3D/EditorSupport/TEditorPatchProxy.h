#pragma once
#include "TerrainEditor.h"

namespace Cool3D
{
	class TerrainEditor;
	class Heightmap;
	class TEditorPatch;
	class CameraBase;
	class TerrainPatchMgr;
	class IUICallBack;
	class Frustum;
	/**	\class TEditorPatchProxy
		\brief TEditorPatch的代理
		\see TEditorPatch
	*/
	class TEditorPatchProxy
	{
	public:
		TEditorPatchProxy(void);
		virtual ~TEditorPatchProxy(void);

		void Create(RECT rc,int id);

		/**	自动计算是否创建真正的内部数据
			\remarks 视点位于Patch内,或者Patch与视点所在Patch相邻则创建real patch,
			否则save real patch to disk,then free it
		*/
		void Update(TerrainEditor *pEditor,Heightmap *pHMap,const RECT& drawRect,const RECT& editRect);
		void UpdateVB(TerrainEditor* pEditor,Heightmap* pHMap,const RECT* pRc);
		void Draw(const Frustum& frustum);
		void DrawPatchEdge(TerrainEditor *pEditor);
		void DrawSubPatchEdge(TerrainEditor *pEditor);
		void AddLayerMtl(TerrainEditor *pEditor,int layerIndex,BYTE initAlpha);
		/**	\todo Remove cache???
		*/
		void RemoveLayer(const TCHAR* szLayerName);
		void ClearLayers();

		void MoveLayer(const TCHAR* szLayerName,int offset);
		void BrushLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush);
		void SmoothLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush);
		void NoiseLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush,float noiseFreq);

		void SaveCache();
		bool BuildRunTimeData(TerrainEditor *pEditor,Heightmap *pHMap,const TCHAR* szBasepath,IUICallBack* pUI);

		static void SetCachePath(const TCHAR* szPath);

		/**	\返回rect
		*/
		void GetMapRect(RECT& rect)
		{
			rect=m_mapRect;
		}
		/** \返回没有重叠的rect
		*/
		void GetMapRectNoOverlap(RECT& rect);
		/**	\取得图层数
		*/
		void GetLayers(POINT ptMap,TerrainEditor *pEditor,Heightmap *pHMap,vector<int>& layers);
		int GetID(){ return m_id;}
		/** 将Lightmap应用到预混合贴图
		*/
		bool ApplyLightMapToCombineImage(const TCHAR* szLightMapPath,const TCHAR* szTrnPath);
		/** 将Lightmap应用到地表装饰层
		*/
		bool ApplyLightMapToDecoLayer(const TCHAR* szLightMapPath,const TCHAR* szTrnPath,TerrainEditor* pEditor);
		TEditorPatch *GetPatch() { return m_pRealPatch; }

		/** 生成导航高度图
		*/
		void BuildNavHeightmap(Heightmap* pHMap,Heightmap& nav);

		/**	生成地表类型图
		*/
		bool BuildLayerTypeMap(TerrainEditor *pEditor,Heightmap *pHMap,Heightmap* pLayerType,IUICallBack* pUI);

		void SetMtlDiffuse(const Color4f& color);
		void SetMtlAmbient(const Color4f& color);
		void SetMtlSpecular(const Color4f& color);
		void SetMtlPower(float power);

	private:
		void GetRealPatch(TerrainEditor *pEditor,Heightmap *pHMap);
		bool GetMeshHeight(VertStream& vertStream,Vector3& pos);
		static tstring s_cachePath;
	private:
		RECT			m_mapRect;		//本Patch在整体height map中占用的Rect
		POINT			m_center;
		int				m_id;
		TEditorPatch	*m_pRealPatch;	//真正的数据
	};
}//namespace Cool3D