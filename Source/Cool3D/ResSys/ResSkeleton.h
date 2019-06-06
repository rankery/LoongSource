#pragma once
#include "..\Cool3D.h"
#include "Resrenderable.h"
#include "..\Util\FArchive.h"
#include "..\Math\Math.h"
#include "..\VFS\IFS.h"
#define VFS_READ
#include "..\Public\SkinBoneDefine.h"
#include "AniMsgTable.h"
#include "ActTimeTable.h"
#include "..\Util\FileName.h"
#include "..\Animation\KeyFrame.h"

namespace Cool3D
{
	class ResSkeletonAction;

	const DWORD SKELETONUNKNOWN		= 0x00000000;
	const DWORD SKELETONUPPERPART	= 0x00000001;
	const DWORD SKELETONLOWERPART	= 0x00000010;
	const DWORD SKELETONHEADPART	= 0x00000100;
	const DWORD SKELETONCOMMONROOT	= 0x00001000;
	const DWORD SKELETONHEADORIGIN	= 0x00010000;
	const DWORD SKELETONUPPERORIGIN = 0x00100000;
	/**	\class ResSkeleton 
		\brief 骨骼动画资源接口
		\remarks
			\par 只提供接口，骨骼动画数据保存在ResSkeletonAction中
			\par 负责Track的加载和释放
			\par 允许Track存入单独的文件中
			\par 提供动作消息表和Track时间长度表
		\see ResSkeletonAction
	*/
	class Cool3D_API ResSkeleton :	public ResRenderable
	{
		struct TrackInfo{
			int nBegin;
			int nEnd;
			int nBaseFrm;//用于帧号转换
			ResSkeletonAction* pRes;//可能被释放
			float lastOpenTime;//用于释放Res
		};
		
	public:
		ResSkeleton(const TCHAR* szName);
		virtual ~ResSkeleton(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const			{	return 0;}
		virtual RenderNode *GetRenderNode(UINT i) const	{	return NULL;}
		virtual const AABBox	*GetAABBox() const		{	return NULL;}
		virtual int GetNumMtl() const					{	return 0;}
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const	{}

		//--
		UINT GetNumFrame() const { return m_numFrame;}
		SkeletonKeyFrame* GetFirstFrame(void);
		SkeletonKeyFrame* GetFrame(UINT index);
		UINT GetNumBone() const;
		DWORD GetPartInfoOfBone(const tstring& boneName) const;
		/**	根据Bone的name查找bone的id(在数组中的index)
			\return -1为没找到
		*/
		int GetBoneID(const TCHAR* szBoneName);
		const SkeletonStruct& GetStruct() const;
		const AniMsgTable* GetMsgTable(){ return &m_msgTable;}

		float GetTrackTime( DWORD dwTrackName );
		float GetTrackTimeFromRes( DWORD dwTrackName );
		bool  GetTrackRange( DWORD dwTrackName, int& nBegin, int& nEnd ); 
		float GetFrmTime( int nFrame );

		int GetNumTrack() {	return m_mapTrackInfo.size(); }
		int GetTrackName( vector<tstring>& trackNames ) const;

		//!	返回是否有新的track被load进来了
		bool PrepareTrack(DWORD dwTrackName);
		//! 检查资源加载情况
		void UpdateTrackRes();
		//! 返回指定Track是否加载完成
		bool IsTrackLoaded(DWORD dwTrackName);
		//! 返回指定Track是否被释放
		bool IsTrackRelease(DWORD dwTrackName);

		//! 更新Track的lastOpenTime
		void UpdateTrackLastOpenTime(DWORD dwTrackName);

		//! Editor support
		void ReloadMsgTable();

	private:
		typedef struct _BonePropties
		{
			DWORD	dwBodyPart;
		} BonePropties;

		void AppendTracks(ResSkeletonAction* pRes);
		void ReleaseTracks();
		Filename MakeTrackFileName(DWORD dwTrackName);
		/** \从文件中加载骨骼公共节点名称
		*/
		void LoadBonesPropties(IFS *pFS, const TCHAR* szFileName, map<tstring, BonePropties>& bonesPropties);

		map<DWORD,TrackInfo*>	m_mapTrackInfo;	//Track信息
		AniMsgTable				m_msgTable;
		ActTimeTable			m_actTimeTable;
		UINT					m_numFrame;

		map<tstring, BonePropties>	m_bonesPropties;

		ResSkeletonAction*		m_pFirstRes;//加载了骨骼数据的Res
		map<DWORD,ResSkeletonAction*> m_asyncList;//异步Res

		DECL_RTTI(ResSkeleton);
	};
}//namespace Cool3D