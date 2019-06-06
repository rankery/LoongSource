// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "MainDlg.h"
#include ".\maindlg.h"
#include "EffectPropDlg.h"
#include "SFXPlug.h"
#include "ClassList.h"

Vector3 g_vRotate = Vector3( 0.0f, _DegToRad( 45.0f ), 0.0f );

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)
CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	m_pEditEffect=NULL;
	m_pPlayerMdl=NULL;
	m_pEquipMdl=NULL;
	m_bAttachedToSG=false;
	m_bUseActMsg=FALSE;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LibList, m_hRenderList);
	DDX_Control(pDX, IDC_PathList, m_hPathList);
	DDX_Control(pDX, IDC_SGEffectList, m_sgEffectList);
	DDX_Control(pDX, IDC_SGPathList, m_sgPathList);
	DDX_Control(pDX, IDC_PlayerAction, m_actList);
	DDX_Control(pDX, IDC_TimeS, m_hTimeScale);
	DDX_Control(pDX, IDC_PlayTime, m_hShowPlayTime);
	DDX_Control(pDX, IDC_ActLoop, m_hActLoop);
	
	DDX_Control(pDX, IDC_Comb_AvatarEle, m_Comb_AvatarEle);
	DDX_Control(pDX, IDC_Comb_BoneTagNodesForEquip, m_Comb_EquiptBoneTagNodes);
	DDX_Control(pDX, IDC_Comb_BoneTagNodes, m_Comb_EffectBoneTagNodes);
	DDX_Control(pDX, IDC_Comb_EquipTagNodes, m_Comb_EffectEquipTagNodes);
	DDX_Control(pDX, IDC_Comb_LOD, m_Comb_LOD);

	DDX_Control(pDX, IDC_FileName, m_hFileName);

	DDX_Control(pDX, IDC_MsgPlay, m_hMsgPlay);
	DDX_Control(pDX, IDC_MsgStop, m_hMsgStop);
	DDX_Control(pDX, IDC_Play, m_hPlay);
	DDX_Control(pDX, IDC_Pause, m_hPause);
	DDX_Control(pDX, IDC_Stop, m_hStop);

	DDX_Control(pDX, IDC_RoleScale, m_hRoleScale);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_LibAdd, OnBnClickedLibadd)
	ON_BN_CLICKED(IDC_LibSave, OnBnClickedLibsave)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LibLoad, OnBnClickedLibload)
	ON_BN_CLICKED(IDC_LibDel, OnBnClickedLibdel)
	ON_LBN_SETFOCUS(IDC_LibList, OnLbnSetfocusLiblist)
	ON_LBN_SETFOCUS(IDC_PathList, OnLbnSetfocusPathlist)
	ON_BN_CLICKED(IDC_EftReset, OnBnClickedEftreset)
	ON_BN_CLICKED(IDC_EftAdd, OnBnClickedEftadd)
	ON_BN_CLICKED(IDC_EftDel, OnBnClickedEftdel)
	ON_BN_CLICKED(IDC_EftSave, OnBnClickedEftsave)
	ON_BN_CLICKED(IDC_EftLoad, OnBnClickedEftload)
	ON_LBN_DBLCLK(IDC_LibList, OnLbnDblclkLiblist)
	ON_LBN_DBLCLK(IDC_PathList, OnLbnDblclkPathlist)
	ON_BN_CLICKED(IDC_PathAdd, OnBnClickedPathadd)
	ON_BN_CLICKED(IDC_LibDelPath, OnBnClickedLibdelpath)
	ON_BN_CLICKED(IDC_PathDel, OnBnClickedPathdel)
	ON_LBN_DBLCLK(IDC_SGEffectList, OnLbnDblclkSgeffectlist)
	ON_LBN_DBLCLK(IDC_SGPathList, OnLbnDblclkSgpathlist)
	ON_LBN_SELCHANGE(IDC_SGEffectList, OnLbnSelchangeSgeffectlist)
	ON_BN_CLICKED(IDC_Play, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Pause, OnBnClickedPause)
	ON_BN_CLICKED(IDC_Stop, OnBnClickedStop)
	ON_BN_CLICKED(IDC_EftNew, OnBnClickedEftnew)
	ON_BN_CLICKED(IDC_LibClone, OnBnClickedLibclone)
	ON_BN_CLICKED(IDC_LibClonePath, OnBnClickedLibclonepath)
	ON_BN_CLICKED(IDC_LibImport, OnBnClickedLibimport)
	ON_EN_CHANGE(IDC_PosY, OnEnChangePosy)
	ON_EN_CHANGE(IDC_PosX, OnEnChangePosx)
	ON_EN_CHANGE(IDC_PosZ, OnEnChangePosz)
	ON_BN_CLICKED(IDC_EftClone, OnBnClickedEftclone)
	ON_BN_CLICKED(IDC_EftPathClone, OnBnClickedEftpathclone)
	ON_LBN_SELCHANGE(IDC_SGPathList, OnLbnSelchangeSgpathlist)
	ON_BN_CLICKED(IDC_LoadBoy, OnBnClickedLoadboy)
	ON_BN_CLICKED(IDC_LoadGirl, OnBnClickedLoadgirl)
	ON_CBN_SELCHANGE(IDC_PlayerAction, OnCbnSelchangePlayeraction)
	ON_EN_CHANGE(IDC_TimeS, OnEnChangeTimes)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AttachMdl, &CMainDlg::OnBnClickedAttachmdl)
	ON_BN_CLICKED(IDC_BindToBone, &CMainDlg::OnBnClickedBindtobone)
	ON_BN_CLICKED(IDC_ChangeAvatar, &CMainDlg::OnBnClickedChangeavatar)
	ON_BN_CLICKED(IDC_BindToEquip, &CMainDlg::OnBnClickedBindtoequip)
	ON_CBN_SELCHANGE(IDC_Comb_LOD, &CMainDlg::OnCbnSelchangeCombLod)
	ON_EN_CHANGE(IDC_MsgPlay, &CMainDlg::OnEnChangeMsgplay)
	ON_EN_CHANGE(IDC_MsgStop, &CMainDlg::OnEnChangeMsgstop)
	ON_BN_CLICKED(IDC_UseActMsg, &CMainDlg::OnBnClickedUseactmsg)
	ON_BN_CLICKED(IDC_ReloadMsgTab, &CMainDlg::OnBnClickedReloadmsgtab)
	ON_BN_CLICKED(IDC_LoadNPC, &CMainDlg::OnBnClickedLoadnpc)
	ON_EN_CHANGE(IDC_RoleScale, &CMainDlg::OnEnChangeRolescale)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

void CMainDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CMainDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

EffectBase *CMainDlg::NewEftAndEdit(bool bLib)
{
	EffectBase *pRet=NULL;
	BeginMFCRes();
	CClassList selType;
	selType.DoModal();
	if(!selType.m_szSelectClass.IsEmpty())
	{
		EffectBase *pNewEffect=(EffectBase *)RTTFactory::Inst()->CreateObj(selType.m_szSelectClass);
		CEffectPropDlg dlg;
		bool bOk=false;
		if(bLib)
			bOk=SetEditEffect_Lib(&dlg,pNewEffect);
		else
			bOk=SetEditEffect_SFX(&dlg,pNewEffect);
		if(bOk)
		{
			if(IDOK==dlg.DoModal())
			{
				pRet=pNewEffect;
			}
			else
				SAFE_DELETE(pNewEffect);
		}
	}
	EndMFCRes();

	return pRet;
}

void CMainDlg::OnBnClickedLibadd()
{
	// TODO: 在此添加控件通知处理程序代码
	EffectBase *pNewEffect=NewEftAndEdit(true);
	if(pNewEffect)
	{
		m_eftLib.AddEft(pNewEffect);
		m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
		if(pNewEffect->IS_KIND_OF(EffectPathCtrl))
			m_hPathList.SelectString(0,pNewEffect->GetName());
		else
			m_hRenderList.SelectString(0,pNewEffect->GetName());
	}
}

void CMainDlg::OnBnClickedLibsave()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL,
		this,0);
	if(IDOK==dlg.DoModal())
	{
		m_eftLib.SaveFile(dlg.GetPathName());
	}
	
}

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EffectBase::s_reloadTex=true;

	CString szTitle;
	GetWindowText(szTitle);
	CString szVer;
#ifdef _DEBUG
	szVer.Format(_T("--Build %s Debug"),__TDATE__);
#else
	szVer.Format(_T("--Build %s Release"),__TDATE__);
#endif
	szTitle+=szVer;
	SetWindowText(szTitle);

	m_hTimeScale.SetWindowText(_T("1.0"));
	
	m_hShowPlayTime.SetWindowText(_T("0.0"));
	SetTimer(1,10,0);//定时器，用来刷新play time显示

	m_hActLoop.SetCheck(1);

	for( int i = 0; i <= SGEffectNode::MAX_LOD; i++ )
	{
		CString str;
		str.Format( _T("%d"), i );
		m_Comb_LOD.AddString( str );
	}
	m_Comb_LOD.SetCurSel( SGEffectNode::sUserLod );
	m_hMsgPlay.EnableWindow( FALSE );
	m_hMsgStop.EnableWindow( FALSE );
	m_hRoleScale.SetWindowText( _T("1.0") );
	CheckDlgButton( IDC_Pick, TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

EffectBase *CMainDlg::GetSelectedLibEffect(int focusList)
{
	CListBox *pFocusList=NULL;
	if(focusList==1)
		pFocusList=&m_hPathList;
	else
		pFocusList=&m_hRenderList;

	int sel=pFocusList->GetCurSel();
	if(sel==-1)
	{
		AfxMessageBox(_T("请先选中一个特效元素"));
		return NULL;
	}

	CString selName;
	pFocusList->GetText(sel,selName);
	EffectBase *pSelEft=m_eftLib.FindEft(selName);
	return pSelEft;
}

void CMainDlg::Libedit(int focusList)
{
	// TODO: 在此添加控件通知处理程序代码
	EffectBase *pSelEft=GetSelectedLibEffect(focusList);
	if(pSelEft)
	{
		BeginMFCRes();
		CEffectPropDlg dlg;
		if(SetEditEffect_Lib(&dlg,pSelEft))
		{
			dlg.DoModal();
			EndMFCRes();
		}

		m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);//name可能改变

		if(pSelEft->IS_KIND_OF(EffectPathCtrl))
			m_hPathList.SelectString(0,pSelEft->GetName());
		else
			m_hRenderList.SelectString(0,pSelEft->GetName());
	}
}

void CMainDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_eftLib.Destroy();

	// Detach 并删除SceneNode
	DetachFromSG();
	if( NULL != m_pPlayerMdl )
	{
		g_editor->GetSceneGraph()->DetachDynamicNode(m_pPlayerMdl);
		SAFE_DELETE( m_pPlayerMdl );
		m_pEquipMdl = NULL;
	}
	SAFE_DELETE( m_pEditEffect );
	// TODO: 在此处添加消息处理程序代码
}

void CMainDlg::OnBnClickedLibload()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL,
		this,0);
	if(IDOK==dlg.DoModal())
	{
		m_eftLib.LoadFile(dlg.GetPathName());
		m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
	}
}

void CMainDlg::OnBnClickedLibimport()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL,
		this,0);
	if(IDOK==dlg.DoModal())
	{
		m_eftLib.LoadFile(dlg.GetPathName(),false);
		m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
	}
}

void CMainDlg::OnBnClickedLibdel()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_hRenderList.GetCurSel();
	if(sel==-1)
	{
		AfxMessageBox(_T("请先选中一个特效元素"));
	}

	CString selName;
	m_hRenderList.GetText(sel,selName);
	
	m_eftLib.RemoveEft(selName);
	m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
}


void CMainDlg::OnBnClickedLibdelpath()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_hPathList.GetCurSel();
	if(sel==-1)
	{
		AfxMessageBox(_T("请先选中一个特效元素"));
	}

	CString selName;
	m_hPathList.GetText(sel,selName);
	
	m_eftLib.RemoveEft(selName);
	m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
}

void CMainDlg::OnLbnSetfocusLiblist()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CMainDlg::OnLbnSetfocusPathlist()
{
	// TODO: 在此添加控件通知处理程序代码
}

bool CMainDlg::SetEditEffect_SFX(CEffectPropDlg* pDlg,EffectBase *pEffect)
{
	bool bPath=pEffect->IS_KIND_OF(EffectPathCtrl);
	if(bPath)
	{
		int sel=m_sgEffectList.GetCurSel();
		if(sel==-1)
		{
			AfxMessageBox(_T("编辑路径必须先选中一个特效渲染元素"));
			return false;
		}
		CString selName;
		m_sgEffectList.GetText(sel,selName);
		EffectPathCtrl *pPath=(EffectPathCtrl *)pEffect;
		pDlg->SetEditEffect(m_pEditEffect->FindEffect(selName),pPath,true);
	}
	else
		pDlg->SetEditEffect(pEffect,NULL,false);
	return true;
}

bool CMainDlg::SetEditEffect_Lib(CEffectPropDlg* pDlg,EffectBase *pEffect)
{
	bool bPath=pEffect->IS_KIND_OF(EffectPathCtrl);
	if(bPath)
	{
		int sel=m_hRenderList.GetCurSel();
		if(sel==-1)
		{
			AfxMessageBox(_T("编辑路径必须先选中一个特效渲染元素"));
			return false;
		}
		CString selName;
		m_hRenderList.GetText(sel,selName);
		EffectPathCtrl *pPath=(EffectPathCtrl *)pEffect;
		pDlg->SetEditEffect(m_eftLib.FindEft(selName),pPath,true);
	}
	else
	{
		int sel=m_hPathList.GetCurSel();
		if(sel!=-1)
		{
			CString selName;
			m_hPathList.GetText(sel,selName);
			pDlg->SetEditEffect(pEffect,(EffectPathCtrl *)m_eftLib.FindEft(selName),false);
		}
		else
		{
			//todo:得到eft所绑定的path
			pDlg->SetEditEffect(pEffect,NULL,false);
		}
	}
	return true;
}
void CMainDlg::OnBnClickedEftreset()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect!=NULL)
	{
		DetachFromSG();
		SAFE_DELETE(m_pEditEffect);
	}
}

