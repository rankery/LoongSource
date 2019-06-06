#pragma once
#include ".\ChatSysDefine.h"

#include "EmotionFrame.h"
#include "PlayerNameTab.h"
#include "StaticNoInput.h"
#include "..\WorldDefine\role_info.h"
#include "..\WorldDefine\chat.h"
#include "..\WorldDefine\msg_show_item.h"


//-----------------------------------------------------------------------------
//!class ChatFrame.h
//!brief 聊天框架
//-----------------------------------------------------------------------------
class NetSession;
struct tagPrivateChatEvent;
struct tagSocialInfoEvent;
struct tagDungeonEvent;
struct tagChannelSetEvent;
class RichEditBoxForChatInput;
class RichEditBoxForChatShow;
class ScrollRichEditBox;
class StaticForCast;
class HttpPicEx;
class GlintPushButton;

struct tagNPCEaseSpeechEvent;
struct tagNS_MonsterEnterCombat;
struct tagNS_RoleDead;
struct tagNS_Skill;
class ChatFrame:
	public GameFrame
{
public:
	enum SendMsgState
	{
		SendMsgState_NULL = 0,
		SendMsgState_Chat,
		SendMsgState_Show,
	};
public:
	ChatFrame(void);
	~ChatFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Render3D();
	virtual VOID Update();
	//!切换中英文提示图标,isEnglish:是否是英文
	VOID SwitchLangueInfo(bool isEnglish=true);

	//!得输入框中输入的的文字,在发送聊天内容时使用
	VOID GetInputText(tstring& str);	

	//获得发言频道
	ESendChatChannel GetSendChannel(){return m_curSendChannel;}

	//获得显示频道
	DWORD GetRecvChannel(){return m_curRecvChannel;}

	//过滤格式自定义格式字符串
	VOID FilterMarkerString(tstring&  strText);

	//删除richEditBox为了进行排版在文字（strText）里自动加上的回车键
	VOID DeleteRETURNS(tstring& strText);

	//设置滚动文本，默认时间为1分钟(时间未定)
	VOID PushScrollText(LPCTSTR szText, float fTime = 60.0f);

	//设置传音，默认时间为1分钟(时间未定)
	VOID PushCastText(LPCTSTR szText, DWORD dwRoleID, float fTime = 30.0f);

	//添加从服务器接受的聊天数据到相应的显示频道
	VOID PushInfo(const TCHAR* strText, ESendChatChannel escc= ESCC_End);

	//!为RichEditBox注册图片（转义字符）
	//!这里由于使用的是固定路径，而且按序号排好图片所以只需要index
	void RegisterPicForRichEdit(const TCHAR *szName,int index);

	//获得输入框当前输入文本的大小
	int GetInputValidTextSize();

	//接收服务器发送的聊天信息
	DWORD NetRecvMsgNS_RoleChat( tagNS_RoleChat* pMsg, DWORD dwParam );

	void OnSendChatMsg(const tstring& strMsg, ESendChatChannel eChannel, DWORD dwDestID=GT_INVALID, BYTE byAutoReply=0);

	//接收win32消息
	void OnWin32Msg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);

	void SetPvtRoleName(const TCHAR* name);

	HKL GetEnglishHKL() { return m_HKLenglish; }

