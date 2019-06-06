#include "stdafx.h"
#include "PlugNavMapEditor.h"
#include "Resource.h"
#include "..\WorldBase\NavMap\NavResMgr.h"
#include "..\Cool3D\EditorSupport\BrushOp.h"
using namespace WorldBase;
using namespace Cool3D;

PlugNavMapEditor	*g_navEditor=NULL;
int g_drawSize=64;

PlugNavMapEditor::PlugNavMapEditor( void )
: m_pNavMapEngine(0)
, m_pTerrainEngine(0)
, m_pCamera(0)
, m_bRender(false)
, m_drag(0)
, m_pManualCangoMap(0)
, m_pNPCNavBMap(0)
, m_pNPCNavHMap(0)
, m_pPlayerNavHMap(0)
, m_pPlayerNavBMap(0)
, m_pPlayerRoadMap(0)
{
	m_cursorPos=Vector3(0,0,0);
	m_lastDoPt.x=m_lastDoPt.y=-1;
}

PlugNavMapEditor::~PlugNavMapEditor( void )
{
	SAFE_DELETE(m_pManualCangoMap);
	SAFE_DELETE(m_pPlayerRoadMap);
	SAFE_DELETE(m_pNPCNavBMap);
	SAFE_DELETE(m_pNPCNavHMap);
}

void PlugNavMapEditor::AttachEngineInterface( void *pInterface,void *pExInterface )
{
	m_pNavMapEngine=(NavMapEditor*)pInterface;
	if(pExInterface!=NULL)
		m_pTerrainEngine=(TerrainEditor*)pExInterface;
}

void PlugNavMapEditor::Update( bool bActive )
{
	//
}

DWORD PlugNavMapEditor::CalMapColor( int x,int y )
{
	int dwColor = Color4ub(0,0,0,0);

	if (m_pManualCangoMap==NULL)
		return dwColor;

	if( m_pManualCangoMap->GetValue(x,y) )
	{
		dwColor= Color4ub(100,0,250,0);	//绿色
	}
	else
	{
		dwColor= Color4ub(100,250,0,0);	//红色
	}

	return dwColor;
}

DWORD PlugNavMapEditor::CalNPCNavColor(int x, int y)
{
	int dwColor = Color4ub(0,0,0,0);

	if (m_pNPCNavBMap==NULL)
		return dwColor;

	if( m_pNPCNavBMap->GetValue(x,y) )
	{
		dwColor= Color4ub(100,0,250,0);	//绿色
	}
	else
	{
		dwColor= Color4ub(100,250,0,0);	//红色
	}

	return dwColor;
}

DWORD PlugNavMapEditor::CalPlayerNavColor(int x, int y)
{
	int dwColor = Color4ub(0,0,0,0);

	if (m_pPlayerNavBMap==NULL)
		return dwColor;

	if( m_pPlayerNavBMap->GetValue(x,y) )
	{
		dwColor= Color4ub(100,0,250,0);	//绿色
	}
	else
	{
		dwColor= Color4ub(100,250,0,0);	//红色
	}

	return dwColor;
}

DWORD PlugNavMapEditor::CalPlayerRoadMapColor( int x, int y )
{
	int dwColor = Color4ub(0,0,0,0);

	if (m_pPlayerRoadMap==NULL)
		return dwColor;

	if( m_pPlayerRoadMap->GetValue(x,y) )
	{
		dwColor= Color4ub(100,0,250,0);	//绿色
	}
	else
	{
		dwColor= Color4ub(100,250,0,0);	//红色
	}

	return dwColor;
}

