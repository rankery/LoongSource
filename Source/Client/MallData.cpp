#include "stdafx.h"
#include "MallData.h"

const INT nTitleLength = 8;

MallData::MallData( void )
{
	m_dwSize = 0;
}

MallData::~MallData( void )
{

}

MallData g_MallData;
MallData* MallData::Inst()
{
	return & g_MallData;
}

void MallData::LoadFromFile()
{
	tstring strPath = g_strLocalPath + _T("\\attdata\\mall_title.xml");
	const TCHAR* szMallName = strPath.c_str();

	list<tstring> mallNameList;
	list<tstring>::iterator iter;

	//读取商城类型
	TObjRef<VarContainer> varMall = CreateObj( "MallProto", "VarContainer" );
	if(!varMall->Load("VFS_System", szMallName, "id", &mallNameList))
		IMSG(_T("Load file mall_title.xml failed"));
	INT i = 0;
	TCHAR szBuff[X_SHORT_NAME] = {0};
	for(iter=mallNameList.begin(); iter!=mallNameList.end(); ++iter)
	{		
		for(; i<nTitleLength; i++)
		{
			tagMallTitleData data;
			_stprintf( szBuff, _T("name%d"), i+1 );
			data.strTitleName = varMall->GetString(szBuff, (*iter).c_str(), _T(""));
			if( data.strTitleName.empty() )
				data.nIndex = GT_INVALID;
			else 
			{
				data.nIndex = i;
				m_dwSize++;
			}
			m_mapMallData.insert(make_pair(i, data));
		}
	}

	KillObj("MallProto");
}