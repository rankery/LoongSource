#pragma once
#include "..\Cool3D.h"
#include "KeyFrame.h"
#include "AnimationCtrl.h"
#include "..\Math\Transform.h"

namespace Cool3D
{
	class ResBase;
	class ResSkeleton;

	/**	\class TimeIndex
		\brief 时间索引，用于查找给定时间点的关键帧
	*/
	class Cool3D_API TimeIndex
	{
	protected:
		float	m_timePos;
		UINT	m_keyIndex;

		static const UINT INVALID_KEY_INDEX = (UINT)-1;	//表示非关键帧索引

	public:
		TimeIndex( float timePos )
			: m_timePos(timePos)
			, m_keyIndex(INVALID_KEY_INDEX)
		{
			//
		}

		TimeIndex( float timePos,UINT keyIndex )
			: m_timePos(timePos)
			, m_keyIndex(keyIndex)
		{
			//
		}

		bool HasKeyIndex( void ) const
		{
			return m_keyIndex!=INVALID_KEY_INDEX;
		}

		float GetTimePos( void ) const
		{
			return m_timePos;
		}

		UINT GetKeyIndex( void ) const
		{
			return m_keyIndex;
		}
	};

	/**	\class AnimationTrack
		\brief 读取保存关键帧信息
	*/
	class Cool3D_API AnimationTrack
	{
	public:
		AnimationTrack(DWORD name,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f);
		virtual ~AnimationTrack(void);

		DWORD				GetName(void) const						{ return m_name; }
		void				SetPartType(BodyPartType partType)		{ m_partType=partType; }
		BodyPartType		GetPartType(void) const					{ return m_partType; }
		UINT				GetNumKeyFrames(void) const;
		KeyFrame*			GetKeyFrame(UINT index) const;
		float				GetKeyFramesAtTime(const TimeIndex& timeIndex,bool bLoop,KeyFrame** keyFrame1,KeyFrame** keyFrame2) const;
		TimeIndex			GetTimeIndex(float timePos) const;
		void				SetWeight(float weight)					{ m_weight=weight; }
		float				GetWeight(void)	const					{ return m_weight; }
		void				SetBlending(bool isBlending)			{ m_isBlending=isBlending; }
		bool				IsBlending(void) const					{ return m_isBlending; }

		virtual void		CreateKeyFrames(ResBase *pRes) = 0;
		virtual void		GetInterpolatedKeyFrame(const TimeIndex& timeIndex,KeyFrame* kf,bool bLoop) const = 0;
		virtual void		Apply(const TimeIndex& timeIndex,bool bLoop,bool IsFadein=false,float t=0.0f) = 0;

	protected:
		typedef std::vector<KeyFrame*> KeyFrameList;
		KeyFrameList		m_keyFrames;

		typedef std::vector<float> KeyFrameTimeList;
		KeyFrameTimeList	m_keyFrameTimes;

		DWORD				m_name;
		float				m_animationLength;
		BodyPartType		m_partType;
		float				m_weight;
		bool				m_isBlending;
	};

	class Cool3D_API NodeAnimationTrack : public AnimationTrack
	{
	public:
		NodeAnimationTrack(const NodeAnimationCtrl *pCtrl,DWORD name,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f);
		virtual ~NodeAnimationTrack(void);

		//--AnimationTrack
		virtual void	CreateKeyFrames(ResBase *pRes);
		virtual void	GetInterpolatedKeyFrame(const TimeIndex& timeIndex,KeyFrame* kf,bool bLoop) const;
		virtual void	Apply(const TimeIndex& timeIndex,bool bLoop,bool IsFadein=false,float t=0.0f);

		const Transform&GetBoneSpace(UINT boneID) const;
		void			GetBoneChildren(UINT boneID,vector<UINT>& children);

		const SkeletonKeyFrame* GetCurKeyFrame(void) const						{ return &m_currentKeyFrame; }
		void			SetLowerPreKeyFrame(const SkeletonKeyFrame* pPreKeyFrame);
		void			SetUpperPreKeyFrame(const SkeletonKeyFrame* pPreKeyFrame);

	private:
		void			CalcBoneSpace(void);
		void			CalcBoneSpaceR(UINT boneID);

	private:
		SkeletonKeyFrame		m_currentKeyFrame;
		ResSkeleton				*m_pRes;
		SkeletonKeyFrame		*m_pLowerPreKeyFrame;
		SkeletonKeyFrame		*m_pUpperPreKeyFrame;
		const NodeAnimationCtrl *m_pCtrl;

		struct tagBoneSpace
		{
			Transform		toWorld;
			UINT			parentID;
			vector<UINT>	children;
		};
		typedef std::vector<tagBoneSpace> BoneTransformList;
		BoneTransformList	m_boneTrans;
	};

	class Cool3D_API VertexAnimationTrack : public AnimationTrack
	{
	public:
		VertexAnimationTrack(DWORD name,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f);
		virtual ~VertexAnimationTrack(void);

		//--AnimationTrack
		virtual void		CreateKeyFrames(ResBase *pRes);
		virtual void		GetInterpolatedKeyFrame(const TimeIndex& timeIndex,KeyFrame* kf,bool bLoop) const;
		virtual void		Apply(const TimeIndex& timeIndex,bool bLoop,bool IsFadein=false,float t=0.0f);

		const VertexKeyFrame*	GetCurKeyFrame(void) const						{ return &m_currentKeyFrame; }
		void				SetPreKeyFrame(const VertexKeyFrame* pPreKeyFrame)	{ m_pPreKeyFrame=pPreKeyFrame; }
		void				SetTagNodes(vector<tagTagNodeSpace>* pTagNodes)		{ m_pTagNodes=pTagNodes; }

	private:
		void				InterpolateTagNodes(int pre,int next,float t) const;

	private:
		VertexKeyFrame			m_currentKeyFrame;
		ResKeyFrame				*m_pRes;
		const VertexKeyFrame	*m_pPreKeyFrame;
		int						m_firstFrm;
		int						m_lastFrm;
		vector<tagTagNodeSpace>	*m_pTagNodes;			//存储当前插值出来的tag node
	};
}//namespace Cool3D