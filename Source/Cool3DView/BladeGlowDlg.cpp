// BladeGlowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cool3DView.h"
#include "BladeGlowDlg.h"
#include "..\Cool3D\SceneGraph\SGBladeGlow.h"
#include ".\bladeglowdlg.h"

// CBladeGlowDlg 对话框

IMPLEMENT_DYNCREATE(CBladeGlowDlg, CDHtmlDialog)

CBladeGlowDlg::CBladeGlowDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CBladeGlowDlg::IDD, CBladeGlowDlg::IDH, pParent)
	, m_fX(0)
	, m_fY(0)
	, m_fZ(0)
	, m_fRadius(0)
{
}

CBladeGlowDlg::~CBladeGlowDlg()
{
}

void CBladeGlowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstTagNodes);
	DDX_Text(pDX, IDC_EDIT_X, m_fX);
	DDX_Text(pDX, IDC_EDIT_Y, m_fY);
	DDX_Text(pDX, IDC_EDIT_Z, m_fZ);
	DDX_Text(pDX, IDC_EDIT_Radius, m_fRadius);
}

BOOL CBladeGlowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CBladeGlowDlg, CDHtmlDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Refresh, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CBladeGlowDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()

bool CBladeGlowDlg::GetTagNodeFrame(TagNodeFrame& tagNodes)
{
	SceneNode* pSGNode = Cool3DModels::Inst()->GetSceneNode();
	if( pSGNode==NULL || !pSGNode->IS_KIND_OF(EntityNode) )
		return false;

	EntityNode* pEntityNode = (EntityNode*)pSGNode;
	ResRenderable *pRes = pEntityNode->GetResObj();
	if( pRes==NULL || !pRes->IS_STATIC_CLASS(ResStaticMesh) )
		return false;

	ResStaticMesh *pStaticMesh = (ResStaticMesh *)pRes;
	if( pStaticMesh==NULL || !pStaticMesh->IsCreated() )
		return false;

	tagNodes = pStaticMesh->GetTagNodes();

	return true;
}

bool CBladeGlowDlg::GetTagNodes(vector<tagTagNode>& tagNodes)
{
	TagNodeFrame frame;
	if( !GetTagNodeFrame(frame) )
		return false;

	tagNodes = frame.nodes;
	return true;
}

bool CBladeGlowDlg::FindTagNode(DWORD name,tagTagNode& tagNode)
{
	vector<tagTagNode> tagNodes;
	if( !GetTagNodes(tagNodes) )
		return false;

	for( size_t i=0; i<tagNodes.size(); i++ )
	{
		if( tagNodes[i].name == name )
		{
			tagNode = tagNodes[i];
			return true;
		}
	}

	return false;
}

bool CBladeGlowDlg::UpdateTagNode(DWORD name,float x, float y, float z, float radius)
{
	SceneNode* pSGNode = Cool3DModels::Inst()->GetSceneNode();
	if( pSGNode==NULL || !pSGNode->IS_KIND_OF(EntityNode) )
		return false;

	EntityNode* pEntityNode = (EntityNode*)pSGNode;
	ResRenderable *pRes = pEntityNode->GetResObj();
	if( pRes==NULL || !pRes->IS_STATIC_CLASS(ResStaticMesh) )
		return false;

	ResStaticMesh *pStaticMesh = (ResStaticMesh *)pRes;
	if( pStaticMesh==NULL || !pStaticMesh->IsCreated() )
		return false;

	vector<tagTagNode>& tagNodes = (vector<tagTagNode>&)pStaticMesh->GetTagNodes().nodes;

	for( size_t i=0; i<tagNodes.size(); i++ )
	{
		if( tagNodes[i].name == name )
		{
			tagNodes[i].trans.x = x;
			tagNodes[i].trans.y = y;
			tagNodes[i].trans.z = z;
			tagNodes[i].rotat.x = radius;
			return true;
		}
	}

	return false;
}

