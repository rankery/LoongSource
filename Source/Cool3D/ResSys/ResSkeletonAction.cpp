#include "StdAfx.h"
#include ".\ResSkeletonAction.h"
#include "..\Util\Exception.h"
#include "..\Util\FileName.h"

namespace Cool3D
{
	IMP_RTTI(ResSkeletonAction,ResRenderable);

	ResSkeletonAction::ResSkeletonAction(const TCHAR* szName) : ResRenderable(szName)
	{
	}

	ResSkeletonAction::~ResSkeletonAction(void)
	{
		for( UINT i=0;i<m_frameBuf.size();i++ )
		{
			SkeletonKeyFrame *pkf=m_frameBuf[i];
			SAFE_DELETE(pkf);
		}
		m_frameBuf.clear();
	}

	UINT ResSkeletonAction::GetNumFrame() const
	{
		return m_frameBuf.size();
	}

	SkeletonKeyFrame* ResSkeletonAction::GetFrame(UINT index)
	{
		ASSERT(index<m_frameBuf.size());
		return m_frameBuf[index];
	}

	UINT ResSkeletonAction::GetNumBone() const
	{
		if(m_frameBuf.size()<1)
			return 0;
		return m_frameBuf[0]->GetNumBone();
	}

	void ResSkeletonAction::ReadKerFrameInfo( IFS *pFS,DWORD hFile,SkeletonKeyFrame *pKF )
	{
		ASSERT(pKF!=NULL);

		FReadValue(pFS,hFile,pKF->m_name);
		unsigned int numBone=0;
		FReadValue(pFS,hFile,numBone);
		pKF->m_boneArray.resize(numBone);
		for(UINT i=0;i<numBone;i++)
		{
			Bone& bn=pKF->m_boneArray[i];
			bn.ReadFile(pFS,hFile);
		}
		FReadValue(pFS,hFile,pKF->m_time);
		pKF->m_tagNodes.ReadFile(pFS,hFile);
	}

	bool ResSkeletonAction::LoadSkeletonFile(const TCHAR* szName,bool bLoadStruct)
	{
		IFS *pFS=GetSafeFS();
		int testSize = pFS->GetSize(szName);
		if(testSize == 0
			|| testSize == -1)
			return false;

		DWORD hFile=pFS->Open(szName);

		if(hFile == NULL
			|| hFile == -1)
			return false;
		

		FSkelHeader header;
		FReadValue(pFS,hFile,header);

		//--read struct
		if(bLoadStruct)
		{
			pFS->Seek(hFile,header.structOffset,FILE_BEGIN);
			m_struct.ReadFile(pFS,hFile);

			//--build bone map
			for(size_t i=0;i<m_struct.m_boneNames.size();i++)
			{
				m_boneMap[m_struct.m_boneNames[i]]=i;
			}
		}

		//--read frames
		pFS->Seek(hFile,header.frameOffset,FILE_BEGIN);

		TrackInfo info;		
		DWORD dwTrackName = 0;

		for(int i=0;i<(int)header.numFrame;i++)
		{
			// read frame info
			SkeletonKeyFrame* pNewFrm=new SkeletonKeyFrame();
			ReadKerFrameInfo(pFS,hFile,pNewFrm);
			m_frameBuf.push_back(pNewFrm);

			// set track info map
			if( i == 0 )
			{
				dwTrackName = pNewFrm->m_name;
				info.fTime = pNewFrm->m_time;
				info.nBegin = m_vecFrameTime.size();
			}
			else if( dwTrackName != pNewFrm->m_name )
			{
				info.nEnd = m_vecFrameTime.size()-1;
				m_mapTrackInfo[ dwTrackName ] = info;

				dwTrackName = pNewFrm->m_name;
				info.fTime = pNewFrm->m_time;
				info.nBegin = m_vecFrameTime.size();
			}
			else
			{
				info.fTime += pNewFrm->m_time;
			}

			// set frame time
			m_vecFrameTime.push_back( pNewFrm->m_time );
		}

		// record the end track info
		info.nEnd = m_vecFrameTime.size()-1;
		if( dwTrackName != 0 )
			m_mapTrackInfo[ dwTrackName ] = info;

		pFS->Close(hFile);

		return true;
	}

	UINT ResSkeletonAction::CreateContent(DWORD param)
	{
		const TCHAR* szName=GetName();

		while(true)
		{
			if(!LoadSkeletonFile(szName,param!=0))
			{
				TCHAR szError[256];
				errno_t err;
				_get_errno(&err);
				_stprintf(szError,_T("Skeleton file open failed ErrorNo:%d"),err);
				if(::MessageBox(NULL,szName,szError,MB_RETRYCANCEL|MB_ICONWARNING)==IDCANCEL)
					THROW_NULLEX(NULL,_T("Skeleton file open failed."),szName);
			}
			else
				break;
		}

		return ResRenderable::CreateContent(param);
	}

	int ResSkeletonAction::GetBoneID(const TCHAR* szBoneName)
	{
		map<tstring,int>::iterator fi=m_boneMap.find(tstring(szBoneName));
		if(fi==m_boneMap.end())
			return -1;
		return fi->second;
	}

	float ResSkeletonAction::GetTrackTime(DWORD dwTrackName) 
	{
		map<DWORD,TrackInfo>::iterator it = m_mapTrackInfo.find( dwTrackName );
		if( it != m_mapTrackInfo.end() )
			return it->second.fTime;

		return 0.0f;
	}

	int	  ResSkeletonAction::GetTrackRange( DWORD dwTrackName, int& nBegin, int& nEnd )
	{
		map<DWORD,TrackInfo>::iterator it = m_mapTrackInfo.find( dwTrackName );
		if( it != m_mapTrackInfo.end() )
		{
			nBegin = it->second.nBegin;
			nEnd = it->second.nEnd;
			return 0;
		}
        
		return -1;
	}

	int ResSkeletonAction::GetTrackName( vector<tstring>& trackNames )
	{
		for( map<DWORD,TrackInfo>::iterator it = m_mapTrackInfo.begin(); it != m_mapTrackInfo.end(); ++it )
		{
			trackNames.push_back( _FourCC2Str( it->first ) );
		}
		return 0;
	}

}//namespace Cool3D