void PlugNavMapEditor::Render( bool bActive )
{
	if(!m_bRender)
		return;

	if(m_pNavMapEngine==NULL
		|| m_pTerrainEngine==NULL
		|| m_pCamera==NULL)
		return;

	Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
	if(pHMap==NULL)
		return;

	Device()->MatLoad(EMS_Prj,m_pCamera->GetPrj());
	Device()->MatLoad(EMS_View,m_pCamera->GetView());
	Device()->FFlushMatrix(false,true,true,false);

	//--
	POINT center=pHMap->World2Tile(m_pCamera->GetLookat());
	int startx=center.x-g_drawSize/2;
	if(startx<0)
		startx=0;
	int starty=center.y-g_drawSize/2;
	if(starty<0)
		starty=0;
	int endx=center.x+g_drawSize/2;
	if(endx>(int)pHMap->Width())
		endx=(int)pHMap->Width();
	int endy=center.y+g_drawSize/2;
	if(endy>(int)pHMap->Height())
		endy=(int)pHMap->Height();

	Vector3 startPos(pHMap->GetStartPos().x,0,pHMap->GetStartPos().y);
	float tileSize=pHMap->GetXScale();

	Vert_PD	vert[5];
	DWORD red=Color4ub(100,250,0,0);
	DWORD green=Color4ub(100,0,250,0);
	DWORD blue=Color4ub(20,0,0,255);
	DWORD yellow=Color4ub(100,155,0,0);

	Matrix4 prjBias,prjBias2;
	m_pCamera->GetPrjZBias(-0.0001f,prjBias);
	m_pCamera->GetPrjZBias(-0.00012f,prjBias2);
	IDevice *pDev=Device();
	pDev->MatPush(EMS_Prj);

	//--draw cursor
	DrawCursor();

	//--draw nav grids
	if(m_mainDlg.GetCango())
	{
		DrawX::BeginMtl();
		StateBlock block;
		block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		block.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		block.SaveState(ERS_FFogEnable);		pDev->FEnableFog(false);
		block.SaveState(ERS_AntialiasedLine);	pDev->SetRenderState(ERS_AntialiasedLine,false);
		block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
		
		int x,y,i;
		for(y=starty;y<endy;y++)
		{
			for(x=startx;x<endx;x++)
			{
				vert[0].diffuse = CalMapColor(x,y);
				vert[1].diffuse = vert[0].diffuse;
				vert[2].diffuse = vert[0].diffuse;
				vert[3].diffuse = vert[0].diffuse;
				vert[4].diffuse = vert[0].diffuse;
				//--draw a nav grid
				Vector3 p(tileSize*x,pHMap->GetHeight(x,y),tileSize*y);
				p+=startPos;
				vert[0].pos=p;
				vert[1].pos=p+Vector3(tileSize,0,0);
				vert[1].pos.y=pHMap->GetSafeHeight(x+1,y);
				vert[2].pos=p+Vector3(tileSize,0,tileSize);
				vert[2].pos.y=pHMap->GetSafeHeight(x+1,y+1);
				vert[3].pos=p+Vector3(0,0,tileSize);
				vert[3].pos.y=pHMap->GetSafeHeight(x,y+1);
				pDev->MatLoad(EMS_Prj,&prjBias);
				pDev->FFlushMatrix(false,false,true,false);
				DrawX::Draw3DQuad(vert);

				//--out line
				for(i=0;i<5;i++)
					vert[i].diffuse=yellow;
				vert[4].pos=vert[0].pos;
				pDev->MatLoad(EMS_Prj,&prjBias2);
				pDev->FFlushMatrix(false,false,true,false);
				pDev->DrawMem(EDraw_LineStrip,4,vert,sizeof(Vert_PD));
			}
		}		
		block.Apply();
		DrawX::EndMtl();
	}
	//NPC格子导航图
	if(m_mainDlg.GetNpcNav())
	{
		if( m_pNPCNavBMap==NULL || m_pNPCNavHMap==NULL )
			return;

		DrawX::BeginMtl();
		StateBlock block;
		block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		block.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		block.SaveState(ERS_FFogEnable);		pDev->FEnableFog(false);
		block.SaveState(ERS_AntialiasedLine);	pDev->SetRenderState(ERS_AntialiasedLine,false);
		block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
	
		int x,y,i;
		for(y=starty;y<endy;y++)
		{
			for(x=startx;x<endx;x++)
			{
				vert[0].diffuse = CalNPCNavColor(x,y);
				vert[1].diffuse = vert[0].diffuse;
				vert[2].diffuse = vert[0].diffuse;
				vert[3].diffuse = vert[0].diffuse;
				vert[4].diffuse = vert[0].diffuse;
				//--draw a nav grid
				Vector3 p(tileSize*x,m_pNPCNavHMap->GetHeight(x,y),tileSize*y);
				p+=startPos;
				vert[0].pos=p;
				vert[1].pos=p+Vector3(tileSize,0,0);
				vert[1].pos.y=m_pNPCNavHMap->GetSafeHeight(x+1,y);
				vert[2].pos=p+Vector3(tileSize,0,tileSize);
				vert[2].pos.y=m_pNPCNavHMap->GetSafeHeight(x+1,y+1);
				vert[3].pos=p+Vector3(0,0,tileSize);
				vert[3].pos.y=m_pNPCNavHMap->GetSafeHeight(x,y+1);
				pDev->MatLoad(EMS_Prj,&prjBias);
				pDev->FFlushMatrix(false,false,true,false);
				DrawX::Draw3DQuad(vert);

				//--out line
				for(i=0;i<5;i++)
					vert[i].diffuse=yellow;
				vert[4].pos=vert[0].pos;
				pDev->MatLoad(EMS_Prj,&prjBias2);
				pDev->FFlushMatrix(false,false,true,false);
				pDev->DrawMem(EDraw_LineStrip,4,vert,sizeof(Vert_PD));
			}
		}	// end for	

		block.Apply();
		DrawX::EndMtl();
	}

	//玩家格子导航图
	if(m_mainDlg.GetPlayerNav())
	{
		if( m_pPlayerNavBMap==NULL || m_pPlayerNavHMap==NULL )
			return;

		DrawX::BeginMtl();
		StateBlock block;
		block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		block.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		block.SaveState(ERS_FFogEnable);		pDev->FEnableFog(false);
		block.SaveState(ERS_AntialiasedLine);	pDev->SetRenderState(ERS_AntialiasedLine,false);
		block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);

		int x,y,i;
		for(y=starty;y<endy;y++)
		{
			for(x=startx;x<endx;x++)
			{
				vert[0].diffuse = CalPlayerNavColor(x,y);
				vert[1].diffuse = vert[0].diffuse;
				vert[2].diffuse = vert[0].diffuse;
				vert[3].diffuse = vert[0].diffuse;
				vert[4].diffuse = vert[0].diffuse;
				//--draw a nav grid
				Vector3 p(tileSize*x,m_pPlayerNavHMap->GetHeight(x,y),tileSize*y);
				p+=startPos;
				vert[0].pos=p;
				vert[1].pos=p+Vector3(tileSize,0,0);
				vert[1].pos.y=m_pPlayerNavHMap->GetSafeHeight(x+1,y);
				vert[2].pos=p+Vector3(tileSize,0,tileSize);
				vert[2].pos.y=m_pPlayerNavHMap->GetSafeHeight(x+1,y+1);
				vert[3].pos=p+Vector3(0,0,tileSize);
				vert[3].pos.y=m_pPlayerNavHMap->GetSafeHeight(x,y+1);
				pDev->MatLoad(EMS_Prj,&prjBias);
				pDev->FFlushMatrix(false,false,true,false);
				DrawX::Draw3DQuad(vert);

				//--out line
				for(i=0;i<5;i++)
					vert[i].diffuse=yellow;
				vert[4].pos=vert[0].pos;
				pDev->MatLoad(EMS_Prj,&prjBias2);
				pDev->FFlushMatrix(false,false,true,false);
				pDev->DrawMem(EDraw_LineStrip,4,vert,sizeof(Vert_PD));
			}
		}	// end for	

		block.Apply();
		DrawX::EndMtl();
	}

	//手工编辑的玩家寻路道路区域
	if(m_mainDlg.GetPlayerRoad())
	{
		if( m_pPlayerRoadMap==NULL || m_pPlayerNavHMap==NULL )
			return;

		DrawX::BeginMtl();
		StateBlock block;
		block.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
		block.SaveState(ERS_SrcBlend);
		block.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		block.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		block.SaveState(ERS_FFogEnable);		pDev->FEnableFog(false);
		block.SaveState(ERS_AntialiasedLine);	pDev->SetRenderState(ERS_AntialiasedLine,false);
		block.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);

		int x,y,i;
		for(y=starty;y<endy;y++)
		{
			for(x=startx;x<endx;x++)
			{
				vert[0].diffuse = CalPlayerRoadMapColor(x,y);
				vert[1].diffuse = vert[0].diffuse;
				vert[2].diffuse = vert[0].diffuse;
				vert[3].diffuse = vert[0].diffuse;
				vert[4].diffuse = vert[0].diffuse;
				//--draw a nav grid
				Vector3 p(tileSize*x,m_pPlayerNavHMap->GetHeight(x,y),tileSize*y);
				p+=startPos;
				vert[0].pos=p;
				vert[1].pos=p+Vector3(tileSize,0,0);
				vert[1].pos.y=m_pPlayerNavHMap->GetSafeHeight(x+1,y);
				vert[2].pos=p+Vector3(tileSize,0,tileSize);
				vert[2].pos.y=m_pPlayerNavHMap->GetSafeHeight(x+1,y+1);
				vert[3].pos=p+Vector3(0,0,tileSize);
				vert[3].pos.y=m_pPlayerNavHMap->GetSafeHeight(x,y+1);
				pDev->MatLoad(EMS_Prj,&prjBias);
				pDev->FFlushMatrix(false,false,true,false);
				DrawX::Draw3DQuad(vert);

				//--out line
				for(i=0;i<5;i++)
					vert[i].diffuse=yellow;
				vert[4].pos=vert[0].pos;
				pDev->MatLoad(EMS_Prj,&prjBias2);
				pDev->FFlushMatrix(false,false,true,false);
				pDev->DrawMem(EDraw_LineStrip,4,vert,sizeof(Vert_PD));
			}
		}	// end for	

		block.Apply();
		DrawX::EndMtl();
	}

	pDev->MatPop(EMS_Prj);
	pDev->FFlushMatrix();

	//--画导航点
	if(m_mainDlg.GetNPCWayPoint())
	{
		DrawWayPoints();
	}
}

