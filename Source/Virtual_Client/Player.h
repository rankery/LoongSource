
#ifndef PLAYER_H_
#define PLAYER_H_
//#include "player_net_cmd_mgr.h"
#include "FrameMgr.h"
#include "Mutex.h"

#include <string>
#include <vector>
using namespace std;

enum EPlayerType
{
	EPT_Login,
	EPT_World,
	EPT_Center,

	EPT_End
};

class VCMutex;

class Player
{
public:
	Player();
	~Player();

	BOOL Init();
	VOID Destroy();

	//用于登陆,跑图
	VOID ThreadUpdate();

	//Frame切换
	BOOL ChangeFrame(FrameMgr::EFrame eFrame);

	//行为设置
	VOID SetPlayerChat(BOOL bChat, BYTE byChannel = 5);
	VOID SetPlayerMove(BOOL bMove);

	//取得行为状态
	BOOL GetChatStatus()	{ return m_bCanChat; }
	BYTE GetChatChannel()	{ return m_byChatChannel; }
	BOOL GetMoveStatus()	{ return m_bCanWalk; }
	VOID GetNextWayPoint();

	//取得Log对象
	TObjRef<Log>& GetLog()	{ return m_pLog; }

	//封装线程锁
	VOID Acquire() { m_pLock->Acquire(); }
	VOID Release() { m_pLock->Release(); }
	BOOL AttemptAcquire() { return m_pLock->AttemptAcquire(); }

public:
	TObjRef<XClient>			m_pWorldSession;	//与world的连接
	TObjRef<StreamTransport>	m_pLoginSession;	//与login的连接

	DWORD						m_dwID;				//机器人ID
	tstring						m_strVersion;		//游戏版本
	tstring						m_strName;			//用户名
	tstring						m_roleName;			//创建角色姓名
	tstring						m_strPassword;		//密码
	tstring						m_strThreadName;	//线程名字
	VOLATILE BOOL				m_bTerminate;		// 线程停止标志
	tstring						m_strMapName;		// Map名称
	Vector3						m_srcPos;			// 行走的起始点
	Vector3						m_desPos;			// 行走的终点
	INT							m_wayPoint;			// 路点类型
	INT							m_type;				//类型 1: 登陆型 2. 登陆,跑图型 3.中心激活
	DWORD						m_SendTime;			//发包间隔
	BOOL						m_bLog;				//是否记Log	
	BOOL						m_bAbandon;			//是否已经废弃
	INT							m_Start_x;			//起始点,用于GM goto
	INT							m_Start_z;			//起始点,用于GM goto
	vector<Vector3>				m_vecWayPoint;
	vector<Vector3>::iterator	m_it;
	tagLoginInfo*				m_pLoginInfo;		//玩家当前登录信息

private:
	TSFPTrunk<Player>	        m_Trunk;
	TObjRef<Thread>				m_pThread;
	TObjRef<VarContainer>		m_pVar;
	TObjRef<Util>				m_pUtil;
	TObjRef<Util>				m_pLoginIPUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<Log>				m_pLog;				//Log
	string						log_name;			//Log名称

	BOOL						m_bCanWalk;			//可以行走
	BOOL						m_bCanCombat;		//可以战斗
	BOOL						m_bCanChat;			//是否聊天

	BYTE						m_byChatChannel;	//聊天频道

	DWORD						m_dwCreatureID;
	DWORD						m_dwMsgCount;
	BOOL						m_bBlock;			//行走失败

private:
	FrameMgr*					m_pFrameMgr;			// Frame管理
	VCMutex*					m_pLock;
};

#endif /* PLAYER_H_ */