private:
	DWORD EventHandler(tagGUIEvent* pEvent);
	BOOL GUIEventHandlerMenu(tagGUIEvent* pEvent);

	DWORD OnGameEventForChat(tagGameEventForChat* pEvent);

	//--得到角色名字、ID
	DWORD OnRecvNetRoleGetName( tagRoleGetNameEvent* pMsg);
	DWORD OnRecvNetRoleGetID( tagRoleGetIDEvent* pMsg);
	//--得到角色的贴图URL
	DWORD OnRecvHeadpicByID( tagGetHeadpicByID* pGameEvent );
	//目标窗口设置私聊
	DWORD OnPrivateChatEvent(tagPrivateChatEvent* pEvent);
	//接收好友上下线提示
	DWORD OnSocialInfoEvent(tagSocialInfoEvent* pEvent);
	//牢狱广播
	DWORD OntagDungeonEvent(tagDungeonEvent* pEvent);
	// NPC休闲动作说话
	DWORD OnNPCEaseSpeechEvent(tagNPCEaseSpeechEvent* pEvent);
	// 频道设置
	DWORD OnChannelSetEvent(tagChannelSetEvent* pGameEvent);
	// 留言
	DWORD OnLeaveMsg(tagGameEvent* pGameEvent);

	//初始化时设置聊天显示频道
	VOID ResetRecvChannel();

	//设置当前说话频道按钮显示
	VOID SetCurChannel(ESendChatChannel eChannel);

	//将发言频道转换为客户端显示频道
	DWORD TranIntEChat2EClient(ESendChatChannel eChannel);

	//输入框输入内容发生变化
	VOID OnInputContextChange();

	//处理命令行切换聊天频道
	ESendChatChannel TranCMD2SendChannel(int nChannel);

	//切换聊天显示频道，玩家通过发言频道listbox选择
	VOID OnSwitchPageShow(int index);

	//点击显示频道接钮时，自动切换发言频道的转换函数，用于界面操作
	ESendChatChannel TranRecv2SendChannel(int nRecvChannel);

	//将聊天信息添加到显示频道
	VOID AddStringToWnd(const tstring& strText, int nPate, ESendChatChannel escc);

	//向输入框添加表情
	void AddInputEmotion(const tstring &str);

	//处理私聊，传入有玩家名字的字符串，返回玩家名字
	tstring HandleStringForPvt(tstring& strText);

	//--发言
	void OnSendCastTalk();
	void OnSendWorld();
	void OnSendPvt();
	void OnSendGuild();
	void OnSendTroop();
	void OnSendCurrent();
	void OnSendAffiche();

	//是否重复发言
	bool IsViladeSpeak(const tstring& str,ESendChatChannel escc);

	//世界频道发言是否间隔小于10秒
	bool IsIntervalSpeak(ESendChatChannel escc);

	//得到相应频道的字体颜色
	const TCHAR* GetChatChannelColor(ESendChatChannel escc);

	//隐藏窗口
	void HideWnd();

	//--展示装备或物品
	void OnRoleSendItemOrEquip(ShowEvent* pEvent);
	DWORD NetRecvMsgNS_RoleShowEquip(tagNS_RoleShowEquip* pMsg, DWORD dwParam);
	DWORD NetRecvMsgNS_RoleShowItem(tagNS_RoleShowItem* pMsg, DWORD dwParam);
	DWORD OnNS_TemmatePickUpBroadCast( tagNS_RoleShowDroptItemInTeam* pMsg, DWORD dwParam );

	void OnRoleShowItem(DWORD data, const TCHAR* szSrcRole, const TCHAR* szDstRole, ESendChatChannel escc);
	void OnRoleShowEquip(const tagEquip& data, const TCHAR* szSrcRole, const TCHAR* szDstRole, ESendChatChannel escc);

	//--个性动作
	BOOL JudePersonalityAct();

	//重新设置焦点
	void SetActive();

	//自动回复
	void AutoReplyPvt(DWORD dwRoleID, LPCTSTR strText);

	void ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow );
	void ForceMoveMenu(const tagPoint& pos);

	//NPC或怪说话
	DWORD OnNS_MonsterEnterCombat(tagNS_MonsterEnterCombat* pMsg, DWORD dwParam);
	DWORD OnNS_RoleDead(tagNS_RoleDead* pMsg, DWORD dwParam);
	DWORD OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);

	//系统发布的跑马灯
	void OnNS_AutoNotice(tagNS_AutoNotice* pMsg, DWORD dwParam);
	tstring AutoNoticeColor(INT nType);

	//频道设置
	void ChannelSet();
	void SaveChannelSet();

	//快捷键设置
	DWORD OnKey(DWORD dwID, BOOL bDown);
	DWORD OnKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	
