#pragma once

#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;
/**	\class IPlugEditor
	\brief 编辑器PlugIn的接口
*/
class IPlugEditor
{
public:
	enum EBuildItems
	{
		EBI_None=0,
		EBI_Terrain=1,//地形
		EBI_River=2,//河流
		EBI_MapObj=4,//地图对象
		EBI_Nav=8,//导航图
		EBI_LightMap=16,//光影贴图
		EBI_CompressTex=32,//贴图压缩
		EBI_RiverDataToTerrain=64,//河流数据写入地形
		EBI_MiniMap=128,//缩略图
	};
	struct tagBuildOptions
	{
		//--Lightmap
		bool bLightMapping;//计算光照和阴影
		bool bRadiosity;//光能传递模拟
		int	 radiosityTimes;//光能传递次数
		bool bBlur;//模糊
		int  blurTimes;//模糊次数
		bool bApplyToCombine;//应用到地表预混合贴图
		bool bApplyToDeco;//应用到地表装饰层
		bool bAmbientOcclusion;//计算天光
		bool bSmallAO;//小地物是否影响天光

		//-navmap
		bool bColliderData;//碰撞数据
		bool bNPCNavMap;//NPC格子导航图
		bool bNPCNavGraph;//NPC路点导航图
		bool bPlayerNavMap;//玩家自动寻路导航图
		float fNPCPassHeight;//NPC可通过高度
		float fNPCNavGraphPassHeight; //NPC路点导航图可通过高度

		//--缩略图
		int  nMiniMapSize;//缩略图大小
	};
public:
	IPlugEditor(void) {}
	virtual ~IPlugEditor(void) {}

	/**	绑定需要编辑的内容的接口
	*/
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL) =0;
	virtual void SetCamera(CameraBase *pCamera) =0;

	virtual void Update(bool bActive) {}
	virtual void Render(bool bActive) {}/**在场景渲染的Begin(),end()之间调用*/
	virtual void RenderPreview()	{}/**在场景渲染的Begin(),end()之外调用,需要自己的Begin,End*/

	/**	返回显示给用户看的插件名称
	*/
	virtual const TCHAR* GetUserName() =0;
	/**	返回内部Class名称
	*/
	virtual const TCHAR* GetClassName() =0;
	/**	外部创建插件时调用一次
	*/
	virtual void Create(DWORD pParentWnd) =0;
	/**	外部销毁插件时调用一次
	*/
	virtual void Destroy() =0;
	/**	显示/隐藏此编辑器
	*/
	virtual void ShowWindow(bool bShow) =0;
	/**	如果插件需要将画面渲染到特定的窗口则返回窗口句柄,否则返回空
	*/
	virtual HWND GetRenderWnd() {	return NULL; }
	virtual bool Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI)	{	return true;}

	/**	操作消息,当宿主程序窗口收到操作时调用相应的函数
	*/
	virtual void OnLButtonDown(UINT nFlags, UINT x,UINT y) =0;
	virtual void OnLButtonUp(UINT nFlags, UINT x,UINT y) =0;
	virtual void OnMouseMove(UINT nFlags, UINT x,UINT y) =0;
	virtual void OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y) =0;
	virtual void OnRButtonDown(UINT nFlags, UINT x,UINT y) =0;
	virtual void OnRButtonUp(UINT nFlags, UINT x,UINT y) =0;
};
