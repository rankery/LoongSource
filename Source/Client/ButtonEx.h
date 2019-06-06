#pragma once
#include "RotaterInRect.h"
#include ".\GameClientDefine.h"

//图片枚举
enum eWndPicType				
{
	EWPICTYPE_CURPIC = 0,			//当前(即背景)
	EWPICTYPE_PUSHPIC,				//按下
	EWPICTYPE_MOVEPIC,				//鼠标经过
	EWPICTYPE_DISABLEPIC,			//禁用
};

class ButtonEx :
	public GUIButton
{
public:
	ButtonEx(void);
	~ButtonEx(void);
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Render();
	virtual VOID Update();
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

	//--逻辑层数据的读写 
	const BtnData& GetBtnData() const { return m_BtnData ; }
	void SetBtnData(EShortcutBtnType eType, DWORD dwParam1, DWORD dwParam2, INT64 n64Param3 );
	void SetBtnData( const BtnData& data ){ 
		SetBtnData( data.GetType(), data.GetData1(), data.GetData2(), data.GetData3() );
	}

	//绘制多边形
	VOID Draw2DPrimitiveUP( EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color=0xFFFFFFFF);

	//技能冷却恢复时间
	int GetBtnRestoreTime(float& fRestoreRatio);

	//--设置四种状态图片color
	void SetDisablePicColor(DWORD color){ m_dwDisablePicColor = color; }
	void SetPushPicColor(DWORD color){ m_dwPushPicColor = color; }
	void SetMovePicColor(DWORD color){ m_dwMovePicColor = color; }
	void SetDisablePic(DWORD color){ m_dwDisablePicColor = color; }

	//获得背景pic
	tstring GetPicFileName() { return m_strPic; }
	void SetPicFileName(const tstring& picName)
	{
		SetWndPic(EWPICTYPE_CURPIC, picName);
	}
	void ClearPic()
	{	
		SetWndPic(EWPICTYPE_CURPIC, _T("data\\ui\\Main\\L_icon_back.bmp"));
		m_dwStartPressTime = 0;
		m_bCanUse = true;
		m_fCDCount = 0.0f;
		SetRefresh(TRUE); 
	}

	virtual INT64 GetItem64ID() { return -1;}

	//触发游戏事件
	void InvokeGameEvent( );			

protected:

	//设置快捷按钮的图片
	void SetWndPic(eWndPicType ePicType, const tstring& strPic);

	//返回值  剩余时间 绘制CD冷却旋转的图片
	int DrawShadow( int nSquareWidth );	

	//绘制冷却时间的显示
	void DrawShadowText();

	//绘制图片
	void DrawImg();

	//转换CD时间(待定)
	void CalCDTime(int nTime);

	//拖动类型转换成快捷按钮类型
	EShortcutBtnType  DragType2SCBtnType(EDragType eDragType)
	{
		if( eDragType > ESBT_Item )
			return ESBT_Null;
		EShortcutBtnType eSbtArr[EDT_End] =	{ ESBT_Null,ESBT_Skill,ESBT_Item };
		return eSbtArr[eDragType];
	}

protected:

	BtnData				m_BtnData;					//快捷按钮保存的数据
	tstring				m_strCDTime;				//计算冷却时间
	float				m_fCDCount;					//CD时间统计
	float				m_fRestoreRatio;			//旋转参数
	static	RotaterInRect	m_Rotater;				//旋转点的计算	
	tagGUIFont*			m_pShadowFont;
	DWORD				m_dwStartPressTime;			//开始画点击按钮的图片

public:

	bool				m_bCanUse;					//是否现在能点击使用此技能

};
