#pragma once

class CountDownFrame : public GameFrame 
{
public:
	CountDownFrame();
	~CountDownFrame();
	/*!
	\brief 初始化
	*/
	virtual BOOL Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam);

	/*!
	\brief 销毁
	*/
	virtual BOOL Destroy();
	
	/*!
	\brief 加载UI
	*/
	virtual BOOL ReloadUI();

	/*!
	\brief 更新
	*/
	virtual VOID Update();
	
	/*!
	\brief 隐藏界面
	*/
	VOID Hide();

	VOID Show();

	/*!
	\brief 设置副本剩余时间
	*/
	VOID SetCDTime(DWORD dwCDTime);
private:
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<GameFrameMgr>	m_pFrameMgr;
	
	GUIWnd					*m_pWnd;
	GUIStatic				*m_pCDTime;
	GUIStatic				*m_pLeftTime;
	int						m_nCDTime;		//副本剩余时间
};