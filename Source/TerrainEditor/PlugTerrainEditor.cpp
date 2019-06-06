#include "StdAfx.h"
#include ".\PlugTerrainEditor.h"
#include "Resource.h"
#include "..\Cool3D\Device\StateBlock.h"

PlugTerrainEditor	*g_editor=NULL;

PlugTerrainEditor::PlugTerrainEditor(void)
{
	m_pEngine=NULL;
	m_pCamera=NULL;
	m_cursorPos=Vector3(0,0,0);
	m_cursorPosRiver=Vector3(0,0,0);
	m_lastDoPt.x=m_lastDoPt.y=-1;
	m_mouseState=MS_NONE;
 }

PlugTerrainEditor::~PlugTerrainEditor(void)
{
}

void PlugTerrainEditor::AttachEngineInterface(void *pInterface,void *pExInterface)
{
	m_pEngine=(TerrainEditor*)pInterface;
}

const TCHAR* PlugTerrainEditor::GetClassName()
{
	return _T("TerrainEditor");
}

const TCHAR* PlugTerrainEditor::GetUserName()
{
	return _T("地形编辑器");
}


static HINSTANCE g_hLastInst=NULL;
void BeginMFCRes()
{
	g_hLastInst=AfxGetResourceHandle();

	HINSTANCE hMyInst=GetModuleHandle(_T("TerrainEditor.dlp"));
	AfxSetResourceHandle(hMyInst);
}
void EndMFCRes()
{
	ASSERT(g_hLastInst!=NULL);
	AfxSetResourceHandle(g_hLastInst);
	g_hLastInst=NULL;
}

void PlugTerrainEditor::Create(DWORD pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	BeginMFCRes();
	CWnd *pPWnd=(CWnd*)pParentWnd;
	m_sheet.AddPages();
	m_sheet.Create(pPWnd,
		WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_VISIBLE
		,0);
	
	m_brushDlg.Create(IDD_BRUSH,NULL);
	m_brushDlg.ShowWindow(SW_SHOW);

	EndMFCRes();

	AlignBrushDlg();
}

void PlugTerrainEditor::AlignBrushDlg()
{
	TEditorSheet *pSheet=&m_sheet;
	if(IsWindow(pSheet->GetSafeHwnd())
		&& IsWindow(m_brushDlg.GetSafeHwnd()))
	{
		CRect rc;
		pSheet->GetWindowRect(&rc);
		CRect myRc;
		m_brushDlg.GetWindowRect(&myRc);
		UINT w=myRc.Width();
		UINT h=myRc.Height();
		myRc.left=rc.left;
		//myRc.right=myRc.left+w;
		myRc.right=myRc.left+rc.Width();
		myRc.top=rc.bottom;
		myRc.bottom=myRc.top+h;
		m_brushDlg.MoveWindow(&myRc,TRUE);
	}
}

void PlugTerrainEditor::Destroy()
{
	m_pEngine=NULL;
}

void PlugTerrainEditor::ShowWindow(bool bShow)
{
	if(bShow)
	{
		m_sheet.ShowWindow(SW_SHOW);
		m_brushDlg.ShowWindow(SW_SHOW);
	}
	else
	{
		m_sheet.ShowWindow(SW_HIDE);
		m_brushDlg.ShowWindow(SW_HIDE);
	}
}

void PlugTerrainEditor::CreateBrush(tagTEditBrush& brush,int inversVal)
{
	if(m_pEngine==NULL || !m_pEngine->IsHeightmapCreated())
		return;
	m_brushDlg.GetSetting(brush.inner,brush.outer,&brush.strength);
	brush.strength*=inversVal;
	POINT tPt=m_pEngine->World2Tile(m_cursorPos);
	brush.centerX=tPt.x;
	brush.centerZ=tPt.y;
	brush.lerp=(ELerpType)m_brushDlg.GetLerpType();
	brush.shape=(EShapeType)m_brushDlg.GetShape();
}