void CMainDlg::OnBnClickedEftadd()
{
	// TODO: 在此添加控件通知处理程序代码
	NewEditEffectIfNULL();

	EffectBase *pSelEft=GetSelectedLibEffect(0);
	if(pSelEft!=NULL)
	{
		EffectBase *pNewEft=m_eftLib.CloneEft(pSelEft->GetName());
		m_pEditEffect->AddEffect(pNewEft);
	}
	UpdateSGEffectList();
}

void CMainDlg::OnBnClickedEftdel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;

	int sel=m_sgEffectList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgEffectList.GetText(sel,szSel);
		m_pEditEffect->RemoveEffect(szSel,true);
		UpdateSGEffectList();
	}
}

void CMainDlg::OnBnClickedEftsave()
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL==m_pEditEffect)
	{
		AfxMessageBox(_T("编辑特效为NULL"));
		return;
	}

	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("特效文件|*.sfx||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		CString szFilePath=dlg.GetPathName();
		szFilePath.MakeLower();
		if(szFilePath.GetLength()<=4
			|| szFilePath.Right(4)!=_T(".sfx"))
			szFilePath+=_T(".sfx");
		FArchive ar;
		ar.OpenForWrite(szFilePath);
		m_pEditEffect->Serialize(ar);
		ar.Close();
	}
		
}

void CMainDlg::OnBnClickedEftload()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("特效文件|*.sfx||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		OnBnClickedEftreset();
		ASSERT(m_pEditEffect==NULL);

		FArchive ar;
		ar.OpenForRead(Kernel::Inst()->DefaultFS(),dlg.GetPathName());
		NewEditEffectIfNULL();
		m_pEditEffect->Deserialize(ar);
		ar.Close();

		UpdateSGEffectList();
		UpdateSGEffectPathList();

		m_hFileName.SetWindowText( dlg.GetPathName() );
	}
}

void CMainDlg::OnLbnDblclkLiblist()
{
	// TODO: 在此添加控件通知处理程序代码
	Libedit(0);
}

void CMainDlg::OnLbnDblclkPathlist()
{
	// TODO: 在此添加控件通知处理程序代码
	Libedit(1);
}

void CMainDlg::AttachToSG( SceneNode* pFather, CString strTagNodeName )
{
	if(m_pEditEffect==NULL)
		return;

	DetachFromSG();
	if( NULL != pFather )
	{
		if( !strTagNodeName.IsEmpty() )
			m_pEditEffect->BindToTagNode( strTagNodeName );
		else
			m_pEditEffect->UnbindToTagNode();
		pFather->AttachChild( m_pEditEffect );
	}
	else
	{
		g_editor->GetSceneGraph()->AttachDynamicNode(m_pEditEffect);
	}
	m_bAttachedToSG = true;
}

void CMainDlg::DetachFromSG()
{
	if(m_pEditEffect==NULL)
		return;

	SceneNode* pFather = (SceneNode*)m_pEditEffect->GetParent();
	if( NULL != pFather )
	{
		m_pEditEffect->UnbindToTagNode();
		pFather->DetachChild( m_pEditEffect );
	}
	else
	{
		g_editor->GetSceneGraph()->DetachDynamicNode(m_pEditEffect);
	}
	m_bAttachedToSG = false;
}

void CMainDlg::NewEditEffectIfNULL()
{
	if(m_pEditEffect==NULL)
	{
		m_pEditEffect=new SGAttachableEffectNode;
	}
}

void CMainDlg::OnBnClickedPathadd()
{
	// TODO: 在此添加控件通知处理程序代码
	NewEditEffectIfNULL();

	EffectBase *pSelEft=GetSelectedLibEffect(1);
	if(pSelEft!=NULL)
	{
		ASSERT(pSelEft->IS_KIND_OF(EffectPathCtrl));
		EffectBase *pNewEft=m_eftLib.CloneEft(pSelEft->GetName());
		m_pEditEffect->AddEffectPath((EffectPathCtrl*)pNewEft);
	}
	UpdateSGEffectPathList();
}


void CMainDlg::UpdateSGEffectList()
{
	m_sgEffectList.ResetContent();
	if(m_pEditEffect==NULL)
		return;
	vector<EffectBase*> eftArray;
	m_pEditEffect->GetEffectArray(eftArray);

	for(int i=0;i<(int)eftArray.size();i++)
	{
		m_sgEffectList.AddString(eftArray[i]->GetName());
	}
}

void CMainDlg::UpdateSGEffectPathList()
{
	m_sgPathList.ResetContent();
	if(m_pEditEffect==NULL)
		return;
	vector<EffectPathCtrl*> pathArray;
	m_pEditEffect->GetEffectPathArray(pathArray);

	for(int i=0;i<(int)pathArray.size();i++)
	{
		m_sgPathList.AddString(pathArray[i]->GetName());
	}
}

