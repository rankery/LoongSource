#include "..\stdafx.h"
#include ".\FSkelFile.h"
#include ".\ExpSkeleton.h"
#include "..\CSExport.h"
#include <assert.h>

FSkelFile::FSkelFile()
{

}

FSkelFile::~FSkelFile()
{
	Clear();
}

void FSkelFile::Clear()
{
	for(size_t i=0;i<m_frameBuf.size();i++)
	{
		SkeletonFrame *pFrm=m_frameBuf[i];
		delete pFrm;
	}
	m_frameBuf.clear();
	m_tracks.clear();
}

bool FSkelFile::LoadFromFile(const TCHAR* szFile)
{
	Clear();
	//--
	FILE *fp=_tfopen(szFile,_T("rb"));
	if(fp==NULL)
		return false;
	//--
	FSkelHeader header;
	FReadValue(fp,header);

	//--read struct
	fseek(fp,header.structOffset,SEEK_SET);
	m_struct.ReadFile(fp);
	//--read frame
	fseek(fp,header.frameOffset,SEEK_SET);
	for(size_t i=0;i<header.numFrame;i++)
	{
		Cool3D::SkeletonFrame *pFrm=new Cool3D::SkeletonFrame;
		pFrm->ReadFile(fp);
		m_frameBuf.push_back(pFrm);
		m_tracks.insert(_FourCC2StrA(pFrm->m_name));
	}
	fclose(fp);

	return true;
}

bool FSkelFile::WriteToFile(const TCHAR* szFile)
{
	FILE *fp=_tfopen(szFile,_T("wb"));
	if(fp==NULL)
		return false;
	//--
	FSkelHeader header;
	wcscpy(header.magic,FSKEL_MAGIC);
	header.ver=1;
	header.numFrame=m_frameBuf.size();
	FWriteValue(fp,header);
	
	//--write skeleton struct
	header.structOffset=ftell(fp);
	m_struct.WriteFile(fp);
	//--write frame
	header.frameOffset=ftell(fp);
	for(size_t i=0;i<m_frameBuf.size();i++)
		m_frameBuf[i]->WriteFile(fp);

	//--write back header
	fseek(fp,0,SEEK_SET);
	FWriteValue(fp,header);
	//--
	fclose(fp);
	return true;
}

bool FSkelFile::RemoveTrack(const TCHAR* szTrackName)
{
	bool bFind=false;
	size_t i;
	DWORD szTrackCC=_MakeFourCCA(tstring(szTrackName));
	for(i=0;i<m_frameBuf.size();i++)
	{
		SkeletonFrame *pFrm=m_frameBuf[i];
		if(pFrm->m_name==szTrackCC)
		{
			delete pFrm;
			m_frameBuf[i]=NULL;
			bFind=true;
		}
	}

	if(bFind)
	{
		m_tracks.erase(tstring(szTrackName));

		vector<SkeletonFrame*>::iterator iter=m_frameBuf.begin();
		while(iter!=m_frameBuf.end())
		{
			if(NULL==(*iter))
				m_frameBuf.erase(iter);
			else
				iter++;
		}
	}
	return bFind;
}

extern MsgBox	g_msgBox;
int FSkelFile::AddTrack(const TCHAR* szTrackName,ExpSkeleton *pExp,TimeValue startTime,
						TimeValue endTime,int sampleRate)
{
	SkeletonStruct stut;
	pExp->CreateSkeletonStruct(stut);
	if(m_struct.m_boneNames.empty())
		m_struct=stut;
	else
	{
		if(stut.m_boneNames.size()!=m_struct.m_boneNames.size())//骨骼个数不同
		{
			g_msgBox.ShowMsg(_T("骨骼个数不对\r\n"));
			return 0;
		}
		for(size_t i=0;i<stut.m_boneNames.size();i++)
		{
			if(stut.m_boneNames[i]!=m_struct.m_boneNames[i])
			{
				string& newName = WideToChar(stut.m_boneNames[i].c_str());
				string& oldName = WideToChar(m_struct.m_boneNames[i].c_str());
				g_msgBox.ShowMsg(_T("骨骼结构不相同,now=%s,old=%s\r\n"),
					newName.c_str(),oldName.c_str());
				return 0;//骨骼结构不同
			}
		}
	}
	m_tracks.insert(szTrackName);

	int ret=0;
	if(sampleRate<=0)
		sampleRate=8;
	const TimeValue sampleTime=SecToTicks(1.0f/sampleRate);//采样间隔时间
	TimeValue t=0;
	for(t=startTime;t<=endTime;t+=sampleTime)
	{
		TimeValue frmTime=sampleTime;
		if(t>endTime)
		{
			TimeValue a=t-endTime;
			t=endTime;
			frmTime-=a;
			assert(frmTime>0);
		}
		SkeletonFrame *pFrm=pExp->CreateFrame(szTrackName,t);
		pFrm->m_time=TicksToSec(frmTime);
		m_frameBuf.push_back(pFrm);
		ret++;
	}

	//--强制输出最后一帧
	if(t-sampleTime < endTime)
	{
		SkeletonFrame *pFrm=pExp->CreateFrame(szTrackName,endTime);
		pFrm->m_time=TicksToSec(endTime-t+sampleTime);
		m_frameBuf.push_back(pFrm);
		ret++;
	}
	
	return ret;
}

