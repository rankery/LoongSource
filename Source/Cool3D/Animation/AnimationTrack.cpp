#include "stdafx.h"
#include "AnimationTrack.h"
#include "KeyFrame.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\ResSys\ResKeyFrame.h"
#include "..\ResSys\VertStream.h"
#include "AnimationCtrl.h"

namespace Cool3D
{
	namespace 
	{
		struct KeyFrameTimeLess
		{
			bool operator() ( const KeyFrame* kf,const KeyFrame* kf2 ) const
			{
				return kf->m_time<kf2->m_time;
			}
		};
	}

	//-----------------------class AnimationTrack---------------------------------
	AnimationTrack::AnimationTrack( DWORD name,BodyPartType partType,bool isBlending,float weight )
		: m_name(name)
		, m_partType(partType)
		, m_weight(weight)
		, m_animationLength(0.0f)
		, m_isBlending(isBlending)
	{
		
	}

	AnimationTrack::~AnimationTrack( void )
	{
		m_keyFrames.clear();
		m_keyFrameTimes.clear();
	}

	UINT AnimationTrack::GetNumKeyFrames( void ) const
	{
		return m_keyFrames.size();
	}

	KeyFrame* AnimationTrack::GetKeyFrame( UINT index ) const
	{
		if( index >= m_keyFrames.size() )
		{
			LOG( _T("Animation Error: index %d > frames size %d\r\n"), index, m_keyFrames.size() );
		}

		return m_keyFrames[index];
	}

	float AnimationTrack::GetKeyFramesAtTime( const TimeIndex& timeIndex,bool bLoop,KeyFrame** keyFrame1,KeyFrame** keyFrame2 ) const
	{
		float t1;	//前一帧的时间
		float t2;	//下一帧的时间

		float timePos=timeIndex.GetTimePos();

		//--寻找在timePos时间点上前后两帧
		UINT i;
		if( timeIndex.HasKeyIndex() )
		{
			i=timeIndex.GetKeyIndex();
		}
		else
		{
			if( timePos > m_animationLength )
				timePos = m_animationLength;

			KeyFrameTimeList::const_iterator it=std::lower_bound(m_keyFrameTimes.begin(),m_keyFrameTimes.end()-1,timePos);
			i=std::distance(m_keyFrameTimes.begin(),it);
		}

		if( i >= m_keyFrames.size() )
		{
			i = m_keyFrames.size()-1;
			LOG( _T("Animation Error: timeIndex %d > frames size %d\r\n"), timeIndex.GetKeyIndex(), m_keyFrames.size() );
		}

		*keyFrame2=*(m_keyFrames.begin()+i);
		t2=m_keyFrameTimes[i];

		if( i==0 )	//当前时间点在第一帧
		{
			// 如果是循环播放，则前一帧是动画的最后一帧
			if( bLoop )
			{
				timePos+=m_animationLength;
				t2+=m_animationLength;
				i=m_keyFrameTimes.size()-1;
			}
		}
		else
			--i;

		*keyFrame1=*(m_keyFrames.begin()+i);

		t1=m_keyFrameTimes[i];

		if( t1==t2 )
		{
			//--相同帧
			return 0.0;
		}
		else
		{
			return (timePos-t1)/(t2-t1);
		}
	}

	TimeIndex AnimationTrack::GetTimeIndex( float timePos ) const
	{
		if( timePos > m_animationLength )
			timePos = m_animationLength;

		if( m_keyFrameTimes.size() > 0 )
		{
			KeyFrameTimeList::const_iterator it=
				std::lower_bound(m_keyFrameTimes.begin(),m_keyFrameTimes.end()-1,timePos);

			return TimeIndex(timePos,std::distance(m_keyFrameTimes.begin(),it));
		}
		else
			return TimeIndex(timePos,0);
	}
	//-----------------------class AnimationTrack---------------------------------

	//-----------------------class NodeAnimationTrack-----------------------------
	NodeAnimationTrack::NodeAnimationTrack( const NodeAnimationCtrl *pCtrl,DWORD name,BodyPartType partType,bool isBlending,float weight )
		: AnimationTrack(name,partType,isBlending,weight)
		, m_pCtrl(pCtrl)
		, m_pRes(0)
		, m_pLowerPreKeyFrame(0)
		, m_pUpperPreKeyFrame(0)
	{

	}

