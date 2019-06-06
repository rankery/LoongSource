#pragma once

class ScrollRichEditBox :
	public GUIRichEditBox
{
public:
	ScrollRichEditBox(void);
	~ScrollRichEditBox(void);
	
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Render();
	virtual VOID Update();

	//设置文本
	BOOL PushScrollText(LPCTSTR szText, float fTime = 60.0f);

	//设置总计在屏幕上滚动多长的时间
	void SetScrollTime(float fTime) { m_fSclTimeCount = fTime; }
	const float& GetScrollTime()const { return m_fSclTimeCount; }

	//设置跑马灯开始的位置
	void SetStartPos(LONG posX, LONG posY) { m_lnSlcFatherWndX = posX; m_lnSlcFatherWndY = posY;}
	bool GetContinueScrText() { return m_bNextText; }
	void SetContinueScrText(bool bVal = false) { m_bNextText = bVal; } 

	void SetClient(const tagPoint& ptClient)	{ m_ptClient = ptClient; }

	void SetWidth(const LONG& width) { m_ptSize.x = width; m_rcText.right = width;}

private:
	float						m_fSclSpeed;				// 跑马灯显示位置速度
	float						m_fSclTimeStart;			// 开始滚定的时间,2秒后开始滚动
	float						m_fSclTimeCount;			// 滚动的总时间，总计滚动多长的时间消失
	LONG						m_lnSlcFatherWndX;			// 跑马灯父控件的最右端坐标
	LONG						m_lnSlcFatherWndY;			
	bool						m_bNextText;				// 当前跑马灯的消息是否已经完毕
};
