#pragma once
#include "..\Cool3D.h"
#include "..\Math\Math.h"
#include "..\Math\Color.h"
#include "..\Public\VertexDefine.h"
#include ".\IUICallBack.h"

namespace Cool3D
{
	class Noise;
	class CameraBase;
	class Image;
	class ResRenderable;
	class SceneGraph;
	class Heightmap;
	class BitMap;
	class VertStream;
	class TRiverEditor;
	class EditorDecoFile;
	
	/**	\struct TMapLayer
		\brief terrain map layer
	*/
	struct Cool3D_API tagTMapLayer
	{
		TCHAR	szName[64];			//名称
		TCHAR	szTexFile[256];		//图素贴图名称
		BYTE	uvSetIndex;			//使用哪套UV坐标
		BYTE	surfaceType;		//地表类型

		BYTE	reserve[63];
		tagTMapLayer()	
		{	
			memset(this,0,sizeof(*this));
		}
	};

	struct Cool3D_API tagTDecoLayer
	{
		TCHAR			szName[64];		//名称,浓度图对应磁盘文件"name.deco",浓度图的每个象素代表这个格子有几个装饰物
		float			fadeOut;		//在距离eye一定范围内消失,用来优化

		int				type;			//0草
		TCHAR			szFileName[256];//装饰物路径[草,as texture]
		Vector3			size;			//大小[草x,y]
		float			sizeRandRate;	//大小的随机量,标准大小的倍数(0~n)
		BYTE			alphaRef;		//[草,alpha mask]
		Vector3			rotateRand;		//pitch,yaw,yoll旋转随机
		float			wave;			//[草，振幅]
		float			speed;			//[草，波速]
		EVertType		vertType;
		int				numVert;

		Color4f			diffuse;		//漫反射材质

		DWORD			dwReserve[54];

		tagTDecoLayer()
		{
			memset(this,0,sizeof(*this));
			fadeOut=128*50;
			size.x=40;
			size.y=60;
			size.z=0;
			alphaRef=244;
			rotateRand.y=90;
			wave=20.0f;
			speed=0.8f;
			vertType=EVType_PT2;
			numVert=0;
			diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		}
	};

	enum Cool3D_API ELerpType
	{
		ELerp_None=0,		//无插值
		ELerp_Linear,	//线性插值
		ELerp_Cosine,	//cos插值
		ELerp_Quadratic,//二次插值
	};
	enum Cool3D_API EShapeType
	{
		EShape_Circle=0,	//圆形笔刷
		EShape_Quad,		//方形笔刷，具有内外两个Quad
		EShape_Line,		//线型笔刷，
		EShape_Plane,		//平滑笔刷
		EShape_TypeDef,		//自定义四边形，用于编辑四边形区域的斜坡
	};
	struct Cool3D_API tagTEditBrush
	{
		int			centerX,
					centerZ;//中心点地形格子坐标
		int			inner,	//笔刷内径大小
					outer;	//笔刷外径大小
		EShapeType	shape;
		ELerpType	lerp;
		int			strength;
	};

	/**	\class TerrainEditor
		\brief 地形编辑器核心功能
	*/
	class Cool3D_API TerrainEditor
	{
	public:
		enum TE_CONST
		{
			//--编辑
			MAX_MAP_LAYER=32,	//最多有多少个图层
			MAX_DECO_LAYER=16,

			//--数据生成
			EPatchAlphaMapSize=256,		//单个Patch贴图的大小有多少*多少个象素
			EPatchSize=128,				//单个Patch有多少*多少个格子,(ETextureSize对应多少*多少个格子的地形)
			EPatchSideVerts=33,			//单个Patch有多少*多少个顶点
			EPatchVerts=33*33,			//单个Patch有多少个顶点
			EPatchSideGridsLod0=16,		//单个Patch Lod0有多少*多少个Grid
			ESimplePatchSize=512,		//单个远处Patch有多少*多少个格子
			ESimplePatchSideVerts=33,	//单个远处Patch有多少*多少个顶点
			ESimplePatchVerts=33*33,	//单个远处Patch有多少个顶点
		};
		enum EEditFocus//当前编辑的焦点数据,如果不是焦点数据就可以减少内部更新的计算
		{
			EFocus_HeightMap =0,
			EFocus_Layer,
			EFocus_Deco
		};

		static int OP_Draw_Dist;
		static int OP_Edit_Dist;
		static int OP_Deco_Draw_Size;
	public:
		TerrainEditor(void);
		virtual ~TerrainEditor(void);

		/**	设置当前地形的名称,必须在创建之前调用
			\remarks 编辑器将使用这个名称作为编辑信息存盘路径;
			还将作为生成时的名称
		*/
		void SetName(const TCHAR* szName);
		const TCHAR* GetName();
		void SetDrawOption(bool bWireFrame,bool showDecoLayer);
		void GetDrawOption(bool& bWireFrame,bool& showDecoLayer);
		
