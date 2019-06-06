//-----------------------------------------------------------------------------
//!\file xui_richeditbox.cpp
//!\author tidy / bwen / zewu / lyp
//!
//!\date 2005-4-13
//! last 2008-1-17
//!
//!\brief 界面系统 richeditbox 控件
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "gui_static.h"
#include "gui_richeditbox.h"
#include "gui_event_define.h"

#define CURSOR_FLICKER_TIME 500  // 光标闪烁间隔

#define GUI_MAX_TRANSFER_LEN	50
#define GUI_DEFAULT_ROW_SPACING_ADDON 0

#define SET_FONT_SIZE	 0
#define SET_FONT_COLOR   1
#define SET_FONT_STYLE	 2
#define SET_KEY_START	 3     //设置关键字  关键字不能超过多于两行或者100字节
#define SET_KEY_END		 4
#define SET_LINK_START	 5	   //设置超链接信息
#define SET_LINK_END     6     

namespace vEngine {
//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
GUIRichEditBox::GUIRichEditBox()
{
	m_pVarMgr->Register(_T("Disable"), &m_bDisable, _T("no yes"));
	m_pVarMgr->Register(_T("ReadOnly"), &m_bReadOnly, _T("no yes"));
	m_pVarMgr->Register(_T("CursorColor"), &m_dwCursorColor);
	m_pVarMgr->Register(_T("CursorTimer"), &m_dwCursorTimer);
	m_pVarMgr->Register(_T("RowSpacing"), &m_nRowSpacing);
	m_pVarMgr->Register(_T("MaxEditTextCount"), &m_nMaxEditTextCount);

	m_dwCursorColor = 0xFF000000;
	m_dwCursorTimer = 100;
	m_nMaxEditTextCount = 20480;
	m_bEditable = TRUE;
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL GUIRichEditBox::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	BOOL bResult = GUIStatic::Init(pSystem, pFather, pXmlElement);

	m_VecCharIntro.clear();

	m_bScrollLast = TRUE;
	m_bEqualTextSize = FALSE;

	// 得到文本高度
	tagPoint ptSize;
	m_pRender->GetTextSize(_T("你"), m_pFont, m_ptSizeChinese);
	m_pRender->GetTextSize(_T("a"), m_pFont, m_ptSizeLetter);

	m_nTextNormalHeight = m_ptSizeChinese.y;

	if( 0 == m_nRowSpacing )	// 如果未设置行高则设置一个
	{
		// 检测一般文本的高度，并加上GUI_DEFAULT_ROW_SPACING_ADDON
		m_nRowSpacing = m_nTextNormalHeight + GUI_DEFAULT_ROW_SPACING_ADDON;
	}

	m_bShowCursor = FALSE;
	m_bHoldCursor = TRUE;
	m_bCanSelect  = TRUE;

	m_bMiddleRender = FALSE;
	m_bDownLine = FALSE;
	m_dwLinkColor = GT_INVALID;

	m_nSelectStart = 0;	// 选择开始处
	m_nSelectEnd = 0;// 选择结束处
	m_nCursorPos = 0;	// 光标位置
	m_nStartDisplayLine = 0;	// 显示的第一个字符，是实际的第几个字符
	
	m_dwMaxTextCount = 100;
	m_VecCharIntro.resize(m_dwMaxTextCount+2);

	m_strText.clear();
	m_strText.push_back(0x0d);
	m_strText.push_back(0x0a);
	m_VecCharIntro[0].nByte = 2;
	m_VecCharIntro[0].dwHeight = m_nRowSpacing;
	m_VecCharIntro[1].nByte = -1;
	SendScrollEvent();

	//初始化注册更改字体的转移字符串
	m_mapFont.Add(_T("<color="), SET_FONT_COLOR); // <color=xxx> 或<color=(xxx,xxx,xxx)>
	m_mapFont.Add(_T("<="), SET_FONT_COLOR); // <color=xxx> 或<color=(xxx,xxx,xxx)>

	//如果size>1000那么字体将被设置为斜体,实际大小为size-1000,if = 0,将为默认字体大小
	m_mapFont.Add(_T("<size="), SET_FONT_SIZE);
	m_mapFont.Add(_T("<style="), SET_FONT_STYLE);

	//设置关键字格式(<keystart>关键字<keyend>
	m_mapKey.Add(_T("<keystart>"), SET_KEY_START);
	m_mapKey.Add(_T("<keyend>"), SET_KEY_END);

	//设置超链接格式
	m_mapLink.Add(_T("<linkstart="), SET_LINK_START);   //<linkstart=xxxxxx>yyyyy<linkend>
	//xxxxx表示链接目标信息,yyyy表示显示出来的信息
	m_mapLink.Add(_T("<linkend>"), SET_LINK_END);

	m_dwLastMoveTime = GetTickCount();
	m_dwTransDepth = 0xff000000;

	//test
	AddText(_T("<=(146,243,255)>Series Quest: Sansheng Stone - The Truth in Dreams (2/8)\n	Fortuneteller Chunyun said you need to find the Sansheng Stone to help Zhou Lingyun. Bring this information to Zhou Lingyun in Liuhe (531,470)."));
	AddText(_T("Yicheng Yamen Watchman (2/3)\nReturn to the Yicheng Yamen Watchman <<=(85,245,183)><linkstart=0>Yicheng,662,261<linkend><=(146,243,255)>>."));


	return bResult;
}

//-----------------------------------------------------------------------------
//销毁
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::Destroy()
{
	m_mapPic.ResetIterator();
	tagGUIImage* pPic = NULL;
	while(m_mapPic.PeekNext(pPic))
	{
		m_pRender->DestroyImage(pPic);
	}

	GUIStatic::Destroy();
}

//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::Update()
{
	static DWORD dwLastTime = timeGetTime();
	// 处理光标的显示与闪烁
	if( m_pSystem->GetActive() == this )
	{
		if( timeGetTime() - dwLastTime > m_dwCursorTimer )
		{
			m_bShowCursor = !m_bShowCursor;
			dwLastTime = timeGetTime();
			this->SetRefresh();
		}
	}

	GUIStatic::Update();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::Render()
{
	if( !IsRefresh() )
		return GUIWnd::Render();

	// 渲染底图
	m_pCanvas->rc.SetLTWH(m_ptView, m_ptSize);
	m_pRender->Draw(m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite);

	if( m_bCanSelect == FALSE  )
		m_nSelectEnd = m_nSelectStart;

	INT nS1 = m_nSelectStart;
	INT nS2 = m_nSelectEnd;
	if( m_nSelectStart > m_nSelectEnd )
	{
		std::swap(nS1, nS2);
	}

	//初始化相关数据
	INT nLine = 0;
	INT nHeigth = m_rcText.top;	//从top算起已经用了的高度
	INT nRowHeight = 0;	  //该行 需要的显示高度
	INT nIntelHeight = 0; //该行实际可用的高度

	INT nLeft = m_ptView.x + m_rcText.left; //行左
	INT nCursorLeft = m_ptView.x + m_rcText.left;

	INT nTextSize = m_strText.size();
	{
		//查找初始行的高度
		INT nPos = m_strText.find_first_of(0x0d, 0);
		ASSERT( nPos >= 0 );
		nRowHeight = m_VecCharIntro[nPos].dwHeight;

		if( TRUE == m_bMiddleRender )
		{
			//居中对齐，计算行初始渲染位置
			nLeft += (m_rcText.right - m_rcText.left - m_VecCharIntro[nPos].nCharSize) / 2;
			nCursorLeft += (m_rcText.right - m_rcText.left - m_VecCharIntro[nPos].nCharSize) / 2;
		}

		if( m_rcText.bottom - m_rcText.top -nHeigth >= nRowHeight )
			nIntelHeight = nRowHeight;
		else
			nIntelHeight = m_rcText.bottom - m_rcText.top - nHeigth;

		if( m_nStartDisplayLine > nLine )
		{
			nIntelHeight = 0;
		}
	}

	//设置文字属性
	DWORD dwTextColor = m_dwTextColor;	 
	tagGUIFont* pFont = m_pFont;
	DWORD dwTextShade = m_dwShadeColor;
	dwTextColor |= m_dwTransDepth;

	INT nStep = 1;
	TCHAR szTemp[3];
	ZeroMemory(szTemp, sizeof(szTemp));

	//文字渲染相关
	tstring strRender;
	INT nTextWidth = 0;
	BOOL bTransparent = FALSE;
	INT nTextHeight = 0;


	BOOL bLink = FALSE;		//指示当前是否是超链接
	DWORD dwLinkColor = GT_INVALID;		//当前超链接鼠标经过的文字颜色
	//关键字的下划线相关
	INT nDownLineLeft = -1;
	INT nDownLineWidth = 0;

	//渲染主循环
	for(INT n=0; n<nTextSize; n+=nStep)
	{
		szTemp[0] = m_strText[n];
		nStep = m_VecCharIntro[n].nByte;

		if( m_bHoldCursor && m_bShowCursor && m_nCursorPos == n )	//显示光标
		{
			m_pCanvas->rc.left  = nCursorLeft-1;
			m_pCanvas->rc.right  = m_pCanvas->rc.left + 2;
			m_pCanvas->rc.bottom = nHeigth + nIntelHeight + m_ptView.y;
			m_pCanvas->rc.top = m_pCanvas->rc.bottom - m_nTextNormalHeight;
			m_pRender->Draw(m_pCanvas, NULL, m_dwCursorColor, m_eAlphaWrite);
		}

		if( nStep == -1 || nStep == 0 )
			break;

		if( szTemp[0] == 0x0d )	//换行符
		{
			//换行之前先显示之前的文字
			if( strRender.size() > 0 )
			{
				INT nbottom = nHeigth + m_ptView.y + nIntelHeight;
				INT nRHeight = 0;

				if( nIntelHeight >= m_nTextNormalHeight )
					nRHeight = m_nTextNormalHeight;
				else
					nRHeight = nIntelHeight;

				if( GT_INVALID == dwLinkColor )
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwTextColor, pFont, bTransparent);
				}
				else
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwLinkColor, pFont, bTransparent);
				}

				nLeft += nTextWidth;

				strRender.clear();
				nTextWidth = 0;
			}

			//显示关键字需要显示的下划线
			if( (TRUE == m_bDownLine || TRUE == bLink)
				&& nDownLineWidth > 0 )
			{
				m_pCanvas->rc.left = nDownLineLeft;
				m_pCanvas->rc.top = nHeigth + m_ptView.y + nIntelHeight;
				m_pCanvas->rc.right = nDownLineLeft+nDownLineWidth;
				m_pCanvas->rc.bottom = nHeigth + m_ptView.y + nIntelHeight;
				m_pRender->Line(m_pCanvas, dwTextColor);

				nDownLineLeft = m_ptView.x + m_rcText.left;
				nDownLineWidth = 0;
			}

			nLine++;	//递增一行
			if( m_nStartDisplayLine > nLine )
			{
				nIntelHeight = 0;
				continue;
			}

			// 初始化新的一行-----------------------------------------------------------
			// 行左可用的起始位置
			nLeft = m_ptView.x + m_rcText.left; 
			nCursorLeft = m_ptView.x + m_rcText.left;

			//->add by bwen
			if(nDownLineLeft!=-1)
				nDownLineLeft = m_ptView.x + m_rcText.left;
			//<-

			//查找一下行的高度
			if( n+1 < nTextSize )
			{
				INT nPos = m_strText.find_first_of(0x0d, n+1);
				if( nPos > 0 )
				{
					nRowHeight = m_VecCharIntro[nPos].dwHeight;

					if( TRUE == m_bMiddleRender )
					{
						//居中对齐，计算行初始渲染位置
						nLeft += (m_rcText.right-m_rcText.left - m_VecCharIntro[nPos].nCharSize) / 2;

						if( nDownLineLeft != -1 )
							nDownLineLeft = nLeft;	 //重新设置

						nCursorLeft += (m_rcText.right-m_rcText.left - m_VecCharIntro[nPos].nCharSize) / 2;
					}
				}
			}

			nHeigth += nIntelHeight;

			if( nHeigth >= m_rcText.bottom || m_rcText.bottom-nHeigth <= GUI_DEFAULT_ROW_SPACING_ADDON ) 
				break;	//已经没有高度可显示该行了