	NodeAnimationTrack::~NodeAnimationTrack( void )
	{
		m_pRes=NULL;
		m_pCtrl=NULL;
		SAFE_DELETE(m_pLowerPreKeyFrame);
		SAFE_DELETE(m_pUpperPreKeyFrame);
	}

	void NodeAnimationTrack::CreateKeyFrames( ResBase *pRes )
	{
		if( pRes == NULL )
		{
			LOG( _T("Animation Error: Res pointer is null\r\n") );
		}

		m_pRes=static_cast<ResSkeleton*>(pRes);

		//--获得帧序列
		int firstFrm,lastFrm;
		if( !m_pRes->GetTrackRange(m_name,firstFrm,lastFrm) )
		{
			TCHAR szError[128];
			_stprintf_s( szError, 100, _T("Animation Error: can't find track name %s!"), _FourCC2Str(m_name).c_str() );
			::MessageBox(NULL, szError, _T("error"), MB_OK);
		}
		if( firstFrm > lastFrm )
		{
			TCHAR szError[128];
			_stprintf_s( szError, 100, _T("Animation Error: first %d > last %d track name %s!"), firstFrm, lastFrm, _FourCC2Str(m_name).c_str() );
			::MessageBox(NULL, szError, _T("error"), MB_OK);
		}

		m_animationLength=0.0f;
		m_keyFrames.clear();
		m_keyFrameTimes.clear();
		for(int i=firstFrm;i<=lastFrm;i++)
		{
			SkeletonKeyFrame *pskf=m_pRes->GetFrame(i);
			m_keyFrames.push_back(pskf);

			m_animationLength+=pskf->m_time;
			m_keyFrameTimes.push_back(m_animationLength);
		}

		//--动画时间
		if( m_pRes->GetTrackTimeFromRes(m_name) <= 0.0f )//增加一个判断动画时间是否有效的log
		{
			LOG( _T("Animation Error: Invalid animation length %f, Track name:%s\r\n"), m_animationLength, _FourCC2Str(m_name).c_str() );
		}

		//--初始化m_currentKeyFrame
		const SkeletonKeyFrame *pskf=m_pRes->GetFrame(firstFrm);
		m_currentKeyFrame.m_name=m_name;
		m_currentKeyFrame.m_boneArray.resize(pskf->m_boneArray.size());
		for(UINT i=0;i<pskf->m_boneArray.size();i++)
		{
			m_currentKeyFrame.m_boneArray[i]=pskf->m_boneArray[i];
		}
		m_currentKeyFrame.m_tagNodes=pskf->m_tagNodes;

		//--初始化m_boneTrans
		m_boneTrans.resize(pskf->m_boneArray.size());
		const SkeletonStruct& sk=m_pRes->GetStruct();
		for(UINT i=0;i<m_pRes->GetNumBone();i++)
		{
			tagBoneSpace& tagbs=m_boneTrans[i];

			tagbs.parentID=sk.m_boneStruct[i].parentID;
			tagbs.children=sk.m_boneStruct[i].children;
		}
	}

	void NodeAnimationTrack::SetLowerPreKeyFrame( const SkeletonKeyFrame* pPreKeyFrame )
	{
		SAFE_DELETE(m_pLowerPreKeyFrame);
		if( pPreKeyFrame != NULL )
		{
			m_pLowerPreKeyFrame = new SkeletonKeyFrame(*pPreKeyFrame);
		}
	}

	void NodeAnimationTrack::SetUpperPreKeyFrame( const SkeletonKeyFrame* pPreKeyFrame )
	{
		SAFE_DELETE(m_pUpperPreKeyFrame);
		if( pPreKeyFrame != NULL )
		{
			m_pUpperPreKeyFrame = new SkeletonKeyFrame(*pPreKeyFrame);
		}
	}

