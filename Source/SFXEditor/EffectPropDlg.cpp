// EffectPropDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "EffectPropDlg.h"
#include ".\effectpropdlg.h"

void Deg2Rad3f(Vector3& rot)
{
	rot.x=_DegToRad(rot.x);
	rot.y=_DegToRad(rot.y);
	rot.z=_DegToRad(rot.z);
}

// CEffectPropDlg 对话框

IMPLEMENT_DYNAMIC(CEffectPropDlg, CDialog)
CEffectPropDlg::CEffectPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectPropDlg::IDD, pParent)
	, m_szStateIndex(_T("0"))
{
	m_bPrivew=true;
	m_pEditEffect=NULL;
	m_numStates=1;
	m_pEftPath=NULL;
	m_pEftRender=NULL;
}

CEffectPropDlg::~CEffectPropDlg()
{
}

void CEffectPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MtlProp, m_mtlProp);
	DDX_Control(pDX, IDC_Preview, m_hPreviewWnd);
	DDX_Text(pDX, IDC_Index, m_szStateIndex);
	DDX_Control(pDX, IDC_State, m_stateProp);
	DDX_Control(pDX, IDC_Time, m_hShowTime);
	DDX_Control(pDX, IDC_ActList, m_actListBox);
	DDX_Control(pDX, IDC_TimeScale, m_edTimeScale);
	DDX_Control(pDX, IDC_DrawBox, m_hDrawBox);
}


BEGIN_MESSAGE_MAP(CEffectPropDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(ID_ApplyEdit, OnBnClickedApplyedit)
	ON_EN_CHANGE(IDC_Index, OnEnChangeIndex)
	ON_BN_CLICKED(IDC_IndexInc, OnBnClickedIndexinc)
	ON_BN_CLICKED(IDC_IndexDec, OnBnClickedIndexdec)
	ON_BN_CLICKED(IDC_DelState, OnBnClickedDelstate)
	ON_BN_CLICKED(IDC_AddState, OnBnClickedAddstate)
	ON_BN_CLICKED(IDC_OpenMdl, OnBnClickedOpenmdl)
	ON_BN_CLICKED(IDC_WireFrame, OnBnClickedWireframe)
	ON_BN_CLICKED(IDC_Play, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Pause, OnBnClickedPause)
	ON_CBN_SELCHANGE(IDC_ActList, OnCbnSelchangeActlist)
	ON_EN_CHANGE(IDC_TimeScale, OnEnChangeTimescale)
	ON_BN_CLICKED(IDC_ShowMdl, OnBnClickedShowmdl)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BKColor, OnBnClickedBkcolor)
	ON_LBN_SELCHANGE(IDC_State, OnLbnSelchangeState)
	ON_LBN_SELCHANGE(IDC_MtlProp, OnLbnSelchangeMtlprop)
	ON_BN_CLICKED(IDC_DrawBox, &CEffectPropDlg::OnBnClickedDrawbox)
END_MESSAGE_MAP()


// CEffectPropDlg 消息处理程序

void CEffectPropDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	DetachEffectFromPreView();

	OnOK();
}

void CEffectPropDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	DetachEffectFromPreView();

	OnCancel();
}

BOOL CEffectPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szTitle=_T("特效元素属性 - ");
	if(m_pEditEffect!=NULL)
	{
		szTitle+=m_pEditEffect->GetClassName();
		SetWindowText(szTitle);

		CopyEftData(m_pEditEffect);
	}


	m_mtlProp.SetFileFilter(_T("TGA文件|*.tga||"));
	
	if(m_bPrivew)
	{
		SetTimer(1,20,0);
		m_preView.Init(m_pEftRender);
		m_preView.GetEffectNode()->AddEffectPath(m_pEftPath);
		m_preView.LoadDefaultAvatar(1);
		m_preView.UpdateActList(&m_actListBox);

		CRect rc;
		m_hPreviewWnd.GetWindowRect(rc);
		int h=rc.Width()*600/800;
		rc.bottom=rc.top+h;
		ScreenToClient(rc);
		m_hPreviewWnd.MoveWindow(rc,FALSE);
	}

	if(!m_pEditEffect->IS_KIND_OF(EffectPathCtrl))
		UpdateMtlProp(false);
	UpdateStateProp(_tstoi(m_szStateIndex),false);

	SetTimer(2,1000,0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEffectPropDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nIDEvent)
	{
	case 1:
		{
			HWND hWnd=m_hPreviewWnd.GetSafeHwnd();
			if(::IsWindow(hWnd))
				m_preView.Render(hWnd,false);

			CString szTime;
			szTime.Format(_T("%.2f"),m_preView.GetEffectNode()->GetRunTime());
			m_hShowTime.SetWindowText(szTime);
		}
		break;
	case 2:
		{
			int sel=m_mtlProp.GetCurSel();
			if(sel!=-1)
				ShowSelectedPropInfo(&m_mtlProp);
			else
				ShowSelectedPropInfo(&m_stateProp);
		}
		break;
	}//endof switch

	CDialog::OnTimer(nIDEvent);
}

void CEffectPropDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnLButtonDown(nFlags,point);

	CDialog::OnLButtonDown(nFlags, point);
}

void CEffectPropDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnLButtonUp(nFlags,point);

	CDialog::OnLButtonUp(nFlags, point);
}

void CEffectPropDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnMouseMove(nFlags, point);

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CEffectPropDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnMouseWheel(nFlags, zDelta, pt);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CEffectPropDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnRButtonDown(nFlags, point);

	CDialog::OnRButtonDown(nFlags, point);
}

void CEffectPropDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_preView.OnRButtonUp(nFlags, point);

	CDialog::OnRButtonUp(nFlags, point);
}

template<typename EffectType,typename prop,typename stateArray>
void _BuildEffect(EffectType *pEftType,prop myProp,stateArray myStates)
{
	pEftType->BeginBuild(myProp);
	for(size_t i=0;i<myStates.size();i++)
	{
		pEftType->AddKeyState(myStates[i]);
	}
	pEftType->EndBuild();
}