const TCHAR* PlugNavMapEditor::GetUserName()
{
	return _T("导航图编辑器");
}

const TCHAR* PlugNavMapEditor::GetClassName()
{
	return _T("NavMapEditor");
}

static HINSTANCE g_hLastInst=NULL;
void BeginMFCRes()
{
	g_hLastInst=AfxGetResourceHandle();

	HINSTANCE hMyInst=GetModuleHandle(_T("NavMapEditor.dlp"));
	AfxSetResourceHandle(hMyInst);
}
void EndMFCRes()
{
	ASSERT(g_hLastInst!=NULL);
	AfxSetResourceHandle(g_hLastInst);
	g_hLastInst=NULL;
}

void PlugNavMapEditor::Create( DWORD pParentWnd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BeginMFCRes();

	m_mainDlg.Create(IDD_MAINDLG);

	EndMFCRes();
}

void PlugNavMapEditor::Destroy()
{
	m_pNavMapEngine=NULL;
	m_pTerrainEngine=NULL;
}

void PlugNavMapEditor::ShowWindow( bool bShow )
{
	if(bShow)
	{
		m_mainDlg.ShowWindow(SW_SHOW);
		m_bRender=true;
	}
	else
	{
		m_mainDlg.ShowWindow(SW_HIDE);
		m_bRender=false;
	}

	if(m_pTerrainEngine!=NULL
		&& m_pManualCangoMap==NULL)
	{
		Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
		if(pHMap!=NULL)
		{
			m_pManualCangoMap=new BitMap;
			m_pManualCangoMap->Create(pHMap->Width(),pHMap->Height(),true);
		}			
	}

	if(m_pTerrainEngine!=NULL
		&& m_pPlayerRoadMap==NULL)
	{
		Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
		if(pHMap!=NULL)
		{
			m_pPlayerRoadMap=new BitMap;
			m_pPlayerRoadMap->Create(pHMap->Width(),pHMap->Height(),false);
		}			
	}
}