	void NodeAnimationTrack::GetInterpolatedKeyFrame( const TimeIndex& timeIndex,KeyFrame* kf,bool bLoop ) const
	{
		SkeletonKeyFrame* kret=static_cast<SkeletonKeyFrame*>(kf);

		KeyFrame *kBase1,*kBase2;
		SkeletonKeyFrame *k1,*k2;

		float t=this->GetKeyFramesAtTime(timeIndex, bLoop, &kBase1, &kBase2);
		k1=static_cast<SkeletonKeyFrame*>(kBase1);
		k2=static_cast<SkeletonKeyFrame*>(kBase2);

		if( t==0.0 )
		{
			//--使用第一帧
			*kret=*k1;
		}
		else
		{
			//--用t进行插值
			for(UINT i=0;i<kret->m_boneArray.size();i++)
			{
				if( m_pCtrl->IsBoneType(i,m_partType) )
				{
					Bone& bret=kret->m_boneArray[i];
					const Bone& b1=k1->m_boneArray[i];
					const Bone& b2=k2->m_boneArray[i];

					D3DXVec3Lerp(&bret.m_trans,&b1.m_trans,&b2.m_trans,t);
					D3DXQuaternionSlerp(&bret.m_rotat,&b1.m_rotat,&b2.m_rotat,t);
				}
			}
		}
	}

	void NodeAnimationTrack::Apply( const TimeIndex& timeIndex,bool bLoop,bool IsFadein,float t )
	{
		if( m_keyFrames.empty() )
			return;

		if( IsFadein )//动作过渡
		{
			if( m_pLowerPreKeyFrame!=NULL 
				&& m_pUpperPreKeyFrame!=NULL
				&& m_pLowerPreKeyFrame->m_name==m_pUpperPreKeyFrame->m_name )
			{
				SkeletonKeyFrame* firstKf=static_cast<SkeletonKeyFrame*>(m_keyFrames[0]);

				for(UINT i=0;i<firstKf->m_boneArray.size();i++)
				{
					if( m_pCtrl->IsBoneType(i,m_partType) )
					{
						Bone& bret=m_currentKeyFrame.m_boneArray[i];
						const Bone& b1=m_pLowerPreKeyFrame->m_boneArray[i];
						const Bone& b2=firstKf->m_boneArray[i];

						D3DXVec3Lerp(&bret.m_trans,&b1.m_trans,&b2.m_trans,t);
						D3DXQuaternionSlerp(&bret.m_rotat,&b1.m_rotat,&b2.m_rotat,t);
					}
				}

				return;
			}

			if( m_pLowerPreKeyFrame!=NULL
				&& m_pLowerPreKeyFrame->m_boneArray.size()>0 )//进行下身动作过渡
			{
				if( m_pUpperPreKeyFrame==NULL
					|| m_pUpperPreKeyFrame->m_boneArray.size()==0 )
				{
					GetInterpolatedKeyFrame(timeIndex,&m_currentKeyFrame,bLoop);
				}

				SkeletonKeyFrame* firstKf=static_cast<SkeletonKeyFrame*>(m_keyFrames[0]);

				for(UINT i=0;i<firstKf->m_boneArray.size();i++)
				{
					if( m_pCtrl->IsBoneType(i,m_partType)
						&& m_pCtrl->IsBoneType(i,BPT_Lower) )
					{
						Bone& bret=m_currentKeyFrame.m_boneArray[i];
						const Bone& b1=m_pLowerPreKeyFrame->m_boneArray[i];
						const Bone& b2=firstKf->m_boneArray[i];

						D3DXVec3Lerp(&bret.m_trans,&b1.m_trans,&b2.m_trans,t);
						D3DXQuaternionSlerp(&bret.m_rotat,&b1.m_rotat,&b2.m_rotat,t);
					}
				}
			}

			if( m_pUpperPreKeyFrame!=NULL
				&& m_pUpperPreKeyFrame->m_boneArray.size()>0 )//进行上身动作过渡
			{
				if( m_pLowerPreKeyFrame==NULL
					|| m_pLowerPreKeyFrame->m_boneArray.size()==0 )
				{
					GetInterpolatedKeyFrame(timeIndex,&m_currentKeyFrame,bLoop);
				}

				SkeletonKeyFrame* firstKf=static_cast<SkeletonKeyFrame*>(m_keyFrames[0]);

				for(UINT i=0;i<firstKf->m_boneArray.size();i++)
				{
					if( m_pCtrl->IsBoneType(i,m_partType)
						&& m_pCtrl->IsBoneType(i,BPT_Upper) )
					{
						Bone& bret=m_currentKeyFrame.m_boneArray[i];
						const Bone& b1=m_pUpperPreKeyFrame->m_boneArray[i];
						const Bone& b2=firstKf->m_boneArray[i];

						D3DXVec3Lerp(&bret.m_trans,&b1.m_trans,&b2.m_trans,t);
						D3DXQuaternionSlerp(&bret.m_rotat,&b1.m_rotat,&b2.m_rotat,t);
					}
				}
			}
		}
		else
			GetInterpolatedKeyFrame(timeIndex,&m_currentKeyFrame,bLoop);
	}

