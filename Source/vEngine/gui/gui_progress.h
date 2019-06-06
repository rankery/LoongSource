//-----------------------------------------------------------------------------
//!\file gui_progress.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-28
//!
//!\brief 界面系统 progress 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统 progress 控件(进度条)
//-----------------------------------------------------------------------------
class VENGINE_API GUIProgress : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();

	VOID SetValue(FLOAT fValue,bool bForceSet=false);
	VOID SetMaxValue(FLOAT fMaxValue);
	VOID SetSpeed(FLOAT fSpeed) { m_fSpeed = fabs(fSpeed); } //当前值向设定值变化的速度

	FLOAT GetCurrentValue() {	return m_fCurrentValue;}
	FLOAT GetMaxValue()	{ return m_fMaxValue; }

	VOID SetForePicColor(DWORD dwForePicColor){ m_dwForePicColor = dwForePicColor; }

	GUIProgress();

protected:
	FLOAT			m_fMaxValue;
	FLOAT			m_fTargetValue;
	FLOAT			m_fCurrentValue;
	FLOAT			m_fSpeed;
	DWORD			m_dwLastTime;
	tagGUIImage*	m_pForePic;


	BOOL		m_bHorizon;					// 是否水平
	tstring		m_strForePicFileName;		// 前景图片文件名
	tagRect		m_rcForePic;				// 前景图片使用区域
	DWORD		m_dwForePicColor;			// 前景图片绘制颜色
};



}	// namespace vEngine {

