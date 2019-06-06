#pragma once
#include "F3DMaxExp.h"
#include <string>

using namespace std;

class F3D_MAX_API BatchExpDlg  
{
public:
	BatchExpDlg();
	virtual ~BatchExpDlg();

	bool Run(LPSTR szFile);
};