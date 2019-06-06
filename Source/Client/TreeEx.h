#pragma once
/**
*class TreeEx
*brief 自定义tree控件
*author hyu
*remark
*base GUITree
*/
class TreeEx :
	public GUITree
{
public:
	TreeEx(void);
	~TreeEx(void);

	//设置项目文本颜色
	BOOL SetItemColor(DWORD dwID, DWORD dwColor);
	//设置showtext
	BOOL SetShowText(DWORD dwID, LPCTSTR szShowText);
};