void CMainDlg::OnBnClickedPathdel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;

	int sel=m_sgPathList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgPathList.GetText(sel,szSel);
		m_pEditEffect->RemovePath(szSel,true);
		UpdateSGEffectPathList();
	}
}

void CMainDlg::OnLbnDblclkSgeffectlist()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;
	int sel=m_sgEffectList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgEffectList.GetText(sel,szSel);

		EffectBase *pEft=m_pEditEffect->FindEffect(szSel);
		ASSERT(pEft!=NULL);

		//打开元素编辑时,先从主编辑器的SceneGraph中detach
		DetachFromSG();

		BeginMFCRes();
		CEffectPropDlg dlg;
		if(SetEditEffect_SFX(&dlg,pEft))
		{
			dlg.DoModal();
			EndMFCRes();
		}
		m_pEditEffect->RebuildEffectRenderNodes();
		UpdateSGEffectList();//名称可能改变
		m_sgEffectList.SetCurSel(sel);

		//元素编辑结束,渲染节点attach回sg
		//AttachToSG();
	}
}

void CMainDlg::OnLbnDblclkSgpathlist()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;
	int sel=m_sgPathList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgPathList.GetText(sel,szSel);

		CString szName;
		EffectPathCtrl *pEft=m_pEditEffect->FindPath(szSel);
		ASSERT(pEft!=NULL);
		szName=pEft->GetName();

		//打开元素编辑时,先从主编辑器的SceneGraph中detach
		DetachFromSG();

		//因为名称可能被编辑改变,所以先删除掉,再重新添加
		m_pEditEffect->RemovePath(pEft->GetName(),false);
		BeginMFCRes();
		CEffectPropDlg dlg;
		if(SetEditEffect_SFX(&dlg,pEft))
		{
			dlg.DoModal();
			EndMFCRes();
		}
		m_pEditEffect->AddEffectPath(pEft);
		UpdateSGEffectPathList();

		//元素编辑结束,渲染节点attach回sg
		//AttachToSG();
	}
}

void CMainDlg::OnLbnSelchangeSgeffectlist()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CMainDlg::OnBnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !m_bAttachedToSG )
		AttachToSG( NULL, NULL );
	if(m_pEditEffect)
		m_pEditEffect->Play();
	OnCbnSelchangePlayeraction();

	OnEnChangeTimes();
}

void CMainDlg::OnBnClickedPause()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect)
		m_pEditEffect->SetTimeScale(0);
	if(m_pPlayerMdl)
	{
		m_pPlayerMdl->GetSkeletonAniCtrl()->SetTimeFactor(0);
	}
}

void CMainDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect)
		m_pEditEffect->Close();
}


void CMainDlg::OnBnClickedEftnew()
{
	// TODO: 在此添加控件通知处理程序代码
	EffectBase *pNewEffect=NewEftAndEdit(false);
	if(pNewEffect)
	{
		NewEditEffectIfNULL();
		if(pNewEffect->IS_KIND_OF(EffectPathCtrl))
		{
			m_pEditEffect->AddEffectPath((EffectPathCtrl*)pNewEffect);
			UpdateSGEffectPathList();
		}
		else
		{
			m_pEditEffect->AddEffect(pNewEffect);
			UpdateSGEffectList();
		}
	}
}

void CMainDlg::OnBnClickedLibclone()
{
	LibCloneEffect(0);
}

void CMainDlg::LibCloneEffect(int focusList)
{
	EffectBase *pSelEft=GetSelectedLibEffect(focusList);
	if(pSelEft==NULL)
	{
		AfxMessageBox(_T("请先从列表中选择一个特效."));
		return;
	}
	EffectBase *pNewEft=m_eftLib.CloneEft(pSelEft->GetName());
	if(pNewEft!=NULL)
	{
		CString szNewName;
		szNewName.Format(_T("%s_%d"),pNewEft->GetName(),GetTickCount);
		if(szNewName.GetLength()>=32)
			szNewName=szNewName.Mid(szNewName.GetLength()-32);
		pNewEft->Rename(szNewName);
		m_eftLib.AddEft(pNewEft);

		m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
		if(focusList==0)
			m_hRenderList.SelectString(0,pNewEft->GetName());
		else
			m_hPathList.SelectString(0,pNewEft->GetName());
	}
	else
	{
		AfxMessageBox(_T("特效复制失败."));
	}
}

void CMainDlg::OnBnClickedLibclonepath()
{
	// TODO: 在此添加控件通知处理程序代码
	LibCloneEffect(1);
}


