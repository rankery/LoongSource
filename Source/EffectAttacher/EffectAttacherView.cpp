// EffectAttacherView.cpp : CEffectAttacherView 类的实现
//

#include "stdafx.h"
#include "EffectAttacher.h"

#include "EffectAttacherDoc.h"
#include "EffectAttacherView.h"
//#include ".\effectattacherview.h"

#include "Cool3DModels.h"
#include ".\effectattacherview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEffectAttacherView

IMPLEMENT_DYNCREATE(CEffectAttacherView, CFormView)

BEGIN_MESSAGE_MAP(CEffectAttacherView, CFormView)
	ON_WM_CREATE()
//	ON_BN_CLICKED(IDC_TEST, OnBnClickedTest)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//ON_LBN_DBLCLK(IDC_STRUCT_LIST, OnLbnDblclkStructList)
//	ON_LBN_DBLCLK(IDC_TRACK_LIST, OnLbnDblclkTrackList)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_AttachEffect, OnBnClickedAttachEffect)
//	ON_WM_SIZE()
ON_WM_MOUSEWHEEL()
//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//ON_BN_CLICKED(IDC_AttachEffectThroughFile, OnBnClickedAttacheffectthroughfile)
ON_BN_CLICKED(IDC_AttachEffectThroughFile, OnBnClickedAttacheffectthroughfile)
ON_BN_CLICKED(IDC_DeleteEffect, OnBnClickedDeleteeffect)
//ON_WM_CLOSE()
ON_BN_CLICKED(IDC_PlayTrack, OnBnClickedPlaytrack)
ON_BN_CLICKED(IDC_SaveEffectToFile, OnBnClickedSaveeffecttofile)
ON_LBN_SELCHANGE(IDC_STRUCT_LIST, &CEffectAttacherView::OnLbnSelchangeStructList)
END_MESSAGE_MAP()

// CEffectAttacherView 构造/析构

CEffectAttacherView::CEffectAttacherView()
	: CFormView(CEffectAttacherView::IDD)
	, m_bArcBall(FALSE)
	, m_bDrag(FALSE)
{
	// TODO: 在此处添加构造代码
}

CEffectAttacherView::~CEffectAttacherView()
{
}

void CEffectAttacherView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DrawWnd, m_drawWnd);
	DDX_Control(pDX, IDC_STRUCT_LIST, m_StructList);
	DDX_Control(pDX, IDC_TRACK_LIST, m_TrackList);
	DDX_Control(pDX, IDC_EFFECT_LIST, m_Effects_ListBox);
}

BOOL CEffectAttacherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式
	return CFormView::PreCreateWindow(cs);
}

void CEffectAttacherView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	static bool firstTime=true;

	if( firstTime == true )
	{
		firstTime = false;

		CRect rc;
		/*
		rc.left=0;
		rc.top=0;
		rc.right=600;
		rc.bottom=70;
		MoveWindow(rc);
		*/
		GetClientRect(rc);
		rc.right -= 5;
		rc.left += 200;
		rc.top += 5;
		rc.bottom -= 5;
		AdjustWindowRect(rc,m_drawWnd.GetStyle(),FALSE);
		m_drawWnd.MoveWindow(rc);

		Cool3DModels::Inst()->initialEngine(m_drawWnd.GetSafeHwnd());
	}
	SceneGraph *pSG=Cool3DModels::Inst()->GetSceneGraph();
	const tagLight& lgt=pSG->GetSunLight();


	//m_amb.SetColor(RGB(lgt.ambient.R,lgt.ambient.G,lgt.ambient.B));
	//m_diff.SetColor(RGB(lgt.diffuse.R,lgt.diffuse.G,lgt.diffuse.B));

	//CString szFov;
	//szFov.Format("%.2f",_RadToDeg(Cool3DModels::Inst()->GetCamera()->GetFOV()));
	//m_hFov.SetWindowText(szFov);

	//m_hAniTimeScale.SetWindowText("1.0");
	//m_hLoopAni.SetCheck(1);
}


// CEffectAttacherView 诊断

#ifdef _DEBUG
void CEffectAttacherView::AssertValid() const
{
	CFormView::AssertValid();
}

