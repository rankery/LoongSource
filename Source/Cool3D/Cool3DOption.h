#pragma once
#include "Cool3D.h"

namespace Cool3D
{
	/** \class Cool3DOption
		\brief 引擎画面选项
		\remarks
			\par 开启NormalMap将自动打开HWSkin
	*/
	class Cool3D_API Cool3DOption
	{
	public:
		enum EOption		
		{
			//--引擎创建前指定的画面选项
			EO_LightMap,		//真实光影
			EO_TerrainDetail,	//地形细节
			EO_Highlight,		//地物高亮	
			EO_NormalMap,		//法线贴图
			EO_HWSkin,			//GPU动画
			EO_GrassAni,		//草动画
			EO_TexDetail,		//贴图精度
			EO_TerrainSpecular, //地形高亮
			EO_WaterSpecular,	//水面高亮

			//--可以在游戏运行时更改的画面选项
			EO_WaterReflect,	//水面反射效果
			EO_WaterRefract,	//水面折射效果
			EO_Music,			//音乐开关
			EO_Sound,			//音效开关
			EO_DebugInfo,		//调试信息
			EO_DrawGrass,		//花草显示
			EO_Num				
		};

		enum ELightMap			//光照图
		{
			ELM_None,			//关
			ELM_Open,			//开
			ELM_End
		};
			
		enum ETerrainDetail		//地形细节
		{
			ETD_Precombine,		//预混合贴图(已叠加lightmap)
			ETD_Editor,			//预混合贴图(未叠加lightmap)+LightMap
			ETD_TwoLayers,		//预混合贴图(已叠加lightmap)+细节纹理
			ETD_MultiLayers,	//多纹理混合
			ETD_End
		};

		enum ETerrainSpecular	//地形高光
		{
			ETS_None,			//关
			ETS_Open,			//开
			ETS_End
		};

		enum EHighlight			//地物高亮
		{
			EHL_None,			//关 
			EHL_Open,			//开
			EHL_End
		};

		enum ENormalMap			//法线贴图
		{
			ENM_None,			//关
			ENM_Open,			//开
			ENM_End
		};

		enum EHWSkin			//硬件加速蒙皮计算
		{
			EHWS_None,			//关
			EHWS_Open,			//开
			EHWS_End
		};

		enum EGrassAni			//草动画
		{
			EGA_None,			//关
			EGA_Open,			//开
			EGA_End
		};

		enum EWaterReflect		//水面反射效果
		{
			EWR_None,			//关
			EWR_Mid,			//中
			EWR_High,			//高
			EWR_End
		};

		enum EWaterRefract		//水面折射效果
		{
			EWRR_None,			//关
			EWRR_Open,			//开
			EWRR_End
		};

		enum ETexDetail			//贴图精度
		{
			ETexDetail_High,	//高
			ETexDetail_Mid,		//中
			ETexDetail_Low,		//低
			ETexDetail_End
		};

		enum EWaterSpecular		//水面高光
		{
			EWS_None,			//关
			EWS_Open,			//开
			EWS_End
		};

		enum EMusic				//音乐
		{
			EM_None,			//关
			EM_Open,			//开
			EM_End
		};

		enum ESound				//音效
		{
			ES_None,			//关
			ES_Open,			//开
			ES_End
		};

		enum EDebugInfo			//调试信息
		{
			EDI_None,			//关
			EDI_Open,			//开
			EDI_End
		};

		enum EDrawGrass			//花草显示
		{
			EDG_None,			//关
			EDG_Open,			//开
			EDG_End
		};

	public:
		/** 有可能因为硬件不支持而失败
		*/
		bool SetOption(EOption opt,int val);
		int  GetOption(EOption opt);
		bool IsSupport(EOption opt,int val);
		/** 验证引擎创建前指定的选项是否合法
		*/
		void CheckCreateOptions();

		/** 编辑器模式下可以在runtime修改任何选项
		*/
		void SetEditorMode(bool bEditorMode);

		static Cool3DOption* Inst();
	private:
		Cool3DOption();
		~Cool3DOption();

		bool			m_bEditorMode;
		int				m_options[EO_Num];
	};

}//namespace Cool3D