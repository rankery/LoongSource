#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

#include "..\Cool3D\public\iplugeditor.h"
#include "Resource.h"
#include "TEditorSheet.h"
#include "BrushDlg.h"

class PlugTerrainEditor :	public IPlugEditor
{
public:
	PlugTerrainEditor(void);
	virtual ~PlugTerrainEditor(void);

	//--IPlugEditor
	virtual void AttachEngineInterface(void *pInterface,void *pExInterface=NULL);
	virtual void SetCamera(CameraBase *pCamera) { m_pCamera=pCamera;}

	virtual void Update(bool bActive);
	virtual void Render(bool bActive);

	virtual const TCHAR* GetUserName();
	virtual const TCHAR* GetClassName();
	virtual void Create(DWORD pParentWnd);
	virtual void Destroy();
	virtual void ShowWindow(bool bShow);

	virtual bool Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);

	virtual void OnLButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnLButtonUp(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseMove(UINT nFlags, UINT x,UINT y);
	virtual void OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y);
	virtual void OnRButtonDown(UINT nFlags, UINT x,UINT y);
	virtual void OnRButtonUp(UINT nFlags, UINT x,UINT y);

	//--
	TerrainEditor*	GetEngine()	{ return m_pEngine; }
	CameraBase* GetCamera()		{ return m_pCamera;}
	TEditorSheet*	GetSheet()	{ return &m_sheet; }
	CBrushDlg*		GetBushDlg(){ return &m_brushDlg; }
	void AlignBrushDlg();

private:
	bool _Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI);
	void CreateBrush(tagTEditBrush& brush,int inversVal=1);
	void DoCurrentTools(tagTEditBrush& brush);
	
	POINT			m_cursorScrPos;		//鼠标光标屏幕坐标
	Vector3			m_cursorPos;		//选中的地面的世界坐标
	Vector3			m_cursorPosRiver;	//选中的河流的世界坐标
	CameraBase		*m_pCamera;
	TerrainEditor	*m_pEngine;
	//--窗口
	TEditorSheet	m_sheet;
	CBrushDlg		m_brushDlg;
	POINT			m_lastDoPt;//上一次操作的鼠标点,防止重复操作
	CString			m_szHmapInfo;

	//鼠标状态集合
	typedef enum _MOUSESTATE
	{
		MS_INVALID				=-1,	//无效状态
		MS_NONE					=0,		//空闲状态
		MS_LDOWN,						//左键按下
		MS_LMOVE,						//左键移动
		MS_RDOWN,						//右键按下
		MS_RMOVE,						//右键移动
		MS_LRDOWN,						//左右键按下
		MS_LRMOVE,						//左右键移动
		MS_LMOVE_RDOWN,					//左键移动右键按下
		MS_RMOVE_LDOWN					//右键移动左键按下
	} MOUSESTATE;
	MOUSESTATE		m_mouseState;		//鼠标状态
	//鼠标操作集合
	typedef enum _MOUSEOPERATION
	{
		MO_INVALID				=-1,	//无效操作
		MO_ONLDOWN				=0,		//左键按下
		MO_ONLUP,						//左键弹起
		MO_ONRDOWN,						//右键按下
		MO_ONRUP,						//右键弹起
		MO_ONMOVE						//鼠标移动
	} MOUSEOPERATION;
	/** 改变鼠标状态
		\param	鼠标操作
	*/
	void ChangeMouseState(MOUSEOPERATION mouseOp);
};

extern PlugTerrainEditor	*g_editor;
void BeginMFCRes();
void EndMFCRes();