	const Transform& NodeAnimationTrack::GetBoneSpace( UINT boneID ) const
	{
		if( boneID >= m_boneTrans.size() )
		{
			LOG( _T("Animation Error: Bone id %d more than skeleton bones size %d\r\n"), boneID, m_boneTrans.size() );
		}

		return m_boneTrans[boneID].toWorld;
	}

	void NodeAnimationTrack::GetBoneChildren( UINT boneID,vector<UINT>& children )
	{
		if( boneID >= m_boneTrans.size() )
		{
			LOG( _T("Animation Error: Bone id %d more than skeleton bones size %d\r\n"), boneID, m_boneTrans.size() );
		}

		children=m_boneTrans[boneID].children;
	}

	void NodeAnimationTrack::CalcBoneSpace( void )
	{
		const SkeletonStruct& sk=m_pRes->GetStruct();

		for( UINT i=0;i<sk.m_rootBones.size();i++ )
		{
			CalcBoneSpaceR(sk.m_rootBones[i]);
		}
	}

	void NodeAnimationTrack::CalcBoneSpaceR( UINT boneID )
	{
		tagBoneSpace& tagbs=m_boneTrans[boneID];
		Bone& b=m_currentKeyFrame.m_boneArray[boneID];

		tagbs.toWorld.LoadIdentity();
		tagbs.toWorld.RotateQuat(b.m_rotat);
		tagbs.toWorld.Translate(b.m_trans);
		if( tagbs.parentID!=-1 )
			tagbs.toWorld.CombineTransRight(m_boneTrans[tagbs.parentID].toWorld.GetMatrix());
		if(b.m_bMirrored)
			tagbs.toWorld.Scale(-1,-1,-1);

		for( UINT i=0;i<tagbs.children.size();i++ )
		{
			CalcBoneSpaceR(tagbs.children[i]);
		}
	}
	//-----------------------class NodeAnimationTrack-----------------------------

	//-----------------------class VertexAnimationTrack---------------------------
	VertexAnimationTrack::VertexAnimationTrack( DWORD name,BodyPartType partType,bool isBlending,float weight )
		: AnimationTrack(name,partType,isBlending,weight)
		, m_pRes(0)
		, m_pPreKeyFrame(0)
		, m_firstFrm(0)
		, m_lastFrm(0)
		, m_pTagNodes(0)
	{

	}

	VertexAnimationTrack::~VertexAnimationTrack( void )
	{
		m_pRes=NULL;
		m_pPreKeyFrame=NULL;
		m_pTagNodes=NULL;
	}

	void VertexAnimationTrack::CreateKeyFrames( ResBase *pRes )
	{
		if( pRes == NULL )
		{
			LOG( _T("Animation Error: Res pointer is null\r\n") );
		}

		m_pRes=static_cast<ResKeyFrame*>(pRes);

		//--获得帧序列
		if( m_pRes->GetTrackRange(m_name,m_firstFrm,m_lastFrm)!=0 )
			return;

		m_animationLength=0.0f;
		for(int i=m_firstFrm;i<=m_lastFrm;i++)
		{
			VertexKeyFrame *pvkf=m_pRes->GetFrame(i);
			m_keyFrames.push_back(pvkf);

			m_animationLength+=pvkf->m_time;
			m_keyFrameTimes.push_back(m_animationLength);
		}

		//--初始化m_currentKeyFrame
		VertexKeyFrame *pvkf=m_pRes->GetFrame(m_firstFrm);
		m_currentKeyFrame.m_pStream=pvkf->m_pStream->Clone(true);
		m_currentKeyFrame.m_index=pvkf->m_index;
	}

