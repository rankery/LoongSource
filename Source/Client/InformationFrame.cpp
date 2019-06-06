#include "StdAfx.h"
#include "InformationFrame.h"
#include "CharacterFrame.h"
#include "NetSession.h"
#include "RoleMgr.h"
#include "HttpPicEx.h"
#include "FilterData.h"
#include "LocalPlayer.h"

const int MAX_RENAME_HISTORY = 10;

InformationFrame::InformationFrame(void) : m_Trunk(this),
	m_pWnd(NULL), m_pWndFather(NULL), m_bisRefreshing(false)
{
}

InformationFrame::~InformationFrame(void)
{
}

BOOL InformationFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	//注册网络消处处理函数
	m_pCmdMgr->Register("NS_GetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_GetRoleVCard), _T("NS_GetRoleVCard"));
	m_pCmdMgr->Register("NS_SetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_SetRoleVCard), _T("NS_SetRoleVCard"));
	m_pCmdMgr->Register("NS_LPRename",	   (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_LPRename),	   _T("NS_LPRename"));

	return bRet;
}

BOOL InformationFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	//注销GUI事件处理函数
	m_pGUI->UnRegisterEventHandler(m_pWndFather->GetFullName().c_str(), m_Trunk.sfp1(&InformationFrame::EventHandler));

	//注销网络消处处理函数
	m_pCmdMgr->UnRegister("NS_GetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_GetRoleVCard));
	m_pCmdMgr->UnRegister("NS_SetRoleVCard", (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_SetRoleVCard));
	m_pCmdMgr->UnRegister("NS_LPRename",	 (NETMSGPROC)m_Trunk.sfp2(&InformationFrame::OnNS_LPRename));

	return bRet;
}

BOOL InformationFrame::ReloadUI()
{
	// 创建
	m_pWndFather				=	m_pGUI->GetWnd(((CharacterFrame*)m_pFather)->GetGUIFullName().c_str());
	m_pWnd						=	m_pWndFather->GetChild(_T("informationpic"));

	m_pStcName					=	(GUIStatic*)m_pWnd->GetChild(_T("rname"));											//名字
	m_pStcParty					=	(GUIStatic*)m_pWnd->GetChild(_T("rparty"));											//帮派
	m_pStcLevel					=	(GUIStatic*)m_pWnd->GetChild(_T("rgrad1"));											//等级
	m_pStcCareer				=	(GUIStatic*)m_pWnd->GetChild(_T("rcareer"));										//职业
	m_pStcPosition1				=	(GUIStatic*)m_pWnd->GetChild(_T("rpost1"));											//帮派职位
	m_pStcPosition2				=	(GUIStatic*)m_pWnd->GetChild(_T("rpost2"));

	m_pComboBoxGender			=	(GUIComboBox*)m_pWnd->GetChild(_T("information\\gender"));							//性别
	m_pComboBoxConstellation	=	(GUIComboBox*)m_pWnd->GetChild(_T("information\\constellation"));					//星座
	m_pComboBoxChZodiac			=	(GUIComboBox*)m_pWnd->GetChild(_T("information\\zodiac"));							//生肖
	m_pComboBoxArea				=	(GUIComboBox*)m_pWnd->GetChild(_T("information\\area"));							//地区
	m_pComboBoxProvince			=	(GUIComboBox*)m_pWnd->GetChild(_T("information\\city"));							//省

	m_pEditCityName				=	(GUIEditBox*)m_pWnd->GetChild(_T("information\\cityname\\cityname_input"));			//市

	m_pEditBoxExPresentation	=	(GUIEditBoxEx*)m_pWnd->GetChild(_T("information\\presentationpic\\presentation"));	//个性签名
	m_pEditRenameHistory		=	(GUIEditBoxEx*)m_pWnd->GetChild(_T("information\\presentationpic2\\presentation"));	//历史更名

	m_pStcHeadPic				=	(HttpPicEx*)m_pWnd->GetChild(_T("information\\headpicback\\headpic"));				//头像

	m_pBtRefresh				=	(GUIButton*)m_pWnd->GetChild(_T("information\\new"));								//刷新

	m_pPbtVisible				=	(GUIPushButton*)m_pWnd->GetChild(_T("visible\\optnutt1"));							//其它玩家可见

	m_pBtSave					=	(GUIButton*)m_pWnd->GetChild(_T("save"));											//保存

	//注册GUI事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndFather->GetFullName().c_str(), m_Trunk.sfp1(&InformationFrame::EventHandler));

	//初始化控件数据
	InitInfoData();

	return TRUE;
}

VOID InformationFrame::Update()
{
    /*! 计算累积时间，让玩家点击刷新时有所延迟 */
    static DWORD dwAccumTime = 0;
    if (m_bisRefreshing)
    {
        dwAccumTime += Kernel::Inst()->GetDeltaTimeDW();
        if (dwAccumTime >= REFRESH_DELAY)
        {
            dwAccumTime = 0;
            m_bisRefreshing = false;
            m_pBtRefresh->SetEnable(true);
            m_pBtRefresh->SetRefresh();
        }
    }
}

VOID InformationFrame::UpdateInformation()
{
	//请求获得远程玩家名贴数据
	tagNC_GetRoleVCard msg;
	msg.dwRoleID = RoleMgr::Inst()->GetLocalPlayerID();

	m_pSession->Send(&msg);
}

DWORD InformationFrame::OnGameEvent( tagGameEvent* pGameEvent )
{
	return 0;
}

DWORD InformationFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
    case EGUIE_Active:
        if( pWnd == m_pComboBoxGender->GetEditBox() )
            m_pGUI->SetActive( m_pComboBoxGender->GetListBox() );
        else if( pWnd == m_pComboBoxConstellation->GetEditBox() )
            m_pGUI->SetActive( m_pComboBoxConstellation->GetListBox() );
        else if( pWnd == m_pComboBoxChZodiac->GetEditBox() )
            m_pGUI->SetActive( m_pComboBoxChZodiac->GetListBox() );
        else if( pWnd == m_pComboBoxArea->GetEditBox() )
            m_pGUI->SetActive( m_pComboBoxArea->GetListBox() );
        else if( pWnd == m_pComboBoxProvince->GetEditBox() )
            m_pGUI->SetActive( m_pComboBoxProvince->GetListBox() );
        break;
	case EGUIE_Click:
		if (pWnd == m_pBtRefresh)	//刷新
		{
			UpdateInformation();
            m_bisRefreshing = true;
            m_pBtRefresh->SetEnable(false);
            m_pBtRefresh->SetRefresh();
		}
		else if (pWnd == m_pBtSave)	//保存
		{
			SaveAll();
		}
		break;
	case EGUIE_PushButtonDown:
		if (pWnd == m_pPbtVisible)	//其它玩家可见
		{
		}
		break;
	case EGUIE_ItemClick:
		if (pWnd == m_pComboBoxConstellation->GetListBox())	//星座选择
		{
			int i = pEvent->dwParam1;
		}
		if (pWnd == m_pComboBoxArea->GetListBox())	//区域选择
		{
			//刷新省份列表
			ChangeProvince((INT)pEvent->dwParam1);
		}
        if (pWnd == m_pComboBoxGender->GetListBox())    // 性别选择
        {
            switch (pEvent->dwParam1)
            {
            case 0:
                m_pStcHeadPic->SetDefaultPic(MALE_PIC_PATH);
            	break;
            case 1:
                m_pStcHeadPic->SetDefaultPic(FEMALE_PIC_PATH);
                break;
            case 2:
                m_pStcHeadPic->SetDefaultPic(_T(""));
            }
        }
		break;

	}
	return 0;
}

