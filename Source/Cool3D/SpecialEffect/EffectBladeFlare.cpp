#include "StdAfx.h"
#include <deque>
#include ".\effectBladeFlare.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "SGeffectNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\Kernel.h"
#include "..\SceneGraph\SGAvatarAttachment.h"
#include "..\SceneGraph\AvatarNode.h"
#include "..\Animation\AnimationCtrl.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectBladeFlare,EffectBase);

	class EffectBladeFlare::Member
	{
		struct tagSampleData
		{
			Vector3		scale;
			Quaternion  rot;
			Vector3     offset;
			tagSampleData() : scale( 1.0f, 1.0f, 1.0f ), offset( 0.0f, 0.0f, 0.0f )
			{ D3DXQuaternionIdentity(&rot); }
		};
	public:
		DynamicMesh				*m_pRMesh;
		tagBladeFlareProp		m_prop;
		bool					m_bActive;						// 现在是否已经激活
		deque<tagFlareLine>		m_track;						// 记录轨迹
		int						m_numLine;
		vector<Vector3>			m_tagNodePos;
		vector<tagSampleData>   m_lastSampleData;				// 上一次采样的数据
		float                   m_lastSampleTime;				// 上一次采样的时间
		vector<int>             m_boneIDs;						// 骨骼ID，绑定骨骼时使用
	public:
		Member() :
		m_pRMesh( NULL ),
		m_bActive( false ),
		m_numLine( 0 ),
		m_lastSampleTime( 0.0f )
		{
		}
		~Member()	
		{
			SAFE_DELETE(m_pRMesh);
		}

		bool UpdataTagNodePosByBoneID( SceneNode* pFatherNode )
		{
			bool bOK = false;

			m_tagNodePos.resize(COSTOM_TAGNODE_NUM);
			m_boneIDs.resize(COSTOM_TAGNODE_NUM);

			// 自定义挂载点时，每个点都要保存一个上次采样的数据
			m_lastSampleData.resize(COSTOM_TAGNODE_NUM);

			for( int i = 0; i < COSTOM_TAGNODE_NUM; i++ )
			{
				m_boneIDs[i] = -1;
				m_tagNodePos[i] = Vector3(0.0f, 0.0f, 0.0f);

				if( 0 == m_prop.szTagNode[i][0] )
					continue;
				if( pFatherNode->IS_KIND_OF(AvatarNode) )
				{
					AvatarNode *pParent=(AvatarNode*)pFatherNode;
					ResSkeleton *pSkel=const_cast<ResSkeleton *>(pParent->GetResource());
					if( NULL == pSkel || !pSkel->IsCreated() )
						return false;

					m_boneIDs[i]=pSkel->GetBoneID(m_prop.szTagNode[i]);
					ASSERT(m_boneIDs[i]!=-1 && "bone not find");
				}
				else if( pFatherNode->IS_KIND_OF(EntityNode) )
				{
					EntityNode *pParent=(EntityNode*)pFatherNode;
					ResRenderable* pRes = pParent->GetResObj();
					if( NULL == pRes || !pRes->IsCreated() || !pRes->IS_STATIC_CLASS(ResSkeleton) )
						return false;

					ResSkeleton *pSkel=(ResSkeleton*)pRes;
					m_boneIDs[i]=pSkel->GetBoneID(m_prop.szTagNode[i]);
					ASSERT(m_boneIDs[i]!=-1 && "bone not find");
				}
				else
				{
					ASSERT( 0 );
					return false;
				}

				if( m_boneIDs[i] != -1 )
					bOK = true;
			}

			// 一块骨骼都没有找到
			if( !bOK )
			{
				m_tagNodePos.clear();
				m_boneIDs.clear();
				m_lastSampleData.clear();
				return false;
			}

			return true;
		}
		bool UpdataTagNodePos( SceneNode* pFatherNode )
		{
			if( NULL == pFatherNode || !pFatherNode->IS_STATIC_CLASS(SGAvatarAttachment) )
				return false;

			SGAvatarAttachment* pAvatarAttachment = (SGAvatarAttachment*)pFatherNode;
			ResRenderable* pRes = pAvatarAttachment->GetResObj();
			if( NULL == pRes || !pRes->IsCreated() )
				return false;

			//拷贝初始点
			m_tagNodePos.clear();
			for( int i = 0; i < MAX_TAGNODE_NUM; i++ )
			{
				TCHAR szTagNodeName[10];
				_stprintf( szTagNodeName, _T("tag_BF%d%d"), m_prop.nIndex, i );

				Matrix4 matTagNode = MathConst::IdentityMat;
				if( pAvatarAttachment->GetModelSpace( szTagNodeName, matTagNode ) )
				{
					Vector3 vOffset = Vector3( matTagNode._41, matTagNode._42, matTagNode._43 );
					m_tagNodePos.push_back(vOffset);
				}
			}

			if( m_tagNodePos.size() < 2 )
				return false;

			// 只需要保存1个采样数据
			m_lastSampleData.resize(1);

			return true;
		}
		void UpdateRes(SceneNode* pFatherNode)
		{
			//已经创建好了则返回
			if(m_pRMesh!=NULL)
				return;

			bool bRet = false;
			if( m_prop.bCustomTagNode )
				bRet = UpdataTagNodePosByBoneID( pFatherNode );
			else
				bRet = UpdataTagNodePos( pFatherNode );

			if( !bRet )
				return;

			//计算总共会有多少段
			m_numLine=int(m_prop.trackTimeLen/m_prop.sampleTime);

			//生成Mesh
			int numTri=(m_numLine-1)*(m_tagNodePos.size()-1)*2;
			m_pRMesh=new DynamicMesh;
			m_pRMesh->Create(0,EDraw_TriangleList,EVType_PTD,numTri*3);
			m_pRMesh->SetDrawLimit(0);
			m_pRMesh->SetIdentityWorldMat( true );
		}

		void AddLine( const Matrix4& mat )
		{
			tagFlareLine newLine;
			for(size_t i=0;i<m_tagNodePos.size();i++)
			{
				newLine.pt[i]=m_tagNodePos[i] * mat;
			}
			AddLine( newLine );
		}

		void AddLine( const tagSampleData& data )
		{
			tagFlareLine newLine;
			Transform trans;
			trans.Scale( data.scale.x, data.scale.y, data.scale.z );
			trans.RotateQuat( data.rot );
			trans.Translate( data.offset );
			for(size_t i=0;i<m_tagNodePos.size();i++)
			{
				newLine.pt[i]=m_tagNodePos[i];
				trans.TransVec3(newLine.pt[i]);
			}

			AddLine( newLine );
		}

		inline void AddLine( const tagFlareLine& newLine )
		{
			if(m_track.size()==m_numLine)
				m_track.pop_front();
			m_track.push_back(newLine);
		}

		inline void AddPointToLine( tagFlareLine& line, const tagSampleData& data, const int nTagIndex )
		{
			Transform trans;
			trans.Scale( data.scale.x, data.scale.y, data.scale.z );
			trans.RotateQuat( data.rot );
			trans.Translate( data.offset );
			line.pt[nTagIndex]=m_tagNodePos[nTagIndex];
			trans.TransVec3(line.pt[nTagIndex]);
		}

		inline void LerpData( tagSampleData& out, const tagSampleData& d1, const tagSampleData& d2, const float t )
		{
			D3DXVec3Lerp( &out.scale, &d1.scale, &d2.scale, t );
			D3DXQuaternionSlerp( &out.rot, &d1.rot, &d2.rot, t );
			D3DXVec3Lerp( &out.offset, &d1.offset, &d2.offset, t );
		}

		bool GetBoneMatrix( Matrix4& out, SceneNode* pFatherNode, int nTagNodeIndex )
		{
			ASSERT( m_prop.bCustomTagNode );

			if( -1 == m_boneIDs[nTagNodeIndex] )
			{
				out = pFatherNode->GetWorldMat();
			}
			else if( pFatherNode->IS_KIND_OF(AvatarNode) )
			{
				AvatarNode *pNode=(AvatarNode*)pFatherNode;
				out = *( pNode->GetSkeletonAniCtrl()->GetBoneSpace( m_boneIDs[nTagNodeIndex] ) );
				out *= pFatherNode->GetWorldMat();
			}
			else if( pFatherNode->IS_KIND_OF(EntityNode) )
			{
				EntityNode *pNode=(EntityNode*)pFatherNode;
				AnimationCtrl* pAni = pNode->GetAniCtrl();
				if( NULL == pAni || !pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
				{
					return false;
				}
				NodeAnimationCtrl* NodepAni = (NodeAnimationCtrl*)pAni;
				out = ( *NodepAni->GetBoneSpace( m_boneIDs[nTagNodeIndex] ) );
				out *= pFatherNode->GetWorldMat();
			}
			else
			{
				return false;
			}
			return true;
		}

		bool IsBonebMirrored( SceneNode* pFatherNode, int nTagNodeIndex )
		{
			ASSERT( m_prop.bCustomTagNode );

			if( -1 == m_boneIDs[nTagNodeIndex] )
			{
				return false;
			}
			else if( pFatherNode->IS_KIND_OF(AvatarNode) )
			{
				AvatarNode *pNode=(AvatarNode*)pFatherNode;
				return pNode->GetSkeletonAniCtrl()->BoneIsMirrored( m_boneIDs[nTagNodeIndex] );
			}
			else if( pFatherNode->IS_KIND_OF(EntityNode) )
			{
				EntityNode *pNode=(EntityNode*)pFatherNode;
				AnimationCtrl* pAni = pNode->GetAniCtrl();
				if( NULL == pAni || !pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
				{
					ASSERT( 0 );
					return false;
				}
				NodeAnimationCtrl* NodepAni = (NodeAnimationCtrl*)pAni;
				return NodepAni->BoneIsMirrored( m_boneIDs[nTagNodeIndex] );
			}
			else
			{
				return false;
			}
		}

		inline void CalData( tagSampleData& out, Matrix4 mat, const bool bMirrored )
		{
			Transform trans;
			trans.LoadMatrix( &mat );
			if( bMirrored )
				trans.Scale( -1.0f, -1.0f, -1.0f );

			D3DXMatrixDecompose( &out.scale, &out.rot, &out.offset, trans.GetMatrix() );
			D3DXQuaternionConjugate( &out.rot, &out.rot );
		}
		
		void Smooth( SceneNode* pFatherNode, float runTime )
		{
			ASSERT(m_lastSampleData.size()==1);

			// 获取数据
			ASSERT( pFatherNode->IS_STATIC_CLASS(SGAvatarAttachment) );
			SGAvatarAttachment* pAttch = (SGAvatarAttachment*)pFatherNode;
			const bool bMirrored = pAttch->IsBindMirroredBone();
			const tagSampleData& lastData = m_lastSampleData[0];
			tagSampleData curData;
			CalData( curData, pFatherNode->GetWorldMat(), bMirrored );

			// 平滑插值
			if( 0 != m_track.size() )
			{
				tagSampleData lerpData;
				const float fLerpStep = m_prop.sampleTime / ( runTime - m_lastSampleTime );
				for( float t = fLerpStep; t < 1.0f; t += fLerpStep )
				{
					LerpData( lerpData, lastData, curData, t );
					if( bMirrored ) 
						lerpData.scale *= -1.0f;

					AddLine( lerpData );
				}
			}

			// 保存数据用于下祯插值
			m_lastSampleData[0] = curData;
		}
		void SmoothCustomTagNode( SceneNode* pFatherNode, float runTime, tagFlareLine& curLine )
		{
			// 获取数据，并生成最后一条线
			vector<bool> vecMirrored(m_lastSampleData.size());
			vector<tagSampleData> curData(m_lastSampleData.size());
			for( size_t nTag = 0; nTag < m_lastSampleData.size(); nTag++ )
			{
				Matrix4 matBone;
				if( !GetBoneMatrix( matBone, pFatherNode, nTag ) )
				{
					ASSERT( 0 );
					return;
				}
				curLine.pt[nTag] = m_tagNodePos[nTag] * matBone;

				tagSampleData& data = curData[nTag];
				const bool bMirrored = IsBonebMirrored( pFatherNode, nTag );
				CalData( data, matBone, bMirrored );
				vecMirrored[nTag] = bMirrored;
			}

			// 平滑插值
			if( 0 != m_track.size() )
			{
				tagFlareLine lerpLine;
				tagSampleData lerpData;
				const float fLerpStep = m_prop.sampleTime / ( runTime - m_lastSampleTime );
				for( float t = fLerpStep; t < 1.0f; t += fLerpStep )
				{
					for( size_t nTag = 0; nTag < m_lastSampleData.size(); nTag++ )
					{
						const tagSampleData& d1 = m_lastSampleData[nTag];
						const tagSampleData& d2 = curData[nTag];
						LerpData( lerpData, d1, d2, t );
						if( vecMirrored[nTag] ) 
							lerpData.scale *= -1.0f;

						AddPointToLine( lerpLine, lerpData, nTag );
					}
					AddLine( lerpLine );
				}
			}

			// 保存数据用于下祯插值
			for( size_t nTag = 0; nTag < m_lastSampleData.size(); nTag++ )
				m_lastSampleData[nTag] = curData[nTag];
		}

		void UpdateFlare(SceneNode* pFatherNode,float runTime)
		{
			if( runTime-m_lastSampleTime < m_prop.sampleTime )
				return;

			if( m_prop.bCustomTagNode )
			{
				tagFlareLine curLine;
				if( m_prop.bNoSmooth )
				{
					for( size_t nTag = 0; nTag < m_lastSampleData.size(); nTag++ )
					{
						Matrix4 matBone;
						if( !GetBoneMatrix( matBone, pFatherNode, nTag ) )
						{
							ASSERT( 0 );
							return;
						}
						curLine.pt[nTag] = m_tagNodePos[nTag] * matBone;
					}
				}
				else
				{
					SmoothCustomTagNode( pFatherNode, runTime, curLine );
				}
				AddLine( curLine );
			}
			else
			{
				if( !m_prop.bNoSmooth )
					Smooth( pFatherNode, runTime );
				AddLine( pFatherNode->GetWorldMat() );
			}
			m_lastSampleTime = runTime;
		}

		void UpdateMesh()
		{
			//至少有两条线才能形成面
			if(m_track.size()<2)
			{
				m_pRMesh->SetDrawLimit(0);
				return;
			}

			Color4ub diffuse = Color4ub( m_prop.color );
			VertStream *pVStream=m_pRMesh->GetVertStream();
			int numVert=pVStream->GetNumVert();
			Vert_PTD *pVB=(Vert_PTD *)pVStream->GetRawBuffer();
			int vi=0;

			deque<tagFlareLine>::iterator iter=m_track.begin();
			deque<tagFlareLine>::iterator lastIter=iter;
			iter++;
			int numPt=m_tagNodePos.size();
			float vStep=1.0f/(numPt-1);
			float uStep=1.0f/(m_track.size()-1);
			float u=uStep;
			for(;iter!=m_track.end();iter++)
			{
				//--get line data
				const tagFlareLine& lastLine=*lastIter;
				const tagFlareLine& line=*iter;
				lastIter=iter;

				for(int i=0;i<numPt-1;i++)
				{
					//--triangle 1
					pVB[vi].pos=lastLine.pt[i];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=i*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;

					pVB[vi].pos=lastLine.pt[i+1];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=(i+1)*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;

					pVB[vi].pos=line.pt[i+1];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=(i+1)*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;

					//--tri 2
					pVB[vi].pos=lastLine.pt[i];
					pVB[vi].uv.x=u-uStep;
					pVB[vi].uv.y=i*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;

					pVB[vi].pos=line.pt[i];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=i*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;

					pVB[vi].pos=line.pt[i+1];
					pVB[vi].uv.x=u;
					pVB[vi].uv.y=(i+1)*vStep;
					pVB[vi].diffuse = diffuse;
					vi++;		

				}//endof for(i)
				ASSERT(vi<=numVert);

				u+=uStep;
			}//endof for
			ASSERT(vi<=numVert);
			m_pRMesh->SetDrawLimit(vi/3);
		}

		void ClearFlare()
		{
			m_track.clear();
		}
	};

	EffectBladeFlare::EffectBladeFlare(void)
	{
		m_p=new Member;
	}

	EffectBladeFlare::~EffectBladeFlare(void)
	{
		delete m_p;
	}

	void EffectBladeFlare::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);

		EffectBase::Serialize(ar);
	}

	void EffectBladeFlare::Deserialize(FArchive& ar)
	{
		tagBladeFlareProp prop;
		ar.ValueRead(prop);

		//build
		Build(prop);

		EffectBase::Deserialize(ar);
	}

	RenderNode* EffectBladeFlare::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectBladeFlare::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		//--是否启动
		if(!m_p->m_bActive)
			return;

		SceneNode* pFather=(SceneNode*)pSGNode->GetParent();
		if( NULL == pFather )
			return;

		if(m_p->m_pRMesh==NULL)
		{
			m_p->UpdateRes(pFather);
			pSGNode->RebuildEffectRenderNodes();
		}
		else
		{
			m_p->UpdateFlare(pFather,runTime);
			m_p->UpdateMesh();
		}
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	const TCHAR* EffectBladeFlare::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectBladeFlare::tagBladeFlareProp EffectBladeFlare::GetProp() const
	{
		return m_p->m_prop;
	}

	void EffectBladeFlare::Build(const tagBladeFlareProp& prop)
	{
		m_p->m_prop=prop;
		SAFE_DELETE( m_p->m_pRMesh );
		ResetPlayState();
	}

	void EffectBladeFlare::ResetPlayState()
	{
		m_p->ClearFlare();
		m_p->m_lastSampleTime = 0.0f;	
		m_p->m_bActive=true;
		if(!m_p->m_bActive)
		{
			if( NULL != m_p->m_pRMesh )
				m_p->m_pRMesh->SetDrawLimit(0);		
		}

		EffectBase::ResetPlayState();
	}

	void EffectBladeFlare::Close(float sgNodeRunTime)
	{
		m_bClosing = true;
		m_closingFade = 0.0f;
		m_p->ClearFlare();
		m_p->m_lastSampleTime = 0.0f;
		m_p->m_bActive = false;
		if( NULL != m_p->m_pRMesh )
			m_p->m_pRMesh->SetDrawLimit(0);
	}

	void EffectBladeFlare::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectBladeFlare::CreateMtl(const tagMtlEffectDesc& desc)
	{
		EffectBase::CreateMtl(desc);

		if(m_pMtl!=NULL)
			m_pMtl->m_addressU=m_pMtl->m_addressV=m_pMtl->m_addressW=ETAddress_Clamp;
	}

	void EffectBladeFlare::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	bool EffectBladeFlare::GetBox(AABBox& out) const
	{
		out.min = Vector3( -50.0f, -50.0f, -50.0f );
		out.max = Vector3(  50.0f,  50.0f,  50.0f );
		return true;
	}

	int EffectBladeFlare::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D