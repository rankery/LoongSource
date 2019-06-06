//-----------------------------------------------------------------------------
//!\file xui_richeditbox.h
//!\author tidy / bwen / zewu / lyp
//!
//!\date 2005-4-13
//! last 2008-1-17
//!
//!\brief 界面系统 richeditbox 控件
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {

struct tagCharIntro
{
	tagGUIImage		*pPic;			// 对转义字符串有效	
	INT				nCharSize;		// 字符宽度
	DWORD			dwHeight;		// 字符高度
	INT				nByte;			// 字节数
	DWORD			dwReserve;		// 保留
};	 //如果是0x0d字符，表示换行，那么dwHeight,nCharSize表示整行的高度，和整行的总显示宽度.



//-----------------------------------------------------------------------------
// 界面系统 RichEditBox 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIRichEditBox : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual GUIWnd* OnActive();	
	virtual VOID OnDeactive(GUIWnd* pWnd);
	virtual BOOL PointInWnd(tagPoint& pt);

	virtual VOID SendEvent(tagGUIEvent* pEvent);// 向外发送事件

	//渲染文字
	virtual VOID RenderText(LPCTSTR szText, INT nLeft, INT nBottom, INT nWidth, INT nHeight,
		DWORD dwColor, tagGUIFont* pFont, BOOL bTransparent);

public:		//相关操作接口

	VOID ClsText();		//删除文本
	//增加文本bNexLine=TRUE表示从下一行开始，否则则续接到最后一行，
	//dwColor=-1,表示默认用m_Data的颜色设置,dwStyle同理, bMatchPic表示是否需要匹配图片
	BOOL AddText(LPCTSTR szText, BOOL bNextLine = FALSE, DWORD dwColor=-1, DWORD dwShade=-1,
		BOOL bMatchPic=TRUE, BOOL bEqualTextSize=FALSE);

	//对字符串进行过滤,过滤掉特殊内部格式(比如颜色设置等)
	BOOL FilterString(tstring& str);  

	//删除文本前头的字符,以行计数,返回实际删除的行数
	INT	DelFrontText(INT nRowNum);	

	//注册转义字符串对应的图片,字符串长度不能超过XUI_MAX_TRANSFER_LEN,字符串暂时设定不能包含双字节字符
	//图片宽度不能超过richeditbox的宽度
	BOOL RegisterPic(LPCTSTR szStr, LPCTSTR szPicFileName);
	//文字滚动(行为单位)
	DWORD ScrollText(BOOL bUp=FALSE);

	//设置字体透明度,=0xff000000表示不透明
	VOID SetTransDepth(DWORD dwTransDepth) { m_dwTransDepth = dwTransDepth; }
	//设置是否显示光标
	VOID SetShowCursor(BOOL bShowCursor=TRUE) { m_bHoldCursor = bShowCursor; }
	//设置光标颜色
	VOID SetCursorColor(DWORD dwCursorColor) {  m_dwCursorColor = dwCursorColor; }

	//设置能否选中文本(隐含了能否复制文本)
	VOID SetCanSelect(BOOL bCanSelect=TRUE) { m_bCanSelect = bCanSelect; }

	//设置容纳的最大字数
	VOID SetMaxTextCount(DWORD dwMaxTextCount);

	//设置是否每行居中显示
	VOID SetMiddleRender(BOOL bMiddle) { m_bMiddleRender = bMiddle; }

	//是否给关键字加下划线
	VOID SetKeyWordDownLine(BOOL bDownLine) { m_bDownLine = bDownLine; }
	//设置超链接的鼠标滑过时的字体颜色
	VOID SetLinkColorMouseHover(DWORD dwColor) { m_dwLinkColor = dwColor; }
	//设置字符所占宽度是否一致
	VOID SetEqualTextSize(BOOL bEqual) { m_bEqualTextSize = bEqual; }

public:		//用户获取信息接口

	//当前有多少行
	INT GetRowNum(INT& nVisibleRowNum);
	//获取某一行实际显示时所占宽度(返回-1表示无效的行号)
	INT GetRowRenderWidth(INT nRowIndex);

	//获取文本信息对外接口
	INT GetText(tstring &str);
	LPCTSTR GetText(INT& nTextSize);//此函数慎用
	//得到光标位置处的key, szKeyWord由外部提供空间(关键字不超过100字节)
	BOOL GetCursorKey(LPTSTR szKeyWord, INT& nStartPos, INT& nEndPos);
	//根据index得到超级链接指向的内容(返回false表示无此超链)
	BOOL GetLinkContent(INT nIndex, tstring &strText);

	VOID SetReadOnly(BOOL bReadOnly) { m_bReadOnly = bReadOnly; }
	BOOL IsCanEdit() { return m_bReadOnly == FALSE; }
	VOID SetScrollLast(BOOL bScrollLast) { m_bScrollLast = bScrollLast; }
	BOOL IsScrollLast() { return m_bScrollLast; }

	BOOL IsShowLastLine();		//当前是否显示完整了最后一行
	VOID ScrollForLastLine(BOOL bReset=FALSE);	//滚动以便能完整显示最后一行
	
	
	GUIRichEditBox();
	//--------------------------------------------------------------------------


protected:
	TMap<tstring, tagGUIImage*>	m_mapPic;	// 转义串对应图片
	TMap<tstring, INT>			m_mapFont;	// 转义串设置字体
	TMap<tstring, INT>			m_mapKey;	// 转义串设置关键字
	TMap<tstring, INT>			m_mapLink;	// 转义串设置超链接格式

	DWORD			m_dwTransDepth;

	BOOL			m_bScrollLast;			// 当不可编辑的时候，如果增加文本，是否自动滚动到最后一行

	BOOL			m_bShowCursor;			// 当前是否显示光标(光标是闪烁的)
	BOOL			m_bHoldCursor;			// 是否拥有光标,与m_bShowCursor不是一回事
	
	DWORD			m_dwLinkColor;			// 设置超链接的鼠标移上时的字体颜色
	INT				m_nMouseLinkIndex;		// 当前鼠标所处的超链接的序号(无则=-1);

	BOOL			m_bCanSelect;			// 是否能够选中(隐含了能否复制)
	BOOL			m_bMiddleRender;		// 是否居中渲染,默认为否，即居左对齐
	BOOL			m_bDownLine;			// 是否给关键字加下划线(默认为不加)

	DWORD			m_dwMaxTextCount;		// 最大字节数
	
	INT				m_nSelectStart;			// 选择开始处
	INT				m_nSelectEnd;			// 选择结束处
	INT				m_nCursorPos;			// 光标位置

	INT				m_nStartDisplayLine;	// 显示的第一行，是实际的第几行

	std::vector<tagCharIntro> m_VecCharIntro; // 该字符相关信息

	INT				m_nTextNormalHeight;	// 文字默认的高度
	tagPoint		m_ptSizeChinese;		// 汉字的默认大小
	tagPoint		m_ptSizeLetter;			// 字母

	BOOL			m_bEqualTextSize;		// 是否所有字符大小一样
	DWORD			m_dwLastMoveTime;		// 上次移动的时刻


	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL			m_bDisable;				// 是否无效
	BOOL			m_bReadOnly;			// 是否只读
	DWORD			m_dwCursorColor;		// 光标颜色
	DWORD			m_dwCursorTimer;		// 光标闪烁间隔
	INT				m_nRowSpacing;			// 行高
	INT				m_nMaxEditTextCount;	// 最大可编辑字节数



protected:	
	VOID	DelSelected();	// 删除选择的字符
	VOID	CopyToClipboard(); // 将选定内容拷到剪贴板 
	VOID	PasteFromClipboard(); // 从剪贴板拷贝文本到当前位置

	/**************************关键代码_start********************************/

	//插入一段字符,插入位置默认为光标处
	VOID	InsertText(LPCTSTR szText, INT& nVirtualPos, DWORD dwColor, 
		DWORD dwShade, BOOL bLinkLastLine=TRUE, BOOL bMatchPic=TRUE) ;
	//在nPos位置换行
	BOOL	NewLine(INT nPos);
	//删除该位置的字符串,注意光标位置需重新设置好
	VOID	DeleteText(INT nBeginPos, INT nEndPos);
	//重新计算始末位置之间的字符序列的信息解析,由于排版的原因,nEndPos可能会向后推移
	VOID	RecalCharIntro(INT nStartPos, INT nEndPos, INT& nCursorPos, INT& nVirtualPos, BOOL bMatchPic=FALSE);
	// 字符串匹配，对应图片
	tagGUIImage* MatchPic(tstring &str, INT& nPos);
	// 字符串匹配转义更改文字属性
	INT MatchFont(tstring &str, INT& nPos, DWORD& nValue);

	/**************************关键代码_end*********************************/

	//双击选中某个字符
	VOID CalSelectForDoubleClick();
	// 由view坐标得到光标位置
	INT	GetCharIndex(INT nViewX, INT nViewY, BOOL bLeft);
	//计算光标上下移动时新的光标位置
	VOID CalCursonForUpDown(BOOL bUp);
	//计算光标左右移动时的光标位置
	VOID CalCursorForLeftRight(BOOL bLeft);
	//重新计算某一行的行高
	VOID RecalRowHeight(INT nRow);
	//处理m_VecCharIntro的偏移
	VOID MoveCharIntro(INT nPos, INT IncSize);

	//能够完整显示的最后一行的结束符序列
	INT	GetLastDisplayIndex();
	//得到该行号行首的字符序列号
	INT GetRowHead(INT nRowIndex);
	//就给出的位置判断所处的超链接的index,无则index=-1
	INT GetLinkIndex(INT nPos);
	//检测是否点击了超链接
	BOOL ClickLink(tagGUIInputMsg* pMsg);


	//发出滚动事件
	BOOL SendScrollEvent();
	//设置当前显示的首行m_nStartDisplayLine
	void SetCurrentTextRow(INT nRowIndex );

	//寻找换行位置,英文单词需整体换行
	INT FindBreakLinePos(INT nPos);

protected:
	//处理鼠标变化,add by zewu 2005-2-9 20:49
	void AdptiveCursor(bool bDrag, BOOL bLink);

	//处理键盘按下的消息(一般是功能键的按下)
	BOOL OnMsgForKeyDown(INT nKey);



};



}	// namespace vEngine {

