#include "StdAfx.h"
#include ".\resskeleton.h"
#include ".\resskeletonaction.h"
#include "..\Util\Exception.h"
#include ".\ResMgr.h"
#include "..\Kernel.h"
#include "..\util\csvfile.h"

namespace Cool3D
{
	const float MAX_FREE_TIME=60.0f;//Track闲置多少秒以后才可以释放
	IMP_RTTI(ResSkeleton,ResRenderable);

	ResSkeleton::ResSkeleton(const TCHAR* szName) : ResRenderable(szName)
	{
		m_pFirstRes=NULL;
		m_numFrame=0;
	}

	ResSkeleton::~ResSkeleton(void)
	{
		typedef std::set<DWORD> DeletedResMap;
		DeletedResMap delResMap;
		for(map<DWORD,TrackInfo*>::iterator it=m_mapTrackInfo.begin(); 
			it!=m_mapTrackInfo.end();++it)
		{
			TrackInfo* pTrack=it->second;

			if(pTrack->pRes!=NULL
				&&pTrack->pRes->IsCreated()
				&&delResMap.find((DWORD)pTrack->pRes)==delResMap.end())
			{
				ResMgr::Inst()->ReleaseSkeleton(pTrack->pRes);
				delResMap.insert((DWORD)pTrack->pRes);
			}

			delete pTrack;
		}
		m_mapTrackInfo.clear();
	}

	SkeletonKeyFrame* ResSkeleton::GetFirstFrame( void )
	{
		if( m_pFirstRes != NULL && m_pFirstRes->IsCreated() )
		{
			return m_pFirstRes->GetFrame(0);
		}

		return NULL;
	}

	SkeletonKeyFrame* ResSkeleton::GetFrame(UINT index)
	{
		for(map<DWORD,TrackInfo*>::iterator it=m_mapTrackInfo.begin(); 
			it!=m_mapTrackInfo.end();++it)
		{
			TrackInfo* pTrack=it->second;

			if((int)index>=pTrack->nBegin
				&& (int)index<=pTrack->nEnd
				&& pTrack->pRes!=NULL//可能被释放
				&& pTrack->pRes->IsCreated())
			{
				index-=pTrack->nBaseFrm;//转换index
				return pTrack->pRes->GetFrame(index);
			}
		}

		return NULL;
	}

	UINT ResSkeleton::GetNumBone() const
	{
		return m_pFirstRes->GetNumBone();
	}

	DWORD ResSkeleton::GetPartInfoOfBone(const tstring& boneName) const
	{
		map<tstring, BonePropties>::const_iterator iter = m_bonesPropties.find(boneName);
		if (iter!=m_bonesPropties.end())
			return (*iter).second.dwBodyPart;

		return SKELETONUNKNOWN;
	}

	float ResSkeleton::GetFrmTime( int nFrame )
	{
		SkeletonKeyFrame* frm=GetFrame(nFrame);
		if(frm!=NULL)
			return frm->m_time;
		return 0.0f;
	}

	const SkeletonStruct& ResSkeleton::GetStruct() const
	{
		return m_pFirstRes->GetStruct();
	}

	UINT ResSkeleton::CreateContent(DWORD param)
	{
		const TCHAR* szName=GetName();

		//--Load first skeleton res
		m_pFirstRes=ResMgr::Inst()->NewSkeleton(szName,1,false);
		m_numFrame=0;
		AppendTracks(m_pFirstRes);

		//--Load msg table
		IFS *pFS=GetSafeFS();

		Filename szMsgFileName=GetName();
		szMsgFileName=szMsgFileName.GetPath() +_T("MsgTable.csv");
		m_msgTable.LoadFromFile(pFS,szMsgFileName.c_str());

		//--Load bones properties
		Filename szBonesFileName=GetName();
		szBonesFileName=szBonesFileName.GetPath() + _T("BonesPropties.csv");
		LoadBonesPropties(pFS, szBonesFileName.c_str(), m_bonesPropties);

		InterlockedIncrement(&m_bCreated);
		m_size = 0;
		return m_size;//返回0字节
	}

	int ResSkeleton::GetBoneID(const TCHAR* szBoneName)
	{
		return m_pFirstRes->GetBoneID(szBoneName);
	}

	float ResSkeleton::GetTrackTime(DWORD dwTrackName) 
	{
		return m_actTimeTable[dwTrackName];
	}