void CEffectAttacherView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CEffectAttacherDoc* CEffectAttacherView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectAttacherDoc)));
	return (CEffectAttacherDoc*)m_pDocument;
}
#endif //_DEBUG


// CEffectAttacherView 消息处理程序

void CEffectAttacherView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateTrackListBox();
	UpdateStructListBox();
}

BOOL CEffectAttacherView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CFormView::PreTranslateMessage(pMsg);
}

int CEffectAttacherView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CEffectAttacherView::OnLButtonDown(UINT nFlags, CPoint _point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateData();
	if(m_bArcBall)
	{
		POINT point;
		point.x=_point.x;
		point.y=_point.y;
		ClientToScreen(&point);
		m_drawWnd.ScreenToClient(&point);
		CRect rc;
		m_drawWnd.GetClientRect(rc);
		if(rc.PtInRect(point))
			Cool3DModels::Inst()->GetArcBall()->OnMouseBtnDown(point.x,point.y);
	}

	m_bDrag=TRUE;
	m_DragPt=_point;
	ClientToScreen(&m_DragPt);
	m_drawWnd.ScreenToClient(&m_DragPt);
	SetCapture();

	CFormView::OnLButtonDown(nFlags, _point);
}

void CEffectAttacherView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bArcBall)
		Cool3DModels::Inst()->GetArcBall()->OnMouseBtnUp();

	m_bDrag=FALSE;
	ReleaseCapture();

	CFormView::OnLButtonUp(nFlags, point);
}

void CEffectAttacherView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFormView::OnRButtonDown(nFlags, point);
}

void CEffectAttacherView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFormView::OnRButtonUp(nFlags, point);
}

bool CEffectAttacherView::UpdateTrackListBox(void)
{
	m_TrackList.ResetContent();
	AnimationCtrl *pAni=Cool3DModels::Inst()->GetSceneNodeAni();

	if(pAni)
	{
		vector<tstring> tracks;
		pAni->GetTrackSet(tracks);
		for(vector<tstring>::iterator iter=tracks.begin();
			iter!=tracks.end();iter++)
		{
			m_TrackList.AddString(iter->c_str());
		}
		return true;
	}
	return false;
}

bool CEffectAttacherView::UpdateStructListBox(void)
{
	bool bRet = false;

	m_StructList.ResetContent();
	AnimationCtrl *pAni=Cool3DModels::Inst()->GetSceneNodeAni();
	if( pAni == NULL )
		return bRet;

	if( pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
	{
		AvatarNode *pNode = static_cast<AvatarNode*>(Cool3DModels::Inst()->GetSceneNode());		;
		const SkeletonStruct& skelStruct=pNode->GetResource()->GetStruct();
		skelStruct.m_boneNames;
		for( UINT i=0; i<skelStruct.m_boneNames.size(); i++ )
		{
			m_StructList.AddString( skelStruct.m_boneNames[i].c_str() );
		}

		bRet = true;
	}
	else if( pAni->IS_STATIC_CLASS(VertexAnimationCtrl) )
	{
		VertexAnimationCtrl *pVertexCtrl = static_cast<VertexAnimationCtrl*>(pAni);

		tstring tagNodeName;
		const vector<tagTagNodeSpace>& tagNodes=pVertexCtrl->GetTagNodes();
		/*
		for(vector<tagTagNodeSpace>::const_iterator iter=tagNodes.begin();
		iter!=tagNodes.end();++iter)
		{
		DWORD tagNodeCode = iter->name;
		tagNodeName = _FourCC2Str(iter->name);
		m_StructList.AddString(tagNodeName.c_str());
		}*/

		for(UINT i=0; i<tagNodes.size(); i++)
		{
			DWORD tagNodeNameCode = tagNodes[i].name;
			m_StructList.AddString(_FourCC2Str(tagNodeNameCode).c_str());
		}

		bRet = true;
	}

	return bRet;
}


//void CEffectAttacherView::OnLbnDblclkTrackList()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	int currentSelectedIndex = m_TrackList.GetCurSel();
//	CString	sellText;
//	m_TrackList.GetText(currentSelectedIndex,sellText);
//	
//	AniCtrl *pAni=Cool3DModels::Inst()->GetSceneNodeAni();
//		pAni->OpenTrack(sellText,EAni_Loop);
//}

void CEffectAttacherView::OnMouseMove(UINT nFlags, CPoint _point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bDrag)
	{
		if(m_bArcBall)
		{
			POINT point;
			point.x=_point.x;
			point.y=_point.y;

			ClientToScreen(&point);
			m_drawWnd.ScreenToClient(&point);
			CRect rc;
			m_drawWnd.GetClientRect(rc);
			if(rc.PtInRect(point))
				Cool3DModels::Inst()->GetArcBall()->OnMouseMove(point.x,point.y);
		}
		else
		{
			CPoint pt=_point;
			ClientToScreen(&pt);
			m_drawWnd.ScreenToClient(&pt);
			float x=float(pt.x-m_DragPt.x);
			float y=float(pt.y-m_DragPt.y);

			CameraEuler *pCamera=Cool3DModels::Inst()->GetCamera();
			if(nFlags&MK_CONTROL)//按住Ctrl,移动Camera
			{
				Vector3 pos(-x,y,0);
				pos*=1.5f;
				pCamera->Move(pos);
			}
			else
			{
				pCamera->SetYaw(pCamera->GetYaw()+_DegToRad(-x*0.15f));
				pCamera->SetPitch(pCamera->GetPitch()+_DegToRad(-y*0.15f));
			}

			m_DragPt=pt;
		}
	}

	CFormView::OnMouseMove(nFlags, _point);
}

