// Login.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "Login.h"


// Login dialog

//IMPLEMENT_DYNAMIC(Login, CDialog)

Login::Login(CWnd* pParent /*=NULL*/)
	: CDialog(Login::IDD, pParent)
{

}

Login::~Login()
{
}

void Login::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Login, CDialog)
END_MESSAGE_MAP()


// Login message handlers
