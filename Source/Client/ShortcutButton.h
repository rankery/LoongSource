#pragma once
#include "buttonex.h"
class ShortcutBarFrame;
/**	brief: 快捷按钮
*  事件说明		EGUISCB_DropCheck	EGUISCB_Drop处理时触发
*						   dwParam1    数据类型
*						   dwParam2    拖动数据1
*						   dwParam3    拖动数据2
*						   dwParam4    鼠标位置
*
*	m_BtnData成员变量释义：
*					 	m_eType;		按钮类型
						m_dwParam1;		对应数据1(skill:id,	item:数目+位置)
						m_dwParam2;		对应数据2(skill:NULL,	item:typeid)
						m_n64Param3;	skill:NULL, item: 64id)	
*
*	物品在快捷栏中的比较特殊，需要记录的数据：
*		1、m_nItemNum用来表示Item的数量。
*/
class ShortcutButton :
	public ButtonEx
{
public:
	ShortcutButton(void);
	~ShortcutButton(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Render();
	virtual VOID Update();
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	virtual VOID OnEvent(tagGUIEvent* pEvent); // 接受事件

	//设置快捷按钮的快捷键值
	void SetKeyBar(const TCHAR*	strKey)	{ m_strKeyBar = strKey;	 }
	tstring& GetKeyBar() { return m_strKeyBar; }

	//设置快捷按钮DIK的值
	void SetDIKBar(DWORD key) { m_dwKeyID = key; }
	DWORD GetDIKBar()const { return m_dwKeyID; }

	//设置快捷按钮的物品数量
	void SetItemNum(const TCHAR* strNum) { m_strItemNum = strNum;  m_nItemNum = _ttoi(strNum); }
	
	//物品数量
	void SetItemNum(int itemNum);
	int GetItemNum()const { return m_nItemNum; }	

	//冷却时间
	void SetCDTime(float time) { m_fCDCount = time; }

	//旋转率
	void SetRestoreRatio(float time) { m_fRestoreRatio = time; }

	//计算点击快捷按钮时闪烁图的时间
	BOOL BeginPressBtn();
	void EndPressBtn();
			
	//--设置/获得快捷按钮可用性
	void SetCanUse(bool bVal) { m_bCanUse = bVal; }
	bool GetCanUse() { return m_bCanUse; }

	//设置不可用
	void SetInvalite(bool bVal) { m_bIsInvalite = bVal; }

	//缓冲按过的快捷键
	void SetCacheImage(BOOL bVal) { m_bCacheImg = bVal; }

	//设置快捷栏单元格右上角的快捷键提示字符，变为醒目红色。
	void SetKeyBarEnable(BOOL bVal){ m_bKeyEnable = bVal; }

    //设置物品类型的数据
    void SetItemData( DWORD dwParam1, DWORD dwParam2);	

protected:

	//清除拖动标志
	void ClearDropFlag()	{ m_nDragFlag = 0; }	

	//绘制快捷按钮上的文字
	void DrawBarText();	

	//绘制点击快捷栏按钮框时显示闪烁的图片
	void DrawPressFrame();


private:

	tstring				m_strKeyBar;				//快捷按钮上面显示的快捷键值
	tstring				m_strItemNum;				//快捷按钮上面显示的物品数量
	int					m_nItemNum;					//物品数量
	BOOL				m_bIsInvalite;					

	tagGUIImage*		m_pBtnInvalitePic;			//不可用时画一个透明图片
	tagGUIImage*		m_pBtnGlintPic[4];			//按下时,连续绘制贴图
	tagGUIImage*		m_pBtnCachePic;				//缓冲技能时绘制的贴图
	int					m_curFrame;
	float				m_frameTime;
	BOOL				m_bCacheImg;				//是否绘制缓冲技能的贴图
	BOOL				m_bKeyEnable;				//是否缓制醒目快捷提示
	DWORD				m_dwKeyID;
	

	int					m_nDragFlag;				// 记录拖放状态 0: 正常,1: 被drag,未被drop
	TObjRef<GameFrameMgr>		m_pFrameMgr;		//frame管理器
	ShortcutBarFrame*	m_pFrame;

};

class SpecialSkillButton
	: public ShortcutButton
{
public:
	SpecialSkillButton(void);
	~SpecialSkillButton(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID OnEvent(tagGUIEvent* pEvent); 
};
