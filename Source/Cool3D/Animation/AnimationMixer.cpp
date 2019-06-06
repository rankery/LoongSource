#include "stdafx.h"
#include "AnimationMixer.h"
#include "..\ResSys\ResSkeleton.h"

namespace Cool3D
{
	//-----------------------class AnimationBone-------------------------------
	void AnimationBone::Blend( float weight,const Vector3& trans,const Quaternion& rotat )
	{
		if( fabsf(m_accumulatedWeight)<1.0e-6f )
		{
			m_trans=trans;
			m_rotat=rotat;

			m_accumulatedWeight=weight;
		}
		else
		{
			float factor;
			factor=weight/(m_accumulatedWeight+weight);
			D3DXVec3Lerp(&m_trans,&m_trans,&trans,factor);
			D3DXQuaternionSlerp(&m_rotat,&m_rotat,&rotat,factor);

			m_accumulatedWeight+=weight;
		}
	}
	//-----------------------class AnimationBone-------------------------------


	//-----------------------class AnimationMixer------------------------------
	AnimationMixer::AnimationMixer( AnimationCtrl *pCtrl )
		: m_pCtrl(pCtrl)
		, m_resIsReady(false)
	{
		ASSERT( m_pCtrl!=NULL );
	}

	AnimationMixer::~AnimationMixer( void )
	{
		m_pCtrl=NULL;
	}
	//-----------------------class AnimationMixer------------------------------

	//-----------------------class NodeAnimationMixer--------------------------
	NodeAnimationMixer::NodeAnimationMixer( NodeAnimationCtrl *pCtrl )
		: AnimationMixer(pCtrl)
	{
		UpdateRes();
	}

	NodeAnimationMixer::~NodeAnimationMixer( void )
	{
		m_tracks.clear();
		m_boneArray.clear();
	}

	void NodeAnimationMixer::UpdateRes( void )
	{
		if( !m_resIsReady )
		{
			const NodeAnimationCtrl *pCtrl=(const NodeAnimationCtrl*)m_pCtrl;
			if( pCtrl->m_pRes!=NULL
				&& pCtrl->m_pRes->IsCreated() )
			{
				SkeletonKeyFrame *pskf=pCtrl->m_pRes->GetFirstFrame();
				m_boneArray.resize(pskf->m_boneArray.size());
				for(UINT i=0;i<pskf->m_boneArray.size();i++)
				{
					AnimationBone& aniBone=m_boneArray[i];
					Bone& bone=pskf->m_boneArray[i];
					aniBone.m_trans=bone.m_trans;
					aniBone.m_rotat=bone.m_rotat;
					aniBone.m_bMirrored=bone.m_bMirrored;
				}

				m_boneTrans.resize(pskf->m_boneArray.size());
				const SkeletonStruct& sk=pCtrl->m_pRes->GetStruct();
				for(UINT i=0; i<pCtrl->m_pRes->GetNumBone(); i++)
				{
					tagBoneSpace& tagbs=m_boneTrans[i];

					tagbs.parentID=sk.m_boneStruct[i].parentID;
					tagbs.children=sk.m_boneStruct[i].children;
				}

				m_resIsReady=true;
			}
		}
	}

	void NodeAnimationMixer::Reset( void )
	{
		m_tracks.clear();
		for( UINT i=0; i<m_boneArray.size(); i++ )
		{
			AnimationBone& bone=m_boneArray[i];
			bone.m_accumulatedWeight=0.0f;
		}
	}

	void NodeAnimationMixer::Update( void )
	{
		UpdateRes();

		if( !m_resIsReady )
			return;

		TrackMap::iterator tracki;
		for( tracki=m_tracks.begin(); tracki!=m_tracks.end(); ++tracki )
		{
			const NodeAnimationTrack *pTrack=tracki->first;
			const SkeletonKeyFrame* pKF=pTrack->GetCurKeyFrame();
			ASSERT( pKF!=NULL );

			for( UINT bonei=0; bonei<pKF->m_boneArray.size(); bonei++ )
			{
				const Bone& bone=pKF->m_boneArray[bonei];
				const NodeAnimationCtrl *pCtrl=static_cast<const NodeAnimationCtrl*>(m_pCtrl);
				if( pCtrl->IsBoneType(bonei,pTrack->GetPartType()) )
				{
					m_boneArray[bonei].Blend(pTrack->GetWeight()*tracki->second,bone.m_trans,bone.m_rotat);
				}
			}
		}

		CalcBoneSpace();
	}

	void NodeAnimationMixer::AddTrack( const NodeAnimationTrack *pTrack, float t )
	{
		m_tracks.insert(make_pair(pTrack,t));
	}

	bool NodeAnimationMixer::CanGetBoneSpace( void ) const
	{
		return m_boneTrans.size() > 0;
	}

