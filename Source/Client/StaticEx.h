#pragma once

//! \class	StaticEx
//! \brief	提提供高亮圈选功能,以及可以更换静态控件的图片
//! \author	hyu
//! \date	2009-3-5
//! \base	GUIStatic
//! \sa		GUIStatic

class StaticEx :
	public GUIStatic
{
public:
	StaticEx(void);
	~StaticEx(void);

	//! 重画渲染过程
	virtual VOID Render();

	VOID SetPicFile(const tstring& strFileName);

	VOID SetFontHeight(INT hValue);

	//! 设置高亮状态
	VOID SetHighLight(BOOL bVal) {bisHighLight = bVal;}

	//! 得到当前高亮状态
	BOOL GetHightLight() {return bisHighLight;}

private:
	BOOL		bisHighLight;	//!< 高亮状态
};
