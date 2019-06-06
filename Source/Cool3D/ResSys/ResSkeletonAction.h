#pragma once
#include "..\Cool3D.h"
#include "resrenderable.h"
#include "..\Util\FArchive.h"
#include "..\Math\Math.h"
#include "..\VFS\IFS.h"
#define VFS_READ
#include "..\Animation\KeyFrame.h"
#include "..\Public\SkinBoneDefine.h"

namespace Cool3D
{
	/**	\class ResSkeletonActionAction
		\brief 人物骨骼数据,含动画帧
		\see ResSkeleton,ResSkin
	*/
	class ResSkeletonAction :	public ResRenderable
	{
		struct TrackInfo{
			int nBegin;
			int nEnd;
			float fTime;
		};
	public:
		ResSkeletonAction(const TCHAR* szName);
		virtual ~ResSkeletonAction(void);

		//--ResBase
		virtual UINT CreateContent(DWORD param);

		//--ResRenderable
		virtual UINT GetNumRenderNode() const			{	return 0;}
		virtual RenderNode *GetRenderNode(UINT i) const	{	return NULL;}
		virtual const AABBox	*GetAABBox() const		{	return NULL;}
		virtual int GetNumMtl() const					{	return 0;}
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const	{}

		//--
		UINT GetNumFrame() const;
		SkeletonKeyFrame* GetFrame(UINT index);
		UINT GetNumBone() const;
		/**	根据Bone的name查找bone的id(在数组中的index)
			\return -1为没找到
		*/
		int GetBoneID(const TCHAR* szBoneName);
		SkeletonStruct& GetStruct() {	return m_struct; }

		float GetTrackTime( DWORD dwTrackName ) ;
		int	  GetTrackRange( DWORD dwTrackName, int& nBegin, int& nEnd ); 
		float GetFrmTime( int nFrame )			{	
			ASSERT( nFrame >= 0 && nFrame < (int)m_vecFrameTime.size() );
			return m_vecFrameTime[nFrame];	
		}

		int GetNumTrack() {	return m_mapTrackInfo.size(); }
		int GetTrackName( vector<tstring>& trackNames );

	private:
		bool LoadSkeletonFile(const TCHAR* szName,bool bLoadStruct);
		void ReadKerFrameInfo(IFS *pFS,DWORD hFile,SkeletonKeyFrame *pKF);

		vector<SkeletonKeyFrame*>	m_frameBuf;
		SkeletonStruct				m_struct;
		map<tstring,int>			m_boneMap;		//用于快速查找
		map<DWORD,TrackInfo>		m_mapTrackInfo;	//Track信息
		vector<float>				m_vecFrameTime;	//每帧时间

		DECL_RTTI(ResSkeletonAction);
	};
}//namespace Cool3D