void CEffectPropDlg::OnBnClickedApplyedit()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateStateProp(_tstoi(m_szStateIndex),true);//存储当前属性页的内容
	if(!m_pEditEffect->IS_KIND_OF(EffectPathCtrl))
		UpdateMtlProp(true);//存储材质属性

	if(!(m_pEditEffect->IS_STATIC_CLASS(EffectTarget))
		&& !(m_pEditEffect->IS_STATIC_CLASS(EffectExplosive))
		&& !(m_pEditEffect->IS_STATIC_CLASS(EffectBladeFlare)))
	{
		if(m_numStates<=1)
		{
			AfxMessageBox(_T("必须至少添加一个状态"));
			return;
		}
	}

	if(m_pEditEffect->IS_STATIC_CLASS(EffectQuad))
		_BuildEffect((EffectQuad *)m_pEditEffect,m_quadProp,m_quadStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCylinder))
		_BuildEffect((EffectCylinder *)m_pEditEffect,m_cyProp,m_cyStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectGrid))
		_BuildEffect((EffectGrid *)m_pEditEffect,m_gridProp,m_gridStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBeam))
		_BuildEffect((EffectBeam *)m_pEditEffect,m_beamProp,m_beamStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSceneDecal))
		_BuildEffect((EffectSceneDecal *)m_pEditEffect,m_scenedecalProp,m_scenedecalStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMotion))
		_BuildEffect((EffectMotion *)m_pEditEffect,m_mtProp,m_mtStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectParticleEmitter))
		_BuildEffect((EffectParticleEmitter *)m_pEditEffect,m_parProp,m_parStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMsg))
		_BuildEffect((EffectMsg *)m_pEditEffect,m_msgProp,m_msgStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMesh))
		_BuildEffect((EffectMesh *)m_pEditEffect,m_meshProp,m_meshStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSound))
		_BuildEffect((EffectSound *)m_pEditEffect,m_soundProp,m_soundStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCross))
		_BuildEffect((EffectCross *)m_pEditEffect,m_crossProp,m_crossStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSphere))
		_BuildEffect((EffectSphere *)m_pEditEffect,m_sphereProp,m_sphereStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectExplosive))
	{
		EffectExplosive *pExp=(EffectExplosive*)m_pEditEffect;
		pExp->Build(m_expProp);
	}
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBladeFlare))
	{
		EffectBladeFlare *pBladeFlare=(EffectBladeFlare*)m_pEditEffect;
		pBladeFlare->Build(m_bladeFlareProp);
	}
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectTarget))
	{
		EffectTarget *pTargetPath=(EffectTarget*)m_pEditEffect;
		pTargetPath->SetProperty(m_targetProp);
	}
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCamera))
		_BuildEffect((EffectCamera *)m_pEditEffect,m_cameraProp,m_cameraStates);

	m_pEditEffect->CreateMtl(m_mtlDesc);

	m_preView.UpdateEffect();//Effect重建了,必须通知pre view

	SGEffectNode *pNode=m_preView.GetEffectNode();
	if(pNode==NULL)
		return;
	pNode->Play();
	GetDlgItem(IDC_Play)->SetWindowText(_T("停止"));
}

void CEffectPropDlg::UpdateMtlProp(bool bSave)
{
	if(!bSave)//如果不是存数据,则创建List
		m_mtlProp.ResetContent();

	int pi=0;
	m_mtlProp.UpdateVal(_T("贴图文件"),m_mtlDesc.szTexName,sizeof(m_mtlDesc.szTexName),pi++,bSave,true);
	m_mtlProp.UpdateVal(_T("混合模式"),m_mtlDesc.blendType,pi++,bSave);
	m_mtlProp.UpdateVal(_T("Z值测试"),m_mtlDesc.bTestZ,pi++,bSave);
	m_mtlProp.UpdateVal(_T("动画横向帧数"),m_mtlDesc.frmU,pi++,bSave);
	m_mtlProp.UpdateVal(_T("动画纵向帧数"),m_mtlDesc.frmV,pi++,bSave);
	m_mtlProp.UpdateVal(_T("动画播放速度"),m_mtlDesc.fps,pi++,bSave);
	m_mtlProp.UpdateVal(_T("动画播放一遍"),m_mtlDesc.bPlayOnce,pi++,bSave);

	m_mtlProp.UpdateVal(_T("横向滚动速度"),m_mtlDesc.fScrollSpeedU,pi++,bSave);
	m_mtlProp.UpdateVal(_T("纵向滚动速度"),m_mtlDesc.fScrollSpeedV,pi++,bSave);
	m_mtlProp.UpdateVal(_T("是否热处理"),m_mtlDesc.bHeatHaze,pi++,bSave);
	m_mtlProp.UpdateVal(_T("热处理扰动系数"),m_mtlDesc.fHeatOffsetFator,pi++,bSave);
	m_mtlProp.UpdateVal(_T("是否受光照影响"),m_mtlDesc.bEnableLight,pi++,bSave);
}


int CEffectPropDlg::UpdateBaseProp(EffectBase::tagProp *pProp,int stateIndex,bool bSave)
{
	ASSERT(stateIndex==0);
	CPropertyItem *pItem=NULL;
	CString szTmp;
	int pi=0;

	m_stateProp.UpdateVal(_T("细节等级"),pProp->m_nLOD,pi++,bSave);
	m_stateProp.UpdateVal(_T("名称"),pProp->m_szName,sizeof(m_quadProp.m_szName),pi++,bSave,false);
	m_stateProp.UpdateVal(_T("启动"),pProp->m_bActive,pi++,bSave);
	m_stateProp.UpdateVal(_T("启动时间"),pProp->m_startTime,pi++,bSave);
	m_stateProp.UpdateVal(_T("循环播放"),pProp->m_loop,pi++,bSave);
	m_stateProp.UpdateVal(_T("关闭过程长度"),pProp->m_closeTime,pi++,bSave);

	return pi;
}