void CMainDlg::SetEffectPos(const Vector3& pos,bool bUpdateEdit)
{
	float fRoleScale=1.0f;
	CString strScale;
	m_hRoleScale.GetWindowText( strScale );
	if( !strScale.IsEmpty() )
		fRoleScale=(float)_tstof(strScale);

	Transform trans;
	trans.Scale(fRoleScale,fRoleScale,fRoleScale);
	trans.Rotate( g_vRotate.y, g_vRotate.x, g_vRotate.z );
	trans.Translate(pos);

	if(m_pPlayerMdl!=NULL)
		m_pPlayerMdl->SetWorldMat(trans.GetMatrix());

	if(m_pEditEffect!=NULL)
		m_pEditEffect->SetWorldMat(trans.GetMatrix());

	if(bUpdateEdit)
	{
		CString szTmp;
		szTmp.Format(_T("%.2f"),pos.x);
		GetDlgItem(IDC_PosX)->SetWindowText(szTmp);
		szTmp.Format(_T("%.2f"),pos.y);
		GetDlgItem(IDC_PosY)->SetWindowText(szTmp);
		szTmp.Format(_T("%.2f"),pos.z);
		GetDlgItem(IDC_PosZ)->SetWindowText(szTmp);
	}
}

void CMainDlg::OnRender()
{
	if( IsDlgButtonChecked(IDC_DrawBox) && NULL != m_pEditEffect )
	{
		AABBox boxEff;
		m_pEditEffect->ReBuildLocalBox();
		m_pEditEffect->GetBox( boxEff );
		DrawX::DrawAABBox( boxEff );
	}
}

void CMainDlg::UpdatePosFromEdit()
{
	CString szX,szY,szZ;
	GetDlgItem(IDC_PosX)->GetWindowText(szX);
	GetDlgItem(IDC_PosY)->GetWindowText(szY);
	GetDlgItem(IDC_PosZ)->GetWindowText(szZ);
	Vector3 pos;
	pos.x=(float)_tstof(szX);
	pos.y=(float)_tstof(szY);
	pos.z=(float)_tstof(szZ);

	SetEffectPos(pos,false);
}

void CMainDlg::OnEnChangePosy()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdatePosFromEdit();
}

void CMainDlg::OnEnChangePosx()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdatePosFromEdit();
}

void CMainDlg::OnEnChangePosz()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdatePosFromEdit();
}

BOOL CMainDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CMainDlg::OnBnClickedEftclone()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;
	int sel=m_sgEffectList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgEffectList.GetText(sel,szSel);

		EffectBase *pEft=m_pEditEffect->FindEffect(szSel);
		ASSERT(pEft!=NULL);
		EffectBase *pNewEft=_CloneEffect(pEft);
		if(pNewEft!=NULL)
		{
			m_eftLib.AddEft(pNewEft);
			m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
		}
	}
}

void CMainDlg::OnBnClickedEftpathclone()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pEditEffect==NULL)
		return;
	int sel=m_sgPathList.GetCurSel();
	if(sel!=-1)
	{
		CString szSel;
		m_sgPathList.GetText(sel,szSel);

		EffectBase *pPath=m_pEditEffect->FindPath(szSel);
		if(pPath!=NULL)
		{
			EffectBase *pNewPath=_CloneEffect(pPath);
			m_eftLib.AddEft(pNewPath);
			m_eftLib.UpdateList(&m_hRenderList,&m_hPathList);
		}
	}
}

void CMainDlg::OnLbnSelchangeSgpathlist()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CMainDlg::OnBnClickedLoadboy()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pPlayerMdl!=NULL)
	{
		DetachFromSG();
		g_editor->GetSceneGraph()->DetachDynamicNode(m_pPlayerMdl);
		SAFE_DELETE(m_pPlayerMdl);
		m_pEquipMdl = NULL;
	}
	m_pPlayerMdl=_LoadDefaultAvatar(0);
	g_editor->GetSceneGraph()->AttachDynamicNode(m_pPlayerMdl);
	AttachToSG( NULL, NULL );
	_LoadAvatarActionList(m_pPlayerMdl,&m_actList);
	_LoadAvatarElementList(&m_Comb_AvatarEle);
	_LoadBoneTagNodeList(&m_Comb_EquiptBoneTagNodes);
	_LoadBoneTagNodeList(&m_Comb_EffectBoneTagNodes);
}

void CMainDlg::OnBnClickedLoadgirl()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pPlayerMdl!=NULL)
	{
		DetachFromSG();
		g_editor->GetSceneGraph()->DetachDynamicNode(m_pPlayerMdl);
		SAFE_DELETE(m_pPlayerMdl);
		m_pEquipMdl = NULL;
	}
	m_pPlayerMdl=_LoadDefaultAvatar(1);
	g_editor->GetSceneGraph()->AttachDynamicNode(m_pPlayerMdl);
	AttachToSG( NULL, NULL );
	_LoadAvatarActionList(m_pPlayerMdl,&m_actList);
	_LoadAvatarElementList(&m_Comb_AvatarEle);
	_LoadBoneTagNodeList(&m_Comb_EquiptBoneTagNodes);
	_LoadBoneTagNodeList(&m_Comb_EffectBoneTagNodes);
}