bool CBladeGlowDlg::SaveTagNodes()
{
	SceneNode* pSGNode = Cool3DModels::Inst()->GetSceneNode();
	if( pSGNode==NULL || !pSGNode->IS_KIND_OF(EntityNode) )
		return false;

	EntityNode* pEntityNode = (EntityNode*)pSGNode;
	ResRenderable *pRes = pEntityNode->GetResObj();
	if( pRes==NULL || !pRes->IS_STATIC_CLASS(ResStaticMesh) )
		return false;

	ResStaticMesh *pStaticMesh = (ResStaticMesh *)pRes;
	if( pStaticMesh==NULL || !pStaticMesh->IsCreated() )
		return false;

	pStaticMesh->SaveTagNodes();
	return true;
}

void CBladeGlowDlg::AttachBladeGlow()
{
	SceneNode* pSGNode = Cool3DModels::Inst()->GetSceneNode();
	SGBladeGlow* pBG = new SGBladeGlow;
	pBG->LoadRes(_T("..\\data\\system\\effect\\BladeGlow\\Glow.tga"));
	pBG->Enable(true);
	pSGNode->AttachChild(pBG);
}

void CBladeGlowDlg::DetachBladeGlow()
{
	SceneNode* pSGNode = Cool3DModels::Inst()->GetSceneNode();
	if( pSGNode!=NULL && pSGNode->IS_KIND_OF(EntityNode) )
	{
		for( int i=0; i<pSGNode->GetNumChildren(); i++ )
		{
			const SceneNode* pChild = pSGNode->GetChild(i);
			if( pChild->IS_STATIC_CLASS(SGBladeGlow) )
			{
				pSGNode->DetachChild(pChild);
				delete pChild;
				break;
			}
		}
	}
}

bool CBladeGlowDlg::Init()
{
	vector<tagTagNode> tagNodes;
	if( !GetTagNodes(tagNodes) )
		return false;
	if( tagNodes.size() < 1 )
		return false;	

	//--显示挂载点列表
	m_lstTagNodes.ResetContent();
	for( size_t i=0; i<tagNodes.size(); i++ )
	{
		tstring name = _FourCC2Str(tagNodes[i].name);
		if( _tcsncmp(name.c_str(),_T("g"),1)==0 ||
			_tcsncmp(name.c_str(),_T("G"),1)==0 )
		{
			int index = m_lstTagNodes.AddString(name.c_str());
			m_lstTagNodes.SetItemData( index, tagNodes[i].name );
		}
	}
	
	//--创建光晕结点
	AttachBladeGlow();

	//--将窗口显示到顶部
	RECT r;
	GetWindowRect(&r);
	SetWindowPos(&wndTopMost,r.left,r.top,r.right-r.left,r.bottom-r.top,0);

	//--设置焦点
	m_lstTagNodes.SetFocus();

	return true;
}

void CBladeGlowDlg::OnClose()
{
	//--删除光晕结点
	DetachBladeGlow();

	//--隐藏窗口
	ShowWindow(SW_HIDE);
}

// CBladeGlowDlg 消息处理程序

HRESULT CBladeGlowDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
}

HRESULT CBladeGlowDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBladeGlowDlg::OnBnClickedRefresh()
{
	if( m_lstTagNodes.GetCount()==0 || m_lstTagNodes.GetCurSel()<0 )
		return;
	
	if( !UpdateData(TRUE) )
		return;

	//--更新数据
	DWORD name = m_lstTagNodes.GetItemData( m_lstTagNodes.GetCurSel() );
	UpdateTagNode( name, m_fX, m_fY, m_fZ, m_fRadius );

	//--重新显示光晕
	DetachBladeGlow();
	AttachBladeGlow();
}

void CBladeGlowDlg::OnBnClickedSave()
{
	if( m_lstTagNodes.GetCount()==0 )
		return;
	
	if( SaveTagNodes() )
	{
		MessageBox(_T("保存成功"));
	}
}

void CBladeGlowDlg::OnLbnSelchangeList()
{
	DWORD name = m_lstTagNodes.GetItemData( m_lstTagNodes.GetCurSel() );

	tagTagNode tagNode;
	if( FindTagNode( name, tagNode ) )
	{
		m_fX = tagNode.trans.x;
		m_fY = tagNode.trans.y;
		m_fZ = tagNode.trans.z;
		m_fRadius = tagNode.rotat.x;
		UpdateData(FALSE);
	}
}