	float ResSkeleton::GetTrackTimeFromRes(DWORD dwTrackName) 
	{
		map<DWORD,TrackInfo*>::iterator iter=m_mapTrackInfo.find(dwTrackName);
		if(iter!=m_mapTrackInfo.end())
		{
			ResSkeletonAction* pRes=iter->second->pRes;
			if(pRes!=NULL
				&&pRes->IsCreated())
			{
				return pRes->GetTrackTime(dwTrackName);
			}
		}
		return 0.0f;
	}

	bool ResSkeleton::GetTrackRange( DWORD dwTrackName, int& nBegin, int& nEnd )
	{
		map<DWORD,TrackInfo*>::iterator it = m_mapTrackInfo.find( dwTrackName );
		if(it!=m_mapTrackInfo.end())
		{
			nBegin = it->second->nBegin;
			nEnd   = it->second->nEnd;
			return true;
		}
        
		return false;
	}

	int ResSkeleton::GetTrackName( vector<tstring>& trackNames ) const
	{
		for(map<DWORD,TrackInfo*>::const_iterator it=m_mapTrackInfo.begin(); 
			 it!=m_mapTrackInfo.end();++it)
		{
			trackNames.push_back(_FourCC2Str(it->first));
		}
		return 0;
	}

	Filename ResSkeleton::MakeTrackFileName(DWORD dwTrackName)
	{
		Filename szResName = GetName();
		tstring szTrackName=_FourCC2Str(dwTrackName);

		Filename szTrackFileName = szResName.GetPath();
		szTrackFileName += szTrackName;
		szTrackFileName += _T(".");
		szTrackFileName += szResName.GetExtension();

		return szTrackFileName;
	}

	void ResSkeleton::LoadBonesPropties(IFS *pFS, const TCHAR* szFileName, map<tstring, BonePropties>& bonesPropties)
	{
		CSVFile csvFile;
		bool ret = csvFile.OpenFile(pFS, szFileName);
		if(!ret)
		{
			csvFile.CloseFile();
		}

		while(true)
		{
			const vector<tstring>& vec = csvFile.GetNextLine();
			if(vec.empty())
				break;

			BonePropties prop;
			prop.dwBodyPart=0;
			switch (_tstoi(vec[1].c_str()))
			{
			case 0:
				prop.dwBodyPart|=SKELETONLOWERPART;
				break;
			case 1:
				prop.dwBodyPart|=SKELETONUPPERPART;
				break;
			case 2:
				prop.dwBodyPart|=SKELETONUPPERPART|SKELETONHEADPART;
				break;
			case 3:
				prop.dwBodyPart|=SKELETONUPPERPART|SKELETONCOMMONROOT;
				break;
			case 4:
				prop.dwBodyPart|=SKELETONUPPERPART|SKELETONHEADPART|SKELETONHEADORIGIN;
				break;
			case 5:
				prop.dwBodyPart|=SKELETONUPPERPART|SKELETONUPPERORIGIN;
			case 100:
				prop.dwBodyPart|=SKELETONLOWERPART|SKELETONCOMMONROOT;
				break;
			}

			m_bonesPropties.insert( make_pair(vec[0], prop ));
		}

		csvFile.CloseFile();
	}

	bool ResSkeleton::PrepareTrack(DWORD dwTrackName)
	{
		ASSERT(IsCreated());

		//--查找是否已经有了这个track
		bool bFind = false;
		
		map<DWORD,TrackInfo*>::iterator it = m_mapTrackInfo.find(dwTrackName);

		bFind=it!=m_mapTrackInfo.end();

		//--
		if(bFind)
		{
			//--更新lastOpenTime
			it->second->lastOpenTime=Kernel::Inst()->GetAccumTime();

			if(it->second->pRes==NULL)//被释放掉了
			{
				//--根据需要释放掉一些Tracks
				ReleaseTracks();

				//--重新load ResSkeletonAction
				Filename szTrackFileName=MakeTrackFileName(dwTrackName);
				it->second->pRes=ResMgr::Inst()->NewSkeleton(szTrackFileName.c_str());
				return true;
			}

			return false;
		}
		else
		{
			map<DWORD,ResSkeletonAction*>::iterator itFind=m_asyncList.find(dwTrackName);
			if(itFind==m_asyncList.end())
			{
				//--根据需要释放掉一些Tracks
				ReleaseTracks();

				//--如果没有找到则需要load
				Filename szTrackFileName=MakeTrackFileName(dwTrackName);
				ResSkeletonAction* pRes=ResMgr::Inst()->NewSkeleton(szTrackFileName.c_str());
				m_asyncList[dwTrackName]=pRes;

				UpdateTrackRes();//立即检测资源加载情况
			}
			return true;
		}
	}