			if( m_rcText.bottom-nHeigth >= nRowHeight )
				nIntelHeight = nRowHeight;
			else
			{
				//可用的高度不够,不能完全显示该行文字或者图片
				//--注：可拖动窗口不去挤最后一行画，zewu 06-3-1
				//--注2：根据bug表，都不去挤最后一行了，yanliang 06-05-11
				break;// 没有可用的高度就不要齐在一起渲染文字
			}
		}
		else if( szTemp[0] < 0 || nStep == 1 ) //是文字
		{
			if( m_nStartDisplayLine > nLine )	
				continue;

			DWORD bTrans = TRUE;
			if( n >= nS1 && n < nS2 )	// 处理被选择区域的渲染
			{
				bTrans = FALSE;
			}

			if( bTransparent != bTrans && nTextWidth != 0 )
			{
				//是否可以显示
				INT nbottom = nHeigth + m_ptView.y + nIntelHeight;
				INT nRHeight = 0;

				if( nIntelHeight >= m_nTextNormalHeight )
					nRHeight = m_nTextNormalHeight;
				else
					nRHeight = nIntelHeight;

				if( GT_INVALID == dwLinkColor )
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwTextColor, pFont, bTransparent);
				}
				else
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwLinkColor, pFont, bTransparent);
				}

				nLeft += nTextWidth;
				strRender.clear();
				nTextWidth = 0;
			}

			bTransparent = bTrans;

			strRender.push_back(szTemp[0]);
			if( szTemp[0] < 0 )
			{
				//双字节字符
				ASSERT( nStep == 2 );
				strRender.push_back( m_strText[n+1] );
			}

			nTextWidth += m_VecCharIntro[n].nCharSize;
			nTextHeight = m_VecCharIntro[n].dwHeight;
			nCursorLeft += m_VecCharIntro[n].nCharSize;

			//下划线得统计
			if( nDownLineLeft != -1 )
			{
				nDownLineWidth += m_VecCharIntro[n].nCharSize;
			}
		}
		else if( nStep > 1 )
		{
			//转义字符串之前,我们先渲染之前的文字
			if( nTextWidth > 0 )
			{
				INT nbottom = nHeigth + m_ptView.y + nIntelHeight;
				INT nRHeight = 0;

				if( nIntelHeight >= m_nTextNormalHeight )
					nRHeight = m_nTextNormalHeight;
				else
					nRHeight = nIntelHeight;

				if( GT_INVALID == dwLinkColor )
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwTextColor, pFont, bTransparent);
				}
				else
				{
					RenderText(strRender.c_str(), nLeft, nbottom, nTextWidth, 
						nRHeight, dwLinkColor, pFont, bTransparent);
				}

				nLeft += nTextWidth;
				strRender.clear();
				nTextWidth = 0;
			}

			// 转义字符串
			if( 0 == m_VecCharIntro[n].nCharSize )
			{
				//不占宽度，说明是字体设置
				INT nFont = (INT)(m_VecCharIntro[n].pPic);
				if( nFont == SET_FONT_COLOR )
				{
					dwTextColor = m_VecCharIntro[n].dwReserve;
					dwTextColor |= m_dwTransDepth;
				}
				else if( nFont == SET_FONT_SIZE )
					pFont = (tagGUIFont*)m_VecCharIntro[n].dwReserve;
				else if( nFont == SET_FONT_STYLE )
					dwTextShade = m_VecCharIntro[n].dwReserve;
				else if( nFont == SET_KEY_START )
				{
					//初始化下划线
					nDownLineLeft = nCursorLeft;
					nDownLineWidth = 0;

				}
				else if( nFont == SET_KEY_END )
				{
					//划下划线

					//显示关键字需要显示的下划线
					if( TRUE == m_bDownLine && nDownLineWidth > 0 )
					{
						m_pCanvas->rc.left = nDownLineLeft;
						m_pCanvas->rc.top = nHeigth + m_ptView.y + nIntelHeight;
						m_pCanvas->rc.right = nDownLineLeft+nDownLineWidth;
						m_pCanvas->rc.bottom = nHeigth + m_ptView.y + nIntelHeight;
						m_pRender->Line(m_pCanvas, dwTextColor);
					}

					//重新初始化
					nDownLineLeft = -1;
					nDownLineWidth = 0;

				}
				else if( nFont == SET_LINK_START )
				{
					bLink = TRUE;	//超链接开始
					if( n == m_nMouseLinkIndex )
					{
						//当前鼠标处在此超链接的位置
						dwLinkColor = m_dwLinkColor;
					}

					//初始化下划线
					nDownLineLeft = nCursorLeft;
					nDownLineWidth = 0;	
				}
				else if( nFont == SET_LINK_END )
				{

					if( nDownLineWidth > 0 )
					{
						m_pCanvas->rc.left = nDownLineLeft;
						m_pCanvas->rc.top = nHeigth + m_ptView.y + nIntelHeight;
						m_pCanvas->rc.right = nDownLineLeft+nDownLineWidth;
						m_pCanvas->rc.bottom = nHeigth + m_ptView.y + nIntelHeight;
						m_pRender->Line(m_pCanvas, dwTextColor);
					}

					bLink = FALSE;	 //超链接结束
					dwLinkColor = GT_INVALID;

					//重新初始化下划线数据
					nDownLineLeft = -1;
					nDownLineWidth = 0;
				}


				continue;
			}

			if( m_nStartDisplayLine > nLine )
				continue;

			//渲染该图片
			m_VecCharIntro[n].pPic->rc.left = 0;
			m_pCanvas->rc.left = nLeft;
			m_VecCharIntro[n].pPic->rc.right = m_VecCharIntro[n].pPic->rc.left + m_VecCharIntro[n].pPic->ptSize.x;
			m_pCanvas->rc.right = m_pCanvas->rc.left + m_VecCharIntro[n].nCharSize;
			m_VecCharIntro[n].pPic->rc.top = 0;

			if( nIntelHeight >= (INT)m_VecCharIntro[n].pPic->ptSize.y )
			{
				m_pCanvas->rc.top = nHeigth + m_ptView.y + GUI_DEFAULT_ROW_SPACING_ADDON + \
					nRowHeight - m_VecCharIntro[n].pPic->ptSize.y;
				m_VecCharIntro[n].pPic->rc.bottom = m_VecCharIntro[n].pPic->rc.top + m_VecCharIntro[n].pPic->ptSize.y;
				m_pCanvas->rc.bottom = nHeigth + m_ptView.y + nIntelHeight;
			}
			else
			{
				m_pCanvas->rc.top = nHeigth + m_ptView.y + GUI_DEFAULT_ROW_SPACING_ADDON;
				m_VecCharIntro[n].pPic->rc.bottom = m_VecCharIntro[n].pPic->rc.top + nIntelHeight;
				m_pCanvas->rc.bottom = nHeigth + m_ptView.y + nIntelHeight;
			}

			// 处理被选择区域的渲染，在图片上面绘制颜色块表示被选中,add by zewu 2005-2-9 20:49
			if( n >= nS1 && n < nS2 )	
				m_pRender->Draw(m_pCanvas, NULL, 0xFFFFFFFF, m_eAlphaWrite);//注意渲染顺序，必须在渲染图片之前才能作为背景色

			m_pRender->Draw(m_pCanvas, m_VecCharIntro[n].pPic, m_dwPicColor|0xFF000000, EGUIAW_Add);

			nLeft += m_VecCharIntro[n].nCharSize;
			nCursorLeft += m_VecCharIntro[n].nCharSize;

			//下划线得统计
			if( nDownLineLeft != -1 )
				nDownLineWidth += m_VecCharIntro[n].nCharSize;
		}
		//....
	}

	return GUIWnd::Render();
}


//-----------------------------------------------------------------------------
//渲染文字
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::RenderText(LPCTSTR szText, INT nLeft, INT nBottom, INT nWidth, INT nHeight,
					   DWORD dwColor, tagGUIFont* pFont, BOOL bTransparent)
{
	m_pCanvas->rc.left = nLeft;
	m_pCanvas->rc.right = m_pCanvas->rc.left + nWidth;
	m_pCanvas->rc.bottom = nBottom;
	m_pCanvas->rc.top = m_pCanvas->rc.bottom - nHeight;

	DWORD dwMyColor = dwColor;
	if( FALSE == bTransparent )	// 处理被选择区域的渲染
	{
		DWORD dwAlpha = dwColor & 0xff000000;
		dwMyColor = ( (~dwColor) & (0x00ffffff) ); // 反色
		dwMyColor |= dwAlpha;//保证Alpha值不变

		m_pRender->Draw(m_pCanvas, NULL, dwColor, m_eAlphaWrite);
		m_pRender->Text(m_pCanvas, szText, pFont, dwMyColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}
	else
	{
		m_pRender->Text(m_pCanvas, szText, pFont, dwColor, m_dwShadeColor, m_eTextAlign, m_bTextWriteAlpha);
	}

}

//-----------------------------------------------------------------------------
// 基本消息处理
//-----------------------------------------------------------------------------
BOOL GUIRichEditBox::OnInputMessage(tagGUIInputMsg* pMsg)
{
	switch( pMsg->dwType )
	{
	case WM_LBUTTONDOWN:
		{
			m_bShowCursor = TRUE;
			// 找到鼠标点击的字符
			INT nChar = GetCharIndex(pMsg->pt.x, pMsg->pt.y, FALSE);
			if( GT_INVALID != nChar )
			{
				m_nSelectStart = m_nSelectEnd = m_nCursorPos = nChar;	// 取消选择

				while( m_nCursorPos > GetLastDisplayIndex() )
					m_nStartDisplayLine++;
			}

			//是否点击超链接
			BOOL bLink = ClickLink(pMsg);
			//改变光标
			AdptiveCursor(TRUE, bLink);
			this->SetRefresh();
		}
		break;
	case WM_LBUTTONUP:
		{
			//是否点击超链接
			//BOOL bLink = ClickLink(pMsg);
		}
		break;

	case WM_LBUTTONDBLCLK:
		{	//双击
			if( m_bCanSelect == TRUE )
			{
				//不是双击超链接,并且文字可选
				CalSelectForDoubleClick();
				this->SetRefresh();
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( m_pSystem->GetActive() != this )
				break;

			//----------------------------------------------
			// 处理鼠标形状变化,add by zewu 2005-2-9 20:49

			//是否处于超链接的位置
			INT nPos = GetCharIndex(pMsg->pt.x, pMsg->pt.y, TRUE);
			m_nMouseLinkIndex = GetLinkIndex(nPos);

			if( GT_INVALID == m_nMouseLinkIndex )
				AdptiveCursor(FALSE, FALSE );
			else
				AdptiveCursor(FALSE, TRUE);
			//-----------------------------------------------

			if( TRUE == PointInWnd(pMsg->pt) )
			{
				// 处理鼠标拖动选择
				if( FALSE == m_bCanSelect )
					break;

				if( pMsg->dwParam1 == MK_LBUTTON )
				{
					//如果是向左方向选择,则选中状态以左为原则,否则以右为原则
					INT nChar = GetCharIndex(pMsg->pt.x, pMsg->pt.y, TRUE);
					if( GT_INVALID != nChar )
					{ //
						if( nChar < m_nSelectStart )
							m_nSelectEnd = m_nCursorPos = nChar;
						else
						{
							nChar  = GetCharIndex(pMsg->pt.x, pMsg->pt.y, FALSE);
							if( GT_INVALID != nChar )
								m_nSelectEnd = m_nCursorPos = nChar;
						}
						this->SetRefresh();
					}
				}
			}
			else
			{
				if( (pMsg->dwParam1 != MK_LBUTTON ) || (GetTickCount()-m_dwLastMoveTime) < 20 )
					break;

				if( FALSE == m_bCanSelect )
					break;

				m_dwLastMoveTime = GetTickCount();

				if( pMsg->pt.y < m_ptView.y )
				{
					//上移动
					CalCursonForUpDown(TRUE);
					while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
						m_nStartDisplayLine--;

				}
				else if( pMsg->pt.y > m_ptView.y + m_ptSize.y )
				{
					//下移动
					CalCursonForUpDown(FALSE);
					while( m_nCursorPos > GetLastDisplayIndex() )
						m_nStartDisplayLine++;
				}

				m_nSelectEnd = m_nCursorPos;
				SendScrollEvent();
				this->SetRefresh();
			}
		}
		break;
	case WM_MOUSEWHEEL:	// 处理鼠标滚轮
		{
			if( pMsg->dwParam1 & 0x80000000 )	// down
			{
				ScrollText(FALSE);
			}
			else // up
			{
				ScrollText(TRUE);
			}
			SendScrollEvent();
			this->SetRefresh();
		}
		break;
	case WM_KEYDOWN:
		this->SetRefresh();
		return OnMsgForKeyDown(pMsg->dwParam1);

	case WM_SYSCHAR:
	case WM_CHAR:
		{
			this->SetRefresh();
			//首先处理ctrl+ ()的快捷键
			if( pMsg->dwParam1 == ('a' & 0x1F) )	// ctrl+a 大小写无关
			{
				m_nSelectStart = 0;
				m_nSelectEnd = m_strText.size() - 2;
				m_nCursorPos = 0;
			}
			else if( pMsg->dwParam1 == ('c' & 0x1F) )	//ctrl+c  大小写无关
			{
				this->CopyToClipboard();
			}
			else if( pMsg->dwParam1 == ('x' & 0x1F) )	//ctrl+x  大小写无关
			{
				this->CopyToClipboard();

				if( m_bReadOnly )
					break;

				this->DelSelected();
			}
			else if( pMsg->dwParam1 == ('v' & 0x1F) )	//ctrl+v  大小写无关
			{
				if( m_bReadOnly )
					break;

				this->DelSelected();
				this->PasteFromClipboard();
			}

			if( m_bReadOnly )
				break;

			if( pMsg->dwParam1 <= 0x1F && pMsg->dwParam1 > 0 )
			{
				// 必须是有效字符,否则不予显示
				return TRUE;
			}

			// 处理基本文字输入
			// 假如有选择，先删除选择部分
			if( m_nSelectStart != m_nSelectEnd )
				this->DelSelected();

			//不能超过字节上限
			if( (INT)m_strText.size() + 1 < m_nMaxEditTextCount )
			{
				if( m_strText.size() + 1 >= m_dwMaxTextCount )
				{
					m_dwMaxTextCount = m_strText.size() + 2;
					m_VecCharIntro.resize(m_dwMaxTextCount+2);
				}

				InsertText((TCHAR*)&pMsg->dwParam1, m_nCursorPos, m_dwTextColor, m_dwShadeColor);
			}

			while( m_nCursorPos > GetLastDisplayIndex() )
				m_nStartDisplayLine++;

			SendScrollEvent();
			return TRUE;
		}
		break;
	default:
		break;
	}

	return GUIStatic::OnInputMessage(pMsg);
}


//-------------------------------------------------------------------------------------
//就给出的位置判断所处的超链接的index,无则index=-1
//-------------------------------------------------------------------------------------
INT GUIRichEditBox::GetLinkIndex(INT nPos)
{
	INT nStartPos = GT_INVALID;
	INT nEndPos = GT_INVALID;
	//首先在光标位置之前的该行以及上一行查找是否由SET_LINK_START	

	if( nPos == 0 )
		return -1;

	INT nRowStart = m_strText.find_last_of(0x0d, nPos-1);
	if( nRowStart <= 0 )
	{
		nRowStart = 0;
	}
	else
	{
		nRowStart = m_strText.find_last_of(0x0d, nRowStart-1);
		if( nRowStart <= 0 )
			nRowStart = 0;
	}

	//查找<keystart>
	INT i = 0;
	for( i=nPos-1; i>=nRowStart; i--)
	{
		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_LINK_END )
		{
			//先找到keyend,则无效
			break;
		}

		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_LINK_START )
		{
			nStartPos = i + m_VecCharIntro[i].nByte;
			break;
		}
	}
	if( nStartPos == GT_INVALID )
	{
		//未找到
		return -1;
	}

	//首先在光标位置之后的该行以及下一行查找是否有<keyend>

	INT nRowEnd = m_strText.find_first_of(0x0d, nPos);
	ASSERT( nRowEnd >= 0 );

	INT nTemp =  m_strText.find_first_of(0x0d, nRowEnd+1);
	if( nTemp >= 0 )
	{
		nRowEnd = nTemp;
	}

	//查找<keyend>
	for( INT j=nPos; j<nRowEnd; j++)
	{
		if( m_VecCharIntro[j].nCharSize == 0 && (INT)(m_VecCharIntro[j].pPic) == SET_LINK_START )
		{
			break;
		}

		if( m_VecCharIntro[j].nCharSize == 0 && (INT)(m_VecCharIntro[j].pPic) == SET_LINK_END )
		{
			nEndPos = j;
			break;
		}
	}
	if( nEndPos == GT_INVALID )
	{
		//未找到
		return -1;
	}

	if( nEndPos <= nStartPos )
	{
		//找到了也不合法,可能关键字为空
		return -1;
	}

	return i;
}


