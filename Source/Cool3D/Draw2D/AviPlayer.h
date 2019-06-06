#pragma once
#include "..\Cool3D.h"
#include <vfw.h>

namespace Cool3D
{
	/**	\class AviPlayer
		\brief ²¥·ÅAVIÎÄ¼þ
	*/
	class Cool3D_API AviPlayer
	{
	public:
		AviPlayer(void);
		~AviPlayer(void);

		void OpenAVI(const TCHAR* szFile);
		void Close();
		void Update(DWORD dwDeltaTime);
		void Draw(HDC hdc);

	private:
		void GrabAVIFrame(int frame);
	private:
		bool			m_bOpen;
		HDRAWDIB		m_hdd;		// Handle For Our Dib
		HDC				m_hdc;
		HBITMAP			m_hBitmap;	// Handle To A Device Dependant Bitmap

		PGETFRAME		m_pgf;	// Pointer To A GetFrame Object
		AVISTREAMINFO	m_psi;	// Pointer To A Structure Containing Stream Info
		PAVISTREAM		m_pavi; // Handle To An Open Stream
		int				m_width;// Video Width
		int				m_height;// Video Height
		long			m_lastframe; // Last Frame Of The Stream
		int				m_mpf;		// Will Hold Rough Milliseconds Per Frame
		BITMAPINFOHEADER m_bmih;	// Header Information For DrawDibDraw Decoding
		BYTE			*m_pdata;	// Pointer To Texture Data

		int			m_next;
		int			m_frame;

	};
}//namespace Cool3D