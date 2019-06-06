#include "stdafx.h"
#include "KeyFrame.h"
#include "..\ResSys\VertStream.h"

namespace Cool3D
{
	//-----------------------class KeyFrame------------------------------------
	IMP_RTTI(KeyFrame,RTTIObj);

	KeyFrame::KeyFrame( void )
		: m_time(0.0f)
	{
		
	}

	KeyFrame::KeyFrame( float time )
		: m_time(time)
	{
		
	}
	//-----------------------class KeyFrame------------------------------------

	//-----------------------class SkeletonKeyFrame----------------------------
	IMP_RTTI(SkeletonKeyFrame,KeyFrame);

	SkeletonKeyFrame::SkeletonKeyFrame( void )
	{

	}

	SkeletonKeyFrame::SkeletonKeyFrame( const SkeletonKeyFrame& rhs )
	{
		Copy(rhs);
	}

	const SkeletonKeyFrame& SkeletonKeyFrame::operator=( const SkeletonKeyFrame& rhs )
	{
		if(this==&rhs)
			return *this;

		Copy(rhs);
		return *this;
	}

	void SkeletonKeyFrame::Copy( const SkeletonKeyFrame& rhs )
	{
		this->m_time=rhs.m_time;
		this->m_name=rhs.m_name;

		this->m_boneArray.resize(rhs.m_boneArray.size());
		for(UINT i=0;i<this->m_boneArray.size();i++)
		{
			this->m_boneArray[i]=rhs.m_boneArray[i];
		}

		this->m_tagNodes=rhs.m_tagNodes;
	}
	//-----------------------class SkeletonKeyFrame----------------------------

	//-----------------------class VertexKeyFrame------------------------------
	IMP_RTTI(VertexKeyFrame,KeyFrame);

	VertexKeyFrame::VertexKeyFrame( void )
		: m_pStream(0)
	{
		
	}

	VertexKeyFrame::~VertexKeyFrame( void )
	{
		SAFE_DELETE(m_pStream);
	}
	//-----------------------class VertexKeyFrame------------------------------
}//namespace Cool3D