void PlugTerrainEditor::DoCurrentTools(tagTEditBrush& brush)
{
	if(m_pEngine==NULL || !m_pEngine->IsHeightmapCreated())
		return;

	CString subName;
	CString toolName=m_sheet.GetActiveTools(subName);
	if(toolName==_T("Brush"))
	{
		m_pEngine->ApplyBrush2HMap(brush);
	}
	else if(toolName==_T("Flatten"))
	{
		m_pEngine->FlattenHMap(brush);
	}
	else if(toolName==_T("Smooth"))
	{
		m_pEngine->SmoothHMap(brush);
	}
	else if(toolName==_T("Noise"))
	{
		float freq=0;
		m_brushDlg.GetNoiseSet(freq);
		m_pEngine->NoiseHMap(brush,freq);
	}
	else if(toolName==_T("Apply"))
	{
		m_pEngine->ApplyValue2HMap(brush);
	}
	else if(toolName==_T("layerBrush"))
	{
		m_pEngine->ApplyBrush2Layer(brush,subName);
	}
	else if(toolName==_T("layerNoise"))
	{
		float freq=0;
		m_brushDlg.GetNoiseSet(freq);
		m_pEngine->NoiseLayer(brush,freq,subName);
	}
	else if(toolName==_T("layerSmooth"))
	{
		m_pEngine->SmoothLayer(brush,subName);
	}
	else if(toolName==_T("decoBrush"))
	{
		m_pEngine->BrushDecoDensityMap(subName,brush,m_pCamera);
	}
	else if(toolName==_T("decoNoise"))
	{
		float freq=0;
		m_brushDlg.GetNoiseSet(freq);
		m_pEngine->NoiseDecoDensityMap(subName,brush,freq,m_pCamera);
	}
	else if(toolName==_T("riverBrush"))
	{
		UINT riverIndex=_tstoi(subName);
		m_pEngine->ApplyBrush2River(brush,riverIndex);
	}
	else if(toolName==_T("riverSmooth"))
	{
		UINT riverIndex=_tstoi(subName);
		m_pEngine->SmoothRiver(brush,riverIndex);
	}
	else if(toolName==_T("riverNoise"))
	{
		UINT riverIndex=_tstoi(subName);
		float freq=0;
		m_brushDlg.GetNoiseSet(freq);
		m_pEngine->NoiseRiver(brush,freq,riverIndex);
	}
}

void PlugTerrainEditor::OnLButtonDown(UINT nFlags, UINT x,UINT y)
{
	ChangeMouseState(MO_ONLDOWN);
}

void PlugTerrainEditor::OnLButtonUp(UINT nFlags, UINT x,UINT y)
{
	ChangeMouseState(MO_ONLUP);
}

void PlugTerrainEditor::OnMouseMove(UINT nFlags, UINT x,UINT y)
{
	m_cursorScrPos.x=x;
	m_cursorScrPos.y=y;

	if(m_pEngine==NULL || !m_pEngine->IsHeightmapCreated())
		return;

	m_pEngine->MousePick(x,y,m_pCamera,m_cursorPos);
	if(m_sheet.GetActiveIndex()==3)	//河流编辑
	{
		//生成m_cursorPosRiver
		UINT riverIndex;
		if(m_sheet.GetRiverPage()->GetSelRiverIndex(riverIndex))
		{
			TRiverEditor *pRE=m_pEngine->GetRiverEditor();
			if(pRE!=NULL)
			{
				pRE->MousePick(riverIndex,x,y,m_pCamera,m_cursorPosRiver);
			}
		}
	}

	if(m_pEngine->IsHeightmapCreated())
	{
		POINT tilePt=m_pEngine->World2Tile(m_cursorPos);
		float h=m_cursorPos.y;

		if(m_sheet.IsShowLayerNum())		
		{
			vector<int> layers;
			m_pEngine->GetLayers(m_cursorPos,layers);

			CString strLayers, strLayer;
			for(size_t i=0; i<layers.size(); i++)
			{
				strLayer.Format(_T("%d"), layers[i]);
				if(i<layers.size()-1)
				{
					strLayer+=_T(",");
				}
				strLayers += strLayer;
			}

			m_szHmapInfo.Format(_T("Terrain: x=%d,z=%d,高度=%.2f,图层=%d(%s)"),tilePt.x,tilePt.y,h,layers.size(),(LPCTSTR)strLayers);
		}
		else
		{
			m_szHmapInfo.Format(_T("Terrain: x=%d,z=%d,高度=%.2f"),tilePt.x,tilePt.y,h);
		}
	}

	const UINT miniDist=38;
	int cx=x-m_lastDoPt.x;
	int cy=y-m_lastDoPt.y;
	if(cx*cx+cy*cy>miniDist*miniDist)
		ChangeMouseState(MO_ONMOVE);
}

void PlugTerrainEditor::OnMouseWheel(UINT nFlags, short zDelta, UINT x,UINT y)
{
	//
}

void PlugTerrainEditor::OnRButtonDown(UINT nFlags, UINT x,UINT y)
{
	ChangeMouseState(MO_ONRDOWN);
}

void PlugTerrainEditor::OnRButtonUp(UINT nFlags, UINT x,UINT y)
{
	ChangeMouseState(MO_ONRUP);
}

void PlugTerrainEditor::Update(bool bActive)
{
	//
}

bool PlugTerrainEditor::Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,
							  tagBuildOptions& options,IUICallBack* pUI)
{
	ResMgr::Inst()->EnableIOAsync(false);
	bool bRet=_Build(szMapName,szSaveBasePath,dwItems,options,pUI);
	ResMgr::Inst()->EnableIOAsync(true);
	return bRet;
}

