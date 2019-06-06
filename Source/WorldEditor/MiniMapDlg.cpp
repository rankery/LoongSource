// MiniMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "WorldEditorMgr.h"
#include ".\MiniMapDlg.h"
#include ".\pluginmgr.h"
#include "EditorCamera.h"
#include ".\editorcameractrl.h"
#include ".\PluginMgr.h"
//#include "..\WorldBase\gamesys\GameMapEditor.h"

// MiniMap 对话框

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float minH = 0.0f, maxH = 20000.0f;

IMPLEMENT_DYNAMIC(MiniMapDlg, CDialog)
MiniMapDlg::MiniMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MiniMapDlg::IDD, pParent)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_xScale = 0;
	m_zScale = 0;
	ASSERT(pParent);
	m_pParent = pParent;
	m_pTE = NULL;
	m_ClientDC = NULL;
	m_pCamera = NULL;
	m_pGameMapEditor = NULL;
}

MiniMapDlg::~MiniMapDlg()
{
}

void MiniMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MiniMapDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL MiniMapDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();
	m_pTE = WorldEditorMgr::Inst()->GetTerrain();
	ASSERT(m_pTE);
	m_pGameMapEditor = WorldEditorMgr::Inst()->GetMap();
	ASSERT(m_pGameMapEditor);
	const Heightmap* pTerrain = m_pTE->GetHeightmap();
	//加载高度图的长度
	if(pTerrain==NULL)
	{
		AfxMessageBox(_T("请先加载地形~!"));
		this->EndDialog(0);				//结束窗体
		m_pParent->PostMessage(WM_DELMINIMAP_DLG,(WPARAM)this);	//向你窗体发送消息
		return false;
	}

	m_xGrid = m_pTE->GetHMapXScale();
	m_zGrid = m_pTE->GetHMapZScale();

	//初始化摄像机
	m_pCamera = (EditorCameraCtrl*)PluginMgr::Inst()->GetCameraCtrl();
	
	//设置定时器
	SetTimer(1000111, 60000, NULL);

	m_ClientDC = new CClientDC(this);

	Update();
	return TRUE;
}

void MiniMapDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	RECT clientRect;
	GetClientRect(&clientRect);
	float xScale = (float)clientRect.right/(float)m_nWidth;
	float zScale = (float)clientRect.bottom/(float)m_nHeight;
	
	ASSERT(m_pCamera);
	
	//m_pCamera->GetCamera()->SetLookAtPos(Vector3(point.y/xScale*50, 0, point.x/zScale*50));
	m_pCamera->GetCamera()->SetLookAtPos(Vector3(point.y/xScale*m_xGrid, 0, point.x/zScale*m_zGrid));
	CDialog::OnLButtonDblClk(nFlags, point);
	Update();
	SendMessage(WM_PAINT);
}

BOOL MiniMapDlg::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void MiniMapDlg::OnTimer(UINT nIDEvent)
{
	Update();
	SendMessage(WM_PAINT);
	CDialog::OnTimer(nIDEvent);
}

void MiniMapDlg::OnPaint()
{
	Update();
	CDialog::OnPaint();
}

void MiniMapDlg::DrawCamera()
{
	//在小地图上画出摄像机当前所在位置
	ASSERT(m_pCamera);
	Vector3 position = m_pCamera->GetCamera()->GetLookat();
	RECT clientRect;
	GetClientRect(&clientRect);
	float xScale = (float)clientRect.right/(float)m_nWidth;
	float zScale = (float)clientRect.bottom/(float)m_nHeight;
	/*float x = (position.z)*xScale/50;
	float y = (position.x)*zScale/50;*/
	float x = (position.z)*xScale/m_zGrid;
	float y = (position.x)*zScale/m_xGrid;
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 0, 0));
	CBrush* brushOld = m_ClientDC->SelectObject(&brush);
	m_ClientDC->Rectangle((int)x-2, (int)y-2, (int)x+2, (int)y+2);
	m_ClientDC->SelectObject(brushOld);
	brush.DeleteObject();
}

void MiniMapDlg::DrawStaticObjOnMiniMap()
{
	//在小地图上绘制静态物
	typedef list<MapObjEdit*> EDIT;
	EDIT  pList ;
	pList=m_pGameMapEditor->GetMapObjEditList();
	EDIT::iterator pIter =pList.begin(); 
	for(; pIter != pList.end(); ++pIter)
	{
		if( (*pIter)->GetType() == MapObjEdit::StaticObj )
		{
			tagStaticMapObj* p = (tagStaticMapObj*)(*pIter)->GetStaticMapObj();
			if(p->bShowMiniMap)
			{
				const Heightmap* pTerrain = m_pTE->GetHeightmap();
				if(!pTerrain)
					return ;
				RECT clientRect;
				GetClientRect(&clientRect);
				POINT position = pTerrain->World2Tile(Vector3(p->fPos[0],p->fPos[1],p->fPos[2]));
				int x = (position.y)/m_zScale;
				int y = (position.x)/m_xScale;
				CBrush brush;
				brush.CreateSolidBrush(RGB(255, 255, 0));
				CBrush* brushOld = m_ClientDC->SelectObject(&brush);
				m_ClientDC->Rectangle((int)x-2, (int)y-2, (int)x+2, (int)y+2);
				m_ClientDC->SelectObject(brushOld);
				brush.DeleteObject();
			}
		}
	}
}

