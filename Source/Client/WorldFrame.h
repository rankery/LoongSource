#pragma once
#include "ClientCamera.h"
#include "..\WorldDefine\msg_map.h"
#include "..\WorldDefine\role_info.h"
#include "../WorldDefine/msg_player_preventlost.h"

/** \class WorldFrame
	\brief 游戏世界逻辑框架
*/
class WorldFrame : public GameFrame
{
public:
	enum EWorldState
	{
		EWState_GotoNewMap,			//接收服务发送的gotomap消息
		EWState_Playing,			//可以开始玩了

	};

public:
	WorldFrame();
	~WorldFrame();

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Render3D();
	virtual VOID Update();

	//--
	ClientCamera* GetCamera()	{ return &m_camera; }
	
private:
	TSFPTrunk<WorldFrame>		m_Trunk;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<AudioSys>			m_pAudioSys;
	TObjRef<GUISystem>			m_pSys;
	ClientCamera				m_camera;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	EWorldState					m_curState;
	TObjRef<Util>				m_pUtil;

	BOOL	LoadRes(const TCHAR* szMapName, DWORD dwMapID, const Vector3& localPlayerPos, const Vector3& localPlayerFaceto);
	void	switchToState(EWorldState state);

	DWORD	NetRecvGotoMap(tagNS_GotoNewMap* pMsg, DWORD pPrama);

	// 响应网络消息
	/*DWORD	NetRecvRoleGetName(tagNS_RoleGetName* pNetCmd, DWORD dwParam);*/
	DWORD	NetRecvRoleGetID(tagNS_RoleGetID* pNetCmd, DWORD dwParam);

	//初始化神龙赐福模块
	DWORD NetRecvLoongBeneInit( tagNS_LoongBenedictionInit* pNetCmd, DWORD dwParam );

	// 断开与服务器联接
	DWORD	OnDisconnectionEvent(tagMsgBoxEvent* pEvent);

	// 处理ESC键
	DWORD HandleESC( DWORD dwID, BOOL bDown );
};

inline WorldFrame* GetWorldFrm()
{
	return (WorldFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("World"));
}