bool PlugNavMapEditor::Build( const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI )
{
	NavResMgr::Inst()->EnableIOAsync(false);
	ResMgr::Inst()->EnableIOAsync(false);
	bool bRet = _Build(szMapName,szSaveBasePath,dwItems,options,pUI);
	ResMgr::Inst()->EnableIOAsync(true);

	return bRet;
}

void PlugNavMapEditor::OnLButtonDown( UINT nFlags, UINT x,UINT y )
{
	m_drag=1;

	if(m_mainDlg.IsEditCango())
		DoEditCango(-1);
	else if(m_mainDlg.IsEditPlayerRoad())
		DoEditPlayerNav(1);
}

void PlugNavMapEditor::OnLButtonUp( UINT nFlags, UINT x,UINT y )
{
	m_drag=0;
}

void PlugNavMapEditor::OnMouseMove( UINT nFlags, UINT x,UINT y )
{
	if(m_mainDlg.IsEditCango())
	{
		if(m_pTerrainEngine==NULL)
			return;

		Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
		if(pHMap==NULL)
			return;

		Ray mRay;
		m_pCamera->GetMouseRay(x,y,mRay);
		if(!pHMap->RayCollision(mRay,m_cursorPos))
			return;

		//--show info
		POINT pt=pHMap->World2Tile(m_cursorPos);

		//--
		int brushVal=0;
		switch(m_drag)
		{
		case 0:
			break;
		case 1://左键拖动
			brushVal=-1;
			break;
		case 2://右键拖动
			brushVal=1;
			break;
		}
		if(brushVal)
		{
			const UINT miniDist=38;
			int cx=x-m_lastDoPt.x;
			int cy=y-m_lastDoPt.y;
			if(cx*cx+cy*cy>miniDist*miniDist)
			{
				DoEditCango(brushVal);
			}
		}
	}
	else if(m_mainDlg.IsEditPlayerRoad())
	{
		if(m_pPlayerNavHMap==NULL)
			return;

		Ray mRay;
		m_pCamera->GetMouseRay(x,y,mRay);
		if(!m_pPlayerNavHMap->RayCollision(mRay,m_cursorPos))
			return;

		//--show info
		POINT pt=m_pPlayerNavHMap->World2Tile(m_cursorPos);

		//--
		int brushVal=0;
		switch(m_drag)
		{
		case 0:
			break;
		case 1://左键拖动
			brushVal=1;
			break;
		case 2://右键拖动
			brushVal=-1;
			break;
		}
		if(brushVal)
		{
			const UINT miniDist=38;
			int cx=x-m_lastDoPt.x;
			int cy=y-m_lastDoPt.y;
			if(cx*cx+cy*cy>miniDist*miniDist)
			{
				DoEditPlayerNav(brushVal);
			}
		}
	}
}

void PlugNavMapEditor::OnMouseWheel( UINT nFlags, short zDelta, UINT x,UINT y )
{
	//
}

void PlugNavMapEditor::OnRButtonDown( UINT nFlags, UINT x,UINT y )
{
	m_drag=2;

	if(m_mainDlg.IsEditCango())
		DoEditCango(1);
	else if(m_mainDlg.IsEditPlayerRoad())
		DoEditPlayerNav(-1);

}

