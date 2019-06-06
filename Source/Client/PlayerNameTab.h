#pragma once
#include ".\NetSession.h"
#include "..\WorldDefine\role_info.h"
#include "..\WorldDefine\msg_role_title.h"
#include "..\WorldDefine\msg_role_vcard.h"
/**	\class PlayerNameTab
	\brief 存储所有曾经用到过的玩家名字和id
*/
class GameFrame;
struct tagRoleGetIDEvent
	:public tagGameEvent
{
	DWORD	dwRoleNameCrc;	// 角色名称crc32
	DWORD	dwRoleID;		// 角色名字crc32
	DWORD	dwParam;
	tagRoleGetIDEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		:tagGameEvent(szEventName, pSenderFrame)
	{}
};
struct tagRoleGetNameEvent
	: public tagGameEvent
{
	BOOL	bResult;		// TRUE = 成功
	DWORD	dwRoleID;		// 请求目标玩家的roleid
	tstring	strRoleName;	// 角色name
	DWORD	dwParam;
	tagRoleGetNameEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		:tagGameEvent(szEventName, pSenderFrame)
	{}
};

struct tagRoleGetSomeNameEvent
	: public tagGameEvent
{
	std::vector<DWORD> IDs;	// 取得了哪些ID的名字了
	tagRoleGetSomeNameEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame)
	{}
};

struct tagGetNameByNameID : public tagGameEvent
{
	BOOL	bResult;		//!< TRUE = 成功
	DWORD	dwNameID;		//!< 请求的NameID
	tstring	strName;		//!< Name
	tagGetNameByNameID(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame) {}
};

struct tagGetHeadpicByID : public tagGameEvent
{
    DWORD       dwRoleID;
    tstring     strImageURL;
    tagGetHeadpicByID(LPCTSTR szEventName, GameFrame* pSenderFrame)
        : tagGameEvent(szEventName, pSenderFrame) {}
};

struct tagRoleGetTitleEvent
	: public tagGameEvent
{
	DWORD		dwRoleID;		// 角色ID
	DWORD		dwTitleID;		// 若为GT_INVALID,无效
	DWORD		dwParam;
	tagRoleGetTitleEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		:tagGameEvent(szEventName, pSenderFrame)
	{}
};

class PlayerNameTab
{
public:
	PlayerNameTab(void);
	~PlayerNameTab(void);

	static PlayerNameTab* Inst();

	void Init();
	void Destroy();

	// 每帧更新
	void Update();

	//!	失败则返回GT_INVALID
	DWORD FindIDByName(const TCHAR* szName);

	//!	失败则返回NULL,不合法的ID返回GFC_NULL
	const TCHAR* FindNameByID(DWORD id);

	//! 失败返回空字符
	const TCHAR* FindNameByNameID(DWORD dwNameID);

    //! 失败返回空字符
    LPCTSTR FindHeadPicURLbyID(DWORD dwRoleID);
	
	//！是否能够得到所需的一堆名字.
	//能全部取得返回true
	//不能全部取得返回false, 且将不能取得的名字通过Cantfinds传出来
	bool CanGetBundleOfNames(const std::vector<DWORD>& IDs, std::vector<DWORD>& Cantfinds);

	bool IsIDExist(DWORD dwID)
	{
		if(m_id2name.find(dwID) != m_id2name.end()) 
			return true;
		else
			return false;
	}

	// 响应网络消息
	void OnNetRoleGetID(tagNS_RoleGetID* pNetCmd, DWORD dwParam);
	
	//! 通过NameID取名字
	void OnNS_GetNamebyNameID(tagNS_GetNamebyNameID* pNetCmd, DWORD dwParam);	//by hyu

    //! 通过id取headpic
    void OnNS_GetRoleHeadPicURL(tagNS_GetRoleHeadPicURL* pNetCmd, DWORD dwParam);

	// 响应请求多个名字的消息
	void OnNS_RoleGetSomeName(tagNS_RoleGetSomeName *pCmd, DWORD);

protected:
	//添加保存记录
	void AddRecord(DWORD id,const TCHAR* szName);

private:
	TSFPTrunk<PlayerNameTab>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	map<DWORD,tstring>	m_id2name;
	map<tstring,DWORD>	m_name2id;				// Role的Name和ID的一一对应
	map<DWORD, tstring> m_NameID2Name;			// NmaeID和Name对应缓冲表 added by hyu
    map<DWORD, tstring> m_RoleID2HeadPicUrl;	// RoleID和头像URL对应缓冲表

	// 正在通过名字请求ID的缓存<NameCrc,Name>
	map<DWORD,tstring>	m_mapName2IDReq;

	pair<DWORD, list<DWORD> >	m_paReqRole;		 // 通过ID取名字
    //通过ID取头像URL的缓存
    set<DWORD>          m_setRoleID2HeadPic;
	//! 通过NameID取Name的缓存 added by hyu
	set<DWORD>			m_setNameID2NameReq;
};