int FSkelFile::AddTrackOffset(const TCHAR* szTrackName,ExpSkeleton *pExp,TimeValue startTime,
						TimeValue endTime,int sampleRate)
{
	SkeletonStruct stut;
	pExp->CreateSkeletonStruct(stut);
	if(m_struct.m_boneNames.empty())
		m_struct=stut;
	else
	{
		if(stut.m_boneNames.size()!=m_struct.m_boneNames.size())//骨骼个数不同
		{
			g_msgBox.ShowMsg(_T("骨骼个数不对"));
			return 0;
		}
		for(size_t i=0;i<stut.m_boneNames.size();i++)
		{
			if(stut.m_boneNames[i]!=m_struct.m_boneNames[i])
			{
				g_msgBox.ShowMsg(_T("骨骼结构不相同,now=%s,old=%s"),
					stut.m_boneNames[i].c_str(),m_struct.m_boneNames[i].c_str());
				return 0;//骨骼结构不同
			}
		}
	}

	int ret=0;
	if(sampleRate<=0)
		sampleRate=8;
	const TimeValue sampleTime=SecToTicks(1.0f/sampleRate);//采样间隔时间
	TimeValue t=0;
	std::vector<TimeValue> frames;
	for(t=startTime;t<=endTime;t+=sampleTime)
	{
		TimeValue frmTime=sampleTime;
		if(t>endTime)
		{
			TimeValue a=t-endTime;
			t=endTime;
			frmTime-=a;
			assert(frmTime>0);
		}
		frames.push_back(frmTime);
		ret++;
	}

	//--强制输出最后一帧
	if(t-sampleTime < endTime)
	{
		frames.push_back(endTime-t+sampleTime);
		ret++;
	}

	//--定位szTrackName在帧缓存中的开始位置
	int trackPos=GetTrackBeginPos(szTrackName);
	if( trackPos<0 )
	{
		g_msgBox.ShowMsg(_T("Track:%s不存在"),szTrackName);
		return 0;
	}

	assert(pExp->GetNumRoot()>0);
	INode *pRootNode=pExp->GetRootNode(0);

	//--获得第一帧中根节点在模型空间中的位移
	Vector3 origin;
	Quaternion quat;
	Max_GetNodeLocalAni(pRootNode,NULL,0,origin,quat);
	origin.y=0.0f;
	TimeValue trackTime=frames[0];

	//--计算每一帧根节点到第一帧根节点的距离，第一帧的距离为0.0f
	for( size_t i=1; i<frames.size(); i++ )
	{
		SkeletonFrame *pFrame=m_frameBuf[trackPos+i];

		Vector3 trans;
		Max_GetNodeLocalAni(pRootNode,NULL,trackTime,trans,quat);//取模型空间中的位移
		trans.y=0.0f;

		trans-=origin;
		pFrame->m_offset=D3DXVec3Length(&trans);

		trackTime+=frames[i];
	}

	return ret;
}

int FSkelFile::GetTrackBeginPos( const TCHAR *szTrackName )
{
	DWORD trackName=_MakeFourCCA(tstring(szTrackName));
	for( int i=0; i<(int)m_frameBuf.size(); i++ )
	{
		const SkeletonFrame *pFrame=m_frameBuf[i];
		if( trackName == pFrame->m_name )
		{
			return i;
		}
	}

	return -1;
}