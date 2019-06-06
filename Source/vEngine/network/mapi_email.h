	// ·¢ËÍÓÊ¼þ
	/*
	LPCTSTR lpszTo = _T("bug@ctcrst.com.cn");	// must NOT be NULL or empty
	LPCTSTR lpszToName = _T("bug@ctcrst.com.cn");		// may be NULL
	LPCTSTR lpszSubject = _T("bug");				// may be NULL
	LPCTSTR lpszMessage = _T("log file");			// may be NULL
	LPCTSTR lpszAttachment = szFileName;		// may be NULL


	HMODULE hMapi = ::LoadLibraryA("MAPI32.DLL");
	if( hMapi == NULL )
	{
		::MessageBox(NULL, "Failed to load MAPI32.DLL.", "vEngine", MB_OK|MB_ICONSTOP);
		return;
	}

	// get proc address for MAPISendMail
	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	ULONG (PASCAL *lpfnResolveName)(LHANDLE, ULONG, LPTSTR, FLAGS, ULONG, lpMapiRecipDesc FAR*);


	(FARPROC&)lpfnSendMail = GetProcAddress(hMapi, "MAPISendMail");
	(FARPROC&)lpfnResolveName = GetProcAddress(hMapi, "MAPIResolveName");
	
	if( lpfnSendMail == NULL )
	{
		::MessageBox(NULL, "Invalid MAPI32.DLL, cannot find MAPISendMail.", "vEngine", MB_OK|MB_ICONSTOP);
		::FreeLibrary(hMapi);
		return;
	}

	// SET UP MAPI STRUCTS
	// file description (for the attachment)
	MapiFileDesc fileDesc;
	memset(&fileDesc, 0, sizeof(fileDesc));

	// ----- attachment path
	TCHAR szTempName[_MAX_PATH*2];
	memset(szTempName, 0, sizeof(szTempName));
	if (lpszAttachment && lpszAttachment[0] != _T('\0'))
		lstrcpyn(szTempName, lpszAttachment, _countof(szTempName)-2);

	// ----- attachment title
	TCHAR szTitle[_MAX_PATH*2];
	memset(szTitle, 0, sizeof(szTitle));
	if (lpszAttachment && lpszAttachment[0] != _T('\0'))
		lstrcpyn(szTitle, GetFilePart(lpszAttachment), _countof(szTitle)-2);

	fileDesc.nPosition = (ULONG)-1;
	fileDesc.lpszPathName = szTempName;
	fileDesc.lpszFileName = szTitle;

	// ===== recipient =====

	MapiRecipDesc recip;
	memset(&recip, 0, sizeof(recip));

	// ----- name
	TCHAR szRecipName[_MAX_PATH*2];
	memset(szRecipName, 0, sizeof(szRecipName));
	if (lpszToName && lpszToName[0] != _T('\0'))
		lstrcpyn(szRecipName, lpszToName, _countof(szRecipName)-2);

	if (lpszToName && lpszToName[0] != _T('\0'))
	{
		recip.lpszName = szRecipName;
	}


	// ----- address
	TCHAR szAddress[_MAX_PATH*2];
	memset(szAddress, 0, sizeof(szAddress));
	lstrcpyn(szAddress, lpszTo, _countof(szAddress)-2);
	recip.lpszAddress = szAddress;
	recip.ulRecipClass = MAPI_TO;

	// ===== message =====
	MapiMessage message;
	memset(&message, 0, sizeof(message));

	// ----- recipient
	message.nRecipCount = 1;
	message.lpRecips = &recip;


	// ----- attachment
	if (lpszAttachment && lpszAttachment[0] != _T('\0'))
	{
		message.nFileCount = 1;
		message.lpFiles = &fileDesc;
	}

	// ----- subject
	TCHAR szSubject[_MAX_PATH*2];
	memset(szSubject, 0, sizeof(szSubject));
	if (lpszSubject && lpszSubject[0] != _T('\0'))
		lstrcpyn(szSubject, lpszSubject, _countof(szSubject)-2);

	if (lpszSubject && lpszSubject[0] != _T('\0'))
	{
		message.lpszSubject = szSubject;
	}
	
	// ----- message
	// message may be large, so allocate buffer
	TCHAR *pszMessage = NULL;
	int nMessageSize = 0;
	if (lpszMessage)
	{
		nMessageSize = lstrlen(lpszMessage);
		if (nMessageSize > 0)
		{
			pszMessage = new TCHAR [nMessageSize + 10];
			memset(pszMessage, 0, nMessageSize + 10);
			lstrcpy(pszMessage, lpszMessage);
		}
	}

	char *pszMessageA = NULL;
	if (nMessageSize > 0)
		message.lpszNoteText = pszMessage;


	// ===== SETUP FINISHED, READY TO SEND =====
	ULONG nError = lpfnSendMail(0, (ULONG)NULL, &message, MAPI_DIALOG, 0);


	// ===== SEND COMPLETE, CLEAN UP =====
	if (pszMessage)
		delete[](pszMessage);
	pszMessage = NULL;

	if (pszMessageA)
		delete[](pszMessageA);
	pszMessageA = NULL;

	if (hMapi)
		::FreeLibrary(hMapi);

	*/