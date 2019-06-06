#pragma once
#include "..\vEngine\gui\gui_staticex.h"

const INT MIN_TIPS_WIDTH = 16;
const INT MAX_TIPS_WIDTH = 216;
/** class	ToolTipStatic
	brief	Tips显示栏
	remarks 显示Tips，根据Tips的大小调整控件大小
*/
class ToolTipStatic :
	public GUIStaticEx
{
public:
	ToolTipStatic();

	virtual BOOL Resize(tagPoint& ptSize);

	virtual VOID SetText(LPCTSTR szText);

protected:
	/** 把字符串解析成items，不考虑控件的大小
	*/
	VOID PreParseText();
	
	/** 重新计算控件的大小
	*/
	BOOL RecalculateSize(tagPoint& ptSize);

};
