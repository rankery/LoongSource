// TWaterPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TerrainEditor.h"
#include "TWaterPage.h"


// TWaterPage 对话框

IMPLEMENT_DYNAMIC(TWaterPage, CPropertyPage)
TWaterPage::TWaterPage()
	: CPropertyPage(TWaterPage::IDD)
{
}

TWaterPage::~TWaterPage()
{
}

void TWaterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TWaterPage, CPropertyPage)
END_MESSAGE_MAP()


// TWaterPage 消息处理程序
