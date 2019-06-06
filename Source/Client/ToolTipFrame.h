#pragma once
#include "render2d.h"

class ToolTipStatic;

/** class	ToolTipFrame
	brief	提示信息控件管理
*/
class ToolTipFrame :
	public GameFrame
{
public:
	ToolTipFrame(void);
	~ToolTipFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	
	virtual BOOL ReloadUI();

	virtual BOOL EscCancel();

	
	/** \显示提示信息
		\param szTip: 格式化好的提示信息,==NULL表示隐藏
		\param szTipsEx: 补充提示信息，多为物品价格
		\param pWnd: 指定为哪个控件启动Tips
		\外部将信息格式化好后调用,内部根据文字信息的大小自动调整提示信息框的大小
	*/
	VOID ShowTips(GUIWnd* pWnd, LPCTSTR szTip, LPCTSTR szTipEx=NULL);
	/** \param rc: 指定为源控件的区域
	*/
	VOID ShowTips(GUIWnd* pWnd, LPCTSTR szTip, const RECT& rc);
	/** \param ptPos：指定在哪个位置显示提示信息
	*/
	VOID ShowTips(tagPoint ptPos, LPCTSTR szTip);
	/** \显示装备比较信息
		\param szTip: 格式化好的装备比较信息,==NULL表示隐藏
		\param szTipsEx: 装备比较信息补充，为有两个装备时准备
		\外部将信息格式化好后调用,内部根据文字信息的大小自动调整提示信息框的大小
	*/
	VOID ShowTipsCompare(LPCTSTR szTip, LPCTSTR szTipEx=NULL);

private:
	/** \根据显示控件的大小重新计算控件的位置
		\param ptSize: 显示控件的大小
	*/
	VOID ResetWindowPoint(tagPoint ptSize);
	/** \param ptPos: 显示控件的位置
	*/
	VOID ResetWindowPoint(tagPoint ptSize, tagPoint ptPos);
	/** \删除UI界面对象
	*/
	BOOL DestroyUI();
	/** \处理桌面的UI事件	
		\param pEvent: UI事件
	*/
	BOOL EventHandler(tagGUIEvent* pEvent);
	/** \显示控件的信息提示
		\param pStatic: 该控件的指针
	*/
	VOID ShowStaticTip(GUIStatic* pStatic);
	/** \将一个控件与另一个控件对齐
		\param pSrcWnd: 需要对齐的控件 
		\param pDstWnd: 与之对齐的控件
		\param eAlign: 对齐的格式
	*/
	VOID AlignWnd(GUIWnd* pSrcWnd, GUIWnd* pDstWnd, EGUIAlign eAlign);

private:

	TSFPTrunk<ToolTipFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<GUIRender>			m_pRender;

	GUIWnd*						m_pWnd;			// Tips界面
	GUIWnd*						m_pWndEx;		// Tips补充界面
	GUIWnd*						m_pWndCompare;	// 装备比较Tips界面
	GUIWnd*						m_pWndCompareEx;// 装备比较Tips补充界面
	ToolTipStatic*				m_pStcTips;		// Tips内容显示控件
	ToolTipStatic*				m_pStcTipsEx;	// Tips补充内容控件
	ToolTipStatic*				m_pStcCompare;	// 装备比较Tips内容显示控件
	ToolTipStatic*				m_pStcCompareEx;// 装备比较Tips补充内容显示控件

	GUIWnd*						m_pSrcWnd;		// 需要显示的控件缓存
};
/** \function ShowTip: 显示提示信息（根据控件找位置）
	\param pWnd: 需要显示信息的控件
	\param szTip: 需要显示的字符串
	\param szTipEx: 补充字符串，多为物品的价格
*/
inline void ShowTip(GUIWnd* pWnd, LPCTSTR szTip, LPCTSTR szTipEx=NULL)
{
	ToolTipFrame* pTips = (ToolTipFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Tooltip"));
	
	if (P_VALID(pTips))
		pTips->ShowTips(pWnd, szTip, szTipEx);	
}
/** \function ShowTip: 显示提示信息（根据坐标找位置）
	\param ptPos: 显示信息框的坐标
	\param szTip: 需要显示的字符串
*/
inline void ShowTip(tagPoint ptPos, LPCTSTR szTip)
{
	ToolTipFrame* pTips = (ToolTipFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Tooltip"));

	if (P_VALID(pTips))
		pTips->ShowTips(ptPos, szTip);
}
/** \function ShowTipCompare: 显示装备比较信息
	\param szTip: 需要显示的字符串
	\param szTipEx: 需要显示的字符串
*/
inline void ShowTipCompare(LPCTSTR szTip, LPCTSTR szTipEx=NULL)
{
	ToolTipFrame* pTips = (ToolTipFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Tooltip"));

	if (P_VALID(pTips))
		pTips->ShowTipsCompare(szTip, szTipEx);
}

/** \function HideTip: 隐藏界面相关Tip
	\param pWnd：相关界面控件指针
*/
inline void HideTip(GUIWnd* pWnd)
{
	POINT pt;
	::GetCursorPos(&pt);
	if(P_VALID(pWnd) && pWnd->PointInWnd(tagPoint(pt.x, pt.y), false))
	{
		ShowTip(NULL ,NULL);
	}
}