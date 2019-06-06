// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ProgressDlg.h"
#include "resource.h"
#include "PluginMgr.h"
#include "WorldEditorMgr.h"
#include "Cool3DView.h"
#include "..\NavMapEditor\PlugNavMapEditor.h"


// CProgressDlg 对话框

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
: CDialog(CProgressDlg::IDD, pParent)
, m_szInfo(_T(""))
, m_szMapName(_T("default"))
, m_bTrn(1)
, m_bNav(1)
, m_bMap(1)
, m_bRiver(1)
, m_bLightMap(1)
, m_bCompress(1)
, m_bCalcLight(TRUE)
, m_bRadiosity(FALSE)
, m_nRadiosityTimes(1)
, m_bBlur(TRUE)
, m_nBlurTimes(1)
, m_bApplyToCombine(TRUE)
, m_bApplyToDeco(TRUE)
, m_bAmbientOcclusion(TRUE)
, m_bColliderData(TRUE)
, m_bNPCNavMap(TRUE)
, m_bNPCNavGraph(TRUE)
, m_bPlayerNavMap(TRUE)
, m_bMiniMap(FALSE)
, m_nMiniMapSize(2048)
, m_fNPCPassHeight(100.0f)
, m_fNPCNavGraphPassHeight(100.0f)
, m_bSmallAO(FALSE)
{
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS, m_ctrl);
	DDX_Text(pDX, IDC_EDIT1, m_szInfo);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_MapName, m_szMapName);
	DDX_Check(pDX, IDC_BuildTerrain, m_bTrn);
	DDX_Check(pDX, IDC_BuildNav, m_bNav);
	DDX_Check(pDX, IDC_BuildMap, m_bMap);
	DDX_Check(pDX, IDC_BuildRiver, m_bRiver);
	DDX_Check(pDX, IDC_BuildLightMap, m_bLightMap);
	DDX_Check(pDX, IDC_CompressTex, m_bCompress);
	DDX_Check(pDX, IDC_LightMapping, m_bCalcLight);
	DDX_Check(pDX, IDC_Radiosity, m_bRadiosity);
	DDX_Text(pDX, IDC_RadiosityTimes, m_nRadiosityTimes);
	DDV_MinMaxInt(pDX, m_nRadiosityTimes, 0, 100);
	DDX_Check(pDX, IDC_Blur, m_bBlur);
	DDX_Text(pDX, IDC_BlurTimes, m_nBlurTimes);
	DDV_MinMaxInt(pDX, m_nBlurTimes, 0, 100);
	DDX_Check(pDX, IDC_ApplyToCombine, m_bApplyToCombine);
	DDX_Check(pDX, IDC_AmbientOcclusion, m_bAmbientOcclusion);
	DDX_Check(pDX, IDC_ColliderData, m_bColliderData);
	DDX_Check(pDX, IDC_NPCNavMap, m_bNPCNavMap);
	DDX_Check(pDX, IDC_NPCNavGraph, m_bNPCNavGraph);
	DDX_Check(pDX, IDC_PlayerNavMap, m_bPlayerNavMap);
	DDX_Check(pDX, IDC_ApplyToDeco, m_bApplyToDeco);

	DDX_Check(pDX, IDC_MiniMap, m_bMiniMap);
	DDX_Text(pDX, IDC_MiniMapSize, m_nMiniMapSize);
	DDX_Text(pDX, IDC_NPCPassHeight, m_fNPCPassHeight);
	DDX_Text(pDX, IDC_NPCNavGraphPassHeight, m_fNPCNavGraphPassHeight);
	DDX_Check(pDX, IDC_SmallAO, m_bSmallAO);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS, OnNMCustomdrawProgress)
	ON_BN_CLICKED(IDC_Build, OnBnClickedBuild)
	ON_BN_CLICKED(IDC_Exit, OnBnClickedExit)
END_MESSAGE_MAP()


void CProgressDlg::Create(int max,int min)
{
	CDialog::Create(IDD_Progress,::AfxGetMainWnd());
	m_ctrl.SetRange32(min,max);
	m_ctrl.SetPos(0);
	CDialog::Create(IDC_PROGRESS2,::AfxGetMainWnd());
	AddMsg(_T(""));
	CenterWindow(AfxGetMainWnd());
	ShowWindow(SW_SHOW);
}