//-------------------------------------------------------------------------------------
//检测是否点击了超链接
//-------------------------------------------------------------------------------------
BOOL GUIRichEditBox::ClickLink(tagGUIInputMsg* pMsg)
{
	//查找是否是超链接
	m_nMouseLinkIndex = GetLinkIndex(m_nCursorPos);

	if( -1 == m_nMouseLinkIndex )
		return FALSE;

	tagGUIEvent event(this, EGUIE_RichLinkClick, pMsg);
	event.dwParam1 = pMsg->dwType;
	event.dwParam2 = m_nMouseLinkIndex;

	SendEvent(&event);	//发送事件让上层处理
	return TRUE;

}


//-------------------------------------------------------------------------------------
//根据index得到超级链接指向的内容(返回false表示无此超链), 
//-------------------------------------------------------------------------------------
BOOL GUIRichEditBox::GetLinkContent(INT nIndex,tstring &strText)
{
	if( nIndex < 0 || (DWORD)nIndex >= m_strText.size() )
		return FALSE;

	if( m_VecCharIntro[nIndex].nCharSize != 0 
		|| (INT)(m_VecCharIntro[nIndex].pPic) != SET_LINK_START )
	{
		return FALSE;
	}

	INT nTextLen = m_VecCharIntro[nIndex].nByte;
	ASSERT( nTextLen >= 1 );

	INT i = 0;
	for(i=nIndex; i<nIndex+nTextLen-1; i++)
	{
		if( m_strText[i] == '=' )
			break;
	}

	if( i >= nIndex+nTextLen-1 )
		return FALSE;


	INT nLen = nTextLen + nIndex - 2 -  i;
	strText = m_strText.substr(i+1, nLen).c_str();
	return TRUE;
}


//-------------------------------------------------------------------------------------
//得到光标位置处的key, szKeyWord由外部提供空间
//-------------------------------------------------------------------------------------
BOOL GUIRichEditBox::GetCursorKey(LPTSTR szKeyWord, INT& nStartPos, INT& nEndPos)
{
	nStartPos = GT_INVALID;
	nEndPos = GT_INVALID;

	//首先在光标位置之前的该行以及上一行查找是否由<keystart>	

	if( m_nCursorPos == 0 )
		return FALSE;

	INT nRowStart = m_strText.find_last_of(0x0d, m_nCursorPos-1);
	if( nRowStart <= 0 )
	{
		nRowStart = 0;
	}
	else
	{
		nRowStart = m_strText.find_last_of(0x0d, nRowStart-1);
		if( nRowStart <= 0 )
			nRowStart = 0;
	}

	//查找<keystart>
	for( INT i=m_nCursorPos-1; i>=nRowStart; i--)
	{
		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_KEY_END )
		{
			//先找到keyend,则无效
			break;
		}

		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_KEY_START )
		{
			nStartPos = i + m_VecCharIntro[i].nByte;
			break;
		}
	}
	if( nStartPos == GT_INVALID )
	{
		//未找到
		return FALSE;
	}

	//首先在光标位置之后的该行以及下一行查找是否有<keyend>

	INT nRowEnd = m_strText.find_first_of(0x0d, m_nCursorPos);
	ASSERT( nRowEnd >= 0 );

	INT nTemp =  m_strText.find_first_of(0x0d, nRowEnd+1);
	if( nTemp >= 0 )
	{
		nRowEnd = nTemp;
	}

	//查找<keyend>
	for( INT i=m_nCursorPos; i<nRowEnd; i++)
	{
		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_KEY_START )
		{
			break;
		}

		if( m_VecCharIntro[i].nCharSize == 0 && (INT)(m_VecCharIntro[i].pPic) == SET_KEY_END )
		{
			nEndPos = i;
			break;
		}
	}
	if( nEndPos == GT_INVALID )
	{
		//未找到
		return FALSE;
	}

	if( nEndPos <= nStartPos )
	{
		//找到了也不合法,可能关键字为空
		return FALSE;
	}

	//拷贝关键字
	_tcsncpy(szKeyWord, m_strText.substr(nStartPos, nEndPos-nStartPos+1).c_str(), nEndPos-nStartPos);
	szKeyWord[nEndPos-nStartPos] = 0;

	return TRUE;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::CalSelectForDoubleClick()
{
	//首先查找该关键字
	TCHAR szKeyWord[100];
	if( TRUE == GetCursorKey(szKeyWord, m_nSelectStart, m_nSelectEnd) )
		return ;

	m_nSelectStart = m_nSelectEnd = m_nCursorPos;

	//先找到该行的行首和行尾
	INT nRowHead = 0;
	INT nRowTail = 0;
	if( m_nCursorPos == 0 )
		nRowHead = 0;
	else
	{
		nRowHead = m_strText.find_last_of(0x0d, m_nCursorPos-1);
		if( nRowHead < 0 )
			nRowHead = 0;
		else 
			nRowHead += m_VecCharIntro[nRowHead].nByte;
	}
	nRowTail = m_strText.find_first_of(0x0d, m_nCursorPos);
	ASSERT( nRowTail >= 0 );

	//先往后面选择
	INT nStep = 1;
	TCHAR ch = 0;
	INT n = 0;
	for(n=m_nCursorPos; n < nRowTail; n+=nStep )
	{
		ch = m_strText.at(n);
		if( ch == _T(' ') || m_VecCharIntro[n].nCharSize == 0 )
			break;

		nStep = m_VecCharIntro[n].nByte;
	}
	m_nSelectEnd = n;

	//往前面选择
	if( m_nCursorPos == 0 )
		m_nSelectStart = 0;
	else 
	{
		for(n=m_nCursorPos-1; n >= nRowHead; n--)
		{
			ch = m_strText.at(n);
			if( ch == _T(' ') || 
				(m_VecCharIntro[n].nCharSize == 0 && m_VecCharIntro[n].nByte != GT_INVALID ) )
			{
				m_nSelectStart = n + m_VecCharIntro[n].nByte;
				break;
			}

			if( n == nRowHead )
			{
				m_nSelectStart = n;
				break;
			}
		}
	}
	m_nCursorPos = m_nSelectEnd;
}


//-----------------------------------------------------------------------------
// 被设置为焦点
//-----------------------------------------------------------------------------
GUIWnd* GUIRichEditBox::OnActive()
{
	// 打开输入法
//	m_pSystem->OpenCloseIme( TRUE );

	// 设置焦点在输入框标志
//	m_pSystem->SetFocusInEditBox( TRUE );

	// 光标移动到字符串末尾
	m_bShowCursor = TRUE;
	return GUIStatic::OnActive();

}


//-----------------------------------------------------------------------------
// 丢失焦点
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::OnDeactive(GUIWnd* pWnd)
{
	// 取消选择
	m_bShowCursor = FALSE;
	this->SetRefresh();

	// 关闭输入法
//	m_pSystem->OpenCloseIme( FALSE );

	// 清空焦点在输入框标志
//	m_pSystem->SetFocusInEditBox( FALSE );
	return GUIStatic::OnDeactive(pWnd);

}

//-----------------------------------------------------------------------------
// 指定点是否在我们窗口内
//-----------------------------------------------------------------------------
BOOL GUIRichEditBox::PointInWnd(tagPoint& pt)
{
	if(m_bShowCursor==FALSE&&m_bCanSelect==FALSE&&m_bReadOnly==TRUE)
	{
		//不显示光标，不可选择,不可拉伸，并且不可编辑，实现虚拟控件行为
		if( m_bDisable || m_bInvisible )
			return FALSE;

		// 检查点是否在我们的所有子窗口内
		std::list<GUIWnd*>::iterator it;
		for(it=m_listChild.begin(); it!=m_listChild.end(); ++it)
		{
			if( TRUE == (*it)->PointInWnd(pt) )
				return TRUE;
		}
		return FALSE;
	}
	return GUIStatic::PointInWnd(pt);
}


