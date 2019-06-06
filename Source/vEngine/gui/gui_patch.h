//-----------------------------------------------------------------------------
//!\file gui_patch.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-06-15
//!
//!\brief 界面系统 patch 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统window控件(可变大小的拼图窗口)
//-----------------------------------------------------------------------------
class VENGINE_API GUIPatch : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();

	GUIPatch();

protected:

	tagGUIImage*	m_pTopPic;			// 上部图片handle
	tagGUIImage*	m_pMidPic;			// 中部图片handle
	tagGUIImage*	m_pDownPic;			// 下部图片handle
	tagGUIImage*	m_pLeftPic;			// 左部图片handle
	tagGUIImage*	m_pRightPic;		// 右部图片handle
	tagGUIImage*	m_pLeftTopPic;		// 左上图片handle
	tagGUIImage*	m_pRightTopPic;		// 右上图片handle
	tagGUIImage*	m_pLeftDownPic;		// 左下图片handle
	tagGUIImage*	m_pRightDownPic;	// 右下图片handle

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	tstring			m_strTopPic;		// 上部图片文件名
	tagRect			m_rcTopPic;			// 上部图片使用区域
	tstring			m_strMidPic;		// 中部图片文件名
	tagRect			m_rcMidPic;			// 中部图片使用区域
	tstring			m_strDownPic;		// 下部图片文件名
	tagRect			m_rcDownPic;		// 下部图片使用区域
	tstring			m_strLeftPic;		// 左部图片文件名
	tagRect			m_rcLeftPic;		// 左部图片使用区域
	tstring			m_strRightPic;		// 右部图片文件名
	tagRect			m_rcRightPic;		// 右部图片使用区域
	tstring			m_strLeftTopPic;	// 左上图片文件名
	tagRect			m_rcLeftTopPic;		// 左上图片使用区域
	tstring			m_strRightTopPic;	// 右上图片文件名
	tagRect			m_rcRightTopPic;	// 右上图片使用区域
	tstring			m_strLeftDownPic;	// 左下图片文件名
	tagRect			m_rcLeftDownPic;	// 左下图片使用区域
	tstring			m_strRightDownPic;	// 右下图片文件名
	tagRect			m_rcRightDownPic;	// 右下图片使用区域

};


}	// namespace vEngine {
