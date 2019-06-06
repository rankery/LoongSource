#pragma once

class TeammateStateWnd;
struct tagAddTeammateEvent;
struct tagDelTeammateEvent;
struct tagLeaveTeamEvent;

/** \class TeammatesStateFrame
    \brief 队友状态界面管理
*/
class TeammatesStateFrame : public GameFrame
{
public:
	enum { WND_OFFSET_X = 20, WND_OFFSET_Y = 1 };

	TeammatesStateFrame();
	virtual ~TeammatesStateFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual VOID Render3D();
private:
	TeammateStateWnd* NewWnd( DWORD dwRoleID, int nIndex );
	VOID DeleteAllWnd();
	DWORD OnAddTeammateEvent(tagAddTeammateEvent* pEvent);
	DWORD OnDelTeammateEvent(tagDelTeammateEvent* pEvent);
	DWORD OnLeaveTeamEvent(tagLeaveTeamEvent* pEvent);
private:
	TSFPTrunk<TeammatesStateFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	tagPoint                        m_ptWndPosStart;      // 队友窗口的起始坐标
	tagPoint						m_ptWndSize;		  // 队友窗口的大小
	vector<TeammateStateWnd*>       m_vecWnd;             // 窗口列表
};

