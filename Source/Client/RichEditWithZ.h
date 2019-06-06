#pragma once
class RichEditWithZ
	:public GUIRichEditBox
{
public:
	RichEditWithZ(void);
	~RichEditWithZ(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	//virtual VOID Render();
	virtual VOID Update();
	//×¢²á±íÇéÍ¼Æ¬
	void RegEmtionPic(const TCHAR* szTran,int index);

	void DrawInfo(const POINT& pt,float z);

	void SetSize(const tagPoint& ptSize) { m_ptSize = ptSize; }
	void SetView(const tagPoint& ptView) { m_ptView = ptView; }

	void SetWidth(const LONG& width) { m_ptSize.x = width; m_rcText.right = width;}
	void SetHeight(const LONG& height) { m_ptSize.y = height; m_rcText.bottom = height;}

	INT GetRowSpacing() const { return m_nRowSpacing;}
	void SetRowSpacing(INT rowSpacing) { m_nRowSpacing = rowSpacing; }

private:

	IDepthFont*		m_pDepthFont;

};