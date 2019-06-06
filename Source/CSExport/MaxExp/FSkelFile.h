#pragma once

#include <max.h>
#include <vector>
#include <set>
#include <string>
using namespace std;

#include "..\..\MaxPublic\SkinBoneDefine.h"
using namespace Cool3D;

class ExpSkeleton;
class FSkelFile  
{
public:
	FSkelFile();
	~FSkelFile();

	bool LoadFromFile(const TCHAR* szFile);
	bool WriteToFile(const TCHAR* szFile);
	bool RemoveTrack(const TCHAR* szTrackName);
	/**
		\return number for frame to add
	*/
	int  AddTrack(const TCHAR* szTrackName,ExpSkeleton *pExp,TimeValue startTime,
		TimeValue endTime,int sampleRate/*ÿ�뼸֡*/);
	int  AddTrackOffset(const TCHAR* szTrackName,ExpSkeleton *pExp,TimeValue startTime,
		TimeValue endTime,int sampleRate);
	const set<tstring>& GetTrackSet() const	{ return m_tracks; }
private:
	void Clear();
	int GetTrackBeginPos(const TCHAR *szTrackName);
private:
	vector<SkeletonFrame*>	m_frameBuf;
	SkeletonStruct			m_struct;
public:
	set<tstring>			m_tracks;
};