void CEffectPropDlg::UpdateBeamState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{
		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_beamProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("绑定路径"),m_beamProp.szPathName,sizeof(m_beamProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("分割次数"),m_beamProp.div,pi++,bSave);
		m_stateProp.UpdateVal(_T("采样时间间隔"),m_beamProp.sampleTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴图坐标V增加"),m_beamProp.divVScale,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_beamStates.size())//是否越界
		{
			return;
		}
		EffectBeam::tagState *pState=&m_beamStates[i];
		int pi=0;
		
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("宽度"),pState->width,pi++,bSave);
		m_stateProp.UpdateVal(_T("宽度增量"),pState->lenWidthInc,pi++,bSave);
		
		m_stateProp.UpdateVal(_T("头部颜色"),pState->headColor,pi++,bSave);
		m_stateProp.UpdateVal(_T("头部透明度"),pState->headColor.A,pi++,bSave);
		
		m_stateProp.UpdateVal(_T("尾部颜色"),pState->endColor,pi++,bSave);
		m_stateProp.UpdateVal(_T("尾部透明度"),pState->endColor.A,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateSceneDecalState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{
		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_scenedecalProp,stateIndex,bSave);
		m_stateProp.UpdateVal(_T("绑定路径"),m_scenedecalProp.szPathName,sizeof(m_scenedecalProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("位置x"),m_scenedecalProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_scenedecalProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_scenedecalProp.offset.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转"),m_scenedecalProp.initRot,pi++,bSave);
		//m_stateProp.UpdateVal(_T("是否使用默认面片"),m_scenedecalProp.bUseDefaultMesh,pi++,bSave);
		//m_stateProp.UpdateVal(_T("是否只使用默认面片"),m_scenedecalProp.bOnlyUseDefaultMesh,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴花范围（前方）"),m_scenedecalProp.rangeFront,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴花范围（后方）"),m_scenedecalProp.rangeBack,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_scenedecalStates.size())//是否越界
		{
			return;
		}
		EffectSceneDecal::tagState *pState=&m_scenedecalStates[i];
		int pi=0;

		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);

		m_stateProp.UpdateVal(_T("颜色"),pState->color,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->color.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("大小x"),pState->size.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("大小y"),pState->size.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("旋转速度"),pState->rotVel,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateGridState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_gridProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("绑定路径"),m_gridProp.szPathName,sizeof(m_gridProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("x分割次数"),m_gridProp.xDiv,pi++,bSave);
		m_stateProp.UpdateVal(_T("y分割次数"),m_gridProp.yDiv,pi++,bSave);

		m_stateProp.UpdateVal(_T("位置x"),m_gridProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_gridProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_gridProp.offset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_gridProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_gridProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_gridProp.initRot.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("曲面类型"),m_gridProp.faceType,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_gridStates.size())//是否越界
		{
			return;
		}
		EffectGrid::tagState *pState=&m_gridStates[i];
		int pi=0;
		
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		
		m_stateProp.UpdateVal(_T("颜色"),pState->color,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->color.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("贴图u重复次数"),pState->uvScale.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴图v重复次数"),pState->uvScale.y,pi++,bSave);

		m_stateProp.UpdateVal(_T("x大小"),pState->size.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y大小"),pState->size.y,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("波速"),pState->waveSpeed,pi++,bSave);
		m_stateProp.UpdateVal(_T("x振幅"),pState->wavePower.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y振幅"),pState->wavePower.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z振幅"),pState->wavePower.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateCylinderState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_cyProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("绑定路径"),m_cyProp.szPathName,sizeof(m_cyProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("分割次数"),m_cyProp.div,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置x"),m_cyProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_cyProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_cyProp.offset.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴图重复次数"),m_cyProp.uScale,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转x"),m_cyProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_cyProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_cyProp.initRot.z,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_cyStates.size())//是否越界
		{
			return;
		}
		EffectCylinder::tagState *pState=&m_cyStates[i];
		int pi=0;
		
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("圈数"),pState->numCircle,pi++,bSave);
		m_stateProp.UpdateVal(_T("高度"),pState->high,pi++,bSave);
		m_stateProp.UpdateVal(_T("顶半径"),pState->topRadius,pi++,bSave);
		m_stateProp.UpdateVal(_T("底半径"),pState->bottomRadius,pi++,bSave);
		m_stateProp.UpdateVal(_T("分割时顶半径增加"),pState->divTopRadiusInc,pi++,bSave);
		m_stateProp.UpdateVal(_T("分割时底半径增加"),pState->divBottomRadiusInc,pi++,bSave);

		m_stateProp.UpdateVal(_T("顶收缩"),pState->topShrink,pi++,bSave);
		m_stateProp.UpdateVal(_T("底收缩"),pState->bottomShrink,pi++,bSave);
		m_stateProp.UpdateVal(_T("分割时顶上升"),pState->divTopRise,pi++,bSave);
		m_stateProp.UpdateVal(_T("分割时底上升"),pState->divBottomRise,pi++,bSave);

		m_stateProp.UpdateVal(_T("顶颜色"),pState->topColor,pi++,bSave);
		m_stateProp.UpdateVal(_T("顶透明度"),pState->topColor.A,pi++,bSave);
		m_stateProp.UpdateVal(_T("底颜色"),pState->bottomColor,pi++,bSave);
		m_stateProp.UpdateVal(_T("底透明度"),pState->bottomColor.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);
	}

}

void CEffectPropDlg::UpdateMsgState(int stateIndex,bool bSave)
{
	if(stateIndex==0)
	{
		int pi=UpdateBaseProp(&m_msgProp,stateIndex,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_msgStates.size())//是否越界
		{
			return;
		}
		EffectMsg::tagState *pState=&m_msgStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->msgTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("消息名称"),pState->szMsg,sizeof(pState->szMsg),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("消息参数1"),pState->szMsgParam1,sizeof(pState->szMsgParam1),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("消息参数2"),pState->szMsgParam2,sizeof(pState->szMsgParam2),pi++,bSave,false);
	}
}

void CEffectPropDlg::UpdateMotionState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_mtProp,stateIndex,bSave);
		m_stateProp.UpdateVal(_T("起始位置x"),m_mtProp.initPos.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("起始位置y"),m_mtProp.initPos.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("起始位置z"),m_mtProp.initPos.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("是否计算朝向"),m_mtProp.bCalcRot,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_mtStates.size())//是否越界
		{
			return;
		}
		EffectMotion::tagState *pState=&m_mtStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		
		m_stateProp.UpdateVal(_T("移动速度x"),pState->vel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("移动速度y"),pState->vel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("移动速度z"),pState->vel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("公转速度x"),pState->revVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("公转速度y"),pState->revVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("公转速度z"),pState->revVel.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateSphereState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_sphereProp,stateIndex,bSave);


		m_stateProp.UpdateVal(_T("绑定路径"),m_sphereProp.szPathName,sizeof(m_sphereProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("位置x"),m_sphereProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_sphereProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_sphereProp.offset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_sphereProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_sphereProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_sphereProp.initRot.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("半径"),m_sphereProp.radius,pi++,bSave);
		m_stateProp.UpdateVal(_T("pitch范围min"),m_sphereProp.pitchRange.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("pitch范围max"),m_sphereProp.pitchRange.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("yaw范围min"),m_sphereProp.yawRange.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("yaw范围max"),m_sphereProp.yawRange.y,pi++,bSave);

		m_stateProp.UpdateVal(_T("分割次数"),m_sphereProp.slices,pi++,bSave);

		m_stateProp.UpdateVal(_T("贴图u重复次数"),m_sphereProp.uvScale.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("贴图v重复次数"),m_sphereProp.uvScale.y,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_sphereStates.size())//是否越界
		{
			return;
		}
		EffectSphere::tagState *pState=&m_sphereStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("颜色"),pState->color,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->color.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("x缩放"),pState->sacle.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y缩放"),pState->sacle.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z缩放"),pState->sacle.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateCrossState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_crossProp,stateIndex,bSave);


		m_stateProp.UpdateVal(_T("绑定路径"),m_crossProp.szPathName,sizeof(m_crossProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("位置x"),m_crossProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_crossProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_crossProp.offset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_crossProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_crossProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_crossProp.initRot.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("面序列长度"),m_crossProp.numFace,pi++,bSave);
		m_stateProp.UpdateVal(_T("产生横向面"),m_crossProp.bHFace,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_crossStates.size())//是否越界
		{
			return;
		}
		EffectCross::tagState *pState=&m_crossStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("颜色"),pState->color,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->color.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("x大小"),pState->size.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y大小"),pState->size.y,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("面间距"),pState->faceDist,pi++,bSave);
		m_stateProp.UpdateVal(_T("面间距增量"),pState->faceDistInc,pi++,bSave);
		m_stateProp.UpdateVal(_T("x大小增量"),pState->faceSizeInc.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y大小增量"),pState->faceSizeInc.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("面透明度降低"),pState->faceAlphaDec,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateQuadState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_quadProp,stateIndex,bSave);


		m_stateProp.UpdateVal(_T("绑定路径"),m_quadProp.szPathName,sizeof(m_quadProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("位置x"),m_quadProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_quadProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_quadProp.offset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_quadProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_quadProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_quadProp.initRot.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("面片类型"),m_quadProp.faceType,pi++,bSave);
		m_stateProp.UpdateVal(_T("残影个数"),m_quadProp.tailSize,pi++,bSave);
		m_stateProp.UpdateVal(_T("残影间隔时间"),m_quadProp.tailInterval,pi++,bSave);
		m_stateProp.UpdateVal(_T("残影生命时间"),m_quadProp.tailLife,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_quadStates.size())//是否越界
		{
			return;
		}
		EffectQuad::tagState *pState=&m_quadStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("颜色"),pState->color,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->color.A,pi++,bSave);

		m_stateProp.UpdateVal(_T("x大小"),pState->size.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y大小"),pState->size.y,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateParticleState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_parProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("绑定路径"),m_parProp.szPathName,sizeof(m_parProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("形状"),m_parProp.shape,pi++,bSave);
		m_stateProp.UpdateVal(_T("最大粒子数"),m_parProp.maxParticles,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始化粒子数"),m_parProp.initParNum,pi++,bSave);

		m_stateProp.UpdateVal(_T("位置x"),m_parProp.pathOffset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置y"),m_parProp.pathOffset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("位置z"),m_parProp.pathOffset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子速度方式"),m_parProp.velType,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子多边形类型"),m_parProp.polygonType,pi++,bSave);
		m_stateProp.UpdateVal(_T("重力系数"),m_parProp.gForce,pi++,bSave);
		m_stateProp.UpdateVal(_T("空气阻力系数"),m_parProp.airForce,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_parProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_parProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_parProp.initRot.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子自动淡出"),m_parProp.bParAutoFade,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子发射到世界中"),m_parProp.bToWorld,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_parStates.size())//是否越界
		{
			return;
		}
		EffectParticleEmitter::tagState *pState=&m_parStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("形状参数1"),pState->shapeParam.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("形状参数2"),pState->shapeParam.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("形状参数3"),pState->shapeParam.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("形状参数4"),pState->shapeParam.w,pi++,bSave);

		m_stateProp.UpdateVal(_T("x自转速度"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("y自转速度"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("z自转速度"),pState->rotVel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子产生的速度"),pState->emitSpeed,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.生命长度Max"),pState->parLifeTimeMax,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.生命长度Min"),pState->parLifeTimeMin,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.速度方向x"),pState->parVelDir.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.速度方向y"),pState->parVelDir.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.速度方向z"),pState->parVelDir.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.速率Max"),pState->parSpeedMax,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.速率Min"),pState->parSpeedMin,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子.摆动Max"),pState->parSwingMax,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.摆动Min"),pState->parSwingMin,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子.自转Max.x"),pState->parRotSpeedMax.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.自转Max.y"),pState->parRotSpeedMax.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.自转Max.z"),pState->parRotSpeedMax.z,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.自转Min.x"),pState->parRotSpeedMin.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.自转Min.y"),pState->parRotSpeedMin.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.自转Min.z"),pState->parRotSpeedMin.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子.大小Max"),pState->parSizeMax,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.大小Min"),pState->parSizeMin,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.大小变化速度"),pState->parSizeVel,pi++,bSave);

		m_stateProp.UpdateVal(_T("粒子.颜色"),pState->parColor,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.透明度"),pState->parColor.A,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.变红速度"),pState->parColorVel.R,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.变绿速度"),pState->parColorVel.G,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.变蓝速度"),pState->parColorVel.B,pi++,bSave);
		m_stateProp.UpdateVal(_T("粒子.变透明速度"),pState->parColorVel.A,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateSoundState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_soundProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("使用3D音效"),m_soundProp.b3DSound,pi++,bSave);
		m_stateProp.UpdateVal(_T("绑定路径"),m_soundProp.szPathName,sizeof(m_soundProp.szPathName),pi++,bSave,false);
		m_stateProp.UpdateVal(_T("时间间隔"),m_soundProp.timeLoop,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_soundStates.size())//是否越界
		{
			return;
		}
		EffectSound::tagState *pState=&m_soundStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->soundTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("循环播放"),pState->bLoop,pi++,bSave);
		m_stateProp.UpdateVal(_T("声音文件"),pState->szSoundPath,sizeof(pState->szSoundPath),pi++,bSave,true);
		m_stateProp.UpdateVal(_T("最小距离"),pState->minDist,pi++,bSave);
		m_stateProp.UpdateVal(_T("最大距离"),pState->maxDist,pi++,bSave);
		m_stateProp.UpdateVal(_T("音量"),pState->volume,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateMeshState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_meshProp,stateIndex,bSave);

		m_stateProp.UpdateVal(_T("模型文件"),m_meshProp.szMeshName,sizeof(m_meshProp.szMeshName),pi++,bSave,true);
		m_stateProp.UpdateVal(_T("绑定路径"),m_meshProp.szPathName,sizeof(m_meshProp.szPathName),pi++,bSave,false);

		m_stateProp.UpdateVal(_T("起始位置x"),m_meshProp.offset.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("起始位置y"),m_meshProp.offset.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("起始位置z"),m_meshProp.offset.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("初始自转x"),m_meshProp.initRot.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转y"),m_meshProp.initRot.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("初始自转z"),m_meshProp.initRot.z,pi++,bSave);
	}
	else
	{
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_meshStates.size())//是否越界
		{
			return;
		}
		EffectMesh::tagState *pState=&m_meshStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("透明度"),pState->alpha,pi++,bSave);

		m_stateProp.UpdateVal(_T("缩放x"),pState->scale.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("缩放y"),pState->scale.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("缩放z"),pState->scale.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("自转速度x"),pState->rotVel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("自转速度y"),pState->rotVel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("自转速度z"),pState->rotVel.z,pi++,bSave);

		m_stateProp.UpdateVal(_T("移动速度x"),pState->vel.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("移动速度y"),pState->vel.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("移动速度z"),pState->vel.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateExpState(int stateIndex,bool bSave)
{
	if(stateIndex!=0)//只有Prop,没有其他state
		return;

	int pi=UpdateBaseProp(&m_expProp,stateIndex,bSave);
	
	m_stateProp.UpdateVal(_T("绑定路径"),m_expProp.szPathName,sizeof(m_expProp.szPathName),pi++,bSave,false);
	m_stateProp.UpdateVal(_T("位置x"),m_expProp.offset.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("位置y"),m_expProp.offset.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("位置z"),m_expProp.offset.z,pi++,bSave);

	m_stateProp.UpdateVal(_T("自转x"),m_expProp.rot.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("自转y"),m_expProp.rot.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("自转z"),m_expProp.rot.z,pi++,bSave);

	m_stateProp.UpdateVal(_T("面片类型"),m_expProp.faceType,pi++,bSave);
	m_stateProp.UpdateVal(_T("本体类型"),m_expProp.bodyType,pi++,bSave);

	m_stateProp.UpdateVal(_T("体形参数1"),m_expProp.bodyParam.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("体形参数2"),m_expProp.bodyParam.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("体形参数3"),m_expProp.bodyParam.z,pi++,bSave);
	m_stateProp.UpdateVal(_T("体形参数4"),m_expProp.bodyParam.w,pi++,bSave);

	m_stateProp.UpdateVal(_T("横向切分"),m_expProp.xDiv,pi++,bSave);
	m_stateProp.UpdateVal(_T("纵向切分"),m_expProp.yDiv,pi++,bSave);

	m_stateProp.UpdateVal(_T("爆炸衰减系数"),m_expProp.powerDec,pi++,bSave);
	m_stateProp.UpdateVal(_T("爆炸能量Max"),m_expProp.maxPower,pi++,bSave);
	m_stateProp.UpdateVal(_T("爆炸能量Min"),m_expProp.minPower,pi++,bSave);

	m_stateProp.UpdateVal(_T("空气阻力系数"),m_expProp.airForce,pi++,bSave);
	m_stateProp.UpdateVal(_T("重力系数"),m_expProp.gForce,pi++,bSave);

	m_stateProp.UpdateVal(_T("爆炸方向x"),m_expProp.dir.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("爆炸方向y"),m_expProp.dir.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("爆炸方向z"),m_expProp.dir.z,pi++,bSave);

	m_stateProp.UpdateVal(_T("贴图u重复次数"),m_expProp.uvScale.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("贴图v重复次数"),m_expProp.uvScale.y,pi++,bSave);

	m_stateProp.UpdateVal(_T("生存时间Max"),m_expProp.maxLifeTime,pi++,bSave);
	m_stateProp.UpdateVal(_T("生存时间Min"),m_expProp.minLifeTime,pi++,bSave);

}

void CEffectPropDlg::UpdateBladeFlareState(int stateIndex,bool bSave)
{
	if(stateIndex!=0)//只有Prop,没有其他state
		return;

	int pi=0;

	m_stateProp.UpdateVal(_T("名称"),m_bladeFlareProp.m_szName,sizeof(m_bladeFlareProp.m_szName),pi++,bSave,false);
	m_stateProp.UpdateVal(_T("持续时间"),m_bladeFlareProp.trackTimeLen,pi++,bSave);
	m_stateProp.UpdateVal(_T("采样间隔"),m_bladeFlareProp.sampleTime,pi++,bSave);
	m_stateProp.UpdateVal(_T("颜色"),m_bladeFlareProp.color,pi++,bSave);
	m_stateProp.UpdateVal(_T("透明度"),m_bladeFlareProp.color.A,pi++,bSave);
	m_stateProp.UpdateVal(_T("编号"),m_bladeFlareProp.nIndex,pi++,bSave);
	if( m_bladeFlareProp.nIndex > 9 || m_bladeFlareProp.nIndex < 0 )
		AfxMessageBox( _T("刀光编号必须在0-9之间") );

	m_stateProp.UpdateVal(_T("不平滑处理"),m_bladeFlareProp.bNoSmooth,pi++,bSave);
	m_stateProp.UpdateVal(_T("自定义挂载点"),m_bladeFlareProp.bCustomTagNode,pi++,bSave);
	m_stateProp.UpdateVal(_T("挂载点名称1"),m_bladeFlareProp.szTagNode[0],sizeof(m_bladeFlareProp.szTagNode[0]),pi++,bSave,false);
	m_stateProp.UpdateVal(_T("挂载点名称2"),m_bladeFlareProp.szTagNode[1],sizeof(m_bladeFlareProp.szTagNode[0]),pi++,bSave,false);
}

void CEffectPropDlg::UpdateTargetState(int stateIndex,bool bSave)
{
	if(stateIndex!=0)//target 只有Prop,没有其他state
		return;

	CString szTmp;
	int pi=UpdateBaseProp(&m_targetProp,stateIndex,bSave);

	m_stateProp.UpdateVal(_T("速度曲线类型"),m_targetProp.speedType,pi++,bSave);
	m_stateProp.UpdateVal(_T("运动时间"),m_targetProp.time,pi++,bSave);
	m_stateProp.UpdateVal(_T("运动速率(用来估计时间)"),m_targetProp.speed,pi++,bSave);
	
	m_stateProp.UpdateVal(_T("起点x"),m_targetProp.startPt.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("起点y"),m_targetProp.startPt.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("起点z"),m_targetProp.startPt.z,pi++,bSave);
	
	m_stateProp.UpdateVal(_T("终点x"),m_targetProp.endPt.x,pi++,bSave);
	m_stateProp.UpdateVal(_T("终点y"),m_targetProp.endPt.y,pi++,bSave);
	m_stateProp.UpdateVal(_T("终点z"),m_targetProp.endPt.z,pi++,bSave);

	m_stateProp.UpdateVal(_T("高度曲线类型"),m_targetProp.upLineType,pi++,bSave);
	m_stateProp.UpdateVal(_T("摆动曲线类型"),m_targetProp.sideLineType,pi++,bSave);
	m_stateProp.UpdateVal(_T("最大高度"),m_targetProp.upMax,pi++,bSave);
	m_stateProp.UpdateVal(_T("最大摆动幅度"),m_targetProp.sideMax,pi++,bSave);
}

void CEffectPropDlg::UpdateCameraState(int stateIndex,bool bSave)
{
	if(stateIndex==0)//0的话显示tagProp
	{

		CPropertyItem *pItem=NULL;
		CString szTmp;
		int pi=UpdateBaseProp(&m_cameraProp,stateIndex,bSave);
		m_stateProp.UpdateVal(_T("衰减系数"),m_cameraProp.attenuationFactor,pi++,bSave);
		m_stateProp.UpdateVal(_T("影响范围"),m_cameraProp.range,pi++,bSave);
	}
	else
	{	
		int i=stateIndex-1;
		ASSERT((i>=0));
		if(i>=(int)m_cameraStates.size())//是否越界
		{
			return;
		}
		EffectCamera::tagState *pState=&m_cameraStates[i];

		int pi=0;
		m_stateProp.UpdateVal(_T("时间"),pState->lastTime,pi++,bSave);
		m_stateProp.UpdateVal(_T("振幅x"),pState->swing.x,pi++,bSave);
		m_stateProp.UpdateVal(_T("振幅y"),pState->swing.y,pi++,bSave);
		m_stateProp.UpdateVal(_T("振幅z"),pState->swing.z,pi++,bSave);
	}
}

void CEffectPropDlg::UpdateStateProp(int stateIndex,bool bSave)
{
	if(m_pEditEffect==NULL)
		return;

	if(!bSave)//如果不是存数据,则是翻页
		m_stateProp.ResetContent();

	if(m_pEditEffect->IS_STATIC_CLASS(EffectQuad))
		UpdateQuadState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCylinder))
		UpdateCylinderState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectGrid))
		UpdateGridState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBeam))
		UpdateBeamState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSceneDecal))
		UpdateSceneDecalState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMotion))
		UpdateMotionState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectParticleEmitter))
		UpdateParticleState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMsg))
		UpdateMsgState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMesh))
		UpdateMeshState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSound))
		UpdateSoundState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectTarget))
		UpdateTargetState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCross))
		UpdateCrossState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSphere))
		UpdateSphereState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectExplosive))
		UpdateExpState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBladeFlare))
		UpdateBladeFlareState(stateIndex,bSave);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCamera))
		UpdateCameraState(stateIndex,bSave);
	else
	{
		ASSERT(0 && _T("Unknow effect type!"));
	}

}