float MiniMapDlg::HeightGene(float& h)
{
	return (h-minH)/(maxH-minH);
}

void MiniMapDlg::GetRGB(float& gene, BYTE& r, BYTE& g, BYTE& b)
{
	
	float maxRed = 255, minRed = 175;
	float maxGreen = 100, minGreen = 255;
	float maxBlue = 0, minBlue = 0;
	float tempRed, tempGreen, tempBlue;

	tempRed = gene*(maxRed-minRed)+minRed;
	tempGreen = gene*(maxGreen-minGreen)+minGreen;
	tempBlue = gene*(maxBlue-minBlue)+minBlue;

	if(tempRed>=255.0f)
		r = 255;
	else 
		r = (BYTE)tempRed;			

	if(tempGreen>=255.0f)
		g = 255;
	else
		g = (BYTE)tempGreen;			

	if(tempBlue>=255.0f)
		b = 255;
	else
		b = (BYTE)tempBlue;
	
}

void MiniMapDlg::Update()
{	

	const Heightmap* pTerrain = m_pTE->GetHeightmap();
	//加载高度图的长度
	if(pTerrain==NULL)
	{
		return;
	}
	m_nWidth = (int)pTerrain->Width();
	m_nHeight = (int)pTerrain->Height();

	m_xScale = m_nWidth/256;
	m_zScale = m_nHeight/256;
	if(m_xScale<=0)
		m_xScale=1;
	else if(m_xScale>8)
		m_xScale=8;
	if(m_zScale<=0)
		m_zScale=1;
	else if(m_zScale>8)
		m_zScale=8;

	//得到显示DC
	CDC*  dc1; 
	dc1 = GetDC();
	//创建内存DC
	CDC	menhDC;
	menhDC.CreateCompatibleDC((CDC*)dc1);	
	GetClientRect(m_rect);
	CBitmap bmpFace, *pOldBmp;
	bmpFace.CreateCompatibleBitmap((CDC*)dc1,m_nWidth/m_xScale,m_nHeight/m_zScale);		//创建一个高度图范围的bmp
	pOldBmp = menhDC.SelectObject(&bmpFace);	

	CBrush brush(RGB(255,255,255));
	menhDC.FillRect(m_rect,&brush);

	int zScale = 0;
	int xScale = 0;
	
	BYTE curRed = 0, curGreen = 0, curBlue = 0;
	for(int x=0; x<m_nWidth; x+=m_xScale)
	{
		for(int z=0; z<m_nHeight; z+=m_zScale)
		{
			
			//int h = pTerrain->GetValue(x, z);
			float h = pTerrain->GetHeight(x, z);
			zScale = z/m_zScale;
			xScale = x/m_xScale;			
			Vector3 vecGround = pTerrain->Tile2World(x, z);

			if(m_pTE->IsRiverOverLayGround(vecGround))			//河流特殊处理
				menhDC.SetPixel(zScale,xScale,RGB(51,255,255));
			else 
			{
				float gene = HeightGene(h);
				GetRGB(gene, curRed, curGreen, curBlue);			
				menhDC.SetPixel(zScale,xScale,RGB(curRed,curGreen,curBlue));
			}
		}
	}
	if(dc1!=NULL)
		dc1->StretchBlt(0,0,m_rect.Width(),m_rect.Height(),&menhDC,0,0,m_nWidth/m_xScale,m_nHeight/m_zScale,SRCCOPY);
	//绘制摄像机
	DrawCamera();
	//绘制标记性静态物
	DrawStaticObjOnMiniMap();
	menhDC.SelectObject(pOldBmp);
	menhDC.DeleteDC();
}

void MiniMapDlg::OnCancel()
{
	CDialog::OnCancel();
	m_pParent->PostMessage(WM_DELMINIMAP_DLG,(WPARAM)this);
	if(m_ClientDC)
		m_ClientDC->DeleteDC();
	if(m_pTE)
		m_pTE = NULL;
	if(m_pGameMapEditor)
		m_pGameMapEditor = NULL;
	if(m_pCamera)
		m_pCamera = NULL;

	DestroyWindow();
}

void MiniMapDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete this;	
}

