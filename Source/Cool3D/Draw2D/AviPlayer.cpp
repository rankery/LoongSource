#include "StdAfx.h"
#include ".\aviplayer.h"
#include "..\Util\DebugMisc.h"
#include "..\Kernel.h"

#pragma comment(lib, "vfw32.lib")		// Search For VFW32.lib While Linking

namespace Cool3D
{
	AviPlayer::AviPlayer(void)
	{
		m_hdd=NULL;
		m_hdc=NULL;
		m_hBitmap=NULL;

		m_width=1;
		m_height=1;
		m_lastframe=1;
		m_mpf=10000;
		m_pdata=NULL;
		m_next=0;
		m_frame=0;
		m_bOpen=false;
	}

	AviPlayer::~AviPlayer(void)
	{
		Close();
	}

	void AviPlayer::OpenAVI(const TCHAR* szFile)
	{
		m_hdd=DrawDibOpen();
		m_hdc=CreateCompatibleDC(0);

		AVIFileInit();

		if(AVIStreamOpenFromFile(&m_pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
		{
			LOG(_T("AVI open failed,%s"),szFile);
			return;
		}

		AVIStreamInfo(m_pavi, &m_psi, sizeof(m_psi));						// Reads Information About The Stream Into m_psi
		m_width=m_psi.rcFrame.right-m_psi.rcFrame.left;					// Width Is Right Side Of Frame Minus Left
		m_height=m_psi.rcFrame.bottom-m_psi.rcFrame.top;					// Height Is Bottom Of Frame Minus Top


		m_lastframe=AVIStreamLength(m_pavi);							// The Last Frame Of The Stream
		m_mpf=AVIStreamSampleToTime(m_pavi,m_lastframe)/m_lastframe;		// Calculate Rough Milliseconds Per Frame

		m_bmih.biSize = sizeof (BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
		m_bmih.biPlanes = 1;											// Bitplanes	
		m_bmih.biBitCount = 32;										// Bits Format We Want (24 Bit, 3 Bytes)
		m_bmih.biWidth = Device()->GetClientWidth();											// Width We Want (256 Pixels)
		m_bmih.biHeight = Device()->GetClientHeight();										// Height We Want (256 Pixels)
		m_bmih.biCompression = BI_RGB;								// Requested Mode = RGB
		
		m_hBitmap = CreateDIBSection (m_hdc, (BITMAPINFO*)(&m_bmih), DIB_RGB_COLORS, (void**)(&m_pdata), NULL, NULL);
		SelectObject(m_hdc, m_hBitmap);								// Select hBitmap Into Our Device Context (hdc)

		m_pgf=AVIStreamGetFrameOpen(m_pavi, NULL);						// Create The PGETFRAME	Using Our Request Mode
		if (m_pgf==NULL)
		{
			return;
		}

		m_bOpen=true;
	}

	void AviPlayer::GrabAVIFrame(int frame)
	{
		if(!m_bOpen)
			return;
		LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pgf, frame);	// Grab Data From The AVI Stream
		m_pdata=(BYTE *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

		// Convert Data To Requested Bitmap Format
		DrawDibDraw (m_hdd, m_hdc, 0, 0, Device()->GetClientWidth(), Device()->GetClientHeight(), lpbi, m_pdata, 0, 0, m_width, m_height, 0);
	}

	void AviPlayer::Close()
	{
		if(m_hBitmap!=NULL)
		{
			DeleteObject(m_hBitmap);										// Delete The Device Dependant Bitmap Object
			m_hBitmap=NULL;
		}
		if(m_hdd!=NULL)
		{
			DrawDibClose(m_hdd);											// Closes The DrawDib Device Context
			m_hdd=NULL;
		}
		if(m_bOpen)
		{
			AVIStreamGetFrameClose(m_pgf);								// Deallocates The GetFrame Resources
			AVIStreamRelease(m_pavi);										// Release The Stream
			AVIFileExit();												// Release The File
		}
		m_bOpen=false;
	}

	void AviPlayer::Update(DWORD dwDeltaTime)
	{
		if(!m_bOpen)
			return;

		int oldframe=m_frame;
		m_next+=dwDeltaTime;											// Increase next Based On The Timer
		m_frame=m_next/m_mpf;												// Calculate The Current Frame

		if (m_frame>=m_lastframe)										// Are We At Or Past The Last Frame?
		{
			m_frame=0;												// Reset The Frame Back To Zero (Start Of Video)
			m_next=0;													// Reset The Animation Timer (next)
		}

		if(m_frame!=oldframe)
			GrabAVIFrame(m_frame);
	}

	void AviPlayer::Draw(HDC hdc)
	{
		if(!m_bOpen)
			return;

		/*char szInfo[64]={0};
		sprintf(szInfo,"%d",m_frame);
		::TextOut(m_hdc,0,0,szInfo,strlen(szInfo));*/

		Device()->BitBltBackbuffer(m_hdc,0,0,Device()->GetClientWidth(),Device()->GetClientHeight());
	}
}//namespace Cool3D