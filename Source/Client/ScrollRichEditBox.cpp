#include "StdAfx.h"
#include "ScrollRichEditBox.h"

ScrollRichEditBox::ScrollRichEditBox(void)
{
}

ScrollRichEditBox::~ScrollRichEditBox(void)
{
}

BOOL ScrollRichEditBox::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bRes = GUIRichEditBox::Init(pSystem, pFather, pXmlElement);
	if(!bRes)
		return bRes;

	//初始化文字滚动速度
	m_fSclSpeed = 50.0f;
	m_fSclTimeCount = 0.0f;
	//开始滚动的时间
	m_fSclTimeStart = 2.0f;
	m_bNextText = true;

	m_bUnClickable = TRUE;
	return bRes;
}

VOID ScrollRichEditBox::Render()
{
	if( IsInvisible() == TRUE )
		return;
	GUIRichEditBox::Render();
}


VOID ScrollRichEditBox::Update()
{
	if( IsInvisible() == TRUE )
		return;

	float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();

	if(m_fSclTimeStart > 0.0f)
		m_fSclTimeStart -= deltaTime;
	else
	{
		m_fSclTimeCount -= deltaTime;							
		if(m_fSclTimeCount < 0.0f && m_ptView.x+m_ptSize.x < 0)	//如果滚动时间到了,且跑马灯跑出父控件则清空
		{
			ClsText();
			SetInvisible(TRUE);									//设置跑马灯隐藏
			m_bNextText = true;
			return;
		}

		if( m_ptView.x+m_ptSize.x < 0)			//全部跑马灯消息都跑出父控件的最左边了
		{
			SetClient(tagPoint(m_lnSlcFatherWndX, m_lnSlcFatherWndY));		//恢复跑马灯开始的位置为最右端
		}
		SetMovable(TRUE);
		INT nSpeed = (INT)(deltaTime*m_fSclSpeed);
		if( nSpeed<=0 )
			nSpeed=1;
		Move(tagPoint(-nSpeed, 0));
		SetMovable(FALSE);
	}
		
	GUIRichEditBox::Update();	
}


BOOL ScrollRichEditBox::PushScrollText(LPCTSTR szText, float fTime)
{
	BOOL bRes = FALSE;
	if( szText==NULL )
		return bRes;

	m_fSclTimeStart = 0.0f;
	SetClient(tagPoint(m_lnSlcFatherWndX, m_lnSlcFatherWndY));		//设置跑马灯开始的位置	
	RefreshViewCoord();
	SetScrollTime(fTime);
	
	ClsText();
	m_ptSize.x = 1024;
	m_rcText.left = 0;
	m_rcText.right = m_ptSize.x;

	bRes = AddText(szText);

	//每次都重新计算跑马灯的长度,这样可以防止由于玩家输入的传音过长,显示不出来的问题
	INT nRowW = this->GetRowRenderWidth(0);
	if( nRowW > 0 )
	{
		this->SetWidth(nRowW+14);
	}

	m_fSclSpeed = (float)((m_lnSlcFatherWndX+nRowW+14)/(fTime/3));

	m_bNextText = false;
	SetInvisible(FALSE);								//设置跑马灯显示
	SetRefresh();
	return bRes;
}
