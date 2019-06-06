#pragma once

#include "Role.h"

class ActiveStatic;

struct tagBuffUI
{
	tagRoleBuff    stBuff;                 // BUFF结构
	ActiveStatic*  pSt_BuffPic;            // BUFF图标
	GUIProgress*   pPro_BuffTime;          // BUFF倒计时进度条
	tagBuffUI() :  pSt_BuffPic( NULL ), pPro_BuffTime( NULL ) { ZeroMemory( &stBuff, sizeof(stBuff) ); }
};

/** \class   BuffUIList
    \brief  
    \remarks 用来管理显示BUFF的控件，内部并不控制控件的生命周期
*/
class BuffUIList
{
public:
	BuffUIList();
	~BuffUIList();

	/** \添加BUFF界面，应按照界面的排列顺序添加
	*/
	BOOL AddUI( ActiveStatic* pBuffStatic, GUIProgress* pTimeProgress );
	/** \添加BUFF
	*/
	BOOL AddBuff( const tagRoleBuff* pBuff );
	/** \清空所有显示的BUFF
	*/
	VOID ClearBuffs();
	/** \更新界面状态
	*/
	VOID Updata();
	/** \处理GUI事件
	*/
	BOOL GUIEventHandler( tagGUIEvent* pEvent, const GUIWnd* pEventWnd, const DWORD dwRoleID );
private:
	vector<tagBuffUI>		m_vecBuffUI;              // BUFF UI列表
	TObjRef<GUISystem>      m_pGUI;
	INT						m_nCurShowNum;            // 当前显示的BUFF数量
	DWORD                   m_dwLastUpdataTipsTime;   // 上次更新BUFF TIPS的时刻
};
