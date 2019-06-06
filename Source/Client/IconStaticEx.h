#pragma once
#include "IconStatic.h"

// 添加右下角画字的功能

class IconStaticEx : public IconStatic
{
public:
	VOID DrawText(LPCTSTR szText);
};