void PlugNavMapEditor::OnRButtonUp( UINT nFlags, UINT x,UINT y )
{
	m_drag=0;
}

void PlugNavMapEditor::DoEditCango( int inv )
{
	if(m_pTerrainEngine==NULL)
		return;
	
	Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
	if(pHMap==NULL)
		return;

	POINT tPt=pHMap->World2Tile(m_cursorPos);
	tagTEditBrush brush;
	brush.centerX=tPt.x;
	brush.centerZ=tPt.y;
	m_mainDlg.GetBrushRadius(brush.inner,brush.outer);
	brush.strength=1;
	brush.strength*=inv;
	brush.lerp=ELerp_Linear;
	brush.shape=(EShapeType)m_mainDlg.GetBrushShape();

	Detail::BOP_ApplyVal applyVal(brush.strength,NULL);
	DoBrush(m_pManualCangoMap,brush,applyVal);
}


void PlugNavMapEditor::DoEditPlayerNav( int inv )
{
	if(m_pPlayerNavHMap==NULL)
		return;

	POINT tPt=m_pPlayerNavHMap->World2Tile(m_cursorPos);
	tagTEditBrush brush;
	brush.centerX=tPt.x;
	brush.centerZ=tPt.y;
	m_mainDlg.GetBrushRadius(brush.inner,brush.outer);
	brush.strength=1;
	brush.strength*=inv;
	brush.lerp=ELerp_Linear;
	brush.shape=(EShapeType)m_mainDlg.GetBrushShape();

	Detail::BOP_ApplyVal applyVal(brush.strength,NULL);
	DoBrush(m_pPlayerRoadMap,brush,applyVal);
}

bool PlugNavMapEditor::_Build( const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,tagBuildOptions& options,IUICallBack* pUI )
{
	if(m_pNavMapEngine==NULL)
		return false;

	//--
	if(m_pTerrainEngine!=NULL
		&& m_pManualCangoMap==NULL)
	{
		Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
		if(pHMap!=NULL)
		{
			m_pManualCangoMap=new BitMap;
			m_pManualCangoMap->Create(pHMap->Width(),pHMap->Height(),true);
		}			
	}
	if(m_pTerrainEngine!=NULL
		&& m_pPlayerRoadMap==NULL)
	{
		Heightmap *pHMap=m_pTerrainEngine->GetHeightmapVar();
		if(pHMap!=NULL)
		{
			m_pPlayerRoadMap=new BitMap;
			m_pPlayerRoadMap->Create(pHMap->Width(),pHMap->Height(),false);
		}			
	}

	::CreateDirectory(szSaveBasePath,NULL);
	TCHAR szPath[512];
	_stprintf(szPath,_T("%s\\%s"),szSaveBasePath,szMapName);
	::CreateDirectory(szPath,NULL);
	_stprintf(szPath,_T("%s\\%s\\Nav\\"),szSaveBasePath,szMapName);
	::CreateDirectory(szPath,NULL);

	_stprintf(szPath,_T("%s\\%s\\"),szSaveBasePath,szMapName);
	if(dwItems&EBI_Nav)
	{
		NavMapEditor::tagBuildOptions& opt=m_pNavMapEngine->GetBuildOptions();
		opt.bColliderData=options.bColliderData;
		opt.bNPCNavMap=options.bNPCNavMap;
		opt.bNPCNavGraph=options.bNPCNavGraph;
		opt.bPlayerNavMap=options.bPlayerNavMap;
		opt.fNPCPassHeight=options.fNPCPassHeight;
		opt.fNPCNavGraphPassHeight=options.fNPCNavGraphPassHeight;

		m_pNavMapEngine->BuildSetManualCangoMap(m_pManualCangoMap);
		m_pNavMapEngine->BuildSetPlayerRoadMap(m_pPlayerRoadMap);
		if(!m_pNavMapEngine->Build(szPath,szMapName,pUI))
			return false;
	}

	return true;
}