		void SetEditFocus(EEditFocus focus);
		/**	画出以摄像机为中心的矩形范围内三所有各自
		*/
		void Draw(SceneGraph *pSG);
		void DrawPatchEdge();
		void DrawSubPatchEdge();
		/**	在地面上画出一个圆圈
			\param,中心点的世界坐标,圆圈的半径(单位:格子)
		*/
		void DrawGroundCircle(float centreX,float centreZ,UINT r);
		void DrawGroundQuad(int left,int right,int top,int bottom,DWORD color);
		void DrawGroundLine(int sx,int sy,int length,float angle=0.0f,DWORD color=0xFFFFFF00);
		void DrawGroundLine(int sx,int sy,int dx,int dy,DWORD color=0xFFFFFF00);

		/**	导入高度图
		*/
		bool ImportHeightMap(const TCHAR* szFileName,float xScale,float zScale,float yScale,Vector2 layerUVScale);
		bool ExportHeightMap(const TCHAR* szFileName);
		void CreateByNoise(float freq,UINT width,UINT height,float xScale,float zScale,float yScale,Vector2 layerUVScale);
		void Create(BYTE initVal,UINT width,UINT height,float xScale,float zScale,float yScale,Vector2 layerUVScale);
		//!	编辑中修改Terrain的基本属性,需要编辑器重启,rebuild才能生效
		void ModAtt(float xScale,float zScale,float yScale);
		/**	导出高度图
		*/
		void ExportHeightMap(Image *pImg);

		bool AddLayer(const TCHAR* szLayerName,const TCHAR* szTileFileName,BYTE uvSetIndex,BYTE surfaceType);
		/**	删除一个图层
			\return 是否找到,并删除了
		*/
		bool RemoveLayer(const TCHAR* szLayerName);
		/**	删除所有图层*/
		void ClearLayers();
		const tagTMapLayer *GetMapLayer(UINT index) const;
		tagTMapLayer* FindMapLayer(const TCHAR* szLayerName);
		/**	把指定的layer在Layer数组中向上或者向下移动
		*/
		void MoveLayer(const TCHAR* szLayerName,int offset);
		void ApplyBrush2Layer(const tagTEditBrush& brush,const TCHAR* szLayerName);
		void SmoothLayer(const tagTEditBrush& brush,const TCHAR* szLayerName);
		void NoiseLayer(const tagTEditBrush& brush,float noiseFreq,const TCHAR* szLayerName);
		void ApplyBrush2HMap(const tagTEditBrush& brush);
		void FlattenHMap(const tagTEditBrush& brush);
		void SmoothHMap(const tagTEditBrush& brush);
		void NoiseHMap(const tagTEditBrush& brush,float noiseFreq);
		void ApplyValue2HMap(const tagTEditBrush& brush);
		void ApplyBrush2River(const tagTEditBrush& brush,UINT riverIndex);
		void SmoothRiver(const tagTEditBrush& brush,UINT riverIndex);
		void NoiseRiver(const tagTEditBrush& brush,float noiseFreq,UINT riverIndex);

		//!	增加一个装饰层
		bool AddDecoLayer(const TCHAR * szLayerName);
		//!	删除一个装饰层
		bool RemoveDecoLayer(const TCHAR * szLayerName);
		//! 返回装饰层数据,允许外部编辑修改
		tagTDecoLayer* GetDecoLayer(const TCHAR * szLayerName);
		tagTDecoLayer* GetDecoLayer(int index);
		void PostEditChangeDecoLayer(const TCHAR * szLayerName,bool bForceRebuild);
		//!	修改装饰物层的浓度图
		void BrushDecoDensityMap(const TCHAR* szLayerName,tagTEditBrush& brush,CameraBase *pCamera);
		void NoiseDecoDensityMap(const TCHAR* szLayerName,const tagTEditBrush& brush,float noiseFreq,CameraBase *pCamera);
		//!	直接导入一个灰度图拷贝到浓度图
		void ImportDensityMap(const TCHAR* szLayerName,const TCHAR* szImgPath,CameraBase *pCamera);
		//!	刷新所有装饰层预览
		void RefreshDecoView(CameraBase *pCamera);
		//!	得到浓度图
		Heightmap *GetDecoDensityMap(const TCHAR * szLayerName);
		//! 得到装饰层文件
		EditorDecoFile *GetDecoFile(const TCHAR *szLayerName);
		//! 得到装饰层的VB IB
		bool GetDecoVBAndIB(const TCHAR * szLayerName,const RECT& rect,
			VertStream * pVertStream, vector<WORD>& indexArray);
		
