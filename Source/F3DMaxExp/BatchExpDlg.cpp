#include "stdafx.h"
#include "BatchExpDlg.h"
#include "resource.h"
using namespace std;

BatchExpDlg::BatchExpDlg()
{
}

BatchExpDlg::~BatchExpDlg()
{

}

bool BatchExpDlg::Run(LPSTR szFile)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 255;
	ofn.lpstrFilter = "XML\0*.xml\0";//"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return (GetOpenFileName(&ofn) == TRUE);
}