VOID InformationFrame::InitInfoData()
{
	//填充性别数据
	{
		//男
		m_pComboBoxGender->GetListBox()->SetText(0, 0, g_StrTable[_T("RoleInfoMale")]);
		m_mapGenderRow.insert(make_pair(0, 1));
		m_mapGenderID.insert(make_pair(1, 0));
		//女
		m_pComboBoxGender->GetListBox()->SetText(1, 0, g_StrTable[_T("RoleInfoFemale")]);
		m_mapGenderRow.insert(make_pair(1, 2));
		m_mapGenderID.insert(make_pair(2, 1));
		//保密
		m_pComboBoxGender->GetListBox()->SetText(2, 0, g_StrTable[_T("RoleInfoDefault")]);
		m_mapGenderRow.insert(make_pair(2, 3));
		m_mapGenderID.insert(make_pair(3, 2));

		//调整下拉框
		tagPoint ptSize	= m_pComboBoxGender->GetListBox()->GetSize();

		ptSize.x = m_pComboBoxGender->GetEditBox()->GetSize().x;
		ptSize.y = m_pComboBoxGender->GetListBox()->GetRowHeight() * 3;

		m_pComboBoxGender->GetListBox()->SetResizable(TRUE);
		m_pComboBoxGender->GetListBox()->SetColWidth(0, ptSize.x);
		m_pComboBoxGender->GetListBox()->Resize(ptSize);
		m_pComboBoxGender->GetListBox()->SetResizable(FALSE);
	}
	

	//填充星座数据
	int i = 0;
	const map<DWORD, tagConstellationProto>* pCstMap =
		RoleInfoProtoData::inst()->GetConstellation();

	for (map<DWORD, tagConstellationProto>::const_iterator citer = pCstMap->begin();
		citer != pCstMap->end(); ++citer)
	{
		//下拉框插入第i行
		m_pComboBoxConstellation->GetListBox()->SetText(i, 0,
			citer->second.strName.c_str());

		//下拉框列表和数据id对照表数据插入
		m_mapConstellationRow.insert(make_pair(i, citer->first));
		m_mapConstellationID.insert(make_pair(citer->first, i));
		++i;

		//调整下拉框
		tagPoint ptSize = m_pComboBoxConstellation->GetListBox()->GetSize();

		ptSize.x = m_pComboBoxConstellation->GetEditBox()->GetSize().x;
		ptSize.y = m_pComboBoxConstellation->GetListBox()->GetRowHeight() * pCstMap->size();

		m_pComboBoxConstellation->GetListBox()->SetResizable(TRUE);
		m_pComboBoxConstellation->GetListBox()->SetColWidth(0, ptSize.x	);
		m_pComboBoxConstellation->GetListBox()->Resize(ptSize);
		m_pComboBoxConstellation->GetListBox()->SetResizable(FALSE);
	}

	//填充生肖数据
	i = 0;
	const map<DWORD, tagChinesZodiacProto>* pCzcMap =
		RoleInfoProtoData::inst()->GetChineseZodiac();

	for (map<DWORD, tagChinesZodiacProto>::const_iterator citer = pCzcMap->begin();
		citer != pCzcMap->end(); ++citer)
	{
		//下拉框插入第i行
		m_pComboBoxChZodiac->GetListBox()->SetText(i, 0,
			citer->second.strName.c_str());

		//下拉框列表和数据id对照表数据插入
		m_mapChineseZodiacRow.insert(make_pair(i, citer->first));
		m_mapChineseZodiacID.insert(make_pair(citer->first, i));
		++i;

		//调整下拉框
		tagPoint ptSize = m_pComboBoxChZodiac->GetListBox()->GetSize();

		ptSize.y = m_pComboBoxChZodiac->GetListBox()->GetRowHeight() *
			RoleInfoProtoData::inst()->GetChineseZodiac()->size();

		m_pComboBoxChZodiac->GetListBox()->SetResizable(TRUE);
		m_pComboBoxChZodiac->GetListBox()->Resize(ptSize);
		m_pComboBoxChZodiac->GetListBox()->SetResizable(FALSE);
	}

	//填充区域数据
	i = 0;
	const map<DWORD, tagAreaProto>* pAreaMap = RoleInfoProtoData::inst()->GetArea();

	for (map<DWORD, tagAreaProto>::const_iterator citer = pAreaMap->begin();
		citer != pAreaMap->end(); ++citer)
	{
		//下拉框插入第i行
		m_pComboBoxArea->GetListBox()->SetText(i, 0,
			citer->second.strName.c_str());
		
		//下拉框列表和数据id对照表数据插入
		m_mapAreaRow.insert(make_pair(i, citer->first));
		m_mapAreaID.insert(make_pair(citer->first, i));
		++i;

		//调整下拉框
		tagPoint ptSize = m_pComboBoxArea->GetListBox()->GetSize();

		ptSize.y = m_pComboBoxArea->GetListBox()->GetRowHeight() *
			RoleInfoProtoData::inst()->GetArea()->size();

		m_pComboBoxArea->GetListBox()->SetResizable(TRUE);
		m_pComboBoxArea->GetListBox()->Resize(ptSize);
		m_pComboBoxArea->GetListBox()->SetResizable(FALSE);
	}
}