private:

	TSFPTrunk<ChatFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameInputMap>		m_pKeyMap;

	GUIWnd*						m_pWnd;
	GUIStatic*					m_pWndCaption;
	GUIWnd*						m_pSclFatherWnd;			//跑马灯你控件
	GUIButton*					m_pBtnHide;					//隐藏按钮
	GUIButton*					m_pBtnChannelSet;			//频道设置按钮
	GUIButton*					m_pBtnLeave;				//留言
	GUIButton*					m_pBtnCurChannel;			//当前发言频道按钮
	GUIButton*					m_pBtnEmotion;				//表情按钮
	GUIStatic*					m_pStaticLangue[2];			//中英文提示
	RichEditBoxForChatInput*	m_pRebInput;				//聊天输入框
	GUIStatic*					m_pInpuBack;				//输入框背景
	GUIListBox*					m_pChannelSel;				//发言频道选择
	GUIWnd*						m_pWndChannel;				//listbox父窗体
	GUIPatch*					m_pMidBack;					//
	ScrollRichEditBox*			m_pScrollRichText;			//跑马灯
	GUIFrameMutex*				m_pMutex;					//接收频道的互斥控件
	//GUIPushButton*				m_pPage[NUM_CHAT_CHANNEL];	//对就显示频道的pushbutton
	GlintPushButton*				m_pPage[NUM_CHAT_CHANNEL];

	RichEditBoxForChatShow*		m_pChatContext[NUM_CHAT_CHANNEL];				//聊天的显示(接收)频道
	GUIScrollBar*				m_pScl[NUM_CHAT_CHANNEL];	//对应的8个滚动条
	GUIButton*					m_pBtnUp[NUM_CHAT_CHANNEL];	//对应滚动条的向上按钮
	GUIButton*					m_pBtnDp[NUM_CHAT_CHANNEL];	//对应滚动条的向下按钮

	GUIWnd*						m_pWndHeadPic;				//传音头像
	GUIWnd*						m_pWndImName;				//传音姓名
	StaticForCast*				m_pStcName[NUM_CHAT_CAST_WND];//传音玩家的姓名
	HttpPicEx*					m_pStcHeadPic[NUM_CHAT_CAST_WND];//传音减低
	GUIWnd*						m_pStcCast;
	ScrollRichEditBox*			m_pCastRichText[NUM_CHAT_CAST_WND];	//传音
	GUIButton*					m_pBtnZoomUp;					//缩放按钮向上
	GUIButton*					m_pBtnZoomDn;					//缩放按钮向下
	
	GUIWnd*						m_pWndWorld;
	GUIButton*					m_pBtnWorldOK;
	GUIButton*					m_pBtnWorldCancel;
	GUIPushButton*				m_pPBtnWorld;
	GUIStatic*					m_pStcExWold;

	GUIWnd*						m_pWndMenuEx;					//扩展菜单
	GUIListBox*					m_pListMenuEx;					//菜单选项

	INT							m_nZoomMax;

	BOOL						m_bChatContext;
	BOOL						m_bHide;

	
	ESendChatChannel			m_curSendChannel;			//记录当前发言频道
	int							m_curRecvChannel;			//当前显示频道
	DWORD						m_dwRecvChalShow[NUM_CHAT_CHANNEL];			//当前显示频道过滤	
	
	LONG						m_lnSlcFatherWndX;			// 跑马灯父控件的client坐标
	LONG						m_lnCastWndX;				//传音父控件的client坐标
	LONG						m_lnCastWndY;

	EmotionFrame*				m_pEmotionFrame;			//表情框架

	
	vector<tagRecvSpeak>		m_vecRecv;					//接受的聊天数据缓存，保存到正确显示出来为止
	vector<tagSendSpeak>		m_vecSpeak;					//已经发出的发言，保留一定时间
	vector<tagSendSpeak>		m_vecPvtSpeak;				//私聊说的话
	vector<tagScroText>			m_vecSclText;				//从服务器接收跑马灯消息缓存
	vector<tagScroText>			m_vecCast;					//保存的传音
	vector<tagScroText>			m_vecCastNoName;			//保存的传音,没有得到角色名s
	DWORD						m_dwLocalID;				//本地玩家ID
	tstring						m_strLocalName;				//本地玩家姓名
	tstring						m_strLastName;				//上一次私聊玩家姓名
	tstring						m_strMenuName;				//扩展菜单点击时的玩家姓名

	vector<tagChatShowEquip>	m_vecRecvEquip;				//接收的展示装备缓存
	vector<tagChatShowItem>		m_vecRecvItem;				//接收的展示物品缓存

	BOOL						m_bWorldOK;					//世界频道发言扣钱确认
	SendMsgState				m_eSendMsgState;
	ShowEvent					m_ShowEvent;
	HKL							m_HKL;
	HKL							m_HKLenglish;

};