//-----------------------------------------------------------------------------
// 由view坐标得到最接近字符的序列号
//-----------------------------------------------------------------------------
INT GUIRichEditBox::GetCharIndex(INT nViewX, INT nViewY, BOOL bLeft)
{
	// 找到鼠标点击的字符
	INT nX = nViewX - m_ptView.x - m_rcText.left;
	INT nY = nViewY - m_ptView.y - m_rcText.top;

	//首先判断属于哪一行
	INT nHeight = 0;
	INT nWidth = 0;

	INT nNewLineIndex = GetRowHead(m_nStartDisplayLine);
	while( nY > nHeight && nNewLineIndex < (INT)m_strText.size() )
	{
		nNewLineIndex = m_strText.find_first_of(0x0d, nNewLineIndex);
		ASSERT( nNewLineIndex >= 0 );
		nHeight += m_VecCharIntro[nNewLineIndex].dwHeight;
		nWidth = m_VecCharIntro[nNewLineIndex].nCharSize;

		nNewLineIndex += m_VecCharIntro[nNewLineIndex].nByte;
	}

	if( nY > nHeight )
	{ 
		//点到字符的后面了，当作是最后一个字符
		return m_strText.size()-2;
	}

	INT nStartPos = 0;
	if( nNewLineIndex <=2 )
		nStartPos = 0;
	else 
	{
		nStartPos = m_strText.find_last_of(0x0d, nNewLineIndex-3);
		if( nStartPos < 0 )
			nStartPos = 0;
		else
			nStartPos += m_VecCharIntro[nStartPos].nByte;
	}


	INT nLeft = 0;
	if( m_bMiddleRender == TRUE )
		nLeft = (m_rcText.right - m_rcText.left - nWidth) / 2;

	if( nX < nLeft )	//左边界阿
		return nStartPos;

	INT n = 0;
	for(n=nStartPos; n<nNewLineIndex-2; )
	{
		if( TRUE == bLeft )
		{
			if( nLeft <= nX && nLeft+m_VecCharIntro[n].nCharSize > nX )
				return n;
		}
		else
		{
			if( nLeft < nX && nLeft+m_VecCharIntro[n].nCharSize >= nX )
				return n+m_VecCharIntro[n].nByte;
		}

		nLeft += m_VecCharIntro[n].nCharSize;
		n += m_VecCharIntro[n].nByte;
	}

	return n;
}


//-----------------------------------------------------------------------------
//删除文本
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::ClsText()
{
	m_strText.clear();

	m_VecCharIntro.clear();
	m_VecCharIntro.resize(m_dwMaxTextCount+2);
	m_strText.push_back(0x0d);
	m_strText.push_back(0x0a);
	m_VecCharIntro[0].nByte = 2;
	m_VecCharIntro[0].dwHeight = m_nRowSpacing;
	m_VecCharIntro[1].nByte = -1;

	m_nSelectStart = 0;	// 选择开始处
	m_nSelectEnd = 0;// 选择结束处
	m_nCursorPos = 0;	// 光标位置
	m_nStartDisplayLine = 0;
}


//-----------------------------------------------------------------------------
// 删除选择的字符
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::DelSelected()
{
	if( m_nSelectStart == m_nSelectEnd ) //　没有文本被选择
		return;

	if( m_nSelectStart > m_nSelectEnd )	// 向后选择
		std::swap(m_nSelectStart, m_nSelectEnd);
	INT nInteChar = m_nSelectEnd - m_nSelectStart;

	DeleteText(m_nSelectStart, m_nSelectEnd);
	m_nCursorPos = m_nSelectEnd = m_nSelectStart;
	while( GT_INVALID == GetRowHead(m_nStartDisplayLine) )
		m_nStartDisplayLine--;

	while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
		m_nStartDisplayLine -- ;
	SendScrollEvent();
}


//-----------------------------------------------------------------------------
// 将选定内容拷到剪贴板 
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::CopyToClipboard()
{
	if( m_nSelectStart != m_nSelectEnd )	// 确定有选中的内容
	{
		if( !OpenClipboard(NULL) )
			return;
		EmptyClipboard(); 

		INT nLen = abs(m_nSelectEnd - m_nSelectStart);
		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (nLen+1) * sizeof(TCHAR)); 
		if( NULL == hglbCopy ) 
		{ 
			CloseClipboard();
			return;
		} 

		// Lock the handle and copy the text to the buffer. 
		LPTSTR lptstrCopy = (LPTSTR)GlobalLock( hglbCopy ); 
		INT nStart = m_nSelectStart < m_nSelectEnd ? m_nSelectStart : m_nSelectEnd;
		memcpy(lptstrCopy, &m_strText[nStart], nLen * sizeof(TCHAR)); 
		lptstrCopy[nLen] = (TCHAR)0;    // null character 
		GlobalUnlock(hglbCopy); 

		// Place the handle on the clipboard. 
		SetClipboardData(CF_UNICODETEXT, hglbCopy); 
		CloseClipboard();
	}
}


//-----------------------------------------------------------------------------
// 从剪贴板拷贝文本到当前位置
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::PasteFromClipboard()
{
	if( FALSE == IsClipboardFormatAvailable(CF_UNICODETEXT) ) 
		return; 

	if( FALSE == OpenClipboard(NULL) )
		return; 

	HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT); 
	if( hglb != NULL ) 
	{ 
		LPTSTR lpstr = (LPTSTR)GlobalLock(hglb); 
		if( NULL != lpstr ) 
		{ 
			AddText(lpstr, FALSE);
			GlobalUnlock(hglb); 
		} 
	} 

	CloseClipboard(); 
	return; 
}


//----------------------------------------------------------------------------
//滚动以便能完整显示最后一行
//----------------------------------------------------------------------------
VOID GUIRichEditBox::ScrollForLastLine(BOOL bReset)
{
	if( m_strText.size() <= 2 )
		return;

	if( bReset == TRUE )
	{
		m_nStartDisplayLine = 0;
	}

	m_nSelectStart = m_nSelectEnd = m_nCursorPos = m_strText.size() - 2;
	while( m_nCursorPos > GetLastDisplayIndex() )
		m_nStartDisplayLine++;

	SendScrollEvent();
}


//----------------------------------------------------------------------------
//增加文本bNexLine=TRUE表示从下一行开始，否则则续接到最后一行，
//dwColor=-1表示默认用m_Data的颜色设置
//----------------------------------------------------------------------------
BOOL GUIRichEditBox::AddText(LPCTSTR szText, BOOL bNextLine, DWORD dwColor, DWORD dwShade, 
							  BOOL bMatchPic, BOOL bEqualTextSize)
{
	BOOL bShowLastLine = IsShowLastLine();

	INT nSize = lstrlen(szText);
	if( nSize == 0 )
		return TRUE;

	if( (INT)m_strText.size() + nSize + 10 > m_nMaxEditTextCount )
		return FALSE;

	if( m_strText.size() + nSize + 10 > m_dwMaxTextCount ) // 预留空间给换行符号
	{
		m_dwMaxTextCount = m_strText.size() + nSize + 10;
		m_VecCharIntro.resize(m_dwMaxTextCount+2);
	}

	//暂存全局变量
	BOOL bEqual = m_bEqualTextSize;
	m_bEqualTextSize = bEqualTextSize;

	INT nVirtualPos = m_nCursorPos;
	if( TRUE == m_bReadOnly )
	{
		//只读只代表不能直接输入, 不表示就不能加入文字
		nVirtualPos = m_strText.size() - 2;
	}

	if( TRUE == bNextLine )
	{
		ASSERT( (INT)m_strText.size() > nVirtualPos );
		if( FALSE == NewLine(nVirtualPos) )
		{
			m_bEqualTextSize = bEqual;
			return FALSE;
		}

		nVirtualPos += 2;
	}

	if( dwColor != GT_INVALID && dwColor != m_dwTextColor )
	{
		TCHAR szFontColor[50];
		_stprintf(szFontColor, _T("<color=%ld>"), dwColor);
		InsertText(szFontColor, nVirtualPos, dwColor, dwShade, FALSE, FALSE);
	}


	TCHAR* szInsertText = new TCHAR[nSize+1];
	ZeroMemory(szInsertText, (nSize+1)*sizeof(TCHAR));
	INT nIndex = 0;
	for(INT n=0; n<nSize; n++)
	{
		//处理包含换行的情况
		if( szText[n] == 0x0d || szText[n] == 0x0a)
		{
			InsertText(szInsertText, nVirtualPos, dwColor, dwShade, FALSE, bMatchPic);
			if( FALSE == NewLine(nVirtualPos) )
			{
				m_bEqualTextSize = bEqual;
				delete[](szInsertText);	///Add by zewu 此处直接return会导致内存泄漏
				return FALSE;
			}

			nVirtualPos += 2;
			if( szText[n] == 0x0d && n+1 < nSize && szText[n+1] == 0x0a )
				n++;

			ZeroMemory(szInsertText, (nSize+1)*sizeof(TCHAR));
			nIndex = 0;
		}
		else if( szText[n] == _T('\t') )
		{
			InsertText(_T("    "), nVirtualPos, dwColor, dwShade, TRUE, FALSE);
		}
		else
		{
			szInsertText[nIndex] = szText[n];
			nIndex++;
		}
	}

	//插入字符串
	InsertText(szInsertText, nVirtualPos, dwColor, dwShade, FALSE, bMatchPic);

	//自动滚动
	if( TRUE == m_bScrollLast )
	{
		m_nSelectStart = m_nSelectEnd = m_nCursorPos = nVirtualPos;
		while( m_nCursorPos > GetLastDisplayIndex() )
			m_nStartDisplayLine++;
	}
	SendScrollEvent();

	delete[](szInsertText);
	m_bEqualTextSize = bEqual;
	this->SetRefresh();
	return TRUE;
}


//----------------------------------------------------------------------------
// 注意：szText不能包含换行符!!!,光标的位置要计算清楚
// bLinkLastLine : 是否联系上一行构成关键字
// bMatchPic : 是否匹配图片
//----------------------------------------------------------------------------
VOID GUIRichEditBox::InsertText(LPCTSTR szText, INT& nVirtualPos, DWORD dwColor, 
									DWORD dwShade, BOOL bLinkLastLine, BOOL bMatchPic) 
{
	if( 0 == lstrlen(szText) )
		return;

	if( GT_INVALID == dwColor )
		dwColor = m_dwTextColor;

	if( GT_INVALID == dwShade )
		dwShade = m_dwShadeColor;

	INT nOriginPos = nVirtualPos;
	tstring str(szText);
	MoveCharIntro(nVirtualPos, str.size());
	m_strText.insert(nVirtualPos, str);
	INT nStrPos = nVirtualPos + str.size();

	//可能需要加上最后的换行符
	INT nStartPos = m_strText.find_last_of(0x0d, nOriginPos);
	if( nStartPos < 0 )
		nStartPos = 0;
	else
	{
		if( nStartPos == 0 )
		{
			nStartPos++;
			if( nStartPos+1 < (INT)m_strText.size() && m_strText[nStartPos] == 0x0a )
				nStartPos++;
		}
		else if( bLinkLastLine == TRUE )
		{
			nStartPos = m_strText.find_last_of(0x0d, nStartPos-1);
			if( nStartPos < 0 )
				nStartPos = 0;
			else 
			{
				nStartPos++;
				if( nStartPos+1 < (INT)m_strText.size() && m_strText[nStartPos] == 0x0a )
					nStartPos++;
			}
		}
		else
		{
			nStartPos++;
			if( nStartPos+1 < (INT)m_strText.size() && m_strText[nStartPos] == 0x0a )
				nStartPos++;
		}
	}

	INT nEndPos = m_strText.find_first_of(0x0d, nVirtualPos);
	ASSERT( nEndPos > 0 );

	RecalCharIntro(nStartPos, nEndPos, nStrPos, nVirtualPos, bMatchPic);
}


//----------------------------------------------------------------------------
//计算光标左右移动时的光标位置
//----------------------------------------------------------------------------
VOID GUIRichEditBox::CalCursorForLeftRight(BOOL bLeft)
{
	if( TRUE == bLeft )
	{
		if( m_nCursorPos == 0 )
			return;

		INT nPos = m_nCursorPos - 1;
		while( nPos >= 0 )
		{
			if( m_VecCharIntro[nPos].nByte != -1 )
			{
				m_nCursorPos = nPos;
				break;
			}
			nPos--;
		}
	}
	else 
	{
		if( m_nCursorPos >= (INT)m_strText.size()-2 )
			return;

		m_nCursorPos += m_VecCharIntro[m_nCursorPos].nByte;
	}

	return;
}