void CProgressDlg::AddMsg(const TCHAR* szFormat,...)
{
	va_list args;
	va_start(args, szFormat);
	TCHAR szBuffer[512];
	 _vstprintf_s(szBuffer, 512, szFormat, args);
	va_end(args);

	UpdateData(TRUE);
	m_szInfo+=szBuffer;
	UpdateData(FALSE);
	m_edit.LineScroll(m_edit.GetLineCount());
	UpdateWindow();
}

void CProgressDlg::SetProgress(int curVal,int maxVal)
{
	if(curVal<0)
		curVal=0;
	if(curVal>maxVal)
		curVal=maxVal;

	m_ctrl.SetRange(0,maxVal);
	m_ctrl.SetPos(curVal);

	CString str;
	str.Format(_T("%d/%d"),curVal,maxVal);
	m_ctrl.SetWindowText(str);

	UpdateWindow();
}

// CProgressDlg 消息处理程序

void CProgressDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CProgressDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

void CProgressDlg::OnNMCustomdrawProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

const TCHAR* CProgressDlg::GetMapName()
{
	UpdateData();
	return m_szMapName;
}

bool CompressFolder(const TCHAR* szFolderPath)
{
	SetCurrentDirectory(Kernel::Inst()->GetAppPath());

	TCHAR szAppPath[512];
	_stprintf_s(szAppPath,512,_T("dxt.exe"));
	TCHAR szCmdLine[512];
	_stprintf_s(szCmdLine,512,_T("%s %s"),szAppPath,szFolderPath);

	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);

	PROCESS_INFORMATION pi;
	if(FALSE==::CreateProcess(szAppPath,szCmdLine,
		NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
		return false;

	::WaitForSingleObject(pi.hProcess,INFINITE); 
	::CloseHandle(pi.hProcess);

	return true;
}

void _FormatTime(DWORD ms,DWORD& hour,DWORD& minute,DWORD& second)
{
	second=ms/1000;
	hour=second/3600;
	second-=hour*3600;

	minute=second/60;
	second-=minute*60;
}

void CProgressDlg::OnBnClickedBuild()
{
	UpdateData(TRUE);

	const TCHAR *szMapName=GetMapName();

	IPlugEditor *pTerrainEditor=PluginMgr::Inst()->GetPlugByClassName(_T("TerrainEditor"));
	IPlugEditor *pNavEditor=PluginMgr::Inst()->GetPlugByClassName(_T("NavMapEditor"));
	IPlugEditor *pMapEditor=PluginMgr::Inst()->GetPlugByClassName(_T("MapObjEditor"));

	const TCHAR* szBasePath=_T("data\\map");

	IPlugEditor::tagBuildOptions opt;
	opt.bApplyToCombine=m_bApplyToCombine==TRUE;
	opt.bApplyToDeco=m_bApplyToDeco==TRUE;
	opt.bBlur=m_bBlur==TRUE;
	opt.blurTimes=m_nBlurTimes;
	opt.bLightMapping=m_bCalcLight==TRUE;
	opt.bRadiosity=m_bRadiosity==TRUE;
	opt.radiosityTimes=m_nRadiosityTimes;
	opt.bAmbientOcclusion=m_bAmbientOcclusion==TRUE;
	opt.bSmallAO=m_bSmallAO==FALSE;
	opt.bColliderData=m_bColliderData==TRUE;
	opt.bNPCNavMap=m_bNPCNavMap==TRUE;
	opt.bNPCNavGraph=m_bNPCNavGraph==TRUE;
	opt.bPlayerNavMap=m_bPlayerNavMap==TRUE;
	opt.nMiniMapSize=m_nMiniMapSize;
	opt.fNPCPassHeight=m_fNPCPassHeight;
	opt.fNPCNavGraphPassHeight=m_fNPCNavGraphPassHeight;

	m_szInfo.Empty();
	UpdateData(FALSE);

	if(ResMgr::Inst()->GetNumLoading()>0)
	{
		AddMsg(_T("生成失败.必须等资源全部加载完成后才能开始."));
		return;
	}

	DWORD dwStartTime=timeGetTime();

	WorldEditorMgr::Inst()->ClearGameMap();
	NullSceneGraph* pSG=(NullSceneGraph*)Cool3DView::Inst()->GetSceneGraph();
	pSG->Clear();
	
	//地形
	if(m_bTrn)
	{
		AddMsg(_T("正在生成地形数据..."));
		if(pTerrainEditor)
		{
			if(pTerrainEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_Terrain,opt,this))
				AddMsg(_T("ok.\r\n"));
			else
			{
				AddMsg(_T("失败.\r\n"));
				return;
			}
		}
		else
		{
			AddMsg(_T("地形编辑器未找到,已跳过...\r\n"));
		}
	}

	//河流
	if(m_bRiver)
	{
		AddMsg(_T("正在生成河流数据..."));
		if(pTerrainEditor)
		{
			if(pTerrainEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_River,opt,this))
				AddMsg(_T("ok.\r\n"));
			else
				AddMsg(_T("失败.\r\n"));
		}
		else
		{
			AddMsg(_T("地形编辑器未找到,已跳过...\r\n"));
		}
	}

	//水下雾信息写入地形
	if(m_bTrn || m_bRiver)
	{
		AddMsg(_T("\t正在把水下雾信息写入地形...\r\n"));
		if(pTerrainEditor)
		{
			if(pTerrainEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_RiverDataToTerrain,opt,this))
				AddMsg(_T("\tok.\r\n"));
			else
				AddMsg(_T("\t失败.\r\n"));
		}
		else
		{
			AddMsg(_T("\t地形编辑器未找到,已跳过...\r\n"));
		}	
	}

	//地图对象
	if(m_bMap)
	{
		AddMsg(_T("正在生成地图对象..."));
		if(pMapEditor)
		{
			if(pMapEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_MapObj,opt,this))
				AddMsg(_T("ok.\r\n"));
			else
				AddMsg(_T("失败.\r\n"));
		}
		else
		{
			AddMsg(_T("Map编辑器未找到,已跳过...\r\n"));
		}
	}

	//导航图
	if(m_bNav)
	{
		AddMsg(_T("正在生成导航图...\r\n"));
		if(pNavEditor)
		{
			if(pTerrainEditor)
			{
				TerrainEditor* pTerrainEngine=WorldEditorMgr::Inst()->GetTerrain();
				NavMapEditor* pNavEngine=WorldEditorMgr::Inst()->GetNavMap();
				GameMapEditor* pMapEngine=WorldEditorMgr::Inst()->GetMap();

				AddMsg(_T("\t生成地形高度图..."));
				Heightmap hmap;
				pTerrainEngine->BuildNavHeightmap(hmap,this);
				pNavEngine->BuildSetHeightmap(&hmap);
				//pNavEngine->BuildSetHeightmap((Heightmap*)pTerrainEngine->GetHeightmap());//test
				AddMsg(_T("ok.\r\n"));

				AddMsg(_T("\t计算水面区域..."));
				BitMap waterArea;
				if(!pTerrainEngine->BuildWaterBitmap(waterArea,this))
					return;
				pNavEngine->BuildSetWaterArea(&waterArea);
				AddMsg(_T("ok.\r\n"));

				AddMsg(_T("\t生成水面高度数据..."));
				Heightmap waterHeight;
				if(!pTerrainEngine->BuildWaterHMap(waterHeight,this))
					return;
				pNavEngine->BuildSetWaterHeight(&waterHeight);
				AddMsg(_T("ok.\r\n"));

				AddMsg(_T("\t计算地表类型..."));
				Heightmap groundType;
				pTerrainEngine->BuildGroundTypeMap(groundType,this);
				pNavEngine->BuildSetGroundType(&groundType);
				AddMsg(_T("ok.\r\n"));

				vector<tagMapPathPoint> pathPoints;
				pMapEngine->GetPathPointVector(pathPoints);
				pNavEngine->BuildSetPathPoints(pathPoints);

				pNavEngine->BuildSetNPCNavMapBuildStartPoints(pMapEngine->GetNPCNavMapBuildStartPoint());
				pNavEngine->BuildSetPlayerNavMapBuildStartPoints(pMapEngine->GetPlayerNavMapBuildStartPoint());

				pNavEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_Nav,opt,this);
			}
			else
				AddMsg(_T("地形编辑器未找到,已跳过...\r\n"));
		}
		else
		{
			AddMsg(_T("导航图编辑器未找到,已跳过...\r\n"));
		}
	}

	//光影贴图
	if(m_bLightMap)
	{
		AddMsg(_T("正在生成光影贴图...\r\n"));
		if(pMapEditor)
		{
			pMapEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_LightMap,opt,this);
		}
		else
		{
			AddMsg(_T("Map编辑器未找到,已跳过...\r\n"));
		}

		//应用到地表预混合贴图
		if(opt.bApplyToCombine)
		{
			AddMsg(_T("\t应用到地表预混合贴图..."));
			if(pTerrainEditor)
			{
				TCHAR szLightMapPath[512];
				_stprintf_s(szLightMapPath,512,_T("%s\\%s\\lightmap"),szBasePath,szMapName);
				TCHAR szTrnPath[512];
				_stprintf_s(szTrnPath,512,_T("%s\\%s\\trn"),szBasePath,szMapName);
				
				TerrainEditor* pEngine=WorldEditorMgr::Inst()->GetTerrain();
				if(pEngine->ApplyLightMapToCombineImage(szLightMapPath,szTrnPath,this))
					AddMsg(_T("ok.\r\n"));
				else
					AddMsg(_T("失败.\r\n"));
			}
			else
			{
				AddMsg(_T("地形编辑器未找到,已跳过...\r\n"));
			}
		}

		//应用到地表装饰层
		if(opt.bApplyToDeco)
		{
			AddMsg(_T("\t应用到地表装饰层..."));
			if(pTerrainEditor)
			{
				TCHAR szLightMapPath[512];
				_stprintf_s(szLightMapPath,512,_T("%s\\%s\\lightmap"),szBasePath,szMapName);
				TCHAR szTrnPath[512];
				_stprintf_s(szTrnPath,512,_T("%s\\%s\\trn"),szBasePath,szMapName);

				TerrainEditor* pEngine=WorldEditorMgr::Inst()->GetTerrain();
				if(pEngine->ApplyLightMapToDecoLayer(szLightMapPath,szTrnPath,this))
					AddMsg(_T("ok.\r\n"));
				else
					AddMsg(_T("失败.\r\n"));
			}
			else
			{
				AddMsg(_T("地形编辑器未找到,已跳过...\r\n"));
			}
		}
	}

	//压缩纹理
	if(m_bCompress)
	{
		AddMsg(_T("正在压缩地形贴图..."));

		TCHAR szPath[512];
		_stprintf_s(szPath,512,_T("%s%s\\%s\\trn"),Kernel::Inst()->GetWorkPath(),szBasePath,szMapName);

		if(CompressFolder(szPath))
			AddMsg(_T("ok.\r\n"));
		else
			AddMsg(_T("失败.\r\n"));


	}
	if(m_bCompress)
	{
		AddMsg(_T("正在压缩光影贴图..."));

		TCHAR szPath[512];
		_stprintf_s(szPath,512,_T("%s%s\\%s\\lightmap"),Kernel::Inst()->GetWorkPath(),szBasePath,szMapName);

		if(CompressFolder(szPath))
			AddMsg(_T("ok.\r\n"));
		else
			AddMsg(_T("失败.\r\n"));
	}

	if(m_bMiniMap)
	{
		AddMsg(_T("正在生成缩略图..."));
		if( m_nMiniMapSize > 0 )
		{
			if(pMapEditor)
			{
				if(pMapEditor->Build(szMapName,szBasePath,IPlugEditor::EBI_MiniMap,opt,this))
					AddMsg(_T("ok.\r\n"));
				else
					AddMsg(_T("失败.\r\n"));
			}
			else
			{
				AddMsg(_T("Map编辑器未找到,已跳过...\r\n"));
			}
		}
		else
		{
			AddMsg(_T("缩略图大小不正确,已跳过...\r\n"));
		}
	}

	DWORD dwBuildTimeByMS=timeGetTime()-dwStartTime;
	DWORD dwHour,dwMinute,dwSecond;
	_FormatTime(dwBuildTimeByMS,dwHour,dwMinute,dwSecond);
	AddMsg(_T("全部生成完毕,总共%d小时%d分%d秒."),dwHour,dwMinute,dwSecond);
}

void CProgressDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
