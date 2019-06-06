#ifndef _COMMON_H_
#define _COMMON_H_

#define X_HUGE_STRING		512
#define X_LONG_STRING		256
#define X_SHORT_STRING		32
#define X_SHORT_NAME			32
//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define GT_INVALID				(-1)
#define GT_VALID(n)				(((INT)(n)) != GT_INVALID)
#define P_VALID(n)				( (((INT)(n)) != GT_INVALID) && ((n) != NULL) )

#define GT_SEEK_SET				0
#define GT_SEEK_CUR				1
#define GT_SEEK_END				2

#ifndef SAFE_DEL
#define SAFE_DEL(p)				{ if(p) { delete(p);		(p) = NULL; } }
#endif
#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(p)		{ if(p) { delete[](p);		(p) = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p) = NULL; } }
#endif
#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h)	{ if(h) { CloseHandle(h);	(h) = NULL; } }
#endif
#ifndef ASSERT
#define ASSERT(f)				assert((f))
#endif

CONST INT GT_NET_BLOCK_TIME = 1000*150;	// 150 ms

// 定义线程函数类型
typedef UINT (WINAPI* THREADPROC)(LPVOID);


//-----------------------------------------------------------------------------
// 优化算法需要的CRC表,与一般的CRC不同
//-----------------------------------------------------------------------------
const unsigned long globe_Crc32Table[256]={
	0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
	0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
	0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
	0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
	0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
	0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
	0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
	0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
	0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
	0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
	0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
	0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
	0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
	0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
	0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
	0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
	0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
	0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
	0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
	0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
	0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
	0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
	0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
	0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
	0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
	0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
	0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
	0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
	0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
	0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
	0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
	0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
};
//-----------------------------------------------------------------------------
// 计算32位CRC,比一般算法快速很多
//-----------------------------------------------------------------------------
static DWORD Crc32(LPCSTR lpData)
{
	//对ASC超过127的字符， 必须用unsigned char, 否则会出错
	LPBYTE pData = (LPBYTE)lpData;
	if( 0 == pData[0] ) 
		return (DWORD)GT_INVALID;

	UINT unResult = *pData++ << 24;
	if( *pData )
	{
		unResult |= *pData++ << 16;
		if( *pData )
		{
			unResult |= *pData++ << 8;
			if( *pData ) unResult |= *pData++;
		}
	}
	unResult = ~ unResult;
	while( *pData )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		pData++;
	}
	return ~unResult;
}

static DWORD Crc32(LPCTSTR lpData)
{
	LPBYTE pData = (LPBYTE)lpData;
	if( !(*pData || *(pData+1)) ) 
		return (DWORD)GT_INVALID;

	UINT unResult = *pData++ << 24;
	unResult |= *pData++ << 16;
	if( *pData || *(pData+1) )
	{
		unResult |= *pData++ << 8;
		unResult |= *pData++;
	}
	unResult = ~ unResult;
	INT nCount = 0;
	while( (nCount&1) ? true : (*pData || *(pData+1)) )
	{
		unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
		++pData;
		++nCount;
	}

	return ~unResult;
}


//-----------------------------------------------------------------------------
// 网络消息基本结构
//-----------------------------------------------------------------------------
struct tagNetCmd
{
	DWORD	dwID;
	DWORD	dwSize;

	DWORD Crc32(LPCSTR lpData)
	{
		UINT unResult;
		LPBYTE pData = (LPBYTE)lpData;
		if( 0 == pData[0] ) 
			return (DWORD)GT_INVALID;
		unResult = *pData++ << 24;
		if( *pData )
		{
			unResult |= *pData++ << 16;
			if( *pData )
			{
				unResult |= *pData++ << 8;
				if( *pData ) unResult |= *pData++;
			}
		}
		unResult = ~ unResult;
		while( *pData )
		{
			unResult = (unResult << 8 | *pData) ^ globe_Crc32Table[unResult >> 24];
			pData++;
		}
		return ~unResult;
	}
};

//-----------------------------------------------------------------------------
// 方便网络消息的定义
//-----------------------------------------------------------------------------

