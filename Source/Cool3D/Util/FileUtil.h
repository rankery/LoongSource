#pragma once
#include <string>
#include <vector>
using namespace std;
#include "..\Cool3D.h"

namespace Cool3D
{
	/** 根据通配符搜索文件
	*/
	void Cool3D_API SearchFiles(const TCHAR* szFind,vector<tstring>& out);

	/** 根据通配符查找并删除文件
	*/
	bool Cool3D_API DeleteFiles(const TCHAR* szFind);

	/** 删除指定目录下的所有文件
	*/
	bool Cool3D_API DeleteAllFiles(const TCHAR* szDir);


}//namespace Cool3D