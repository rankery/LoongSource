#pragma once


// TWaterPage 对话框

class TWaterPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TWaterPage)

public:
	TWaterPage();
	virtual ~TWaterPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_Water };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
