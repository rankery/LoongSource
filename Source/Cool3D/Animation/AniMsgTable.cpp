#include "StdAfx.h"
#include ".\animsgtable.h"
#include "..\util\csvfile.h"
#include "..\util\Exception.h"
#include "..\..\public\mathbasedefine.h"

namespace F3D
{
	AniMsgTable::AniMsgTable(void)
	{
	}

	AniMsgTable::~AniMsgTable(void)
	{
	}

	bool AniMsgTable::LoadFromFile(IFS *pFS,const char* szFileName)
	{
		CSVFile csvFile;
		bool ret = csvFile.OpenFile(pFS, szFileName);
		if(!ret)
		{
			csvFile.CloseFile();
			return false;
		}

		while(true)
		{
			vector<string> vec = csvFile.GetNextLine();
			if(vec.empty())
				break;
			tagAniMsg tag;
			tag.trackName = _MakeFourCC(vec[0]);
			tag.time = (float)atof(vec[1].c_str());
			tag.msgCode = _MakeFourCC(vec[2]);
			m_msgArray.push_back(tag);
		}

		csvFile.CloseFile();
		return true;
	}

	vector<tagAniMsg> AniMsgTable::GetTrackMsgArray(DWORD trackName)
	{
		vector<tagAniMsg> ret;

		for(vector<tagAniMsg>::iterator iter = m_msgArray.begin(); iter != m_msgArray.end(); ++iter)
			if((*iter).trackName == trackName)
				ret.push_back(*iter);

		return ret;
	}

	bool AniMsgTable::SaveToFile(const char* szFileName)
	{
		FILE* pFile = fopen(szFileName, "wb");
		if(!pFile)
		{
			ASSERT(0 && "CSV File Open Error!");
			fclose(pFile);
			return false;
		}

		for(vector<tagAniMsg>::iterator iter = m_msgArray.begin(); iter != m_msgArray.end(); ++iter)
		{
			char szTime[100] = { 0 };
			sprintf(szTime, "%f", (*iter).time);
			string str = _FourCC2Str((*iter).trackName) + "," + szTime + "," + _FourCC2Str((*iter).msgCode) + "\n";
			fputs(str.c_str(), pFile);
		}

		fclose(pFile);
		return true;
	}
}//namespace F3D