//----------------------------------------------------------------------------
//计算光标上下移动时新的光标位置
//----------------------------------------------------------------------------
VOID GUIRichEditBox::CalCursonForUpDown(BOOL bUp)
{
	if( TRUE == bUp )
	{
		if( m_nCursorPos == 0)
			return;

		INT nPreLineEnd = m_strText.find_last_of(0x0d, m_nCursorPos-1);
		if( nPreLineEnd < 0 )
			return;  //因为光标所在已经是最顶行
		INT nIntelIndex = m_nCursorPos - (nPreLineEnd+m_VecCharIntro[nPreLineEnd].nByte);	
		ASSERT( nIntelIndex >= 0 );
		INT nPreLineBeing = 0;
		if( nPreLineEnd > 0 )
			nPreLineBeing = m_strText.find_last_of(0x0d, nPreLineEnd-1);
		if( nPreLineBeing < 0 )
			nPreLineBeing = 0;
		else
			nPreLineBeing += m_VecCharIntro[nPreLineBeing].nByte;
		if( nPreLineEnd-nPreLineBeing > nIntelIndex )
		{
			for(int i= 0; i<=nIntelIndex; )
			{
				m_nCursorPos = nPreLineBeing + i;
				i += m_VecCharIntro[m_nCursorPos].nByte;
			}
		}
		else
			m_nCursorPos = nPreLineEnd;
	}
	else
	{//光标下移
		INT nBegin = 0;
		if( m_nCursorPos > 0 )
		{	
			nBegin = m_strText.find_last_of(0x0d, m_nCursorPos-1);
			if( nBegin < 0 )
				nBegin = 0;
			else
				nBegin += m_VecCharIntro[nBegin].nByte;
		}
		else
			nBegin = 0;

		INT nIntelIndex = m_nCursorPos - nBegin;	
		ASSERT( nIntelIndex >= 0 );

		INT nNextLineBegin = m_strText.find_first_of(0x0d, m_nCursorPos);
		if( nNextLineBegin == m_strText.size()-2 )
			return;	//已经是在最后一行

		nNextLineBegin += m_VecCharIntro[nNextLineBegin].nByte;
		INT nNextLineEnd = m_strText.find_first_of(0x0d, nNextLineBegin);
		ASSERT( nNextLineEnd >= 0 );

		if( nNextLineEnd-nNextLineBegin > nIntelIndex )
		{
			for(int i= 0; i<=nIntelIndex; )
			{
				m_nCursorPos = nNextLineBegin + i;
				i += m_VecCharIntro[m_nCursorPos].nByte;
			}
		}
		else
			m_nCursorPos = nNextLineEnd;
	}
}


//----------------------------------------------------------------------------------------------
//注册转义字符对应的图片,规定必须是两个字母组合而成
//----------------------------------------------------------------------------------------------
BOOL GUIRichEditBox::RegisterPic(LPCTSTR szStr, LPCTSTR szPicFileName)
{
	INT nLen = lstrlen(szStr);
	if( nLen <= 1 )
	{
		ASSERT( FALSE && "该转义字符串过短" );
		return FALSE;
	}
	//必须保证全部是单字节
	for(INT i=0; i<nLen; i++)
	{
		ASSERT( szStr[i] > 0 && "转义字符串不能含有双字节" );
	}

	tagGUIImage* pPic = m_mapPic.Peek(tstring(szStr));
	if( P_VALID(pPic) )
	{
		ASSERT( FALSE && "该转义字符串已经被注册" );
		return FALSE;
	}

	tagRect rc;
	pPic = m_pRender->CreateImage(szPicFileName, rc);
	if( !P_VALID(pPic) )
	{
		ASSERT( FALSE && "richeditbox load image failed!" );
		return FALSE;
	}

	m_mapPic.Add(tstring(szStr), pPic);
	return TRUE;
}


//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
VOID GUIRichEditBox::MoveCharIntro(INT nPos, INT IncSize)
{
	if( IncSize > 0 )
	{
		//把前面的往后挪
		tagCharIntro var;
		ZeroMemory(&var, sizeof(var));
		m_VecCharIntro.insert(m_VecCharIntro.begin() + nPos, IncSize, var);
	}
	else if( IncSize <= 0 )
	{ 
		//把后面的挪到前面
		m_VecCharIntro.erase(m_VecCharIntro.begin() + nPos, m_VecCharIntro.begin() + (nPos-IncSize));
	}
}


//----------------------------------------------------------------------------------------------
//在某处换行
//----------------------------------------------------------------------------------------------
BOOL GUIRichEditBox::NewLine(INT nPos)
{
	if( (INT)m_strText.size() + 2 >= m_nMaxEditTextCount )
		return FALSE;

	if( m_strText.size() + 2 >= m_dwMaxTextCount )
	{
		m_dwMaxTextCount = m_strText.size() + 4;
		m_VecCharIntro.resize(m_dwMaxTextCount + 2);
	}


	MoveCharIntro(nPos, 2);

	TCHAR szNewLine[3];
	ZeroMemory(szNewLine, 3*sizeof(TCHAR));
	szNewLine[0] = 0x0d;
	szNewLine[1] = 0x0a;

	m_strText.insert(nPos, szNewLine);

	m_VecCharIntro[nPos].nByte = 2;
	m_VecCharIntro[nPos].dwHeight = m_nRowSpacing;
	m_VecCharIntro[nPos+1].nByte = -1;

	INT nFirstHead = 0;
	if( nPos > 0 )
		nFirstHead = m_strText.find_last_of(0x0d, nPos-1);
	if( nFirstHead < 0 )
		nFirstHead = 0;
	else 
		nFirstHead += m_VecCharIntro[nFirstHead].nByte;
	INT nLastHead = m_strText.find_first_of(0x0d, nPos+2);
	ASSERT( nLastHead > 0 );
	if(nLastHead==std::string::npos)
	{
		return FALSE;
	}

	INT nHeight = 0;
	INT nWidth = 0;

	INT nStep = 1;
	for(INT n=nFirstHead; n<nPos; n+=nStep)
	{
		if( (INT)m_VecCharIntro[n].dwHeight+GUI_DEFAULT_ROW_SPACING_ADDON > nHeight )
			nHeight = m_VecCharIntro[n].dwHeight+GUI_DEFAULT_ROW_SPACING_ADDON;

		nWidth += m_VecCharIntro[n].nCharSize;
		
		nStep = m_VecCharIntro[n].nByte;
	}

	m_VecCharIntro[nPos].dwHeight = nHeight;
	if( nHeight > m_nRowSpacing )
		m_VecCharIntro[nPos].dwHeight = nHeight;
	else
		m_VecCharIntro[nPos].dwHeight = m_nRowSpacing;

	m_VecCharIntro[nPos].nCharSize = nWidth;

	nHeight = 0;
	nWidth = 0;
	for(INT n=nPos+2; n<nLastHead; n+=nStep)
	{
		if( (INT)m_VecCharIntro[n].dwHeight+GUI_DEFAULT_ROW_SPACING_ADDON > nHeight )
			nHeight = m_VecCharIntro[n].dwHeight+GUI_DEFAULT_ROW_SPACING_ADDON;

		nWidth += m_VecCharIntro[n].nCharSize;
		nStep = m_VecCharIntro[n].nByte;
	}

	m_VecCharIntro[nLastHead].dwHeight = nHeight;
	if( nHeight < m_nRowSpacing )
		m_VecCharIntro[nLastHead].dwHeight = m_nRowSpacing;
	else
		m_VecCharIntro[nLastHead].dwHeight = nHeight;

	m_VecCharIntro[nLastHead].nCharSize = nWidth;
	return TRUE;
}


//----------------------------------------------------------------------------------------------
//删除某段字符串
//----------------------------------------------------------------------------------------------
VOID GUIRichEditBox::DeleteText(INT nBeginPos, INT nEndPos)
{
	ASSERT( nBeginPos >= 0 );
	if( nEndPos <= nBeginPos )
		return;
	if( nEndPos > (INT)m_strText.size() -2 )
		return;

	INT nOriginPos = nBeginPos;
	MoveCharIntro(nBeginPos, nBeginPos-nEndPos);
	m_strText.erase(nBeginPos, nEndPos-nBeginPos);
	INT nStartPos = 0;
	if( nBeginPos > 0 )
	{
		nStartPos = m_strText.find_last_of(0x0d, nBeginPos-1);
		if( nStartPos < 0 )
			nStartPos = 0;
		else
		{
			if( nStartPos == 0 )
				nStartPos += m_VecCharIntro[nStartPos].nByte;
			else
			{
				nStartPos = m_strText.find_last_of(0x0d, nStartPos-1);
				if( nStartPos < 0 )
					nStartPos = 0;
				else 
					nStartPos += m_VecCharIntro[nStartPos].nByte;
			}
		}
	}
	else
		nStartPos = 0;

	INT nFinishPos = m_strText.find_first_of(0x0d, nBeginPos);
	ASSERT( nFinishPos >= 0 );

	RecalCharIntro(nStartPos, nFinishPos, nOriginPos, m_nCursorPos, TRUE);
}


//----------------------------------------------------------------------------------------------
// 字符串匹配转义图片
//----------------------------------------------------------------------------------------------
tagGUIImage* GUIRichEditBox::MatchPic(tstring &str, INT& nPos)
{
	tagGUIImage* pPic = (tagGUIImage*)GT_INVALID;
	INT nTextSize = str.size();
	for(INT n=0; n<nTextSize; n++)	
	{
		tstring temp = str.substr(n, nTextSize-n);
		pPic = m_mapPic.Peek( str.substr(n, nTextSize-n) );
		if( P_VALID(pPic) )
		{
			nPos = n;
			return pPic;
		}
	}
	return pPic;
}


//----------------------------------------------------------------------------------------------
// 字符串匹配转义更改文字属性
//----------------------------------------------------------------------------------------------
INT GUIRichEditBox::MatchFont(tstring &str, INT &nPos, DWORD &nValue)
{
	INT nFont = GT_INVALID;

	//先看看是不是设置关键字
	INT nTextSize = str.size();
	if( nTextSize <= 0 )
		return GT_INVALID;

	if( str[0] != '<' )
		return GT_INVALID;

	//不符合关键字
	if( str[nTextSize-1] != '>' )
		return GT_INVALID;


	tstring temp;
	m_mapKey.ResetIterator();
	while( m_mapKey.PeekNext(temp, nFont) )
	{
		nPos = str.find(temp, 0);
		if( nPos != str.npos )
		{
			nValue = 0;
			return nFont;
		}
	}


	m_mapLink.ResetIterator();
	while( TRUE == m_mapLink.PeekNext(temp, nFont) )
	{
		if( nFont != SET_LINK_END )
			continue;

		nPos = str.find(temp, 0);
		if( nPos != str.npos )
		{
			nValue = 0;
			return nFont;
		}
	}

	nFont = GT_INVALID;

	//是否达到基本要求,否则是普通文字
	if( nTextSize <=2 || str[nTextSize-1] != '>' )
		return nFont;

	INT nEqualPos = str.find_last_of('=', nTextSize );
	if( nEqualPos <= 0 || nEqualPos == nTextSize-2 )	//没找到或者紧靠着'>'也无效
		return nFont;

	INT nSubNum = 0;
	for(INT n=nEqualPos-1; n>=0; n--)	
	{
		nSubNum = nEqualPos - n + 1;
		tstring temp = str.substr(n, nSubNum);

		//检测是否颜色字体设置
		nFont = m_mapFont.Peek( str.substr(n, nSubNum) );
		if( nFont != GT_INVALID)
		{
			nSubNum = str.size()-2 + 1 - (nEqualPos+1);
			tstring temp = str.substr(nEqualPos+1, nSubNum);
			if( temp.size() == 0 )
				return GT_INVALID;

			if( nFont == SET_FONT_COLOR )
			{//设置颜色,看设置的值格式为(255,255,255);
				if( temp[0] != _T('(') || temp[temp.size()-1] != _T(')') )
				{
					nValue = _ttol(temp.c_str());
					nPos = n;
					return nFont;
				}
				INT r,g,b;
				r = g = b = 0;
				INT nPos = 1;
				INT nPausePos = temp.find_first_of(_T(','), nPos);
				if( nPausePos < 0 ) return GT_INVALID;
				r = _ttol( temp.substr(nPos, nPausePos-nPos).c_str() );
				nPos = nPausePos + 1;
				nPausePos = temp.find_first_of(_T(','), nPos);
				if( nPausePos < 0 ) return GT_INVALID;
				g = _ttol( temp.substr(nPos, nPausePos-nPos).c_str() );
				nPos = nPausePos + 1;
				if( nPos >= (INT)temp.size()-1 )
					return GT_INVALID;
				b = _ttol( temp.substr(nPos, temp.size()-1-nPos).c_str() );

				nValue = RGB(r,g,b);
			}
			else if( nFont == SET_FONT_SIZE )
			{//设置字符大小
				nValue = _ttol( temp.c_str() );
			}
			else if( nFont == SET_FONT_STYLE )
			{//设置字体
				nValue = _ttol( temp.c_str() );
			}
			else 
			{
				ASSERT( FALSE );
			}
			nPos = n;
			return nFont;
		}

		//检测是否超链接设置
		nFont = m_mapLink.Peek( str.substr(n, nSubNum) );
		if( SET_LINK_START == nFont )
		{
			nPos = n;
			return nFont;
		}
	}
	return nFont;
}