	void ResSkeleton::AppendTracks(ResSkeletonAction* pRes)
	{
		vector<tstring> trackNames;
		pRes->GetTrackName(trackNames);

		UINT baseFrame = m_numFrame;
		int nBegin,nEnd;
		for(size_t i=0;i<trackNames.size();i++)
		{
			tstring& szTrackName=trackNames[i];
			DWORD name=_MakeFourCC(szTrackName);

			int ret=pRes->GetTrackRange(name,nBegin,nEnd);
			ASSERT(ret==0);

			map<DWORD,TrackInfo*>::iterator iter=m_mapTrackInfo.find(name);
			if( iter == m_mapTrackInfo.end() )
			{
				TrackInfo* pNewTrack=new TrackInfo;
				pNewTrack->pRes=pRes;
				pNewTrack->nBaseFrm=baseFrame;
				pNewTrack->nBegin=baseFrame+nBegin;
				pNewTrack->nEnd=baseFrame+nEnd;
				pNewTrack->lastOpenTime=Kernel::Inst()->GetAccumTime();
				m_mapTrackInfo[name]=pNewTrack;

				m_numFrame += nEnd - nBegin + 1;
			}
			else//警告加载了重复名称的动作
			{
				TCHAR szError[128];
				_stprintf_s( szError, 100, _T("Animation Error: load track %s again from file %s!"), szTrackName.c_str(), pRes->GetName() );
				::MessageBox( NULL, szError, _T("error"), MB_OK );
			}
		}
	}

	void ResSkeleton::UpdateTrackRes()
	{
		ASSERT(IsCreated());

		for(map<DWORD,ResSkeletonAction*>::iterator iter=m_asyncList.begin();
			iter!=m_asyncList.end();)
		{
			ResSkeletonAction* pRes=iter->second;
			if(pRes->IsCreated())
			{
				AppendTracks(pRes);
				iter=m_asyncList.erase(iter);
			}
			else
				iter++;
		}
	}

	bool ResSkeleton::IsTrackLoaded(DWORD dwTrackName)
	{
		map<DWORD,TrackInfo*>::iterator iter=m_mapTrackInfo.find(dwTrackName);
		return iter!=m_mapTrackInfo.end()
			&& iter->second->pRes!=NULL
			&& iter->second->pRes->IsCreated();
	}

	bool ResSkeleton::IsTrackRelease( DWORD dwTrackName )
	{
		map<DWORD,TrackInfo*>::iterator find1 = m_mapTrackInfo.find(dwTrackName);
		if( find1 != m_mapTrackInfo.end() && find1->second->pRes != NULL )
			return false;

		return true;
	}

	void ResSkeleton::UpdateTrackLastOpenTime(DWORD dwTrackName)
	{
		map<DWORD,TrackInfo*>::iterator iter=m_mapTrackInfo.find(dwTrackName);
		if(iter!=m_mapTrackInfo.end())
		{
			iter->second->lastOpenTime=Kernel::Inst()->GetAccumTime();
		}
	}

	void ResSkeleton::ReleaseTracks()
	{
		for(map<DWORD,TrackInfo*>::iterator it=m_mapTrackInfo.begin(); 
			it!=m_mapTrackInfo.end();++it)
		{
			TrackInfo* pTrack=it->second;

			if(pTrack->pRes!=NULL
				&&pTrack->pRes->IsCreated())
			{
				if(pTrack->pRes!=m_pFirstRes//禁止释放FirstRes
					&&pTrack->pRes->GetNumTrack()==1 //只能释放包含1个Track的Res
					&&Kernel::Inst()->GetAccumTime()-pTrack->lastOpenTime>=MAX_FREE_TIME)
				{
					//TRACE("Time %d ResSkeleton release track:%s\n",GetTickCount(),_FourCC2Str(it->first).c_str());

					ResMgr::Inst()->ReleaseSkeleton(pTrack->pRes);
					pTrack->pRes=NULL;
				}
			}
		}
	}

	void ResSkeleton::ReloadMsgTable()
	{
		MultiMsgMap& msgMap = m_msgTable.GetMsgMap();
		msgMap.clear();

		IFS *pFS=GetSafeFS();
		Filename szMsgFileName=GetName();
		szMsgFileName=szMsgFileName.GetPath() +_T("MsgTable.csv");
		m_msgTable.LoadFromFile(pFS,szMsgFileName.c_str());
	}
}//namespace Cool3D

