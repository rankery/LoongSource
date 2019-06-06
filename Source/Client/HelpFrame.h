#pragma once

#include "AppliedTree.h"
#include "IconStatic.h"
#include "HelpData.h"


class HelpFrame : public GameFrame
{
public:
	HelpFrame();
	~HelpFrame();

	virtual BOOL Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	bool IsVisible() const { return m_bVisible; }
	void Show() { m_pWnd->SetInvisible(FALSE); m_bVisible = true; }
	void Hide() { m_pWnd->SetInvisible(TRUE); m_bVisible = false; }
	

private:
	BOOL Initilize();
	BOOL EventHandler(tagGUIEvent* pEvent);
	
	void AddCategory(const tagCategory *pCategory);
	DWORD AddSubCategory(const tagSubCategory *pSubCategory);

    void ReleaseKeyMap();

	void OnGameInfoClick();

	void OnEvent_OpenNewHelp(tagGameEvent *pEvent);
	void OnEvent_CloseNewHelp(tagGameEvent *pEvent);

private:
	TSFPTrunk<HelpFrame> m_trunk;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<GameFrameMgr>	m_pFrameMgr;
	GUIWnd *m_pWnd;
	AppliedTree *m_pMenuTree;
	IconStatic *m_pImg;

	GUIButton *m_pMallButton;
	GUIButton *m_pBackButton;
	GUIButton *m_pNextButton;
	GUIButton *m_pCloseButton;

	
	typedef std::map<DWORD, std::vector<tstring>* > KeyMap;
	KeyMap m_mapKey2Pictures;
	typedef std::map<DWORD, int> IndexMap;

	DWORD m_curItemID;
	DWORD m_defaultItemID;
	IndexMap m_mapKey2Index;
	HelpData m_data;

	bool m_bVisible;
};