//随区域下拉框更改而更改省份下拉框数据
VOID InformationFrame::ChangeProvince( INT areaRow )
{
	//清空省份下拉框
	m_pComboBoxProvince->GetListBox()->Clear();
	m_pComboBoxProvince->GetEditBox()->SetText(_T(""));

	//清除行id对照map
	m_mapProvinceRow.clear();
	m_mapProvinceID.clear();

	//查找选中的区域id
	map<INT, DWORD>::const_iterator citerAreaRow = m_mapAreaRow.find(areaRow);
	if (citerAreaRow == m_mapAreaRow.end())
		return;

	DWORD areaID = citerAreaRow->second;

	//根据区域id获得区域原型数据
	const map<DWORD, tagAreaProto> *mapArea = RoleInfoProtoData::inst()->GetArea();
	map<DWORD, tagAreaProto>::const_iterator citerArea = mapArea->find(areaID);
	if (citerArea == mapArea->end())
		return;

	//填充省份数据
	int i = 0;
	for (map<DWORD, tagProvinceProto>::const_iterator citer =
		citerArea->second.mapProvince.begin();
		citer != citerArea->second.mapProvince.end(); ++citer)
	{
		m_pComboBoxProvince->GetListBox()->SetText(i, 0, citer->second.strName.c_str());
		//保存行/id对照表
		m_mapProvinceRow.insert(make_pair(i, citer->first));
		m_mapProvinceID.insert(make_pair(citer->first, i));
		++i;

		//调整下拉框
		tagPoint ptSize = m_pComboBoxProvince->GetListBox()->GetSize();

		ptSize.y = m_pComboBoxProvince->GetListBox()->GetRowHeight() *
			citerArea->second.mapProvince.size();

		m_pComboBoxProvince->GetListBox()->SetResizable(TRUE);
		m_pComboBoxProvince->GetListBox()->Resize(ptSize);
		m_pComboBoxProvince->GetListBox()->SetResizable(FALSE);
	}
}