#ifndef CMD_START
#define CMD_START(name)	\
struct tag##name : public tagNetCmd{	\
	tag##name(){ ZeroMemory(this,sizeof(*this));  \
	dwID = Crc32(#name); dwSize = sizeof(*this); }
#define CMD_END		};
#endif

#include <process.h> // for _beginthread()

//控件边界
#define CONTROL_BORDER 2

//创建控件辅助函数
static wxSizer *CreateSizerWithText(wxWindow *parent, wxControl *control, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
								bool bPwd = false, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize, wxControl *control2 = NULL)
{
	wxSizer *sizeRow = new wxBoxSizer(style);
	wxTextCtrl *text = NULL;
	if (bPwd == true)
	{
		//密码
		text = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, wxTE_PASSWORD);
	}
	else
	{
		text = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, wxTE_PROCESS_ENTER);
	}
	if (control != NULL)
	{
		sizeRow->Add(control, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	}

	sizeRow->Add(text, 1, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);

	if (control2 != NULL)
	{
		sizeRow->Add(control2, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	}

	if(ppText)
		*ppText = text;

	return sizeRow;
}

static wxSizer *CreateSizerWithTextAndLabel(wxWindow *parent, const wxString& label, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
									 bool bPwd = false, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize)
{
	return CreateSizerWithText(parent, new wxStaticText(parent, wxID_ANY,label),id, ppText, bPwd, style, size);
}

static wxSizer *CreateSizerWithTextAndLabels(wxWindow *parent, const wxString& label, const wxString& label2, wxWindowID id = wxID_ANY,wxTextCtrl **ppText = NULL, 
									 bool bPwd = false, int style = wxHORIZONTAL, const wxSize& size = wxDefaultSize)
{
	return CreateSizerWithText(parent, new wxStaticText(parent, wxID_ANY,label),id, ppText, bPwd, style, size, new wxStaticText(parent, wxID_ANY,label2));
}

static wxCheckBox* CreateCheckBoxAndAddToSizer(wxWindow *parent, wxSizer *sizer, const wxString& label, wxWindowID id)
{
	wxCheckBox *checkbox = new wxCheckBox(parent, id, label);
	sizer->Add(checkbox, 0, wxLEFT | wxRIGHT, 5);
	sizer->Add(0, 2, 0, wxGROW); // spacer

	return checkbox;
}
// create a sizer containing a button and a text ctrl
static wxSizer* CreateSizerWithTextAndButton(wxWindow *parent, wxWindowID idBtn, const wxString& label, wxWindowID id, wxTextCtrl **ppText)
{
	return CreateSizerWithText(parent, new wxButton(parent, idBtn, label), id, ppText);
}

//
static wxSizer* CreateSizerWithTextAndComboBox(wxWindow *parent, wxWindowID idComboBox, const wxString& label, wxComboBox **ppComboBox, int n=0, const wxString choices[] = NULL)
{
	wxSizer* sizeRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* staticText = new wxStaticText(parent, wxID_ANY, label);
	*ppComboBox = new wxComboBox(parent, idComboBox, wxEmptyString, wxDefaultPosition, wxDefaultSize, n, choices, wxCB_READONLY);
	sizeRow->Add(staticText, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	sizeRow->Add(*ppComboBox, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	return sizeRow;
}

//wxString to TCHAR
static void wxStringToTCHAR(wxString& string,  TCHAR* tchar_array, int size_array)
{
	int length = string.Len();
	if (length + 1 >= size_array)
	{
		length = size_array;
	}
	const wxChar* myStringChars = string.c_str();
	for (int i = 0; i < length; i++)
	{
		tchar_array[i] = myStringChars[i];
	}
	tchar_array[length] = _T('\0');
}

#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0
//-----------------------------------------------------------------------------
// 将unicode转成unicode8
//-----------------------------------------------------------------------------
static LPCSTR UnicodeToUnicode8(LPCTSTR szSrc, LPSTR szOut)
{
	if( !szOut )
		return NULL;

	LPSTR szDest = szOut;

	while( *szSrc )
	{
		if(*szSrc < 0x80)	// 0xxxxxxx
		{
			*szDest++ = (BYTE)*szSrc;
		}
		else if( *szSrc < 0x800 )	// 110xxxxx 10xxxxxx
		{
			*szDest++ = ((BYTE)(MASK2BYTES | *szSrc >> 6));
			*szDest++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
		}
		else	// 1110xxxx 10xxxxxx 10xxxxxx
		{
			*szDest++ = ((BYTE)(MASK3BYTES | *szSrc >> 12));
			*szDest++ = ((BYTE)(MASKBYTE | *szSrc >> 6 & MASKBITS));
			*szDest++ = ((BYTE)(MASKBYTE | *szSrc & MASKBITS));
		}
		++szSrc;
	}

	*szDest = 0; // 加上0结束符
	return szOut;
}


//-----------------------------------------------------------------------------
// 将unicode8转成unicode
//-----------------------------------------------------------------------------
LPCTSTR	Unicode8ToUnicode(LPCSTR szSrc, LPTSTR szOut)
{
	if( !szOut )
		return NULL;

	LPTSTR szDest = szOut;

	while( *szSrc )
	{
		if( !(*szSrc & MASKBYTE) )	// 0xxxxxxx
		{
			*szDest++ = *szSrc++;
		}
		else if( (*szSrc & MASK3BYTES) == MASK3BYTES )	// 1110xxxx 10xxxxxx 10xxxxxx
		{
			*szDest++ = ((*szSrc & 0x0F) << 12) | ((*(szSrc+1) & MASKBITS) << 6) | (*(szSrc+2) & MASKBITS);
			++szSrc;
			++szSrc;
			++szSrc;
		}
		else if( (*szSrc & MASK2BYTES) == MASK2BYTES )	// 110xxxxx 10xxxxxx
		{
			*szDest++ = ((*szSrc & 0x1F) << 6) | (*(szSrc+1) & MASKBITS);
			++szSrc;
			++szSrc;
		}
	}

	*szDest = 0; // 加上0结束符
	return szOut;
}

#endif /* _COMMON_H_ */