void CMainDlg::OnBnClickedLoadnpc()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("骨骼动画文件|*.fskel||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		if(m_pPlayerMdl!=NULL)
		{
			DetachFromSG();
			g_editor->GetSceneGraph()->DetachDynamicNode(m_pPlayerMdl);
			SAFE_DELETE(m_pPlayerMdl);
			m_pEquipMdl = NULL;
		}
		m_pPlayerMdl=_LoadNPC(dlg.GetPathName());
		g_editor->GetSceneGraph()->AttachDynamicNode(m_pPlayerMdl);
		AttachToSG( NULL, NULL );
		_LoadAvatarActionList(m_pPlayerMdl,&m_actList);
		m_Comb_AvatarEle.ResetContent();
		m_Comb_EquiptBoneTagNodes.ResetContent();
		m_Comb_EffectBoneTagNodes.ResetContent();
	}
}

void CMainDlg::OnCbnSelchangePlayeraction()
{
	// TODO: 在此添加控件通知处理程序代码

	int sel=m_actList.GetCurSel();
	if(sel!=-1)
	{
		CString szName;
		m_actList.GetLBText(sel,szName);
		_PlayAvatarAction(m_pPlayerMdl,szName,m_hActLoop.GetCheck()!=0);
	}
}

void CMainDlg::OnEnChangeTimes()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString szTxt;
	m_hTimeScale.GetWindowText(szTxt);
	float timescale=(float)_tstof(szTxt);
	if(m_pEditEffect)
		m_pEditEffect->SetTimeScale(timescale);
	if(m_pPlayerMdl)
	{
		m_pPlayerMdl->GetSkeletonAniCtrl()->SetTimeFactor(timescale);
	}
}

void CMainDlg::OnTimer(UINT nIDEvent)
{
	// 特效播放时间
	if(m_pEditEffect!=NULL)
	{
		CString szTime;
		szTime.Format(_T("%.2f"),m_pEditEffect->GetRunTime());
		m_hShowPlayTime.SetWindowText(szTime);
	}

	// 动作播放时间
	if( NULL != m_pPlayerMdl )
	{
		NodeAnimationCtrl* pAniCtrl=m_pPlayerMdl->GetSkeletonAniCtrl();
		if(pAniCtrl!=NULL)
		{
			CString szTrackName;
			m_actList.GetWindowText( szTrackName );
			CString szTime;
			szTime.Format(_T("%.2f"),pAniCtrl->GetTrackPosition(szTrackName));
			SetDlgItemText( IDC_ActPlayTime, szTime );
		}
	}

	// 使用动作消息点播放停止特效
	if( m_bUseActMsg && NULL != m_pPlayerMdl && NULL != m_pEditEffect )
	{
		NodeAnimationCtrl* pAniCtrl=m_pPlayerMdl->GetSkeletonAniCtrl();
		if(pAniCtrl!=NULL)
		{
			DWORD dwMsg=pAniCtrl->PeekFrameMsg();
			if( 0 != dwMsg )
			{
				TCHAR szAniMsg[5];
				ZeroMemory(szAniMsg,sizeof(szAniMsg));
				_FourCC2Str(dwMsg,szAniMsg);
				if( m_strMsgPlay == szAniMsg )
					m_pEditEffect->Play();
				else if( m_strMsgStop == szAniMsg )
					m_pEditEffect->Close();
				else if( 0 == _tcscmp( _T("BGN"), szAniMsg ) )
					m_pEditEffect->Close();
			}
		}
	}


	CDialog::OnTimer(nIDEvent);
}

void CMainDlg::OnBnClickedChangeavatar()
{
	if( NULL == m_pPlayerMdl )
	{
		AfxMessageBox( _T("还没有导入角色模型！") );
		return;
	}
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("蒙皮文件(*.fskin)|*.fskin|所有文件(*.*)|*.*||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		CString strEleName;
		m_Comb_AvatarEle.GetWindowText( strEleName );
		if( !_ChangeSkin( m_pPlayerMdl, strEleName, dlg.GetPathName() ) )
		{
			AfxMessageBox( _T("更换蒙皮失败！") );
		}
	}
}