	void NodeAnimationMixer::GetAllBonePos( vector<Vector3>& bonesPos ) const
	{
		for( size_t i=0; i<m_boneTrans.size(); i++ )
		{
			const Matrix4* pMat = m_boneTrans[i].toWorld.GetMatrix();
			Vector3 pos(pMat->_41,pMat->_42,pMat->_43);
			bonesPos.push_back( pos );
		}
	}

	void NodeAnimationMixer::GetAllBoneMat( vector<Matrix4>& bonesMat ) const
	{
		for( size_t i=0; i<m_boneTrans.size(); i++ )
		{
			const Matrix4* pMat = m_boneTrans[i].toWorld.GetMatrix();
			bonesMat.push_back( *pMat );
		}
	}

	const Matrix4* NodeAnimationMixer::GetBoneSpace( UINT boneID ) const
	{
		if( m_boneTrans.size()==0 )
		{
			return NULL;
		}

		ASSERT(boneID<m_boneTrans.size() && _T("Bone id more than skeleton bones size"));

		if( m_boneArray[boneID].m_accumulatedWeight>0.0f )	//返回有效的骨骼矩阵
		{
			return m_boneTrans[boneID].toWorld.GetMatrix();
		}

		return NULL;
	}

	bool NodeAnimationMixer::BoneIsMirrored( UINT boneID ) const
	{

		ASSERT(boneID<m_boneTrans.size() && _T("Bone id more than skeleton bones size"));

		return m_boneArray[boneID].m_bMirrored;
	}

	void NodeAnimationMixer::CalcBoneSpace( void )
	{
		const NodeAnimationCtrl *pCtrl=(const NodeAnimationCtrl*)m_pCtrl;
		const SkeletonStruct& sk=pCtrl->m_pRes->GetStruct();

		for( UINT i=0;i<sk.m_rootBones.size();i++ )
		{
			CalcBoneSpaceR(sk.m_rootBones[i]);
		}
	}

	void NodeAnimationMixer::CalcBoneSpaceR( DWORD boneID )
	{
		tagBoneSpace& tagbs=m_boneTrans[boneID];
		AnimationBone& b=m_boneArray[boneID];

		if( b.m_accumulatedWeight>0.0f )	//进行过混合的骨骼才进行矩阵运算
		{
			tagbs.toWorld.LoadIdentity();

			tagbs.toWorld.RotateQuat(b.m_rotat);
			tagbs.toWorld.Translate(b.m_trans);
			if( tagbs.parentID!=-1 )
				tagbs.toWorld.CombineTransRight(m_boneTrans[tagbs.parentID].toWorld.GetMatrix());
			if(b.m_bMirrored)
				tagbs.toWorld.Scale(-1,-1,-1);

			//--如果是上身原点骨骼或者头部原点骨骼，还需要进行额外的旋转
			const NodeAnimationCtrl *pCtrl=static_cast<const NodeAnimationCtrl*>(m_pCtrl);
			if( pCtrl->IsBoneType(boneID,BPT_UpperOrigin) )
			{
				float yaw=pCtrl->GetUpperYaw();
				float pitch=pCtrl->GetUpperPitch();
				float roll=pCtrl->GetUpperRoll();
				Quaternion upperRotate;
				D3DXQuaternionRotationYawPitchRoll(&upperRotate,pitch,yaw,roll);//为何yaw和pitch会颠倒
				tagbs.toWorld.RotateQuat(upperRotate);
			}
			if( pCtrl->IsBoneType(boneID,BPT_HeadOrigin) )
			{
				float yaw=pCtrl->GetHeadYaw();
				float pitch=pCtrl->GetHeadPitch();
				float roll=pCtrl->GetHeadRoll();
				Quaternion headRotate;
				D3DXQuaternionRotationYawPitchRoll(&headRotate,pitch,yaw,roll);//为何yaw和pitch会颠倒
				tagbs.toWorld.RotateQuat(headRotate);
			}

			for( UINT i=0; i<tagbs.children.size(); i++ )
			{
				CalcBoneSpaceR(tagbs.children[i]);
			}
		}
	}
	//-----------------------class NodeAnimationMixer--------------------------


	//-----------------------class VertexAnimationMixer------------------------
	VertexAnimationMixer::VertexAnimationMixer( VertexAnimationCtrl *pCtrl )
		: AnimationMixer(pCtrl)
	{
		UpdateRes();
	}

	VertexAnimationMixer::~VertexAnimationMixer( void )
	{
		m_trackArray.clear();
	}

	void VertexAnimationMixer::UpdateRes( void )
	{
		
	}

	void VertexAnimationMixer::Update( void )
	{

	}

	void VertexAnimationMixer::Reset( void )
	{

	}

	void VertexAnimationMixer::AddTrack( const VertexAnimationTrack *pTrack )
	{

	}

	VertStream* VertexAnimationMixer::GetFrameVertStream( void ) const
	{
		return NULL;
	}
	//-----------------------class VertexAnimationMixer------------------------
}//namespace Cool3D