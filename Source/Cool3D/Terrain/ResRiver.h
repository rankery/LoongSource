#pragma once
#include "..\ResSys\ResRenderable.h"

namespace Cool3D
{
	class TRiverEditor;
	class IVertBuffer;
	class VertStream;
	class Heightmap;

	//--河流的材质
	struct Cool3D_API tagRiverMtlDesc
	{
		TCHAR	szWaterTex[256];		//水面贴图的路径
		TCHAR	szBumpMap[256];			//凹凸贴图的路径

		int		waterFrm;				//水面贴图有多少帧(每帧一个文件)
		float	waterFps;				//水面贴图帧率

		float	uTileSize;				//多少个格子重复一次u
		float	vTileSize;				//多少个格子重复一次v

		bool	bEnableEffect;			//开启反射、折射、高光效果
		Color4f blendColor;				//反射效果混合颜色和混合因子
		float	bumpScale;				//bump map缩放值

		int		bumpFrm;				//反射贴图有多少帧				----- 未使用
		float	power;					//镜面系数
		float	reflectPower;			//反射效果镜面系数
		float	alpha;					//透明度

		Color4f	diffuse;				//漫反射材质
		Color4f ambient;				//环境光材质
		Color4f	specular;				//高光材质

		Vector2 bumpUVScroll;           //bump map UV的滚动速度
		float   reflectSpecularScale;   //反射效果高光缩放
		float   underWaterFogStart;     //水下雾起始高度
		float   underWaterFogEnd;       //水下雾结束高度
		Vector3 underWaterFogColor;     //水下雾颜色
		bool	bCloseEdgeFade;			//关闭边缘过渡
		BYTE	reserve[31];			//保留字段

		tagRiverMtlDesc()
		{
			memset(this,0,sizeof(tagRiverMtlDesc));
			waterFrm=1;
			waterFps=16.0f;
			bEnableEffect=true;
			blendColor=Color4f(0.5f,0,0.4f,0.4f);
			bumpScale=0.02f;
			uTileSize=64.0f;
			vTileSize=64.0f;
			alpha=1.0f;
			power=32.0f;
			reflectPower=64.0f;
			diffuse=ambient=specular=Color4f(1.0f,1.0f,1.0f,1.0f);
			bumpUVScroll=Vector2( 0.01f, 0.0f );
			reflectSpecularScale=0.2f;
		}
	};

	/**	\class ResRiver
		\brief 保存河流的公共资源
	*/
	class ResRiver : public ResRenderable
	{
		struct tagHeader
		{
			TCHAR	magic[8];
			UINT	riverID;
			bool	bEnableEffect;		//开启反射、折射、高光效果
			float	height;
			UINT	numVerts;
			DWORD	vertsOffset;
		};

	public:
		ResRiver(const TCHAR *szName);
		virtual ~ResRiver();

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const;
		virtual RenderNode *GetRenderNode(UINT i) const;
		virtual const AABBox *GetAABBox() const;
		virtual int GetNumMtl() const;
		virtual void CreateMtlArray(vector<IMaterial*>& mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const {}

		//--供外部编辑器调用，用于build河流
		static bool EditorSave(TRiverEditor *pEditor,UINT riverID,const TCHAR *szBasePath);

		//--
		IVertBuffer *GetNormalAlphaVB();
		bool IsEnableEffect();
		float GetHeight();
		UINT GetRiverID();

	private:
		static void FillVertStream(VertStream *pVS,Heightmap *pHMap);

	private:
		class Member;
		Member *m_p;

		DECL_RTTI(ResRiver);
	};
}//namespace Cool3D