void PlugNavMapEditor::DrawCursor()
{
	//--draw cursor
	int inner=0,outer=0;
	m_mainDlg.GetBrushRadius(inner,outer);

	EShapeType shape=(EShapeType)m_mainDlg.GetBrushShape();
	if(shape==EShape_Quad)			// 绘制方形笔刷
	{
		POINT cursorPos={
			(LONG)(m_cursorPos.x/m_pTerrainEngine->GetHMapXScale()),
			(LONG)(m_cursorPos.z/m_pTerrainEngine->GetHMapZScale())
		};
		DrawGroundQuad(cursorPos.x-inner,cursorPos.x+inner,
			cursorPos.y-inner,cursorPos.y+inner,0xFFFFFF00);
		DrawGroundQuad(cursorPos.x-outer,cursorPos.x+outer,
			cursorPos.y-outer,cursorPos.y+outer,0xFFFFFF00);
	}
	else if(shape== EShape_Circle)		// 绘制圆形笔刷
	{
		DrawGroundCircle(m_cursorPos.x,m_cursorPos.z,inner);
		DrawGroundCircle(m_cursorPos.x,m_cursorPos.z,outer);
	}
	else if(shape== EShape_Line)		// 绘制线形笔刷
	{
		POINT cursorPos={
			(LONG)(m_cursorPos.x/m_pTerrainEngine->GetHMapXScale()),
			(LONG)(m_cursorPos.z/m_pTerrainEngine->GetHMapZScale())
		};			
		DrawGroundLine(cursorPos.x,cursorPos.y,inner,(float)outer);
	}
	else if(shape== EShape_Plane)		// 绘制平滑型笔刷
	{
		POINT cursorPos={
			(LONG)(m_cursorPos.x/m_pTerrainEngine->GetHMapXScale()),
			(LONG)(m_cursorPos.z/m_pTerrainEngine->GetHMapZScale())
		};
		DrawGroundQuad(cursorPos.x-inner,cursorPos.x+inner,
			cursorPos.y-inner,cursorPos.y+inner,0xFFFFFF00);
	}
}

void PlugNavMapEditor::DrawGroundCircle( float centreX,float centreZ,UINT r )
{
	if(m_pTerrainEngine==NULL)
		return;

	Heightmap *pHMap=NULL;
	if(m_mainDlg.GetCango())
		pHMap=m_pTerrainEngine->GetHeightmapVar();
	else if(m_mainDlg.GetPlayerRoad())
		pHMap=m_pPlayerNavHMap;
	
	if(pHMap==NULL)
		return;

	DWORD color=0xFFFFFF00;
	float radius=r*pHMap->GetXScale();
	vector<Vert_PD> vertBuf;

	Vert_PD v;
	v.diffuse=color;
	POINT tilePt;

	float offset=8.0f;
	Vert_PD startPt;
	startPt.diffuse=color;
	startPt.pos.x=centreX+radius;
	startPt.pos.z=centreZ;
	tilePt=pHMap->World2Tile(startPt.pos);
	startPt.pos.y=pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
	vertBuf.push_back(startPt);

	int numSplit=max(36,r*r);
	for(int i=1;i<numSplit;i++)
	{
		float a=(FLOAT_PI*2/numSplit)*i;
		v.pos.x=centreX+cosf(a)*radius;
		v.pos.z=centreZ+sinf(a)*radius;
		tilePt=pHMap->World2Tile(v.pos);
		v.pos.y=pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
		vertBuf.push_back(v);
	}
	vertBuf.push_back(startPt);

	m_pTerrainEngine->DrawLineStrip(vertBuf);
}

void PlugNavMapEditor::DrawGroundQuad( int left,int right,int top,int bottom,DWORD color )
{
	if(m_pTerrainEngine==NULL)
		return;

	Heightmap *pHMap=NULL;
	if(m_mainDlg.GetCango())
		pHMap=m_pTerrainEngine->GetHeightmapVar();
	else if(m_mainDlg.GetPlayerRoad())
		pHMap=m_pPlayerNavHMap;

	if(pHMap==NULL)
		return;

	if(left<0)
		left=0;
	if(top<0)
		top=0;
	if(right>(int)(pHMap->Width()-1))
		right=(int)(pHMap->Width()-1);
	if(bottom>(int)(pHMap->Height()-1))
		bottom=(int)(pHMap->Height()-1);

	vector<Vert_PD> vertBuf;

	Vert_PD v;
	v.diffuse=color;

	UINT x=left;
	UINT y;
	//--left
	float offset=8.0f;
	for(y=top;y<(UINT)bottom;y++)
	{
		v.pos=pHMap->Tile2World(x,y);
		v.pos.y+=offset;
		vertBuf.push_back(v);
	}

	//--bottom
	y=bottom;
	for(x=left;x<(UINT)right;x++)
	{
		v.pos=pHMap->Tile2World(x,y);
		v.pos.y+=offset;
		vertBuf.push_back(v);
	}
	//--right
	x=right;
	for(y=bottom;y>(UINT)top;y--)
	{
		v.pos=pHMap->Tile2World(x,y);
		v.pos.y+=offset;
		vertBuf.push_back(v);
	}

	//--top
	y=top;
	for(x=right;x>(UINT)left;x--)
	{
		v.pos=pHMap->Tile2World(x,y);
		v.pos.y+=offset;
		vertBuf.push_back(v);
	}

	m_pTerrainEngine->DrawLineStrip(vertBuf);
}