DWORD InformationFrame::OnNS_GetRoleVCard( tagNS_GetRoleVCard* pMsg, DWORD dwParam )
{
	//获得已设置数据
	tagVCardData* pVCardData = (tagVCardData*)pMsg->pData;

	map<DWORD, INT>::iterator it;

	//写入用户已存在服务器的设置

	//性别
	it = m_mapGenderID.find((DWORD)pVCardData->customVCardData.bySex);
	if (it != m_mapGenderID.end())
	{
		m_pComboBoxGender->GetListBox()->SetCurSelectedRow(it->second);
		LPCTSTR szTemp = m_pComboBoxGender->GetListBox()->GetText(it->second, 0);
		m_pComboBoxGender->GetEditBox()->SetText(szTemp);
	}

	//星座
	it = m_mapConstellationID.find((DWORD)pVCardData->customVCardData.byConstellation);
	if (it != m_mapConstellationID.end())
	{
		m_pComboBoxConstellation->GetListBox()->SetCurSelectedRow(it->second);
		LPCTSTR szTemp = m_pComboBoxConstellation->GetListBox()->GetText(it->second, 0);
		m_pComboBoxConstellation->GetEditBox()->SetText(szTemp);
	}

	//生肖
	it = m_mapChineseZodiacID.find((DWORD)pVCardData->customVCardData.byChineseZodiac);
	if (it != m_mapChineseZodiacID.end())
	{
		m_pComboBoxChZodiac->GetListBox()->SetCurSelectedRow(it->second);
		LPCTSTR szTemp = m_pComboBoxChZodiac->GetListBox()->GetText(it->second, 0);
		m_pComboBoxChZodiac->GetEditBox()->SetText(szTemp);
	}

	//区域
	it = m_mapAreaID.find((DWORD)pVCardData->customVCardData.byArea);
	if (it != m_mapAreaID.end())
	{
		m_pComboBoxArea->GetListBox()->SetCurSelectedRow(it->second);
		LPCTSTR szTemp = m_pComboBoxArea->GetListBox()->GetText(it->second, 0);
		m_pComboBoxArea->GetEditBox()->SetText(szTemp);
		
		//更新省份下拉列表
		ChangeProvince(it->second);

		//省市
		map<DWORD, INT>::iterator iter;
		iter = m_mapProvinceID.find((DWORD)pVCardData->customVCardData.byProvince);
		if (iter != m_mapProvinceID.end())
		{
			m_pComboBoxProvince->GetListBox()->SetCurSelectedRow(iter->second);
			szTemp = m_pComboBoxProvince->GetListBox()->GetText(iter->second, 0);
			m_pComboBoxProvince->GetEditBox()->SetText(szTemp);
		}
	}

	//城市
	m_pEditCityName->SetText(pVCardData->customVCardData.chCity);

	//头像URL
	TCHAR szImageURL[200] = {_T('\0')};
	memcpy(szImageURL, pVCardData->customVCardData.byData, (INT)pVCardData->customVCardData.byHeadUrlSize);
    tstring strImageURL = szImageURL;
    m_pStcHeadPic->SetPicURL(strImageURL);

    if (pVCardData->customVCardData.bySex == 1)
        m_pStcHeadPic->SetDefaultPic(MALE_PIC_PATH);
    else if (pVCardData->customVCardData.bySex == 2)
        m_pStcHeadPic->SetDefaultPic(FEMALE_PIC_PATH);
    else
        m_pStcHeadPic->SetDefaultPic(_T(""));

    m_pStcHeadPic->SetInvisible(FALSE);
    m_pStcHeadPic->SetRefresh(TRUE);

	//个性签名
	TCHAR szbuff[200] = {_T('\0')};
	INT nOffset = (INT)pVCardData->customVCardData.byHeadUrlSize;
	INT nSize = (INT)pVCardData->customVCardData.bySignatureSize;
	memcpy(szbuff, pVCardData->customVCardData.byData + nOffset, nSize);
	m_pEditBoxExPresentation->SetText(szbuff);

	// 更名记录
	TCHAR szNameHistory[X_HUGE_NAME] = {_T('\0')};
	nOffset += nSize;
	nSize = (INT)pVCardData->customVCardData.byNameHistory;
	memcpy(szNameHistory, pVCardData->customVCardData.byData + nOffset, nSize);
	m_pEditRenameHistory->SetText(szNameHistory);

	//可见性单选框
	m_pPbtVisible->SetState(pVCardData->customVCardData.bVisibility ? EGUIBS_PushDown : EGUIBS_PopUp);
	return 0;
}

