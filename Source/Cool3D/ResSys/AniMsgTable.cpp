#include "StdAfx.h"
#include ".\animsgtable.h"
#include "..\util\csvfile.h"
#include "..\util\Exception.h"
#include "..\Public\mathbasedefine.h"

namespace Cool3D
{
	AniMsgTable::AniMsgTable(void)
	{
	}

	AniMsgTable::~AniMsgTable(void)
	{
	}

	bool AniMsgTable::LoadFromFile(IFS *pFS,const TCHAR* szFileName)
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
			vector<tstring> vec = csvFile.GetNextLine();
			if(vec.empty())
				break;

			tagAniMsg tag;
			tag.time = (float)_tstof(vec[1].c_str());
			tag.msgCode = _MakeFourCC(vec[2]);

			m_msgMap.insert( make_pair( _MakeFourCC(vec[0]), tag ) );
		}

		csvFile.CloseFile();
		return true;
	}

	void AniMsgTable::CheckMsgForTime( DWORD trackName, float lastTime, float curTime, list<DWORD>& msgCodes ) const
	{
		MultiMsgMap::const_iterator it = m_msgMap.find( trackName );
		while( it != m_msgMap.end() )
		{
			if( it->first != trackName )	// ½áÊø
				break;

			if( it->second.time>=lastTime && it->second.time<curTime )
			{
				const tagAniMsg& aniMsg=it->second;
				msgCodes.push_back(aniMsg.msgCode);
			}

			++it;
		}
	}

	bool AniMsgTable::SaveToFile(const TCHAR* szFileName)
	{
		FILE* pFile = _tfopen(szFileName, _T("wb"));
		if(!pFile)
		{
			ASSERT(0 && _T("CSV File Open Error!"));
			fclose(pFile);
			return false;
		}

		tstring str;
		TCHAR szTime[100] = { 0 };

		for( MultiMsgMap::iterator it = m_msgMap.begin(); it != m_msgMap.end(); ++it )
		{
			_stprintf(szTime, _T("%f"), it->second.time);
			str += _FourCC2Str(it->first) + _T(',') + szTime + _T(',') + _FourCC2Str(it->second.msgCode) + _T('\n');
		}

		/*for(vector<tagAniMsg>::const_iterator iter = m_msgArray.begin(); iter != m_msgArray.end(); ++iter)
		{
			TCHAR szTime[100] = { 0 };
			sprintf(szTime, "%f", (*iter).time);
			str += _FourCC2Str((*iter).trackName) + ',' + szTime + ',' + _FourCC2Str((*iter).msgCode) + '\n';
		}*/

		str += _T('\0');
		fwrite(str.c_str(), sizeof(TCHAR), str.size(), pFile);

		fclose(pFile);
		return true;
	}
}//namespace Cool3D