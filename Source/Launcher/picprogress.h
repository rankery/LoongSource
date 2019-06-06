#pragma once


// CPicProgress

class CPicProgress : public CStatic
{
	DECLARE_DYNAMIC(CPicProgress)

public:
	CPicProgress();
	virtual ~CPicProgress();

private:
	int m_nLValue;  //最低值
	int m_nHValue;   //最高值
	int m_nPos;	  //当前值
	float m_fLength;	//当前进度值的实际显示长度
	CRect m_rect;
	CBitmap * m_bmpOld;
	CBitmap m_bmpBack;  //背景图片
	CBitmap m_bmpFore;  //前景图片
	CDC * m_pDCBack;
	CDC * m_pDCFore;
	BITMAP m_bmback;
	BITMAP m_bmfore;

public:
	int GetPos();
	int	OffsetPos( int nPos );
	//设置当前值
	BOOL SetPos( int nPos );
	//设置进度条的值范围
	void SetRange( int nLower, int nHigh );
	//取得进度条的值范围
	void GetRange(int &nLower, int& nHigh);
	//设置前景色
	void SetForceBmp(LPCTSTR szPicPath);
	//设置背景色
	void SetBackBmp(LPCTSTR szPicPath);	

protected:

	afx_msg void OnPaint();		//重载渲染

	DECLARE_MESSAGE_MAP()
};