void CEffectAttacherView::OnBnClickedAttachEffect()
{
	// TODO: 在此添加控件通知处理程序代码
	CChooseEffectDialog dlg;
	if(IDOK==dlg.DoModal())
	{
		if(dlg.m_EffectAddress != _T("") && dlg.m_AttachPoint != _T(""))
		{
			Cool3DModels::Inst()->AttachEffect(dlg.m_EffectAddress,dlg.m_AttachPoint);

			//更新EFFECTS ListBox
			UpdateEffectsListBox();
		}
		else
		{
			AfxMessageBox(_T("连接点和特效地址必须全部被指定~~~~~~~~"));
		}
	}
}
BOOL CEffectAttacherView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	float dist=Cool3DModels::Inst()->GetCamera()->GetDist();
	Cool3DModels::Inst()->GetCamera()->SetDist(dist+(zDelta/500.0f*dist));

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEffectAttacherView::OnFileSave(LPCTSTR szPath)
{
	EffectAttachingFile attachingFileObject;
	if( (Cool3DModels::Inst()->m_UsefulMember.m_pEntity!=NULL) && (Cool3DModels::Inst()->m_UsefulMember.m_EntityPath.c_str()!=NULL) )
	{
		list<Cool3DModels::EffectProperty>::iterator  effectPropertyIterator = Cool3DModels::Inst()->m_UsefulMember.m_EffectList.begin();
		if(!attachingFileObject.ClearFileContent())
			AfxMessageBox(_T("不能清空要保存到文件的内容！"));
		for(effectPropertyIterator; effectPropertyIterator!=Cool3DModels::Inst()->m_UsefulMember.m_EffectList.end(); effectPropertyIterator++)
		{
			attachingFileObject.AddContent(&(effectPropertyIterator->m_AttachPointName),
				&(effectPropertyIterator->m_EffectPath)
				);
		}

		attachingFileObject.Save( szPath );
	}
	else if( (Cool3DModels::Inst()->m_UsefulMember.m_pAvatar!=NULL) && (Cool3DModels::Inst()->m_UsefulMember.m_AvatarPath.c_str()!=NULL) )
	{
		list<Cool3DModels::EffectProperty>::iterator  effectPropertyIterator = Cool3DModels::Inst()->m_UsefulMember.m_EffectList.begin();
		if(!attachingFileObject.ClearFileContent())
			AfxMessageBox(_T("不能清空要保存到文件的内容！"));
		for(effectPropertyIterator; effectPropertyIterator!=Cool3DModels::Inst()->m_UsefulMember.m_EffectList.end(); effectPropertyIterator++)
		{
			attachingFileObject.AddContent(&(effectPropertyIterator->m_AttachPointName),&(effectPropertyIterator->m_EffectPath));
		}

		attachingFileObject.Save( szPath );
	}
	else
	{
		AfxMessageBox(_T("没有加载模型文件前不能保存!"));
	}
}