void CEffectPropDlg::OnEnChangeIndex()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateStateProp(_tstoi(m_szStateIndex),true);
	UpdateData();
	UpdateStateProp(_tstoi(m_szStateIndex),false);
}

void CEffectPropDlg::OnBnClickedIndexinc()
{
	// TODO: 在此添加控件通知处理程序代码
	int n=_tstoi(m_szStateIndex);
	n++;
	if(n>=m_numStates)
		n=m_numStates-1;
	UpdateStateProp(_tstoi(m_szStateIndex),true);
	m_szStateIndex.Format(_T("%d"),n);
	UpdateData(FALSE);
	UpdateStateProp(_tstoi(m_szStateIndex),false);
}

void CEffectPropDlg::OnBnClickedIndexdec()
{
	// TODO: 在此添加控件通知处理程序代码
	int n=_tstoi(m_szStateIndex);
	n--;
	if(n<0)
		n=0;
	UpdateStateProp(_tstoi(m_szStateIndex),true);
	m_szStateIndex.Format(_T("%d"),n);
	UpdateData(FALSE);
	UpdateStateProp(_tstoi(m_szStateIndex),false);
}

void CEffectPropDlg::OnBnClickedDelstate()
{
	// TODO: 在此添加控件通知处理程序代码
	int n=_tstoi(m_szStateIndex);
	if(n==0)
	{
		AfxMessageBox(_T("不能删除初始状态"));
		return;
	}

	m_numStates--;
	if(m_pEditEffect->IS_STATIC_CLASS(EffectQuad))
		m_quadStates.erase(m_quadStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCylinder))
		m_cyStates.erase(m_cyStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectGrid))
		m_gridStates.erase(m_gridStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBeam))
		m_beamStates.erase(m_beamStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSceneDecal))
		m_scenedecalStates.erase(m_scenedecalStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMotion))
		m_mtStates.erase(m_mtStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectParticleEmitter))
		m_parStates.erase(m_parStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMsg))
		m_msgStates.erase(m_msgStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMesh))
		m_meshStates.erase(m_meshStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSound))
		m_soundStates.erase(m_soundStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCross))
		m_crossStates.erase(m_crossStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSphere))
		m_sphereStates.erase(m_sphereStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCamera))
		m_cameraStates.erase(m_cameraStates.begin()+n-1);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectTarget)
		|| m_pEditEffect->IS_STATIC_CLASS(EffectExplosive)
		|| m_pEditEffect->IS_STATIC_CLASS(EffectBladeFlare))
	{
		//do nothing
	}

	//--向前翻页
	m_szStateIndex.Format(_T("%d"),n-1);
	UpdateData(FALSE);
	UpdateStateProp(_tstoi(m_szStateIndex),false);	
}