		bool LoadInfo();
		/**	使用name存盘
			\see SetName()
		*/
		bool SaveInfo() const;
		/**	创建RunTime渲染需要的地形数据
			\remarks 自动存盘到 basepath\\myname\\*.*
			\par 请使用相当路径
			\see SetName()
		*/
		bool BuildTerrain(const TCHAR* szBasePath,IUICallBack* pUI);
		/** 创建河流数据
		*/
		bool BuildRiver(const TCHAR* szBasePath,IUICallBack* pUI);
		/** 把河流的某些数据写入到地形块中
		*/
		bool WriteRiverDataToTerrainPatch(const TCHAR* szTerrainBasePath,IUICallBack* pUI);

		bool MousePick(UINT cursorX,UINT cursorY,CameraBase *pCamera,Vector3& out);

		void SetLayerUVScale(BYTE uvSetIndex,Vector2 uvScale);
		Vector2 GetLayerUVScale(BYTE uvSetIndex) const;
		bool IsHeightmapCreated() const;
		POINT World2Tile(const Vector3& wpt);
		UINT GetHMapWidth();
		UINT GetHMapHeight();
		BYTE GetHMapValue(UINT hmapX,UINT hmapY);
		float GetHMapYScale();
		float GetHMapXScale();
		float GetHMapZScale();
		Vector2 GetStartPos();

		void SetDetailMap(const TCHAR* szPath);
		const TCHAR* GetDetailMap() const;
		/*使用当前噪声的设置重新生成噪声*/
		void ResetNoise();
		
		const Heightmap *GetHeightmap() const;
		Heightmap *GetHeightmapVar();

		void NewRiver(int riverHeight);
		void DelRiver(UINT riverIndex);
		int GetRiverNum();
		TRiverEditor *GetRiverEditor();
		//--判断某点地表是否被河流覆盖
		bool IsRiverOverLayGround(Vector3& vecGround);

		void DrawLineStrip(vector<Vert_PD>& vertBuf);

		/**	获得指定点所在的Patch使用的贴图层数
		*/		
		void GetLayers(Vector3& pos,vector<int>& layers);
		//! 模糊Blend图片
		bool BlurBlendImg(const TCHAR* szBasePath);

		//! 指定在刷地形时是否显示方盒
		void SetGroundRole(bool bVal);
		void DrawGroundRole(float &x, float &y, float& z);

		/** 将Lightmap应用到地表预混合贴图
		*/
		bool ApplyLightMapToCombineImage(const TCHAR* szLightMapPath,const TCHAR* szTrnPath,IUICallBack* pUI);
		/** 将Lightmap应用到地表装饰层
		*/
		bool ApplyLightMapToDecoLayer(const TCHAR* szLightMapPath,const TCHAR* szTrnPath,IUICallBack* pUI);

		/**	终止记录画刷动作，暂时只支持针对高度图的操作
		*/
		void EndBrushRecord();

		/** 执行一次画刷的撤销功能，暂时只支持针对高度图的操作
		*/
		void UndoBrush();
		/** 执行一次画刷的重做功能，暂时只支持针对高度图的操作
		*/
		void RedoBrush();
		/** 是否可以执行画刷的撤销功能
		*/
		bool GetUndoBrushEnable();
		/** 是否可以执行画刷的重做功能
		*/
		bool GetRedoBrushEnable();

		/** 生成导航高度图
		*/
		void BuildNavHeightmap(Heightmap& nav,IUICallBack* pUI);
		/** 生成水面区域bitmap
		*/
		bool BuildWaterBitmap(BitMap& bitmap,IUICallBack* pUI);
		/** 生成水面高度图
		*/
		bool BuildWaterHMap(Heightmap& hmap,IUICallBack* pUI);

		/**	生成地表类型图
		*/
		void BuildGroundTypeMap(Heightmap& layerType,IUICallBack* pUI);

		//--设置地表材质
		void SetMtlDiffuse(const Color4f& mtlColor);
		void SetMtlAmbient(const Color4f& mtlColor);
		void SetMtlSpecular(const Color4f& mtlColor);
		void SetMtlPower(float power);
		//--获得地表材质
		const Color4f& GetMtlDiffuse();
		const Color4f& GetMtlAmbient();
		const Color4f& GetMtlSpecular();
		float GetMtlPower();

	private:
		void DrawTerrain(SceneGraph *pSG);
		void DrawRiver(SceneGraph *pSG);
		void DrawDeco(SceneGraph *pSG);

	protected:
		class Member;
		Member *m_p;
	};

	class SGTrnDecoPatch;
	SGTrnDecoPatch *NewTrnDecoNode(int type);
}//namespace Cool3D
