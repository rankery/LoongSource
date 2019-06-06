#include "stdafx.h"
#include "SerActiveData.h"

class SerActiveSort
{
public:
	bool operator() (const tagSerActiveData& data1, const tagSerActiveData& data2)
	{
		int nHour1; 
		int nHour2; 
		TCHAR* szFormat = _T("%d");
		_stscanf( data1.strTime.c_str(), szFormat, &nHour1 );
		_stscanf( data2.strTime.c_str(), szFormat, &nHour2 );

		if( data1.eStartMode == data2.eStartMode )
		{
			return nHour1 < nHour2;
		}
		else if( data1.eStartMode > data2.eStartMode )		//按日期的活动 排在 按星期几的活动 之前
		{
			return true;
		}
		return false;
	}
};

SerActiveData::SerActiveData( void )
{

}

SerActiveData::~SerActiveData( void )
{

}

SerActiveData g_SerActiveData;
SerActiveData* SerActiveData::Inst()
{
	return &g_SerActiveData;
}

void SerActiveData::LoadFromFile()
{
	tstring strPath = g_strLocalPath + _T("\\attdata\\serveractive.xml");
	const TCHAR* szActName = strPath.c_str();

	list<tstring> actNameList;
	list<tstring>::iterator iter;
	
	//读取商城类型
	TObjRef<VarContainer> varSerActive = CreateObj( "SerActiveProto", "VarContainer" );
	if(!varSerActive->Load("VFS_System", szActName, "id", &actNameList))
		IMSG(_T("Load file serveractive.xml failed"));
	
	TCHAR szBuff[X_SHORT_NAME] = {0};
	for( iter=actNameList.begin(); iter!=actNameList.end(); ++iter )
	{
		tagSerActiveData data;
		data.dwID = varSerActive->GetDword( _T("id"), (*iter).c_str(), GT_INVALID );
		
		data.eStartMode = (EServerActiveMode)varSerActive->GetInt( _T("mode"), (*iter).c_str(), GT_INVALID );
		data.nMonth	= (INT)varSerActive->GetInt( _T("month"), (*iter).c_str(), GT_INVALID  );
		data.nDay	= (INT)varSerActive->GetInt( _T("day"), (*iter).c_str(), GT_INVALID  );
		// ! add by douyubo 节日活动

		tstring strColor = varSerActive->GetString( _T("color"), (*iter).c_str(), _T("") );
		data.dwColor = _tcstoul(strColor.c_str(), NULL, 16);
		data.strActiveName = varSerActive->GetString( _T("name"), (*iter).c_str(), _T("") );
		data.strContent = varSerActive->GetString( _T("content"), (*iter).c_str(), _T("") );
		data.strTime = varSerActive->GetString( _T("time"), (*iter).c_str(), _T("") );
		for( int i=0; i<MAX_WEEK; i++ )
		{
			_sntprintf( szBuff, X_SHORT_NAME, _T("date%d"), i+1 );
			data.byDate[i] = (BYTE)( varSerActive->GetDword( szBuff, (*iter).c_str(), 0 ) );
		}
		//m_mapSerActiveData.insert( make_pair( data.dwID, data ) );
		m_vecData.push_back(data);
	}

	SerActiveSort sortFun;
	std::sort(m_vecData.begin(), m_vecData.end(), sortFun);

	for( vector<tagSerActiveData>::iterator it=m_vecData.begin(); it!=m_vecData.end(); ++it )
	{
		m_mapSerActiveData.insert( make_pair( it->dwID, (*it) ) );
	}
}