template<typename myState>
int _AddState(vector<myState>& states)
{
	myState newState;
	int s=states.size();
	if(s>0)
		newState=states[s-1];//拷贝上一状态
	states.push_back(newState);

	return states.size()+1;
}

void CEffectPropDlg::OnBnClickedAddstate()
{
	// TODO: 在此添加控件通知处理程序代码

	//--保存当前页
	UpdateStateProp(_tstoi(m_szStateIndex),true);
	
	//-- 添加一个新页
	if(m_pEditEffect->IS_STATIC_CLASS(EffectQuad))
		m_numStates=_AddState(m_quadStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCylinder))
		m_numStates=_AddState(m_cyStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectGrid))
		m_numStates=_AddState(m_gridStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectBeam))
		m_numStates=_AddState(m_beamStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSceneDecal))
		m_numStates=_AddState(m_scenedecalStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMotion))
		m_numStates=_AddState(m_mtStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectParticleEmitter))
		m_numStates=_AddState(m_parStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMsg))
		m_numStates=_AddState(m_msgStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMesh))
		m_numStates=_AddState(m_meshStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectMesh))
		m_numStates=_AddState(m_meshStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSound))
		m_numStates=_AddState(m_soundStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCross))
		m_numStates=_AddState(m_crossStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectSphere))
		m_numStates=_AddState(m_sphereStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectCamera))
		m_numStates=_AddState(m_cameraStates);
	else if(m_pEditEffect->IS_STATIC_CLASS(EffectTarget)
		||m_pEditEffect->IS_STATIC_CLASS(EffectExplosive)
		||m_pEditEffect->IS_STATIC_CLASS(EffectBladeFlare))
	{
		//do nothing
	}
	
	//--翻到最后页(新增的)
	m_szStateIndex.Format(_T("%d"),m_numStates-1);
	UpdateData(FALSE);
	UpdateStateProp(_tstoi(m_szStateIndex),false);	
}