//----------------------------------------------------------------
//当前是否显示完整了最后一行
//----------------------------------------------------------------
BOOL GUIRichEditBox::IsShowLastLine()
{
	INT nHeight = 0;
	INT nLineEnd = 0;
	INT nSaveEnd = 0;
	INT i = 0;
	for(i=GetRowHead(m_nStartDisplayLine); i<(INT)m_strText.size();)
	{
		nLineEnd = m_strText.find_first_of(0x0d, i);
		ASSERT( nLineEnd >= 0 );
		nHeight += m_VecCharIntro[nLineEnd].dwHeight;
		if( nHeight > m_rcText.bottom - m_rcText.top )
		{//显示不完整
			break;	
		}
		i = nLineEnd + m_VecCharIntro[nLineEnd].nByte;
		nSaveEnd = nLineEnd;
	}
	if( i == GetRowHead(m_nStartDisplayLine) )
		return FALSE;	// 一行都无法显示完整

	if( (m_strText.size() - nSaveEnd) <= 2 )
		return TRUE;
	else 
		return FALSE;
}

//----------------------------------------------------------------
//能够完整显示的最后一行的结束符序列
//----------------------------------------------------------------
INT GUIRichEditBox::GetLastDisplayIndex()
{
	INT nHeight = 0;
	INT nLineEnd = 0;
	INT nSaveEnd = 0;
	INT i = 0;
	for(i=GetRowHead(m_nStartDisplayLine); i<(INT)m_strText.size();)
	{
		nLineEnd = m_strText.find_first_of(0x0d, i);
		ASSERT( nLineEnd >= 0 );
		if( nLineEnd < 0 )
			return 0;

		nHeight += m_VecCharIntro[nLineEnd].dwHeight;
		if( nHeight > m_rcText.bottom - m_rcText.top )
		{//显示不完整
			break;	
		}
		i = nLineEnd + m_VecCharIntro[nLineEnd].nByte;
		nSaveEnd = nLineEnd;
	}

	if( i == GetRowHead(m_nStartDisplayLine) )
		nSaveEnd = m_strText.find_first_of(0x0d, i);	//一行都无法显示完整

	return nSaveEnd;
}


//----------------------------------------------------------------------------------------------
//得到该行号行首的字符序列号
//----------------------------------------------------------------------------------------------
INT GUIRichEditBox::GetRowHead(INT nRowIndex)
{
	if( nRowIndex == 0 )
		return 0;
	INT nLine = 0;
	INT nLineEnd = 0;
	for(INT i=0; i<(INT)m_strText.size(); )
	{
		nLineEnd = m_strText.find_first_of(0x0d, i);
		ASSERT(nLineEnd >= 0);
		if(nLineEnd < 0)
			return 0;

		nLine++;
		if( nLine == nRowIndex )
			return nLineEnd + m_VecCharIntro[nLineEnd].nByte;
		i = nLineEnd + m_VecCharIntro[nLineEnd].nByte;
	}
	return GT_INVALID;
}


//---------------------------------------------------------------------------------
//当前有多少行
//---------------------------------------------------------------------------------
INT GUIRichEditBox::GetRowNum(INT& nVisibleRowNum)
{
	INT nRowNum = 0;
	nVisibleRowNum = 0;
	INT nHeight = 0;
	for(INT i=0; i<(INT)m_strText.size(); )
	{
		i = m_strText.find_first_of(0x0d, i);
		ASSERT( i >= 0 );
		if( nRowNum >= m_nStartDisplayLine && nHeight < m_rcText.bottom - m_rcText.top )
		{
			nHeight += m_VecCharIntro[i].dwHeight;

			if( nHeight <= m_rcText.bottom - m_rcText.top )
			{
				//不会包含最后被勉强显示的一行(不够完整高度的目前不会被勉强显示)
				nVisibleRowNum++;
			}
		}
		tagCharIntro* p = &m_VecCharIntro[i];
		i += m_VecCharIntro[i].nByte;
		nRowNum++;
	}
	return nRowNum;
}


//-----------------------------------------------------------------------------
// 处理朋友发的事件
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::SendEvent(tagGUIEvent* pEvent)
{
	switch(pEvent->eEvent)
	{
	case EGUIE_Scroll:
		this->SetCurrentTextRow(pEvent->dwParam1);
		SetRefresh(TRUE);
		break;
	}

	GUIStatic::SendEvent(pEvent);
}