DWORD InformationFrame::OnNS_SetRoleVCard( tagNS_SetRoleVCard* pMsg, DWORD dwParam )
{
	//响应保存名贴数据
	if (RoleMgr::Inst()->GetLocalPlayerID() != pMsg->dwRoleID)
		return 0;

	//处理错误
	switch (pMsg->dwErrorCode)
	{
	case 0:
		break;
	}
	return 0;
}

VOID InformationFrame::SaveAll()
{
	//image url，传回去空串（运营决定，不往数据库里存，仅仅传回去空串）
	LPCTSTR szImageURL = _T("");

	//url字节长度
	INT nImageURLByteSize = (_tcslen(szImageURL) + 1) * sizeof(TCHAR);

	//取得个性签名
    tstring strText = m_pEditBoxExPresentation->GetText(FALSE);
    strText = strText.substr(0, 52);
    FilterData::Inst()->WordsFilteror(strText);
	LPCTSTR szText = strText.c_str();

	//个性签名字节长度
	INT nSignatureByteSize = ( _tcslen(szText) + 1 ) * sizeof(TCHAR);

	// 取得更名记录
	tstring strNameText = m_pEditRenameHistory->GetText(FALSE);
	LPCTSTR szNameText = strNameText.c_str();

	// 更名记录长度
	INT nNameHisByteSize = ( _tcslen(szNameText) + 1 ) * sizeof(TCHAR);

	//重新分配结构体空间(处理变长消息)
	DWORD					dwSize = ( sizeof(tagNC_SetRoleVCard) - 1 ) + nImageURLByteSize + nSignatureByteSize + nNameHisByteSize;
	BYTE*					pbyTemp = new BYTE[dwSize];
	ZeroMemory(pbyTemp, dwSize);

	//原消息结构空间
	tagNC_SetRoleVCard		vCardDataTemp;

	//新消息结构地址
	tagNC_SetRoleVCard*		pVCardData = (tagNC_SetRoleVCard*)pbyTemp;

	//拷贝原结构体(不包括byData[1])
	memcpy(pbyTemp, &vCardDataTemp, sizeof(vCardDataTemp));

	//拷贝image url
	memcpy(pbyTemp + sizeof(vCardDataTemp) - 1, szImageURL, nImageURLByteSize);

	//拷贝个性签名
	memcpy(pbyTemp + sizeof(vCardDataTemp) - 1 + nImageURLByteSize, szText, nSignatureByteSize);	//别忘了'\0'

	//拷贝更名记录
	memcpy(pbyTemp + sizeof(vCardDataTemp) - 1 + nImageURLByteSize + nSignatureByteSize , szNameText, nNameHisByteSize);	//别忘了'\0'

	//结构体成员赋值
	pVCardData->dwSize								=	dwSize;
	pVCardData->dwRoleID							=	RoleMgr::Inst()->GetLocalPlayerID();
	pVCardData->customVCardData.byHeadUrlSize		=	(BYTE)nImageURLByteSize;
	pVCardData->customVCardData.bySignatureSize		=	(BYTE)nSignatureByteSize;
	pVCardData->customVCardData.byNameHistory		=	(BYTE)nNameHisByteSize;
	pVCardData->customVCardData.bVisibility			=	m_pPbtVisible->GetState() == EGUIBS_PushDown ? TRUE : FALSE;
	pVCardData->customVCardData.bySex				=	(BYTE)FindValuebyKey<INT, DWORD>(&m_mapGenderRow, m_pComboBoxGender->GetListBox()->GetCurSelectedRow());
	pVCardData->customVCardData.byConstellation		=	(BYTE)FindValuebyKey<INT, DWORD>(&m_mapConstellationRow, m_pComboBoxConstellation->GetListBox()->GetCurSelectedRow());
	pVCardData->customVCardData.byChineseZodiac		=	(BYTE)FindValuebyKey<INT, DWORD>(&m_mapChineseZodiacRow, m_pComboBoxChZodiac->GetListBox()->GetCurSelectedRow());
	pVCardData->customVCardData.byArea				=	(BYTE)FindValuebyKey<INT, DWORD>(&m_mapAreaRow, m_pComboBoxArea->GetListBox()->GetCurSelectedRow());
	pVCardData->customVCardData.byProvince			=	(BYTE)FindValuebyKey<INT, DWORD>(&m_mapProvinceRow, m_pComboBoxProvince->GetListBox()->GetCurSelectedRow());
    tstring strCity = m_pEditCityName->GetText();
    strCity = strCity.substr(0, LEN_CITY_NAME - 1);
    FilterData::Inst()->WordsFilteror(strCity);
	_tcscpy_s(pVCardData->customVCardData.chCity, sizeof(pVCardData->customVCardData.chCity)/sizeof(TCHAR), strCity.c_str());

	//发送消息
	m_pSession->Send(pVCardData);

	//释放空间
	SAFE_DELETE(pbyTemp);


}