void CEffectAttacherView::OnBnClickedAttacheffectthroughfile()
{ 
	TCHAR szFilters[]= _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE,_T("csv"), _T("*.csv"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						szFilters,this,0);
	if(IDOK==dlg.DoModal())
	{
		UpdateData();

		if(Cool3DModels::Inst()->m_UsefulMember.m_pEntity != NULL)
		{
			//开始添加
			tstring  effectAttachingFileContent;
			EffectAttachingFile effectAttachingFileObject;

			effectAttachingFileContent = effectAttachingFileObject.Load( dlg.GetFileName() );
			if( effectAttachingFileContent.empty() )
			{
				AfxMessageBox(_T("文件中未指定任何特效"));
			}
			else
			{
				if(Cool3DModels::Inst()->AttachEffectThroughFileContent(&effectAttachingFileContent))
				{
					//更新EFFECTS ListBox
					UpdateEffectsListBox();
				}
			}
		}
		else if( Cool3DModels::Inst()->m_UsefulMember.m_pAvatar != NULL )
		{
			//开始添加
			tstring  effectAttachingFileContent;
			EffectAttachingFile effectAttachingFileObject;

			effectAttachingFileContent = effectAttachingFileObject.Load( dlg.GetFileName() );
			if( effectAttachingFileContent.empty() )
			{
				AfxMessageBox(_T("文件中未指定任何特效"));
			}
			else
			{
				if(Cool3DModels::Inst()->AttachEffectThroughFileContent(&effectAttachingFileContent))
				{
					//更新EFFECTS ListBox
					UpdateEffectsListBox();
				}
			}
		}

		UpdateData(FALSE);
	}
}

bool CEffectAttacherView::UpdateEffectsListBox(void)
{
	m_Effects_ListBox.ResetContent();

	int lastSellIndex = 0;
	list<Cool3DModels::EffectProperty>::iterator effectListIterator = Cool3DModels::Inst()->m_UsefulMember.m_EffectList.begin();
	for(effectListIterator; effectListIterator!=Cool3DModels::Inst()->m_UsefulMember.m_EffectList.end(); effectListIterator++)
	{
		lastSellIndex = m_Effects_ListBox.AddString(effectListIterator->m_EffectPath.c_str());
	}
	if(lastSellIndex!=0)
		m_Effects_ListBox.SetSel(lastSellIndex);

	return false;
}

void CEffectAttacherView::OnBnClickedDeleteeffect()
{
	CString	currentSellText;
	int currentSellIndex = m_Effects_ListBox.GetCurSel();
	if(currentSellIndex != LB_ERR)
	{
		m_Effects_ListBox.GetText(currentSellIndex,currentSellText);
		Cool3DModels::Inst()->DetachEffect((LPCTSTR)currentSellText);
		UpdateEffectsListBox();
	}
}

void CEffectAttacherView::OnBnClickedPlaytrack()
{
	// TODO: 在此添加控件通知处理程序代码
	int currentSelectedIndex = m_TrackList.GetCurSel();
	CString	sellText;
	m_TrackList.GetText(currentSelectedIndex,sellText);

	AnimationCtrl *pAni=Cool3DModels::Inst()->GetSceneNodeAni();
	pAni->OpenTrack(sellText,true);
}

void CEffectAttacherView::OnBnClickedSaveeffecttofile()
{
	// TODO: 在此添加控件通知处理程序代码

	TCHAR szFilters[]= _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE,_T("csv"), _T("*.csv"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilters,this,0);
	if(IDOK==dlg.DoModal())
	{
		OnFileSave( dlg.GetFileName() );
	}
}

void CEffectAttacherView::OnLbnSelchangeStructList()
{
	CString	currentSellText;
	int currentSellIndex = m_StructList.GetCurSel();
	if(currentSellIndex != LB_ERR)
	{
		m_StructList.GetText(currentSellIndex,currentSellText);
		Cool3DModels::Inst()->SetSelectedNode((LPCTSTR)currentSellText);
	}
}
