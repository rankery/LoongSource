// DxtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dxt.h"
#include "DxtDlg.h"
#include <dxerr9.h>
#include <dxtlib/dxtlib.h>
#include ".\dxtdlg.h"

#include "tga.h"
#include "Bmp.h"
#include <iostream>
#include <fstream>
#include <ios>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDxtDlg dialog

CDxtDlg::CDxtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDxtDlg::IDD, pParent)
	, m_bSubDir(FALSE)
{
	//{{AFX_DATA_INIT(CDxtDlg)
	m_szMsg = _T("");
	m_szPath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDxtDlg)
	DDX_Control(pDX, IDC_MSG, m_editMsg);
	DDX_Text(pDX, IDC_MSG, m_szMsg);
	DDX_Text(pDX, IDC_PATH, m_szPath);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_SUBDIR, m_bSubDir);
}

BEGIN_MESSAGE_MAP(CDxtDlg, CDialog)
	//{{AFX_MSG_MAP(CDxtDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_MSG, OnChangeMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDxtDlg message handlers

BOOL CDxtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	if(NULL==m_pD3D)
	{
		::AfxMessageBox("D3D9 Object Create failed!");
		return TRUE;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	

	HRESULT hr;
	/*hr=m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_REF, m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDevice);*/


	hr=m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDevice);
	if(FAILED(hr))
	{
		::AfxMessageBox("D3D9 Device Create failed!");
		return TRUE;
	}

	CString msg="D3D init ok...\r\n";
	
	D3DFORMAT dxtn[5] = { D3DFMT_DXT1, D3DFMT_DXT2, D3DFMT_DXT3, D3DFMT_DXT4, D3DFMT_DXT5 };
	for(int i=0;i<5;i++)
	{
		hr=m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,D3DFMT_X8R8G8B8,0,D3DRTYPE_TEXTURE,dxtn[i]);
		BOOL ok=SUCCEEDED(hr);
		CString tmp;
		tmp.Format("DXT%d=%d  ",i+1,ok);
		msg+=tmp;
	}

	ShowMsg(msg);

	LPCTSTR cmdline=AfxGetApp()->m_lpCmdLine;
	if(cmdline!=NULL
		&& strlen(cmdline)>0)
	{
		m_szPath=cmdline;
		m_numError=0;
		CompressFolder(m_szPath);
		CDialog::OnOK();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDxtDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDxtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDxtDlg::OnChangeMsg() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	m_editMsg.LineScroll(m_editMsg.GetLineCount(),0);
	
}

void CDxtDlg::ShowMsg(const char *lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[2048];

	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
	ASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
	va_end(args);

	
	m_szMsg=szBuffer;
	m_szMsg+="\r\n";
	UpdateData(FALSE);
	m_editMsg.SetSel(-1,0);
	m_editMsg.LineScroll(m_editMsg.GetLineCount(),0);
}

void CDxtDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_numError=0;
	UpdateData();
	if(m_szPath.IsEmpty())
		return;

	CompressFolder(m_szPath);
	ShowMsg("\r\nCompress complished--%d error(s).",m_numError);
}


void CDxtDlg::CompressFolder(LPCTSTR path)
{
	CString root=path;
	if(root.Right(1)!="\\")
		root+="\\";

	CFileFind find;
	CString f;
	f.Format("%s*.*",root);
	BOOL bfind=find.FindFile(f);

	while(bfind)
	{
		bfind=find.FindNextFile();
		f=find.GetFilePath();
		if(find.IsDirectory())
		{
			if(!find.IsDots() && m_bSubDir)
			{
				CompressFolder(f);
			}
		}
		else
		{
			f.MakeLower();
			if( f.Right(4)==".tga" || f.Right(4)==".bmp" )
			{
				ShowMsg("%s...",f);
				if(CompressByNV(f))
				{
					DeleteFile(f);
				}
			}
		}
	}//endof while(bfind)
}

NV_ERROR_CODE nvDXTWriteCallback(const void *buffer, 
								 size_t count,  
								 const MIPMapData * mipMapData, 
								 void * userData)
{
	if (mipMapData)
	{
		// means a MIP map
	}
	else
	{
		// otherwise file header data
	}

	std::ostream* s = static_cast<std::ostream*>(userData);
	s->write(static_cast<const char*>(buffer), (std::streamsize)count);
	return s->good() ? NV_OK : NV_READ_FAILED;
}

BOOL CDxtDlg::CompressByNV(LPCTSTR filename)
{
	CString saveas=filename;
	saveas.MakeLower();

	RGBAImage rgbaImage;
	UINT depth;
	if(saveas.Right(4)==".tga")
	{
		TGA tga;
		tga.ReadTGA(filename,rgbaImage);

		// 图片的长宽必须是4的倍数
		if( (tga.GetWidth() % 4 > 0) || (tga.GetHeight() % 4 > 0) )
			return FALSE;

		depth = tga.GetDepth();
		saveas.Replace(".tga",".dds");
	}
	else if( saveas.Right(4)==".bmp" )
	{
		BMP bmp;
		bmp.ReadBMP(filename,rgbaImage);

		// 图片的长宽必须是4的倍数
		if( (bmp.GetWidth() % 4 > 0) || (bmp.GetHeight() % 4 > 0) )
			return FALSE;

		depth = bmp.GetDepth();
		saveas.Replace(".bmp",".dds");
	}

	nvCompressionOptions options;
	std::ofstream ofile(saveas, std::ios::out|std::ios::binary);
	options.user_data = &ofile;

	options.SetQuality(kQualityHighest,400);
	options.DoNotGenerateMIPMaps();
	//fpPixel w;
	//w.r=0.3086f;
	//w.g=0.6094f;
	//w.b=0.0820f;
	//w.a=0.0f;
	//options.SetGreyScale(w);
	switch(depth)
	{
	case 24:
		options.SetTextureFormat(kTextureTypeTexture2D,kDXT1);
		break;
	case 32:
		options.SetTextureFormat(kTextureTypeTexture2D,kDXT5);
		break;
	}

	NV_ERROR_CODE hres = nvDDS::nvDXTcompress(
		rgbaImage,
		&options,
		nvDXTWriteCallback);

	ofile.flush();

	return TRUE;
}

void CDxtDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