template<class TKey, class TValue>
TValue InformationFrame::FindValuebyKey( map<TKey, TValue>* pMap, TKey tKey )
{
	map<TKey, TValue>::const_iterator citer = pMap->find(tKey);
	if (citer == pMap->end())
		return 0;

	return citer->second;
}

tstring InformationFrame::GetRenameHis()
{
	tstring strHis = m_pEditRenameHistory->GetText(false);
	int nPos = 0;
	int nNum = 0;
	tstring StrNew;
	while( ( nPos = strHis.find( '\r', nPos + 1 ) ) != strHis.npos )
	{
		nNum++;
		if( MAX_RENAME_HISTORY == nNum )
		{
			int nLength = strHis.length();
			int nFirstPos = strHis.find( '\r', 0 );
			StrNew = strHis.substr( nFirstPos + 2 , nLength - nFirstPos );
			return StrNew;
		}
	}
	return strHis;
}


DWORD InformationFrame::OnNS_LPRename( tagNS_LPRename *pNetCmd, DWORD )
{
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if(!P_VALID(pLP))
		return 0;
	if( pNetCmd->dwRoleID != pLP->GetPlayerLoverID() )
		return 0;

	if( pNetCmd->dwErrorCode == E_Success )
	{
		tstring strName = GetRenameHis();
		TCHAR szName[X_LONG_NAME] = {0};
		_sntprintf(szName, X_LONG_NAME, g_StrTable[_T("RenameHistory")], pNetCmd->szOldName, pNetCmd->szNewName );
		tstringstream info;
		info << strName << szName << _T("\n");
		m_pEditRenameHistory->SetText(info.str().c_str());
		SaveAll();
	}
	
	return 0;
}