void PlugNavMapEditor::DrawGroundLine( int sx,int sy,int length,float angle/*=0.0f*/,DWORD color/*=0xFFFFFF00*/ )
{
	if(m_pTerrainEngine==NULL)
		return;

	Heightmap *pHMap=NULL;
	if(m_mainDlg.GetCango())
		pHMap=m_pTerrainEngine->GetHeightmapVar();
	else if(m_mainDlg.GetPlayerRoad())
		pHMap=m_pPlayerNavHMap;

	if(pHMap==NULL)
		return;

	if(sx<0)	sx=0;		// 当线段的起始点小于零时取0
	if(sy<0)	sy=0;

	vector<Vert_PD> vertBuf;

	Vert_PD v;
	POINT tilePt;
	v.diffuse=color;	// 取颜色

	//--Line
	float offset=8.0f;		
	for(UINT i=1;i<(UINT)length;i++)
	{			
		v.pos.x = (sx + cosf(_DegToRad(angle))*i)*pHMap->GetXScale();
		v.pos.z = (sy + sinf(_DegToRad(angle))*i)*pHMap->GetXScale();
		tilePt=pHMap->World2Tile(v.pos);
		v.pos.y = pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
		vertBuf.push_back(v);
	}

	m_pTerrainEngine->DrawLineStrip(vertBuf);
}

void PlugNavMapEditor::DrawGroundLine( int sx,int sy,int dx,int dy,DWORD color/*=0xFFFFFF00*/ )
{
	if(m_pTerrainEngine==NULL)
		return;

	Heightmap *pHMap=NULL;
	if(m_mainDlg.GetCango())
		pHMap=m_pTerrainEngine->GetHeightmapVar();
	else if(m_mainDlg.GetPlayerRoad())
		pHMap=m_pPlayerNavHMap;

	if(pHMap==NULL)
		return;

	if(sx<0)	sx=0;		// 当线段的起始点小于零时取0
	if(sy<0)	sy=0;
	if(dx<0)	dx=0;		// 当线段的终止点小于零时取0
	if(dy<0)	dy=0;
	if(sx>(INT)(pHMap->Width()-1))
		sx=(INT)(pHMap->Width()-1);
	if(sy>(INT)(pHMap->Height()-1))
		sy=(INT)(pHMap->Height()-1);
	if(dx>(INT)(pHMap->Width()-1))
		dx=(INT)(pHMap->Width()-1);
	if(dy>(INT)(pHMap->Height()-1))
		dy=(INT)(pHMap->Height()-1);

	float length = sqrt((float)((float)(dx-sx)*(dx-sx) + (float)(dy-sy)*(dy-sy)));	//线段的长度
	float angle;											// 线段的斜率
	if(dx==sx)
	{
		if (dy > sy)
		{
			angle = _DegToRad(90.0f);
		}
		else			
			angle = _DegToRad(270.0f);
	}
	else if(dx > sx)
		angle =	atan(((float)(dy-sy))/((float)(dx-sx)));
	else
		angle =	3.1415926f + atan(((float)(dy-sy))/((float)(dx-sx)));


	vector<Vert_PD> vertBuf;

	Vert_PD v;
	POINT tilePt;
	v.diffuse=color;	// 取颜色

	//--Line
	float offset=8.0f;

	v.pos.x = sx*pHMap->GetXScale();			// 第一个点
	v.pos.z = sy*pHMap->GetZScale();
	tilePt=pHMap->World2Tile(v.pos);
	v.pos.y = pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
	vertBuf.push_back(v);

	for(UINT i=1;i<= length;i++)			// <=直到最后一点
	{			
		v.pos.x = (sx + cosf(/*_DegToRad*/(angle))*i)*pHMap->GetXScale();
		v.pos.z = (sy + sinf(/*_DegToRad*/(angle))*i)*pHMap->GetXScale();

		tilePt=pHMap->World2Tile(v.pos);
		v.pos.y = pHMap->GetSafeHeight(tilePt.x,tilePt.y) + offset;
		vertBuf.push_back(v);
	}

	m_pTerrainEngine->DrawLineStrip(vertBuf);
}



bool PlugNavMapEditor::Save()
{
	if(m_pTerrainEngine==NULL
		|| m_pManualCangoMap==NULL
		|| m_pPlayerRoadMap==NULL)
		return false;

	return m_pNavMapEngine->Save(m_pTerrainEngine->GetName(),m_pManualCangoMap,m_pPlayerRoadMap);
}

bool PlugNavMapEditor::Load()
{
	if(m_pTerrainEngine==NULL
		|| m_pManualCangoMap==NULL
		|| m_pPlayerRoadMap==NULL)
		return false;

	return m_pNavMapEngine->Load(m_pTerrainEngine->GetName(),m_pManualCangoMap,m_pPlayerRoadMap);
}

