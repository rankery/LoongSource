#pragma once
#include "..\Terrain\ResRiver.h"
#include ".\TerrainEditor.h"

namespace Cool3D
{
	class SceneGraph;
	class IUICallBack;
	class Heightmap;
	class BitMap;
	class TRiverPatchProxy;
	/** \class TRiverEditor
		\brief 河流编辑器核心功能
		\remarks 使用类似地形刷子的方式画出河流
	*/
	class Cool3D_API TRiverEditor
	{
	public:
		enum
		{
			//--编辑
			MAX_RIVER=32,

			//--数据生成
			EPatchSize=128,				//单个Patch有多少*多少个格子
			EPatchAlphaMapSize=128,		//单个Patch透明贴图的大小有多少*多少个像素
			EPatchSideVerts=17,			//单个Patch有多少*多少个顶点
			EPatchVerts=17*17,			//单个Patch有多少个顶点
		};

		static UINT OP_Draw_Dist;

		struct tagRiver
		{
			UINT						id;				//河流ID
			int							height;			//河流的高度
			tagRiverMtlDesc				mtlDesc;		//材质描述信息
			Heightmap					*pHMap;			//河流高度图，用于生成顶点信息
			vector<TRiverPatchProxy*>	patchs;			//块集合
		};

	public:
		TRiverEditor();
		virtual ~TRiverEditor();

		//--存盘
		bool Save() const;
		//--读盘
		bool Load();

		//--渲染
		void Draw(SceneGraph *pSG);

		//--
		int Build(const TCHAR* szBasePath,Heightmap* pTerrainHmap,IUICallBack* pUI);

		//--在地面上画出一个圆圈 param中心点的世界坐标,圆圈的半径(单位:格子)
		void DrawGroundCircle(UINT riverIndex,float centreX,float centreZ,UINT r);
		void DrawGroundQuad(UINT riverIndex,int left,int right,int top,int bottom,DWORD color);

		//--编辑操作，将画刷应用于河流强度图
		void ApplyBrush2River(UINT riverIndex,const tagTEditBrush& brush);
		void SmoothRiver(UINT riverIndex,const tagTEditBrush& brush);
		void NoiseRiver(UINT riverIndex,const tagTEditBrush& brush,float noiseFreq);

		//--外部调用
		bool MousePick(UINT riverIndex,UINT cursorX,UINT cursorY,CameraBase *pCamera,Vector3& out);
		void SetName(const TCHAR *szMapName);
		tagRiverMtlDesc *GetRiverMtlDescByID(UINT riverID);
		tagRiverMtlDesc *GetRiverMtlDescByIndex(UINT riverIndex);
		bool RemoveRiver(UINT riverIndex);
		bool AddRiver(int riverHeight,UINT sizeW,UINT sizeH,float xScale,float zScale,float yScale);
		void ClearRivers();
		UINT GetRiverNum();
		const tagRiver& GetRiverByIndex( UINT riverIndex );
		int GetRiverHeightByIndex(UINT riverIndex);
		int GetRiverHeightByID(UINT riverID);
		Heightmap *GetRiverHeightMapByIndex(UINT riverIndex);
		Heightmap *GetRiverHeightMapByID(UINT riverID);
		bool IsRiverOverLayGround(const Vector3& vecGround);
		//--外部改变河流
		void ApplyHeight2River(UINT riverIndex,int height);
		void ApplyMtl2River(UINT riverIndex);

		/** 生成水面区域bitmap
		*/
		bool BuildWaterBitmap(BitMap& bitmap,Heightmap* pTrnHMap,IUICallBack* pUI);
		/** 生成水面高度图
		*/
		bool BuildWaterHMap(Heightmap& hmap,Heightmap* pTrnHMap,IUICallBack* pUI);

	private:
		void DrawLineStrip(vector<Vert_PD>& vertBuf);

		bool CheckRivers(UINT curRiverIndex,UINT curPatchIndex);

	private:
		class Member;
		Member	*m_p;
	};
}//namespace Cool3D