bool PlugTerrainEditor::_Build(const TCHAR* szMapName,const TCHAR* szSaveBasePath,DWORD dwItems,
							   tagBuildOptions& options,IUICallBack* pUI)
{
	if(m_pEngine==NULL || !m_pEngine->IsHeightmapCreated())
		return false;

	::CreateDirectory(szSaveBasePath,NULL);
	TCHAR szPathTerrain[512],szPathRiver[512];
	_stprintf(szPathTerrain,_T("%s\\%s"),szSaveBasePath,szMapName);
	::CreateDirectory(szPathTerrain,NULL);
	_stprintf(szPathTerrain,_T("%s\\%s\\Trn\\"),szSaveBasePath,szMapName);
	::CreateDirectory(szPathTerrain,NULL);

	if(dwItems&EBI_Terrain)
	{
		if(!m_pEngine->BuildTerrain(szPathTerrain,pUI))
			return false;
	}

	_stprintf(szPathRiver,_T("%s\\%s\\river\\"),szSaveBasePath,szMapName);
	::CreateDirectory(szPathRiver,NULL);

	if(dwItems&EBI_River)
	{
		if(!m_pEngine->BuildRiver(szPathRiver,pUI))
			return false;
	}

	// 把河流数据写入地形
	if( dwItems&EBI_RiverDataToTerrain )
	{
		if( !m_pEngine->WriteRiverDataToTerrainPatch(szPathTerrain,pUI) )
			return false;
	}

	return true;
}

void PlugTerrainEditor::Render(bool bActive)
{
	if(m_pEngine==NULL || !m_pEngine->IsHeightmapCreated())
		return;

	if(bActive)
	{
		m_pEngine->SetEditFocus(TerrainEditor::EEditFocus(m_sheet.GetActiveIndex()));
		//--
		IDevice *pDev=Device();
		pDev->MatLoad(EMS_Prj,m_pCamera->GetPrj());
		pDev->MatLoad(EMS_View,m_pCamera->GetView());
		pDev->FFlushMatrix();

		int inner=0,outer=0;
		m_brushDlg.GetSetting(inner,outer);

		StateBlock sb;
		sb.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(false);

		EShapeType shape=(EShapeType)m_brushDlg.GetShape();
		if(shape==EShape_Quad)
		{
			POINT cursorPos={
				m_cursorPos.x/m_pEngine->GetHMapXScale(),
				m_cursorPos.z/m_pEngine->GetHMapZScale()
			};

			if(m_sheet.GetActiveIndex()==3)	//河流编辑
			{
				UINT riverIndex;
				if(m_sheet.GetRiverPage()->GetSelRiverIndex(riverIndex))
				{
					TRiverEditor *pRE=m_pEngine->GetRiverEditor();
					if(pRE!=NULL)
					{
						Heightmap *pHMap=pRE->GetRiverHeightMapByIndex(riverIndex);
						POINT cursorPosRiver={
							m_cursorPosRiver.x/pHMap->GetXScale(),
							m_cursorPosRiver.z/pHMap->GetZScale()
						};
						pRE->DrawGroundQuad(riverIndex,cursorPosRiver.x-inner,cursorPosRiver.x+inner,
							cursorPosRiver.y-inner,cursorPosRiver.y+inner,0xFFFFFF00);
						pRE->DrawGroundQuad(riverIndex,cursorPosRiver.x-outer,cursorPosRiver.x+outer,
							cursorPosRiver.y-outer,cursorPosRiver.y+outer,0xFFFFFF00);
					}
				}
			}
			else
			{
				m_pEngine->DrawGroundQuad(cursorPos.x-inner,cursorPos.x+inner,
					cursorPos.y-inner,cursorPos.y+inner,0xFFFFFF00);
				m_pEngine->DrawGroundQuad(cursorPos.x-outer,cursorPos.x+outer,
					cursorPos.y-outer,cursorPos.y+outer,0xFFFFFF00);
				m_pEngine->DrawGroundRole(m_cursorPos.x, m_cursorPos.y, m_cursorPos.z);
			}
		}
		else
		{
			if(m_sheet.GetActiveIndex()==3)	//河流编辑
			{
				UINT riverIndex;
				if(m_sheet.GetRiverPage()->GetSelRiverIndex(riverIndex))
				{
					TRiverEditor *pRE=m_pEngine->GetRiverEditor();
					if(pRE!=NULL)
					{
						pRE->DrawGroundCircle(riverIndex,m_cursorPosRiver.x,m_cursorPosRiver.z,inner);
						pRE->DrawGroundCircle(riverIndex,m_cursorPosRiver.x,m_cursorPosRiver.z,outer);
					}
				}
			}
			else
			{
				m_pEngine->DrawGroundCircle(m_cursorPos.x,m_cursorPos.z,inner);
				m_pEngine->DrawGroundCircle(m_cursorPos.x,m_cursorPos.z,outer);
				m_pEngine->DrawGroundRole(m_cursorPos.x, m_cursorPos.y, m_cursorPos.z);
			}
		}

		sb.Apply();

		if(!m_szHmapInfo.IsEmpty())
		{
			int x=2;
			int y=pDev->GetClientHeight()-16;
			DrawX::Draw2DQuad(x,y-2,m_szHmapInfo.GetLength()*8,18,false,Color4ub(200,5,0,0));
			DrawX::DrawDebugString(m_szHmapInfo,x,y);
		}
	}
	if(m_sheet.IsShowSubPatchEdge())
		m_pEngine->DrawSubPatchEdge();
	if(m_sheet.IsShowEdge())
		m_pEngine->DrawPatchEdge();
}