bool PlugNavMapEditor::LoadNPCNavMap()
{
	if(m_pNPCNavBMap)
		SAFE_DELETE(m_pNPCNavBMap);
	if(m_pNPCNavHMap)
		SAFE_DELETE(m_pNPCNavHMap);
	m_pNPCNavBMap = new BitMap;
	m_pNPCNavHMap = new Heightmap;
	return m_pNavMapEngine->LoadNPCNavMap(m_pTerrainEngine->GetName(), m_pNPCNavBMap, m_pNPCNavHMap);
}

bool PlugNavMapEditor::LoadNPCNavGraph()
{
	m_npcNavGraph.LoadFromFile(m_pTerrainEngine->GetName(),Kernel::Inst()->DefaultFS());
	return true;
}

bool PlugNavMapEditor::LoadPlayerNavMap()
{
	if(m_pPlayerNavBMap)
		SAFE_DELETE(m_pPlayerNavBMap);
	if(m_pPlayerNavHMap)
		SAFE_DELETE(m_pPlayerNavHMap);
	m_pPlayerNavBMap = new BitMap;
	m_pPlayerNavHMap = new Heightmap;
	return m_pNavMapEngine->LoadPlayerNavMap(m_pTerrainEngine->GetName(), m_pPlayerNavBMap, m_pPlayerNavHMap);
}

void PlugNavMapEditor::DisplayNPCCangoMap()
{
	TCHAR szPath[512];
	_stprintf(szPath, _T("%s\\%s\\npccango.bmp"), _T(".\\Data\\NavEditor"), m_pTerrainEngine->GetName());

	::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
	ShellExecute(NULL, _T("open"), _T("mspaint.exe"), szPath, NULL, SW_SHOWMAXIMIZED);
}

void PlugNavMapEditor::DisplayPlayerCangoMap()
{
	TCHAR szPath[512];
	_stprintf(szPath, _T("%s\\%s\\playercango.bmp"), _T(".\\Data\\NavEditor"), m_pTerrainEngine->GetName());

	::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
	ShellExecute(NULL, _T("open"), _T("mspaint.exe"), szPath, NULL, SW_SHOWMAXIMIZED);
}

void PlugNavMapEditor::DisplayManuelCangoMap()
{
	TCHAR szPath[512];
	_stprintf(szPath, _T("%s\\%s\\cango.bmp"), _T(".\\Data\\NavEditor"), m_pTerrainEngine->GetName());

	::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
	ShellExecute(NULL, _T("open"), _T("mspaint.exe"), szPath, NULL, SW_SHOWMAXIMIZED);
}

void PlugNavMapEditor::DisplayRoadMap()
{
	TCHAR szPath[512];
	_stprintf(szPath, _T("%s\\%s\\road.bmp"), _T(".\\Data\\NavEditor"), m_pTerrainEngine->GetName());

	::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
	ShellExecute(NULL, _T("open"), _T("mspaint.exe"), szPath, NULL, SW_SHOWMAXIMIZED);
}

void PlugNavMapEditor::ClearCangoMap()
{
	if(m_pTerrainEngine==NULL
		|| m_pManualCangoMap==NULL)
		return;

	UINT width=m_pManualCangoMap->Width();
	UINT height=m_pManualCangoMap->Height();
	SAFE_DELETE(m_pManualCangoMap);
	m_pManualCangoMap=new BitMap;
	m_pManualCangoMap->Create(width,height,true);
}

void PlugNavMapEditor::ClearPlayerRoadMap()
{
	if(m_pTerrainEngine==NULL
		|| m_pPlayerRoadMap==NULL)
		return;

	UINT width=m_pPlayerRoadMap->Width();
	UINT height=m_pPlayerRoadMap->Height();
	SAFE_DELETE(m_pPlayerRoadMap);
	m_pPlayerRoadMap=new BitMap;
	m_pPlayerRoadMap->Create(width,height,false);
}

void PlugNavMapEditor::DrawWayPoints()
{
	const vector<NPCNavGraph::tagPathPointRT>& points=m_npcNavGraph.GetPathPoints();

	for(size_t i=0;i<points.size();++i)
	{
		const NPCNavGraph::tagPathPointRT& point=points[i];

		//--画导航点盒子
		AABBox box;
		box.min.x=point.pos.x-40.0f;
		box.min.y=point.pos.y;
		box.min.z=point.pos.z-40.0f;
		box.max.x=point.pos.x+40.0f;
		box.max.y=point.pos.y+80.0f;
		box.max.z=point.pos.z+40.0f;
		DrawX::DrawAABBox(box,0x00FFB029);

		const vector<int>& nearPoints=point.nearPoints;
		for(size_t j=0;j<nearPoints.size();++j)
		{
			const NPCNavGraph::tagPathPointRT& nearPoint=points[nearPoints[j]];

			//--画相邻导航点的连线
			Vector3 v1 = point.pos;
			Vector3 v2 = nearPoint.pos;
			v1.y += 50.0f;
			v2.y += 50.0f;
			DrawX::Draw3DLine(v1,v2,0x00FFB029);
		}
	}
}