	void VertexAnimationTrack::Apply( const TimeIndex& timeIndex,bool bLoop,bool IsFadein,float t )
	{
		if( m_keyFrames.empty() )
			return;

		if( IsFadein
			&& m_pPreKeyFrame!=NULL
			&& m_pPreKeyFrame->m_pStream!=NULL )//动作过渡
		{
			const VertexKeyFrame *vkf1=m_pPreKeyFrame;
			const VertexKeyFrame *vkf2=static_cast<const VertexKeyFrame*>(m_keyFrames[0]);

			//--对顶点信息(pos,normal)进行插值
			int count=m_currentKeyFrame.m_pStream->GetBufferSize();
			const float *pSrcf=(const float*)vkf1->m_pStream->GetRawBuffer();
			const float *pDestf=(const float*)vkf2->m_pStream->GetRawBuffer();
			float *pBuf=(float*)(m_currentKeyFrame.m_pStream->GetRawBuffer());
			LinearLerp_SSE(pBuf,pSrcf,pDestf,t,count/sizeof(float));

			//--挂载点插值
			InterpolateTagNodes(vkf1->m_index,vkf2->m_index,t);
		}
		else
		{
			GetInterpolatedKeyFrame(timeIndex,&m_currentKeyFrame,bLoop);
		}
	}

	void VertexAnimationTrack::GetInterpolatedKeyFrame( const TimeIndex& timeIndex,KeyFrame* kf,bool bLoop ) const
	{
		VertexKeyFrame* kret=static_cast<VertexKeyFrame*>(kf);
		KeyFrame *kf1, *kf2;
		float t=GetKeyFramesAtTime(timeIndex, bLoop, &kf1, &kf2);

		VertexKeyFrame* vkf1=static_cast<VertexKeyFrame*>(kf1);
		VertexKeyFrame* vkf2=static_cast<VertexKeyFrame*>(kf2);
		kret->m_index = vkf2->m_index;

		//--在最后一帧和第一帧进行插值时，对UV插值要进行特殊处理
		if( (DWORD)vkf2 == (DWORD)m_keyFrames[0] )
		{
			int numVert=kret->m_pStream->GetNumVert();
			int stride=kret->m_pStream->GetStride();
			BYTE *pSrcB=(BYTE*)vkf1->m_pStream->GetRawBuffer();
			BYTE *pDestB=(BYTE*)vkf2->m_pStream->GetRawBuffer();
			BYTE *pBuf=(BYTE*)(kret->m_pStream->GetRawBuffer());
			ASSERT( (kret->m_pStream->GetType()==EVType_PNT) || (kret->m_pStream->GetType()==EVType_PNT2) || (kret->m_pStream->GetType()==EVType_PNGT) );

			Vector3 *pPos=NULL;
			Vector3 *pNormal=NULL;
			int i;
			for(i=0;i<numVert;i++)
			{
				//--Pos
				pPos=(Vector3*)pBuf;
				D3DXVec3Lerp(pPos,(Vector3*)pSrcB,(Vector3*)pDestB,t);

				//--Normal
				pNormal=pPos+1;
				D3DXVec3Lerp(pNormal,(Vector3*)pSrcB+1,(Vector3*)pDestB+1,t);
				D3DXVec3Normalize(pNormal,pNormal);

				//--UV
				*(Vector2*)(pNormal+1)=*(Vector2*)(pDestB+24);

				pBuf+=stride;
				pSrcB+=stride;
				pDestB+=stride;
			}
		}
		else
		{
			//--对顶点信息(pos,normal)进行插值
			int count=kret->m_pStream->GetBufferSize();
			const float *pSrcf=(const float*)vkf1->m_pStream->GetRawBuffer();
			const float *pDestf=(const float*)vkf2->m_pStream->GetRawBuffer();
			float *pBuf=(float*)(kret->m_pStream->GetRawBuffer());
			LinearLerp_SSE(pBuf,pSrcf,pDestf,t,count/sizeof(float));
		}

		//--挂载点插值
		InterpolateTagNodes(vkf1->m_index,vkf2->m_index,t);
	}

	void VertexAnimationTrack::InterpolateTagNodes( int pre,int next,float t ) const
	{
		if( m_pTagNodes!=NULL )
		{
			const vector<TagNodeFrame>& tagFrameArray=m_pRes->GetTagFrameArray();
			const TagNodeFrame& tagSrc=tagFrameArray[pre];
			const TagNodeFrame& tagDst=tagFrameArray[next];
			int numTag=m_pTagNodes->size();
			ASSERT(numTag==tagSrc.nodes.size());
			for(int i=0;i<numTag;i++)
			{
				const tagTagNode& src=tagSrc.nodes[i];
				const tagTagNode& dst=tagDst.nodes[i];
				(*m_pTagNodes)[i].LerpTrans(src.trans,src.rotat,dst.trans,dst.rotat,
					t,src.bMirrored);
			}
		}
	}
	//-----------------------class VertexAnimationTrack---------------------------
}//namespace Cool3D