void PlugTerrainEditor::ChangeMouseState(MOUSEOPERATION mouseOp)
{
	switch (mouseOp)
	{
	case MO_ONLDOWN:
		{
			switch (m_mouseState)
			{
			case MS_NONE:
				{
					m_mouseState=MS_LDOWN;
				}
				break;
			case MS_RDOWN:
				{
					m_mouseState=MS_LRDOWN;
				}
				break;
			case MS_RMOVE:
				{
					m_mouseState=MS_RMOVE_LDOWN;
				}
				break;
			}
		}
		break;
	case MO_ONLUP:
		{
			switch (m_mouseState)
			{
			case MS_LDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, 1);
					DoCurrentTools(brush);
					m_pEngine->EndBrushRecord();

					m_mouseState=MS_NONE;
				}
				break;
			case MS_LRDOWN:
				{
					m_mouseState=MS_RDOWN;
				}
				break;
			case MS_LMOVE:
				{
					m_pEngine->EndBrushRecord();
					m_mouseState=MS_NONE;
				}
				break;
			case MS_LRMOVE:
				{
					m_mouseState=MS_RDOWN;
				}
				break;
			case MS_LMOVE_RDOWN:
				{
					m_pEngine->EndBrushRecord();
					m_mouseState=MS_RDOWN;
				}
				break;
			case MS_RMOVE_LDOWN:
				{
					m_mouseState=MS_RMOVE;
				}
				break;
			}
		}
		break;
	case MO_ONRDOWN:
		{
			switch (m_mouseState)
			{
			case MS_NONE:
				{
					m_mouseState=MS_RDOWN;
				}
				break;
			case MS_LDOWN:
				{
					m_mouseState=MS_LRDOWN;
				}
				break;
			case MS_LMOVE:
				{
					m_mouseState=MS_LMOVE_RDOWN;
				}
				break;
			}
		}
		break;
	case MO_ONRUP:
		{
			switch (m_mouseState)
			{
			case MS_RDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, -1);
					DoCurrentTools(brush);
					m_pEngine->EndBrushRecord();
					m_mouseState=MS_NONE;
				}
				break;
			case MS_LRDOWN:
				{
					m_mouseState=MS_LDOWN;
				}
				break;
			case MS_RMOVE:
				{
					m_pEngine->EndBrushRecord();
					m_mouseState=MS_NONE;
				}
				break;
			case MS_LRMOVE:
				{
					m_mouseState=MS_LDOWN;
				}
				break;
			case MS_LMOVE_RDOWN:
				{
					m_mouseState=MS_LMOVE;
				}
				break;
			case MS_RMOVE_LDOWN:
				{
					m_pEngine->EndBrushRecord();
					m_mouseState=MS_LDOWN;
				}
				break;
			}
		}
		break;
	case MO_ONMOVE:
		{
			switch (m_mouseState)
			{
			case MS_LDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, 1);
					DoCurrentTools(brush);
					m_mouseState=MS_LMOVE;
				}
				break;
			case MS_RDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, -1);
					DoCurrentTools(brush);
					m_mouseState=MS_RMOVE;
				}
				break;
			case MS_LRDOWN:
				{
					m_mouseState=MS_LRMOVE;
				}
				break;
			case MS_LMOVE:
				{
					tagTEditBrush brush;
					CreateBrush(brush, 1);
					DoCurrentTools(brush);
				}
				break;
			case MS_RMOVE:
				{
					tagTEditBrush brush;
					CreateBrush(brush, -1);
					DoCurrentTools(brush);
				}
				break;
			case MS_LMOVE_RDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, 1);
					DoCurrentTools(brush);
				}
				break;
			case MS_RMOVE_LDOWN:
				{
					tagTEditBrush brush;
					CreateBrush(brush, -1);
					DoCurrentTools(brush);
				}
				break;
			}
		}
		break;
	}
}