void CMainDlg::OnBnClickedAttachmdl()
{
	if( NULL == m_pPlayerMdl )
	{
		AfxMessageBox( _T("还没有导入角色模型！") );
		return;
	}

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("静态模型文件(*.fsm)|*.fsm|关键帧文件(*.fak)|*.fak|所有文件(*.*)|*.*||"),
		this,0);
	if(IDOK==dlg.DoModal())
	{
		if( NULL != m_pEquipMdl )
		{
			DetachFromSG();
			m_pPlayerMdl->DetachChild( m_pEquipMdl );
			SAFE_DELETE( m_pEquipMdl );
			AttachToSG( NULL, NULL );
		}
		m_pEquipMdl=new SGAvatarAttachment;
		try
		{
			const bool bIOAsync = ResMgr::Inst()->IsIOAsync();
			ResMgr::Inst()->EnableIOAsync( false );
			m_pEquipMdl->LoadRes(dlg.GetPathName());
			ResMgr::Inst()->EnableIOAsync( bIOAsync );	
		}
		catch(...)
		{
			SAFE_DELETE( m_pEquipMdl );
			AfxMessageBox( _T("加载装备模型失败！"));
			EndMFCRes();
			return;
		}

		CString strTagNodeName;
		m_Comb_EquiptBoneTagNodes.GetWindowText(strTagNodeName);
		if( !strTagNodeName.IsEmpty() )
			m_pEquipMdl->BindToTagNode(strTagNodeName);
		m_pPlayerMdl->AttachChild(m_pEquipMdl);

		_LoadEquipTagNodeList( m_pEquipMdl, &m_Comb_EffectEquipTagNodes );
	}	
}

void CMainDlg::OnBnClickedBindtobone()
{
	if( NULL == m_pPlayerMdl )
	{
		AfxMessageBox( _T("还没有导入角色模型！") );
		return;
	}

	DetachFromSG();
	CString strTagNodeName;
	m_Comb_EffectBoneTagNodes.GetWindowText(strTagNodeName);
	AttachToSG( m_pPlayerMdl, strTagNodeName );
}

void CMainDlg::OnBnClickedBindtoequip()
{
	if( NULL == m_pPlayerMdl )
	{
		AfxMessageBox( _T("还没有导入角色模型！") );
		return;
	}

	if( NULL == m_pEquipMdl )
	{
		AfxMessageBox( _T("没有装备可以挂载！") );
		return;
	}

	DetachFromSG();
	CString strTagNodeName;
	m_Comb_EffectEquipTagNodes.GetWindowText(strTagNodeName);
	AttachToSG( m_pEquipMdl, strTagNodeName );
}
void CMainDlg::OnCbnSelchangeCombLod()
{
	// TODO: Add your control notification handler code here
	SGEffectNode::sUserLod = m_Comb_LOD.GetCurSel();
}

void CMainDlg::OnEnChangeMsgplay()
{
	m_hMsgPlay.GetWindowText( m_strMsgPlay );
}

void CMainDlg::OnEnChangeMsgstop()
{
	m_hMsgStop.GetWindowText( m_strMsgStop );
}

void CMainDlg::OnBnClickedUseactmsg()
{
	m_bUseActMsg = IsDlgButtonChecked(IDC_UseActMsg);
	if( m_bUseActMsg )
	{
		m_hMsgPlay.EnableWindow( TRUE );
		m_hMsgStop.EnableWindow( TRUE );

		m_hPlay.EnableWindow( FALSE );
		m_hPause.EnableWindow( FALSE );
		m_hStop.EnableWindow( FALSE );
	}
	else
	{
		m_hMsgPlay.EnableWindow( FALSE );
		m_hMsgStop.EnableWindow( FALSE );

		m_hPlay.EnableWindow( TRUE );
		m_hPause.EnableWindow( TRUE );
		m_hStop.EnableWindow( TRUE );
	}
	if( NULL != m_pEditEffect )
		m_pEditEffect->Close();
}

void CMainDlg::OnBnClickedReloadmsgtab()
{
	if( NULL != m_pPlayerMdl )
		m_pPlayerMdl->ReloadMsgTable();
}
void CMainDlg::OnEnChangeRolescale()
{
	CString strScale;
	m_hRoleScale.GetWindowText( strScale );
	if( strScale.IsEmpty() )
		return;

	float fRoleScale=(float)_tstof(strScale);
	if( NULL != m_pPlayerMdl )
	{
		const Matrix4 mat = m_pPlayerMdl->GetWorldMat();
		
		Transform trans;
		trans.Scale( fRoleScale, fRoleScale, fRoleScale );
		trans.Rotate( g_vRotate.y, g_vRotate.x, g_vRotate.z );
		trans.Translate( Vector3( mat._41, mat._42, mat._43 ) );
		m_pPlayerMdl->SetWorldMat(trans.GetMatrix());
	}
	if( NULL != m_pEditEffect )
	{
		const Matrix4 mat = m_pEditEffect->GetWorldMat();

		Transform trans;
		trans.Scale( fRoleScale, fRoleScale, fRoleScale );
		trans.Rotate( g_vRotate.y, g_vRotate.x, g_vRotate.z );
		trans.Translate( Vector3( mat._41, mat._42, mat._43 ) );
		m_pEditEffect->SetWorldMat(trans.GetMatrix());
	}
}