void CEffectPropDlg::OnBnClickedOpenmdl()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("静态模型文件|*.fsm;*.fscn||"),
	this,0);
	if(IDOK==dlg.DoModal())
	{
		CString szPath=dlg.GetPathName();
		GetDlgItem(IDC_MdlPath)->SetWindowText(szPath);
		m_preView.LoadMdl(szPath);
	}
}

void CEffectPropDlg::OnBnClickedWireframe()
{
	// TODO: 在此添加控件通知处理程序代码
	if(IsDlgButtonChecked(IDC_WireFrame))
		Device()->SetFillMode(EFill_Wireframe);
	else
		Device()->SetFillMode(EFill_Solid);
}

template<typename EffectType,typename prop,typename stateArray>
int _CopyEffectData(EffectType *pEftType,prop& myProp,stateArray& myStates)
{
	myProp=pEftType->GetProp();
	myStates.clear();

	for(int i=0;i<pEftType->GetNumStates();i++)
		myStates.push_back(pEftType->GetState(i));

	return myStates.size()+1;
}

void CEffectPropDlg::CopyEftData(EffectBase *pEft)
{
	m_mtlDesc=pEft->GetMtlDesc();

	if(pEft->IS_STATIC_CLASS(EffectQuad))
		m_numStates=_CopyEffectData((EffectQuad *)pEft,m_quadProp,m_quadStates);
	else if(pEft->IS_STATIC_CLASS(EffectCylinder))
		m_numStates=_CopyEffectData((EffectCylinder *)pEft,m_cyProp,m_cyStates);
	else if(pEft->IS_STATIC_CLASS(EffectGrid))
		m_numStates=_CopyEffectData((EffectGrid *)pEft,m_gridProp,m_gridStates);
	else if(pEft->IS_STATIC_CLASS(EffectBeam))
		m_numStates=_CopyEffectData((EffectBeam *)pEft,m_beamProp,m_beamStates);
	else if(pEft->IS_STATIC_CLASS(EffectSceneDecal))
		m_numStates=_CopyEffectData((EffectSceneDecal *)pEft,m_scenedecalProp,m_scenedecalStates);
	else if(pEft->IS_STATIC_CLASS(EffectMotion))
		m_numStates=_CopyEffectData((EffectMotion*)pEft,m_mtProp,m_mtStates);
	else if(pEft->IS_STATIC_CLASS(EffectParticleEmitter))
		m_numStates=_CopyEffectData((EffectParticleEmitter*)pEft,m_parProp,m_parStates);
	else if(pEft->IS_STATIC_CLASS(EffectMsg))
		m_numStates=_CopyEffectData((EffectMsg*)pEft,m_msgProp,m_msgStates);
	else if(pEft->IS_STATIC_CLASS(EffectMesh))
		m_numStates=_CopyEffectData((EffectMesh*)pEft,m_meshProp,m_meshStates);
	else if(pEft->IS_STATIC_CLASS(EffectSound))
		m_numStates=_CopyEffectData((EffectSound*)pEft,m_soundProp,m_soundStates);
	else if(pEft->IS_STATIC_CLASS(EffectCross))
		m_numStates=_CopyEffectData((EffectCross*)pEft,m_crossProp,m_crossStates);
	else if(pEft->IS_STATIC_CLASS(EffectSphere))
		m_numStates=_CopyEffectData((EffectSphere*)pEft,m_sphereProp,m_sphereStates);
	else if(pEft->IS_STATIC_CLASS(EffectCamera))
		m_numStates=_CopyEffectData((EffectCamera*)pEft,m_cameraProp,m_cameraStates);
	else if(pEft->IS_STATIC_CLASS(EffectExplosive))
	{
		m_numStates=1;
		m_expProp=((EffectExplosive*)pEft)->GetProperty();
	}
	else if(pEft->IS_STATIC_CLASS(EffectBladeFlare))
	{
		m_numStates=1;
		m_bladeFlareProp=((EffectBladeFlare*)pEft)->GetProp();
	}
	else if(pEft->IS_STATIC_CLASS(EffectTarget))
	{
		m_numStates=1;
		m_targetProp=((EffectTarget*)pEft)->GetProperty();
	}
}
void CEffectPropDlg::OnBnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	SGEffectNode *pNode=m_preView.GetEffectNode();
	if(pNode==NULL)
		return;

	CString szBtnName;
	CWnd *pBtn=GetDlgItem(IDC_Play);
	pBtn->GetWindowText(szBtnName);
	if(szBtnName==_T("播放"))
	{
		pNode->Play();
		OnCbnSelchangeActlist();//重新播放动作
		pBtn->SetWindowText(_T("停止"));
	}
	else if(szBtnName==_T("停止"))
	{
		pNode->Close();
		pBtn->SetWindowText(_T("播放"));
	}
}
void CEffectPropDlg::OnBnClickedPause()
{
	// TODO: 在此添加控件通知处理程序代码
		SGEffectNode *pNode=m_preView.GetEffectNode();
	if(pNode==NULL)
		return;

	CString szBtnName;
	CWnd *pBtn=GetDlgItem(IDC_Pause);
	pBtn->GetWindowText(szBtnName);
	if(szBtnName==_T("暂停"))
	{
		pNode->Pause();
		pBtn->SetWindowText(_T("继续"));
	}
	else if(szBtnName==_T("继续"))
	{
		pNode->Resume();
		pBtn->SetWindowText(_T("暂停"));
	}
}

