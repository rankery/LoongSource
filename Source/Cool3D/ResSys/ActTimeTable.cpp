#include "StdAfx.h"
#include ".\ActTimeTable.h"
#include "..\util\csvfile.h"
#include "..\util\Exception.h"
#include "..\Public\mathbasedefine.h"

namespace Cool3D
{
	ActTimeTable::ActTimeTable(void)
	{
	}

	ActTimeTable::~ActTimeTable(void)
	{
	}

	bool ActTimeTable::LoadFromFile(IFS *pFS,const TCHAR* szFileName)
	{
		CSVFile csvFile;
		bool ret = csvFile.OpenFile(pFS, szFileName);
		if(!ret)
		{
			csvFile.CloseFile();
			return false;
		}

		m_actTimeMap.clear();

		while(true)
		{
			vector<tstring> vec = csvFile.GetNextLine();
			if(vec.size()!=2)
				break;

			DWORD trackName = _MakeFourCC(vec[0]);
			float actTime = float(_tstof(vec[1].c_str()))/1000.0f;

			m_actTimeMap.insert( make_pair( trackName,actTime ) );
		}

		csvFile.CloseFile();
		return true;
	}

	float ActTimeTable::operator[](DWORD dwTrackName)
	{
		map<DWORD,float>::iterator iter=m_actTimeMap.find(dwTrackName);
		if(iter!=m_actTimeMap.end())
			return iter->second;
		return 0.0f;
	}

}//namespace Cool3D