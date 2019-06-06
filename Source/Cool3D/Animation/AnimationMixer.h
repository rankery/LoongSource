#pragma once
#include "..\Public\SkinBoneDefine.h"
#include "AnimationCtrl.h"
#include "AnimationTrack.h"

namespace Cool3D
{
	class AnimationBone : public Bone
	{
	public:
		AnimationBone(void)
			:m_accumulatedWeight(0) 
		{}

		void Blend(float weight,const Vector3& trans,const Quaternion& rotat);

	public:
		float m_accumulatedWeight;	//记录累计权重
	};

	/**	\class AnimationMixer
		\brief 动画混合器基类
	*/
	class AnimationMixer
	{
	public:
		AnimationMixer(AnimationCtrl *pCtrl);
		virtual ~AnimationMixer(void);

		virtual void UpdateRes(void) = 0;
		virtual void Update(void) = 0;
		virtual void Reset(void) = 0;

	protected:
		const AnimationCtrl	*m_pCtrl;
		bool				m_resIsReady;
	};

	/**	\class NodeAnimationMixer
		\brief 骨骼动画混合器，用于计算骨骼矩阵，支持上下身分开、动作混合
	*/
	class NodeAnimationMixer : public AnimationMixer
	{
	public:
		NodeAnimationMixer(NodeAnimationCtrl *pCtrl);
		virtual ~NodeAnimationMixer(void);

		//--AnimationMixer
		virtual void	UpdateRes(void);
		virtual	void	Update(void);
		virtual void	Reset(void);

		void			AddTrack(const NodeAnimationTrack *pTrack,float t);
		bool			CanGetBoneSpace(void)	const;
		void			GetAllBonePos(vector<Vector3>& bonesPos) const;
		void			GetAllBoneMat(vector<Matrix4>& bonesMat) const;
		const Matrix4*	GetBoneSpace(UINT boneID) const;
		bool			BoneIsMirrored( UINT boneID ) const;

	private:
		void			CalcBoneSpace(void);
		void			CalcBoneSpaceR(DWORD boneID);
		
	private:
		struct tagBoneSpace
		{
			Transform		toWorld;
			UINT			parentID;
			vector<UINT>	children;
		};
		typedef std::vector<tagBoneSpace> BoneTransformList;
		BoneTransformList	m_boneTrans;

	private:
		typedef std::map<const NodeAnimationTrack*,float> TrackMap;
		typedef vector<AnimationBone> BoneArray;
		TrackMap	m_tracks;
		BoneArray	m_boneArray;
	};

	/**	\class VertexAnimationMixer
		\brief 顶点动画混合器
	*/
	class VertexAnimationMixer : public AnimationMixer
	{
	public:
		VertexAnimationMixer(VertexAnimationCtrl *pCtrl);
		virtual ~VertexAnimationMixer(void);

		//--AnimationMixer
		virtual void	UpdateRes(void);
		virtual	void	Update(void);
		virtual void	Reset(void);

		void			AddTrack(const VertexAnimationTrack *pTrack);
		VertStream*		GetFrameVertStream(void) const;

	private:
		vector<const VertexAnimationTrack*>	m_trackArray;
	};

}//namespace Cool3D