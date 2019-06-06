#pragma once
#include "RoleInfoProtoData.h"
#include "..\WorldDefine\msg_role_vcard.h"
#include "..\WorldDefine\role_att.h"

const tstring MALE_PIC_PATH     = _T("data\\ui\\state\\L_status-image-male.bmp");
const tstring FEMALE_PIC_PATH   = _T("data\\ui\\state\\L_status-image-female.bmp");

class HttpPicEx;

const DWORD REFRESH_DELAY = 3000;   //!< 单位：毫秒

/**
	*class InformationFrame
	*brief 角色名贴界面
	*author hyu
	*base GameFrame
	*remark
*/
class InformationFrame : public GameFrame
{
public:
	InformationFrame(void);
	~InformationFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

	//刷新服务器上的角色名贴到本地
	VOID UpdateInformation();

private:

	//初始化控件数据
	VOID InitInfoData();

	//处理游戏frame事件
	DWORD OnGameEvent(tagGameEvent* pGameEvent);

	//处理ui事件
	DWORD EventHandler(tagGUIEvent* pEvent);

	//当点击区域时更新省列表
	VOID ChangeProvince(INT areaRow);

	//处理获取角色名贴数据网络消息
	DWORD OnNS_GetRoleVCard(tagNS_GetRoleVCard* pMsg, DWORD dwParam);

	//保存角色名贴数据
	VOID SaveAll();

	//处理设置角色名贴数据网络消息
	DWORD OnNS_SetRoleVCard(tagNS_SetRoleVCard* pMsg, DWORD dwParam);

	// 玩家改名
	DWORD OnNS_LPRename( tagNS_LPRename *pNetCmd, DWORD );

	// 只保存10条更名记录
	tstring GetRenameHis();

private:
	template<class TKey, class TValue>
	TValue FindValuebyKey(map<TKey, TValue>* pMap, TKey tKey);

    bool                            m_bisRefreshing;        // 是否正在刷新

	TSFPTrunk<InformationFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;

	GUIWnd*							m_pWnd;
	GUIWnd*							m_pWndFather;

	GUIStatic*						m_pStcName;					//名字
	GUIStatic*						m_pStcParty;				//帮派
	GUIStatic*						m_pStcLevel;				//等级
	GUIStatic*						m_pStcCareer;				//职业
	GUIStatic*						m_pStcPosition1;			//帮派职位
	GUIStatic*						m_pStcPosition2;

	GUIComboBox*					m_pComboBoxGender;			//性别
	GUIComboBox*					m_pComboBoxConstellation;	//星座
	GUIComboBox*					m_pComboBoxChZodiac;		//生肖
	GUIComboBox*					m_pComboBoxArea;			//地区
	GUIComboBox*					m_pComboBoxProvince;		//省

	GUIEditBox*						m_pEditCityName;			//城市

	GUIEditBoxEx*					m_pEditBoxExPresentation;	//个性签名
	GUIEditBoxEx*					m_pEditRenameHistory;		// 更名历史

	HttpPicEx*						m_pStcHeadPic;				//头像

	GUIButton*						m_pBtRefresh;				//刷新

	GUIPushButton*					m_pPbtVisible;				//其它玩家可见

	GUIButton*						m_pBtSave;					//保存

	map<INT, DWORD>					m_mapConstellationRow;		//星座控件中行和id对应表
	map<DWORD, INT>					m_mapConstellationID;

	map<INT, DWORD>					m_mapChineseZodiacRow;		//生肖控件中行和id对应表
	map<DWORD, INT>					m_mapChineseZodiacID;

	map<INT, DWORD>					m_mapAreaRow;				//区域控件中行和id对应表
	map<DWORD, INT>					m_mapAreaID;

	map<INT, DWORD>					m_mapProvinceRow;			//省市控件中行和id对应表
	map<DWORD, INT>					m_mapProvinceID;

	map<INT, DWORD>					m_mapGenderRow;				//性别控件中行和id对应表
	map<DWORD, INT>					m_mapGenderID;


};
