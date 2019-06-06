#pragma once
#include "..\Util\RTTIObj.h"
#include "..\Cool3D.h"
#include "..\ResSys\AniMsgTable.h"
#include "..\Public\MathBaseDefine.h"
#include "..\Math\Transform.h"

namespace Cool3D
{
	class ResBase;
	class ResSkeleton;
	class ResKeyFrame;
	class AnimationState;
	class NodeAnimationTrack;
	class VertexAnimationTrack;
	class VertStream;
	class NodeAnimationMixer;
	class VertexAnimationMixer;

	enum BodyPartType
	{
		BPT_All		= 0,	// 全身
		BPT_Upper,			// 上身
		BPT_Lower,			// 下身
		BPT_Head,			// 头部
		BPT_UpperOrigin,	// 上身根节点
		BPT_HeadOrigin		// 头部根节点
	};

	struct Cool3D_API tagTagNodeSpace
	{
		DWORD		name;
		Transform	trans;

		void LerpTrans(const Vector3& pos1,const Quaternion& rotat1,
			const Vector3& pos2,const Quaternion& rotat2,float r,bool bMirrored)
		{
			Vector3 pos;
			Quaternion rotat;

			D3DXVec3Lerp(&pos,&pos1,&pos2,r);
			D3DXQuaternionSlerp(&rotat,&rotat1,&rotat2,r);

			trans.LoadIdentity();
			trans.RotateQuat(rotat);
			trans.Translate(pos);

			if(bMirrored)
				trans.Scale(-1,-1,-1);
		}
	};	

	/**	\class AnimationCtrl
		\brief 动画控制器基类
		\remarks
			提供一些控制器基本的功能
	*/
	class Cool3D_API AnimationCtrl : public RTTIObj
	{
	public:
		AnimationCtrl(void);
		virtual ~AnimationCtrl(void);

		virtual void		BindAnimationRes(ResBase *pRes) = 0;
		virtual const ResBase*	GetRes(void) = 0;
		/**	打开一个动画
			\param timePos，动画开始播放的时间点，-1为从最后一帧开始播放
		*/
		virtual bool		OpenTrack(const TCHAR *szName,bool loop,float timePos=0.0f,float fadeinTime=0.0f,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f) = 0;
		virtual void		CloseTrack(const TCHAR *szName) = 0;
		virtual void		ClearTrack(void) = 0;
		virtual const AniMsgTable* GetAniMsgTable(void) const = 0;
		virtual void		UpdateRes(void) = 0;
		virtual void		Update(void) = 0;
		virtual void		ApplyTime(float offset) = 0;
		virtual void		GetTrackSet(vector<tstring>& trackNames) = 0;
		virtual DWORD		PeekFrameMsg(void) = 0;

		bool				IsTrackEnd(const TCHAR *szName) const;
		/** 获得某个动作的当前运行时间
			\ret 如果返回值<0.0f，表示动作不存在
		*/
		float				GetTrackPosition(const TCHAR *szName) const;
		void				SetTimeFactor(float factor) { m_timeFactor=factor; }
		void				SetEnable(const TCHAR *szName,bool enabled);
		bool				IsFadeinTime(const TCHAR *szName) const;

		typedef std::map<DWORD,AnimationState*> AnimationStateList;

	protected:
		struct tagCmd 
		{
			DWORD			name;
			bool			loop;
			float			timePos;
			float			fadeinTime;
			BodyPartType	partType;
			bool			isBlending;
			float			weight;
		};
		virtual void		ExecuteCmd(const tagCmd& cmd) = 0;
		virtual bool		TrackIsReady(DWORD dwTrackName) = 0;

	protected:
		float				m_timeFactor;		//时间因子
		AnimationStateList	m_stateList;
		float				m_lastUpdateTime;

		typedef std::list<tagCmd> CacheCmdList;
		CacheCmdList		m_cacheCmd;


		DECL_RTTI(AnimationCtrl);
	};

	/**	\class NodeAnimationCtrl
		\brief 节点动画控制器
		\remarks
			控制骨骼动画，在AvatarNode中调用此类进行动画控制
	*/
	class Cool3D_API NodeAnimationCtrl : public AnimationCtrl
	{
		friend class NodeAnimationMixer;

	public:
		NodeAnimationCtrl(void);
		virtual ~NodeAnimationCtrl(void);

		//--AnimationCtrl
		virtual void	BindAnimationRes(ResBase *pRes);
		virtual const ResBase*	GetRes(void) { return (ResBase*)m_pRes; }
		virtual bool	OpenTrack(const TCHAR *szName,bool loop,float timePos=0.0f,float fadeinTime=0.0f,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f);
		virtual void	CloseTrack(const TCHAR *szName);
		virtual void	ClearTrack(void);
		virtual const AniMsgTable* GetAniMsgTable(void) const;
		virtual void	UpdateRes(void);
		virtual void	Update(void);
		virtual void	ApplyTime(float offset);
		virtual void	GetTrackSet(vector<tstring>& trackNames);
		virtual DWORD	PeekFrameMsg(void);