//-----------------------------------------------------------------------------
//发出滚动事件
//-----------------------------------------------------------------------------
BOOL GUIRichEditBox::SendScrollEvent()
{
	// 发出滚动消息
	INT nVisibleRowNum = 0;
	tagGUIEvent event(this, EGUIE_Scroll);
	event.dwParam1 = (DWORD)m_nStartDisplayLine;
	event.dwParam2 = GetRowNum(nVisibleRowNum); 
	event.dwParam3 = nVisibleRowNum;
	OnEvent(&event);

	return TRUE;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void GUIRichEditBox::SetCurrentTextRow(INT nRowIndex )
{
	INT nVisibleRowNum = 0;
	if( nRowIndex >=0 && nRowIndex < GetRowNum(nVisibleRowNum) )
	{
		m_nStartDisplayLine = nRowIndex;
		while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
			CalCursonForUpDown(FALSE);
		while( m_nCursorPos > GetLastDisplayIndex() )
			CalCursonForUpDown(TRUE);
	}
}


//----------------------------------------------------------------------------------
//文字滚动(行为单位)
//----------------------------------------------------------------------------------
DWORD	GUIRichEditBox::ScrollText(BOOL bUp)
{
	if( TRUE == bUp )
	{
		if( m_nStartDisplayLine > 0 )		
		{//显示的不是第一行
			m_nStartDisplayLine--;
			if( m_nCursorPos > GetLastDisplayIndex() )
				CalCursonForUpDown(TRUE);
		}
	}
	else
	{
		INT nLastDisplayIndex = GetLastDisplayIndex();
		if( nLastDisplayIndex < (INT)m_strText.size()-2 )
		{//没有显示到最后一行,可以往下滚动
			m_nStartDisplayLine++;
			if( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
				CalCursonForUpDown(FALSE);
		}
	}

	return m_nStartDisplayLine;
}


//----------------------------------------------------------------------------------
//重新计算始末位置之间的字符序列的信息解析,由于排版的原因,nEndPos可能会向后推移
//----------------------------------------------------------------------------------
VOID GUIRichEditBox::RecalCharIntro(INT nStartPos, INT nEndPos, INT& nCursorPos, INT& nVirtualPos, BOOL bMatchPic)
{
	tagGUIImage* pPic = (tagGUIImage*)GT_INVALID;

	tagPoint ptSize;
	TCHAR szTemp[3];
	ZeroMemory(szTemp, 3*sizeof(TCHAR));

	INT nHeight = m_nRowSpacing;
	INT nWidth = 0;
	INT nSaveWidth = 0;
	INT nSaveHeight = 0;

	INT nStep = 1;
	tstring str;
	tstring strLastLine;
	tstring strTotal;

	tstring strPic;
	tstring strLastLinePic;
	tstring strTotalPic;

	nVirtualPos = nStartPos;

	INT n = 0;
	for(n=nStartPos; n<nEndPos; n+=nStep)
	{
		szTemp[0] = m_strText[n];	

		if( szTemp[0] == 0x0d )
		{
			if( n+1 < (INT)m_strText.size() && m_strText[n+1] == 0x0a )
			{
				nStep = 2;
				m_VecCharIntro[n+1].nByte = -1;		
			}
			else 
				nStep = 1;
			m_VecCharIntro[n].nByte = nStep;
			m_VecCharIntro[n].dwHeight = nHeight;
			m_VecCharIntro[n].nCharSize = nWidth;


			strLastLine = str;
			str.clear();

			if( TRUE == bMatchPic )
			{
				strLastLinePic = str;
				strPic.clear();
			}


			nSaveHeight = nHeight;
			nSaveWidth = nWidth;
			nHeight = m_nRowSpacing;
			nWidth = 0;
			continue;			
		}
		else if( szTemp[0] < 0 )
		{
			//双字节字符
			str.clear();
			strLastLine.clear();  //因为匹配字符串不能包含双字节字符
			strTotal.clear();

			if( TRUE == bMatchPic )
			{
				strPic.clear();
				strLastLinePic.clear();  //因为匹配字符串不能包含双字节字符
				strTotalPic.clear();
			}

			szTemp[1] = m_strText[n+1];

			if( nWidth + m_ptSizeChinese.x > m_rcText.right - m_rcText.left )
			{ 
				//放不下了,只能添加换行符，同时剩下的和下一行联一起，也就是取消两行之间的换行符
				if( n <= nCursorPos) //因为要换行，往后推移2个字符
					nCursorPos += 2; 

				TCHAR szNewLine[3];
				ZeroMemory(szNewLine, 3*sizeof(TCHAR));

				if( m_strText.size() + 2 >= m_dwMaxTextCount)
				{
					m_dwMaxTextCount = m_strText.size() + 4;
					m_VecCharIntro.resize(m_dwMaxTextCount + 2);
				}

				szNewLine[0] = 0x0d;
				szNewLine[1] = 0x0a;
				MoveCharIntro(n, 2);
				m_strText.insert(n, szNewLine);
				m_VecCharIntro[n].nByte = 2;
				m_VecCharIntro[n].dwHeight = nHeight;
				m_VecCharIntro[n].nCharSize = nWidth;
				m_VecCharIntro[n+1].nByte = -1;

				nEndPos += 2;
				nStep = 2;


				nSaveHeight = nHeight;
				nSaveWidth = nWidth;
				nHeight = m_nRowSpacing;
				nWidth = 0;


				continue;
			}

			m_VecCharIntro[n].nByte = 2;
			m_VecCharIntro[n].nCharSize = m_ptSizeChinese.x;
			m_VecCharIntro[n].dwHeight = m_ptSizeChinese.y;
			m_VecCharIntro[n+1].nByte = -1;

			if( m_ptSizeChinese.y+GUI_DEFAULT_ROW_SPACING_ADDON > nHeight )
			{
				nHeight = m_ptSizeChinese.y + GUI_DEFAULT_ROW_SPACING_ADDON;
			}

			nWidth += m_ptSizeChinese.x;
			nStep = 2;

			if( n < nCursorPos && n+nStep >= nCursorPos )
				nVirtualPos = n + nStep;

		}
		else
		{
			//单字节
			if( strLastLine.size() + str.size() >= GUI_MAX_TRANSFER_LEN )
			{
				//匹配的字符串过长了
				if( strLastLine.size() > 0 )
					strLastLine.erase(0, 1);
				else
					str.erase(0, 1);
			}

			if( TRUE == bMatchPic )
			{
				if( strLastLinePic.size() + strPic.size() >= GUI_MAX_TRANSFER_LEN )
				{
					//匹配的字符串过长了
					if( strLastLinePic.size() > 0 )
						strLastLinePic.erase(0, 1);
					else 
						strPic.erase(0, 1);
				}

				strPic.push_back(szTemp[0]);
				strTotalPic = strLastLinePic + strPic;
			}


			if( (strLastLine.size()+str.size()) == 0 )
			{
				//空的匹配字符串
				if( szTemp[0] == _T('<') )
				{
					//'<'是关键字的起始标志,否则肯定不是关键字
					str.push_back(szTemp[0]);
					strTotal = strLastLine + str;
				}
			}
			else if( szTemp[0] != _T('<') )
			{
				//可能符合关键字
				str.push_back(szTemp[0]);
				strTotal = strLastLine + str;
			}
			else
			{
				//新的'<'进来,前面的还没匹配好,那肯定作废
				str.clear();
				strLastLine.clear();

				str.push_back(szTemp[0]);
				strTotal = str;
			}

			INT nPicPos = 0;
			if( !bMatchPic || !P_VALID((pPic=MatchPic(strTotalPic, nPicPos))) )
			{
				//不是图片
				DWORD dwValue = 0;
				INT nFont = MatchFont(strTotal, nPicPos, dwValue);	//匹配字体设置
				if( nFont != GT_INVALID )
				{
					//成功
					INT nFontHead = 0;
					INT nRemoveWidthIndex = 0;
					INT nBytes = strTotal.size() - nPicPos;
					if( nBytes > (INT)str.size() )
					{
						//说明含有上一行的字符,恢复现场
						INT nLastLineEnd = m_strText.find_last_of(0x0d, n);
						ASSERT( nLastLineEnd );
						nHeight = nSaveHeight;
						nWidth = nSaveWidth;
						INT nIntelLineSize = m_VecCharIntro[nLastLineEnd].nByte;
						MoveCharIntro(nLastLineEnd, -nIntelLineSize);
						m_strText.erase(nLastLineEnd, nIntelLineSize);

						if( n<= nCursorPos) //因为要换行，往后推移
							nCursorPos -= nIntelLineSize; 

						n = n - nIntelLineSize;
						nEndPos -= nIntelLineSize;

						nFontHead = n - nBytes + 1;
						INT nLastLineChar = nBytes - str.size();	//上一行参与的字符
						nRemoveWidthIndex = nLastLineEnd;

					}
					else
					{
						nFontHead = n - nBytes + 1;
						nRemoveWidthIndex = n;
					}

					for(INT i=nFontHead+1; i<=n ;i++)
					{
						m_VecCharIntro[i].nByte = -1;
					}

					INT nInteWidth = 0;
					for(INT i=nFontHead; i<nRemoveWidthIndex; i++)
					{
						nInteWidth += m_VecCharIntro[i].nCharSize;
					}
					nWidth -= nInteWidth;

					m_VecCharIntro[nFontHead].nByte = nBytes;
					m_VecCharIntro[nFontHead].nCharSize = 0;
					m_VecCharIntro[nFontHead].dwReserve = dwValue;
					m_VecCharIntro[nFontHead].pPic = (tagGUIImage*)nFont;
					m_VecCharIntro[nFontHead].dwHeight = 0;

					str.clear();
					strLastLine.clear();
					strTotal.clear();

					if( TRUE == bMatchPic )
					{
						strPic.clear();
						strLastLinePic.clear();
						strTotalPic.clear();
					}

					nStep = nBytes;
					n = nFontHead;

					if( n < nCursorPos && n+nStep >= nCursorPos )
						nVirtualPos = n + nStep;

					continue;
				}

				szTemp[1] = 0;

				if( m_bEqualTextSize == TRUE )
					ptSize = m_ptSizeLetter;
				else
					m_pRender->GetTextSize(szTemp, m_pFont, ptSize);

				if( nWidth + ptSize.x > m_rcText.right - m_rcText.left )
				{ 
					//放不下了,只能添加换行符，同时剩下的和下一行联一起，也就是取消两行之间的换行符
					if( n<= nCursorPos) //因为要换行，往后推移2个字符
						nCursorPos += 2; 

					if( m_strText.size() + 2 >= m_dwMaxTextCount)
					{
						m_dwMaxTextCount = m_strText.size() + 4;
						m_VecCharIntro.resize(m_dwMaxTextCount + 2);
					}

					//寻找换行位置
					n=FindBreakLinePos(n);

					//删除行首的空格
					if( m_strText[n] == _T(' ') )
					{
						m_strText.erase(n,1);
						MoveCharIntro(n,-1);
						nEndPos--;
						nCursorPos--;
					}

					TCHAR szNewLine[3];
					ZeroMemory(szNewLine, 3*sizeof(TCHAR));
					szNewLine[0] = 0x0d;
					szNewLine[1] = 0x0a;
					MoveCharIntro(n, 2);
					m_strText.insert(n, szNewLine);
					m_VecCharIntro[n].nByte = 2;
					m_VecCharIntro[n].dwHeight = nHeight;
					m_VecCharIntro[n].nCharSize = nWidth;
					m_VecCharIntro[n+1].nByte = -1;

					nEndPos += 2;
					nStep = 2;

					if( str.size() > 0 )
					{
						str.erase(str.size()-1, 1);
					}
					strLastLine = str;
					str.clear();

					if( TRUE == bMatchPic )
					{
						if( strPic.size() > 0 )
						{
							strPic.erase(strPic .size()-1, 1);
						}
						strLastLinePic = strPic;
						strPic.clear();
					}



					nSaveHeight = nHeight;
					nSaveWidth = nWidth;
					nHeight = m_nRowSpacing;
					nWidth = 0;

					if( n < nCursorPos && n+nStep >= nCursorPos )
						nVirtualPos = n + nStep;

					continue;
				}

				m_VecCharIntro[n].nByte = 1;
				m_VecCharIntro[n].nCharSize = ptSize.x;
				m_VecCharIntro[n].dwHeight = ptSize.y;

				if( ptSize.y+GUI_DEFAULT_ROW_SPACING_ADDON > nHeight )
					nHeight = ptSize.y + GUI_DEFAULT_ROW_SPACING_ADDON;

				nWidth += ptSize.x;
				nStep = 1;

				if( n < nCursorPos && n+nStep >= nCursorPos )
					nVirtualPos = n + nStep;
			}
			else
			{	
				//匹配成功,是转义图片
				INT nPicHead = 0;
				INT nRemoveWidthIndex = 0;
				INT nBytes = strTotalPic.size() - nPicPos;

				if( nBytes > (INT)strPic.size() )
				{
					//说明含有上一行的字符,恢复现场
					INT nLastLineEnd = m_strText.find_last_of(0x0d, n);
					INT nIntelLineSize = m_VecCharIntro[nLastLineEnd].nByte;
					ASSERT( nLastLineEnd );
					nHeight = nSaveHeight;
					nWidth = nSaveWidth;
					MoveCharIntro(nLastLineEnd, -nIntelLineSize);
					m_strText.erase(nLastLineEnd, nIntelLineSize);
					//因为要换行，往后推移2个字符
					if( n<= nCursorPos) 
						nCursorPos -= nIntelLineSize; 

					n = n - nIntelLineSize;
					nEndPos -= nIntelLineSize;

					nPicHead = n - nBytes + 1;
					//上一行参与的字符
					INT nLastLineChar = nBytes - strPic.size();	
					nRemoveWidthIndex = nLastLineEnd;

				}
				else
				{
					nPicHead = n - nBytes + 1;
					nRemoveWidthIndex = n;
				}


				INT nInteWidth = 0;
				for(INT i=nPicHead; i<nRemoveWidthIndex; i++)
				{
					nInteWidth += m_VecCharIntro[i].nCharSize;
				}
				if( nWidth + pPic->ptSize.x - nInteWidth > m_rcText.right - m_rcText.left )
				{ 
					// 放不下了,只能添加换行符，同时剩下的和下一行联一起，也就是取消两行之间的换行符
					if( n<= nCursorPos) //因为要换行，往后推移2个字符
						nCursorPos += 2;

					if( nEndPos < (INT)m_strText.size()-2 )
					{
						MoveCharIntro(nEndPos, -2);
						m_strText.erase(nEndPos, 2);
					}

					if( m_strText.size() + 2 >= m_dwMaxTextCount)
					{
						m_dwMaxTextCount = m_strText.size() + 4;
						m_VecCharIntro.resize(m_dwMaxTextCount + 2);
					}

					TCHAR szNewLine[3];
					ZeroMemory(szNewLine, 3*sizeof(TCHAR));

					szNewLine[0] = 0x0d;
					szNewLine[1] = 0x0a;
					MoveCharIntro(nPicHead, 2);
					m_strText.insert(nPicHead, szNewLine);
					m_VecCharIntro[nPicHead].nByte = 2;
					m_VecCharIntro[nPicHead].dwHeight = nHeight;
					m_VecCharIntro[nPicHead].nCharSize = nWidth;
					m_VecCharIntro[nPicHead+1].nByte = -1;

					nEndPos += 2;
					n = nPicHead;
					nStep = 2;

					str.clear();
					strLastLine.clear();
					strTotal.clear();

					if( TRUE == bMatchPic )
					{
						strPic.clear();
						strLastLinePic.clear();
						strTotalPic.clear();
					}


					nSaveHeight = nHeight;
					nSaveWidth = nWidth;
					nHeight = m_nRowSpacing;
					nWidth = 0;
					continue;
				}

				m_VecCharIntro[nPicHead].nByte = n - nPicHead + 1;
				m_VecCharIntro[nPicHead].nCharSize = pPic->ptSize.x;
				m_VecCharIntro[nPicHead].dwHeight = pPic->ptSize.y;
				m_VecCharIntro[nPicHead].pPic = pPic;
				for(INT i=nPicHead+1; i<=n ;i++)
				{
					m_VecCharIntro[i].nByte = -1;
				}

				if( pPic->ptSize.y > nHeight )
					nHeight = pPic->ptSize.y;

				nWidth += pPic->ptSize.x - nInteWidth;
				n = nPicHead;
				nStep = m_VecCharIntro[nPicHead].nByte;

				if( nPicHead < nCursorPos && nPicHead+nStep >= nCursorPos )
					nVirtualPos = nPicHead + nStep;

				str.clear();
				strLastLine.clear();
				strTotal.clear();

				if( TRUE == bMatchPic )
				{
					strPic.clear();
					strLastLinePic.clear();
					strTotalPic.clear();
				}
			}
		}
	}

	if( m_strText[n] == 0x0d && m_strText[n+1] == 0x0a )
	{
		m_VecCharIntro[n].dwHeight = nHeight;	//刷新该行的高度
		m_VecCharIntro[n].nCharSize = nWidth;
		if( nHeight < m_nRowSpacing )
			m_VecCharIntro[n].dwHeight = m_nRowSpacing;
	}

}

//-----------------------------------------------------------------------------
//获取文本信息对外接口
//-----------------------------------------------------------------------------
INT	GUIRichEditBox::GetText(tstring &str) 
{ 
	INT nTextSize = (INT)(m_strText.size()-2);
	str = m_strText.substr(0, nTextSize); 
	ASSERT( str.size() == nTextSize );
	return nTextSize; 
}

LPCTSTR GUIRichEditBox::GetText(INT& nTextSize)  
{
	//注意有效长度！！！
	nTextSize = (INT)(m_strText.size()-2);
	return (m_strText.c_str()); 
}

//-----------------------------------------------------------------------------
//设置容纳的最大字数
//-----------------------------------------------------------------------------
VOID GUIRichEditBox::SetMaxTextCount(DWORD dwMaxTextCount)
{
	m_nMaxEditTextCount = dwMaxTextCount;
}


//-----------------------------------------------------------------------------
//删除文本前头的字符,以行计数,返回实际删除的行数
//-----------------------------------------------------------------------------
INT	GUIRichEditBox::DelFrontText(INT nRowNum)
{
	INT nDelRowNum = 0;
	INT nTextSize = m_strText.size();

	INT i = 0;
	for(i=0; i<nTextSize;)
	{
		i = m_strText.find_first_of(0x0d, i);
		ASSERT( i >= 0 );

		nDelRowNum++;
		if( nDelRowNum == nRowNum )
			break;

		i += m_VecCharIntro[i].nByte;
	}

	//如果没有到最后的换行符,我们就可以把这个换行符也删掉
	if( i < nTextSize-2 )
		i += m_VecCharIntro[i].nByte;	
	//如果没有文本可删,直接返回
	if( i == 0 )
		return 0;

	//先调用
	MoveCharIntro(0, -i);
	//后调用，顺序不能乱
	m_strText.erase(0, i);
	nTextSize = m_strText.size();

	m_nCursorPos -= i;
	m_nSelectStart -= i;
	m_nSelectEnd -= i;
	if( m_nCursorPos < 0 )
		m_nCursorPos = 0;
	if( m_nSelectStart < 0 )
		m_nSelectStart = 0;		
	if( m_nSelectEnd < 0 )
		m_nSelectEnd = 0;

	//正确设置初始显示的行
	m_nStartDisplayLine -= nDelRowNum;
	if( m_nStartDisplayLine < 0 )
		m_nStartDisplayLine = 0;

	return nDelRowNum;			
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GUIRichEditBox::AdptiveCursor(bool bDrag, BOOL bLink)
{
	/*
	if( TRUE == bDrag )
	{
		m_pRender->SetCursor(_T("sizewe"));   //
	}
	else if( TRUE == bLink )
	{
		m_pRender->SetCursor(_T("hand"));
	}
	else
	{
		m_pRender->SetCursor(_T("ibeam"));   //		
	}*/
}



//--------------------------------------------------------------------------------
//处理键盘按下的消息(一般是功能键的按下)
//--------------------------------------------------------------------------------
BOOL GUIRichEditBox::OnMsgForKeyDown(INT nKey)
{
	switch( nKey )
	{
	case VK_LEFT:	// 处理键盘左方向键
		{
			m_bShowCursor = TRUE;
			if( GetKeyState(VK_SHIFT) < 0 )
			{ // shift键按下了

				if( FALSE == m_bCanSelect )
					break;

				INT nOriginPos = m_nCursorPos;
				CalCursorForLeftRight(TRUE);	
				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
			{
				if( m_nSelectStart == m_nSelectEnd )
					CalCursorForLeftRight(TRUE);
				else
					m_nCursorPos = m_nSelectEnd < m_nSelectStart ? m_nSelectEnd : m_nSelectStart;
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;
			}

			while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
				m_nStartDisplayLine--;
			SendScrollEvent();
		}
		return TRUE;
	case VK_RIGHT:	// 处理键盘右方向键
		{
			m_bShowCursor = TRUE;
			if( GetKeyState(VK_SHIFT) < 0 )
			{ 
				// shift键按下了
				if( FALSE == m_bCanSelect )
					break;

				INT nOriginPos = m_nCursorPos;
				CalCursorForLeftRight(FALSE);	
				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
			{
				if( m_nSelectStart == m_nSelectEnd )
					CalCursorForLeftRight(FALSE);
				else
					m_nCursorPos = m_nSelectEnd > m_nSelectStart ? m_nSelectEnd : m_nSelectStart;
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;
			}

			while( m_nCursorPos > GetLastDisplayIndex() )
			{
				m_nStartDisplayLine++;
			}
			SendScrollEvent();
		}
		return TRUE;
	case VK_UP:// 处理键盘上方向键
		{
			//计算新的光标位置
			INT nOriginPos = m_nCursorPos;
			CalCursonForUpDown(TRUE);
			while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
				m_nStartDisplayLine--;
			SendScrollEvent();

			// 处理选择
			if( GetKeyState(VK_SHIFT) < 0 )
			{
				if( FALSE == m_bCanSelect )
					break;

				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;
		}
		return TRUE;
	case VK_DOWN:
		{
			//计算新的光标位置
			INT nOriginPos = m_nCursorPos;
			CalCursonForUpDown(FALSE);
			while( m_nCursorPos > GetLastDisplayIndex() )
				m_nStartDisplayLine++;
			SendScrollEvent();
			// 处理选择
			if( GetKeyState(VK_SHIFT) < 0 )
			{
				if( FALSE == m_bCanSelect )
					break;

				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;

		}
		return TRUE;
	case VK_HOME:
		{
			//移动到行首,如果按住了ctrl则到文本首部
			INT nOriginPos = m_nCursorPos;

			if( (GetKeyState(VK_LCONTROL)&0x80) || (GetKeyState(VK_RCONTROL)&0x80) )
			{
				//按住了CTRL键,光标移动到文本首字符
				if( FALSE == m_bCanSelect )
					break;

				m_nStartDisplayLine = 0;
				m_nCursorPos = 0;
				SendScrollEvent();
			}
			else 
			{//光标移动到行首
				if( nOriginPos > 0 )
				{
					INT nNewLineIndex = m_strText.find_last_of(0x0d, nOriginPos-1);
					if( nNewLineIndex < 0 )
						m_nCursorPos = 0;
					else
					{
						m_nCursorPos = nNewLineIndex + 2;
					}
				}
			}

			if( GetKeyState(VK_SHIFT) < 0 )
			{
				if( FALSE == m_bCanSelect )
					break;

				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;
		}
		return TRUE;

	case VK_END:
		{
			//移动到行尾,如果按住了ctrl，则移动到文本末尾
			INT nOriginPos = m_nCursorPos;
			if( (GetKeyState(VK_LCONTROL)&0x80) || (GetKeyState(VK_RCONTROL)&0x80) )
			{
				if( FALSE == m_bCanSelect )
					break;

				m_nCursorPos = m_strText.size() - 2;
				while ( m_nCursorPos > GetLastDisplayIndex() )
					m_nStartDisplayLine++;
				SendScrollEvent();
			}
			else 
			{
				INT nNewLineIndex = m_strText.find_first_of(0x0d, nOriginPos);
				ASSERT( nNewLineIndex >= 0 );
				m_nCursorPos = nNewLineIndex;
			}

			if( GetKeyState(VK_SHIFT) < 0 )
			{
				if( FALSE == m_bCanSelect )
					break;

				if( m_nSelectStart != m_nSelectEnd )
					m_nSelectEnd = m_nCursorPos;
				else
				{
					m_nSelectEnd = m_nCursorPos;
					m_nSelectStart = nOriginPos;
				}
			}
			else
				m_nSelectStart = m_nSelectEnd = m_nCursorPos;
		}
		return TRUE;
	
	case VK_DELETE:	// 处理del删除
		{
			if( m_bReadOnly )
				break;

			if( m_nSelectStart != m_nSelectEnd )
				this->DelSelected();
			else 
			{
				INT nStartPos = m_nCursorPos;
				DeleteText(nStartPos, nStartPos+m_VecCharIntro[nStartPos].nByte);	

				m_nCursorPos = nStartPos;
				//保证光标位置在视野范围内
				while( GT_INVALID == GetRowHead(m_nStartDisplayLine) )
					m_nStartDisplayLine--;

				while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
					m_nStartDisplayLine -- ;
				SendScrollEvent();
			}
		}
		return TRUE;

	case VK_BACK:
		{
			if( TRUE == m_bReadOnly )
				break;

			if( (GetKeyState(VK_LCONTROL) & 0x80) 
				|| (GetKeyState(VK_RCONTROL) & 0x80) )
				break;

			if( m_nSelectStart != m_nSelectEnd ) // 有选中，删选中
			{
				this->DelSelected();
			}
			else if( m_nCursorPos > 0 )
			{
				INT nStartPos = m_nCursorPos - 1;
				while( nStartPos >= 0 )
				{
					if( m_VecCharIntro[nStartPos].nByte != -1 )
					{
						break;
					}
					nStartPos--;
				}
				DeleteText(nStartPos, m_nCursorPos);
				m_nCursorPos = nStartPos;

				//保证光标位置在视野范围内
				while( GT_INVALID == GetRowHead(m_nStartDisplayLine) )
					m_nStartDisplayLine--;

				while( m_nCursorPos < GetRowHead(m_nStartDisplayLine) )
					m_nStartDisplayLine -- ;
				SendScrollEvent();
			}
		}
		return TRUE;
	case VK_RETURN:	// 处理回车
		{
			if( (GetKeyState(VK_LCONTROL) & 0x80) 
				|| (GetKeyState(VK_RCONTROL) & 0x80) )
			{
				//ctrl+enter
				if( TRUE == m_bReadOnly )
					break;
				if( FALSE == NewLine(m_nCursorPos) )
					break;

				m_nCursorPos += 2;

				while( m_nCursorPos > GetLastDisplayIndex() )
					m_nStartDisplayLine++;

				SendScrollEvent();
			}
			else
			{
				tagGUIEvent event(this, EGUIE_EditBoxEnter);
				SendEvent(&event);
			}
		}
		return TRUE;
	case VK_TAB:	// TAB键由上层处理
		{
			//暂定4个空格
			if( TRUE == m_bReadOnly )
				break;

			//字节数限制
			if( (INT)m_strText.size() + 4 >= m_nMaxEditTextCount )
				break;

			if( m_strText.size() + 4 >= m_dwMaxTextCount )
			{
				m_dwMaxTextCount += 5;
				m_VecCharIntro.resize(m_dwMaxTextCount+2);
			}

			InsertText(_T("    "), m_nCursorPos, m_dwTextColor, m_dwShadeColor);
			while( m_nCursorPos > GetLastDisplayIndex() )
				m_nStartDisplayLine++;
			SendScrollEvent();
		}
		return TRUE;
	}

	return FALSE;
}


//--------------------------------------------------------------------------------
//获取某一行实际显示时所占宽度(返回-1表示无效的行号)
//--------------------------------------------------------------------------------
INT GUIRichEditBox::GetRowRenderWidth(INT nRowIndex)
{
	//得到该行号行首的字符序列号
	INT nHeadIndex = GetRowHead(nRowIndex);

	if( nHeadIndex == GT_INVALID )
	{
		//未找到
		return GT_INVALID;
	}

	INT nRowWidth = 0;

	INT nTextSize = m_strText.size();
	for(INT i=nHeadIndex; i<nTextSize;)
	{
		if( m_strText[i] == 0x0a || m_strText[i] == 0x0d )
			break;	 //换行就跳出

		nRowWidth += m_VecCharIntro[i].nCharSize;

		i += m_VecCharIntro[i].nByte;
	}

	return nRowWidth;
}


//--------------------------------------------------------------------------------
//对字符串进行过滤,过滤掉特殊内部格式(比如颜色设置等)
//--------------------------------------------------------------------------------
BOOL GUIRichEditBox::FilterString(tstring& str)
{
	BOOL bCheck = TRUE;
	tstring strKey;
	INT nFont = GT_INVALID;
	INT nTemp = 0;

	while( TRUE == bCheck )
	{
		bCheck = FALSE;

		//首先清除关键字
		m_mapKey.ResetIterator();
		while( m_mapKey.PeekNext(strKey, nFont) )
		{
			INT nPos = str.find(strKey, 0);
			if( str.npos != nPos )
			{
				str.erase(nPos, strKey.size());
				bCheck = TRUE;
			}			
		}

		//其次清除超链接
		m_mapLink.ResetIterator();
		while( TRUE == m_mapLink.PeekNext(strKey, nFont) )
		{
			INT nPos = str.find(strKey, 0);
			if( str.npos != nPos )
			{
				str.erase(nPos, strKey.size());
				bCheck = TRUE;
			}			
		}

		//清除特殊转义字符串

		m_mapFont.ResetIterator();
		while( TRUE == m_mapFont.PeekNext(strKey, nFont) )
		{
			INT nPos = 0;
			nTemp = 0;
			while( str.npos != (nPos=str.find(strKey, nPos)) )
			{
				nTemp = nPos + strKey.size();

				if( SET_FONT_SIZE == nFont )	
				{
					//设置字体大小

					nTemp = str.find(_T(">"), nTemp);

					if( nTemp == str.npos )
						break;

					nTemp += 1;
					str.erase(nPos, nTemp-nPos);
					bCheck = TRUE;

				}
				else if( SET_FONT_STYLE == nFont )
				{
					//设置类型
					nTemp = str.find(_T(">"), nTemp);

					if( nTemp == str.npos )
						break;

					nTemp += 1;
					str.erase(nPos, nTemp-nPos);
					bCheck = TRUE;
				}
				else if( SET_FONT_COLOR == nFont )
				{
					nTemp = str.find(_T(">"), nTemp);

					if( nTemp == str.npos )
						break;

					nTemp += 1;
					str.erase(nPos, nTemp-nPos);
					bCheck = TRUE;
				}
				//刷新下次的位置
				nPos = nTemp;
			}
		}

	}

	return TRUE;
}

inline bool _IsLetterOrSymbol(TCHAR ch)
{
	return ch>32&&ch<127;
}

INT GUIRichEditBox::FindBreakLinePos(INT nPos)
{
	ASSERT(nPos>=0 && nPos<(int)m_strText.size());

	//当前字符是否是英文字母/标点？
	if(!(m_strText[nPos] > 32 && m_strText[nPos] < 127) )
		return nPos;

	//向前寻找单词的起始位置
	int i;
	for(i=nPos-1; i>=0; i--)
	{
		if(!(m_strText[i] > 32 && m_strText[i] < 127) )	// 空格或换行符等
			break;
	}

	//到达字符缓存区起始位置？
	if( i < 0 )
		return nPos;

	//遇到换行符？
	if( '\n' == m_strText[i] || '\r' == m_strText[i] )
		return nPos;

	return i+1;
}


}	// namespace vEngine {