void CEffectPropDlg::OnCbnSelchangeActlist()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel=m_actListBox.GetCurSel();
	if(sel!=-1)
	{
		CString szName;
		m_actListBox.GetLBText(sel,szName);
		m_preView.PlayAvatarAction(szName);
	}
}

void CEffectPropDlg::OnEnChangeTimescale()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString szTxt;
	m_edTimeScale.GetWindowText(szTxt);
	if(!szTxt.IsEmpty())
		m_preView.SetTimeScale((float)_tstof(szTxt));
}

void CEffectPropDlg::OnBnClickedShowmdl()
{
	// TODO: 在此添加控件通知处理程序代码
	m_preView.SetShowMdl(IsDlgButtonChecked(IDC_ShowMdl)==1);
}

void CEffectPropDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DetachEffectFromPreView();

	CDialog::OnClose();
}

void CEffectPropDlg::DetachEffectFromPreView()
{
	SGEffectNode *pSGNode=m_preView.GetEffectNode();
	if(m_pEftPath)
		pSGNode->RemovePath(m_szEftPathName,false);
//不能	if(m_pEditEffect)
//		pSGNode->RemoveEffect(m_pEditEffect->GetName());
	if(m_pEftRender)
		pSGNode->RemoveEffect(m_pEftRender->GetName(),false);
}