		bool			CanGetBoneSpace(void)	const;
		void			GetAllBonePos(vector<Vector3>& bonesPos) const;
		void			GetAllBoneMat(vector<Matrix4>& bonesMat) const;
		const Matrix4*	GetBoneSpace(UINT boneID) const;
		const Matrix4*	GetBoneSpace(const TCHAR* szBoneName) const;
		bool			BoneIsMirrored(UINT boneID) const;
		void			DrawSkeleton(void) const;
		void			RotateUpper(float yaw,float pitch,float roll);
		void			RotateUpperYaw(float yaw)	{ m_upperYaw = yaw; }
		void			RotateHead(float yaw,float pitch,float roll);
		void			RotateHeadYaw(float yaw)	{ m_headYaw = yaw; }
		void			RotateHeadPitch(float pitch){ m_headPitch = pitch; }
		float			GetUpperYaw(void)	const	{ return m_upperYaw; }
		float			GetUpperPitch(void) const	{ return m_upperPitch; }
		float			GetUpperRoll(void)	const	{ return m_upperRoll; }
		float			GetHeadYaw(void)	const	{ return m_headYaw; }
		float			GetHeadPitch(void)	const	{ return m_headPitch; }
		float			GetHeadRoll(void)	const	{ return m_headRoll; }
		void			SyncTrack(const TCHAR *szName);
		bool			IsBoneType(UINT boneID,BodyPartType partType) const;

		typedef std::map<DWORD, NodeAnimationTrack*> NodeTrackList;
		typedef std::vector<DWORD> BoneTypeList;
		typedef std::set<DWORD> ActiveTrackList;

	protected:
		virtual void	ExecuteCmd(const tagCmd& cmd);
		virtual bool	TrackIsReady(DWORD dwTrackName);

	private:
		void			GetBoneChildren(UINT boneID,vector<UINT>& children) const;

	protected:
		ResSkeleton			*m_pRes;
		bool				m_resIsReady;
		NodeTrackList		m_nodeTrackList;
		BoneTypeList		m_boneTypeList;

	private:
		ActiveTrackList		m_activeTracks;
		float				m_upperYaw;
		float				m_upperPitch;
		float				m_upperRoll;
		float				m_headYaw;
		float				m_headPitch;
		float				m_headRoll;

		NodeAnimationMixer	*m_pMixer;


		DECL_RTTI(NodeAnimationCtrl);
	};

	/**	\class VertexAnimationCtrl
		\brief 顶点动画控制器
		\remarks
			控制顶点变形动画，在EntityNode中调用此类进行动画控制
			关键帧动画应该不需要同时存在多个活动Track，待改进
	*/
	class Cool3D_API VertexAnimationCtrl : public AnimationCtrl
	{
	public:
		VertexAnimationCtrl(void);
		virtual ~VertexAnimationCtrl(void);

		//-AnimationCtrl
		virtual void		BindAnimationRes(ResBase *pRes);
		virtual const ResBase*	GetRes(void) { return (ResBase*)m_pRes; }
		virtual bool		OpenTrack(const TCHAR *szName,bool loop,float timePos=0.0f,float fadeinTime=0.0f,BodyPartType partType=BPT_All,bool isBlending=false,float weight=1.0f);
		virtual void		CloseTrack(const TCHAR *szName);
		virtual void		ClearTrack(void);
		virtual const AniMsgTable* GetAniMsgTable(void) const;
		virtual void		UpdateRes(void);
		virtual void		Update(void);
		virtual void		ApplyTime(float offset);
		virtual void		GetTrackSet(vector<tstring>& trackNames);
		virtual DWORD		PeekFrameMsg(void);

		VertStream*			GetFrameVertStream(void) const;
		bool				GetTagSpace(const TCHAR* szTagName,Matrix4& out);
		const vector<tagTagNodeSpace>& GetTagNodes() const { return m_tagNodes; }

		typedef std::map<DWORD, VertexAnimationTrack*> VertexTrackList;

	protected:
		virtual void		ExecuteCmd(const tagCmd& cmd);
		virtual bool		TrackIsReady(DWORD dwTrackName);

	protected:
		ResKeyFrame				*m_pRes;
		bool					m_resIsReady;
		VertexTrackList			m_vertexTrackList;

	private:
		bool					m_hasActiveTrack;
		DWORD					m_activeTrack;
		vector<tagTagNodeSpace>	m_tagNodes;			//存储当前插值出来的tag node


		DECL_RTTI(VertexAnimationCtrl);
	};
}//namespace Cool3D