void CEffectPropDlg::OnBnClickedBkcolor()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog dlg(0,0,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF c=dlg.GetColor();
		Color4ub Cool3Dc(255,GetRValue(c),GetGValue(c),GetBValue(c));
		m_preView.SetBkColor(Cool3Dc);
	}
}

void CEffectPropDlg::OnLbnSelchangeState()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSelectedPropInfo(&m_stateProp);
}

void CEffectPropDlg::OnLbnSelchangeMtlprop()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSelectedPropInfo(&m_mtlProp);
}

void CEffectPropDlg::ShowSelectedPropInfo(CPropertyList* pList)
{
	int sel=pList->GetCurSel();
	if(sel==-1)
		return;
	CPropertyItem *pItem=(CPropertyItem *)pList->GetItemDataPtr(sel);
	CString szText=_T("NULL");
	if(pItem!=NULL)
		szText.Format(_T("%s: %s"),pItem->m_propName,pItem->m_curValue);
	GetDlgItem(IDC_ListInfo)->SetWindowText(szText);
}
BOOL CEffectPropDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CEffectPropDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CEffectPropDlg::OnBnClickedDrawbox()
{
	// TODO: Add your control notification handler code here
	m_preView.EnableDrawBox( TRUE == m_hDrawBox.GetCheck() );
}
