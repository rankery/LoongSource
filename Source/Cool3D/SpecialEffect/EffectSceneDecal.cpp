#include "StdAfx.h"
#include ".\effectscenedecal.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "..\RenderMtl\MMUVTrans.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\Device\DrawX.h"
#include "..\SceneGraph\NullSceneGraph.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectSceneDecal,EffectBase);

	class EffectSceneDecal::Member
	{
	public:
		tagDecalProp		m_prop;
		DynamicMesh			*m_pRMesh;
		vector<tagState>	m_keyStates;			// 每个关键帧的状态	

		int					m_nCurState;			// 当前的关键帧状态index
		float				m_stateTime;			// 当前状态保持了多少时间
		bool				m_bActive;				// 现在是否已经激活
		tagState            m_curState;				// 当前状态
		float				curRot;					// 当前自转

		bool				m_bUserPos;
		Vector3				m_userPos;
		Vector3             m_vPosNode;  			// 当前SGNode空间坐标
		Vector3             m_vNormal;				// SGNode空间的法向量
		Vector3             m_vUpVec;     
		Vector3             m_vRightVec;           
		Matrix4             m_matWorld;				// 世界矩阵
		bool                m_bForceUpdataMesh;     // 强制更新Mesh
	public:
		Member():curRot(0),m_userPos(0,0,0),m_vPosNode(0,0,0),
				 m_vNormal(MathConst::AxisY),m_vUpVec(MathConst::AxisZ),m_vRightVec(MathConst::AxisX),
				 m_bForceUpdataMesh( false )
		{
			m_nCurState=0;
			m_stateTime=0;
			m_pRMesh=NULL;
			m_bActive=true;

			m_bUserPos=false;
		}

		~Member()
		{
			Destroy();
		}

		void Destroy()
		{
			SAFE_DELETE(m_pRMesh);
		}

		int BuildDecalMeshFromSG( SceneGraph* pSG, SGEffectNode *pSGNode, const Matrix4& matWorld, const tagState& state )
		{
			AABBox boxWorld;
			boxWorld.min.x = - state.size.x * 0.5f;
			boxWorld.min.y = - state.size.y * 0.5f;
			boxWorld.min.z = - m_prop.rangeFront;
			boxWorld.max.x =   state.size.x * 0.5f;
			boxWorld.max.y =   state.size.y * 0.5f;
			boxWorld.max.z =   m_prop.rangeBack;
			boxWorld.Transform( matWorld );
			
			pSG->BuildDecalMesh( boxWorld, m_pRMesh, &m_vNormal );

			const int numVert = m_pRMesh->BuildGetNumPos();
			if( numVert <= 0 )
			{
				m_pRMesh->SetDrawLimit( 0 );
				return 0;
			}

			Vector3 uvw(0,0,1);
			const Vector2 invSize( 1.0f / state.size.x, 1.0f / state.size.y );
			Matrix4 matInvNodeToWorld;
			D3DXMatrixInverse( &matInvNodeToWorld, NULL, &pSGNode->GetWorldMat() );

			for( int i = 0; i < numVert; i++ )
			{
				Vector3& vertPos=m_pRMesh->BuildGetPos(i);
				D3DXVec3TransformCoord( &vertPos, &vertPos, &matInvNodeToWorld );

				Vector3 v = vertPos - m_vPosNode;
				uvw.x = D3DXVec3Dot( &v, &m_vRightVec ) * invSize.x + 0.5f;
				uvw.y = D3DXVec3Dot( &v, &m_vUpVec    ) * invSize.y + 0.5f;
				m_pRMesh->AddTexCoord( uvw );

				m_pRMesh->AddDiffuse( state.color );
			}
			return numVert;
		}

		void BuildDefaultMesh( const Matrix4& matNode, const tagState& state )
		{
			Vector2 vHalfSize = state.size * 0.5f;

			m_pRMesh->AddPos( Vector3( -vHalfSize.x, -vHalfSize.y, 0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 0.0f, 1.0f, 1.0f ) );

			m_pRMesh->AddPos( Vector3( -vHalfSize.x, vHalfSize.y,  0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 0.0f, 0.0f, 1.0f ) );

			m_pRMesh->AddPos( Vector3(  vHalfSize.x, vHalfSize.y,  0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 1.0f, 0.0f, 1.0f ) );

			m_pRMesh->AddPos( Vector3( -vHalfSize.x, -vHalfSize.y, 0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 0.0f, 1.0f, 1.0f ) );

			m_pRMesh->AddPos( Vector3(  vHalfSize.x, vHalfSize.y,  0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 1.0f, 0.0f, 1.0f ) );

			m_pRMesh->AddPos( Vector3(  vHalfSize.x, -vHalfSize.y, 0.0f ) );
			m_pRMesh->AddTexCoord( Vector3( 1.0f, 1.0f, 1.0f ) );


			for( int i = 0; i < 6; i++ )
			{
				m_pRMesh->AddDiffuse( state.color );
				Vector3& vertPos=m_pRMesh->BuildGetPos(i);
				D3DXVec3TransformCoord( &vertPos, &vertPos, &matNode );
			}
		}

		void UpdataDecalMesh(SceneGraph* pSG, SGEffectNode *pSGNode, const tagState& state )
		{
			if( NULL == pSG || NULL == pSGNode )
				return;

			Matrix4 matNode = MathConst::IdentityMat;
			matNode._11 = m_vRightVec.x; matNode._12 = m_vRightVec.y; matNode._13 = m_vRightVec.z;
			matNode._21 = m_vUpVec.x;    matNode._22 = m_vUpVec.y;    matNode._23 = m_vUpVec.z;
			matNode._31 = m_vNormal.x;   matNode._32 = m_vNormal.y;   matNode._33 = m_vNormal.z;
			matNode._41 = m_vPosNode.x;  matNode._42 = m_vPosNode.y;  matNode._43 = m_vPosNode.z;
			Matrix4 matWorld = matNode * pSGNode->GetWorldMat();

			if( m_matWorld == matWorld && state.size == m_curState.size && !m_bForceUpdataMesh )
			{
				// 不需要更新
				if( state.color == m_curState.color )
					return;
				else
				{
					// 只更新颜色
					VertStream* pVertStream = m_pRMesh->GetVertStream();
					ASSERT( NULL != pVertStream );
					const UINT nNumVert = pVertStream->GetNumVert();
					if( 0 == nNumVert)
						return;
					ASSERT( EVType_PTpD == pVertStream->GetType() );
					Vert_PTpD *pVert=(Vert_PTpD *)pVertStream->GetRawBuffer();
					for( UINT i = 0; i < pVertStream->GetNumVert(); i++ )
					{
						pVert->diffuse = Color4ub( state.color );
						pVert++;
					}
					return;
				}
			}

			// 更新顶点
			m_pRMesh->BeginBuild(m_prop.m_groupID,EDraw_TriangleList);

			int numDraw = -1;
			if( !m_prop.bOnlyUseDefaultMesh )
			{
				int nVertBuild = BuildDecalMeshFromSG( pSG, pSGNode, matWorld, state );
				if( 0 == nVertBuild )
				{
					if( m_prop.bUseDefaultMesh || pSG->IS_STATIC_CLASS( NullSceneGraph ) )
						BuildDefaultMesh( matNode, state );
					else
						numDraw = 0;
				}
			}
			else
				BuildDefaultMesh( matNode, state );

			m_pRMesh->EndBuild();
			m_pRMesh->SetDrawLimit( numDraw );
			m_matWorld = matWorld;
			m_bForceUpdataMesh = false;
		}
	};

	EffectSceneDecal::EffectSceneDecal(void)
	{
		m_p=new Member;
	}

	EffectSceneDecal::~EffectSceneDecal(void)
	{
		delete m_p;
	}

	void EffectSceneDecal::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectSceneDecal::Deserialize(FArchive& ar)
	{
		tagDecalProp prop;
		ar.ValueRead(prop);
		vector<tagState> keyStates;
		DeserialValVector(ar,keyStates);

		//build
		BeginBuild(prop);
		for(size_t i=0;i<keyStates.size();i++)
			AddKeyState(keyStates[i]);
		EndBuild();

		EffectBase::Deserialize(ar);
	}

	RenderNode* EffectSceneDecal::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectSceneDecal::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if(m_p->m_pRMesh==NULL)
			return;
		//--是否启动
		if(!m_p->m_bActive)
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
		}
		if(!m_p->m_bActive)
			return;

		m_p->m_stateTime+=deltaTime;
		//--插值出当前的状态
		m_p->m_nCurState=AdvanceState(m_p->m_nCurState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_nCurState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_nCurState>=0 
			&& m_p->m_nCurState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_nCurState];

		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			int nextState=m_p->m_nCurState+1;
			bool bLerp=true;
			if(nextState>=(int)m_p->m_keyStates.size())
			{
				if(!m_p->m_prop.m_loop)//已经播放到最后一个状态,而且不是循环播放,则不需插值
					bLerp=false;
				nextState=0;
			}

			if(bLerp)
			{
				tagState& ts=m_p->m_keyStates[nextState];
				float r=m_p->m_stateTime/curState.lastTime;

				D3DXVec4Lerp((Vector4*)&curState.color,(Vector4*)&curState.color,(Vector4*)&ts.color,r);
				D3DXVec2Lerp(&curState.size,&curState.size,&ts.size,r);
				curState.rotVel=LinearLerp(curState.rotVel,ts.rotVel,r);
			}
		}

		m_p->curRot+=curState.rotVel*deltaTime;

		if(m_p->m_bUserPos)
			m_p->m_vPosNode=m_p->m_userPos;
		else
		{
			m_p->m_vPosNode=m_p->m_prop.offset;
			EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
			if(pPath)
				m_p->m_vPosNode+=pPath->GetCurPos();
		}

		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.color.A*=m_closingFade;
		}

		// 更新Mesh
		m_p->UpdataDecalMesh( pSG, pSGNode, curState );

		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
		if(m_pMtl!=NULL)
		{
			int numMod=m_pMtl->GetNumMods();
			MMUVTrans *pUVRot=(MMUVTrans *)m_pMtl->GetModifier(numMod-1);//最后加入的一个
			if(pUVRot!=NULL && 0!=m_p->curRot)
			{
				Matrix4 mat1 = MathConst::IdentityMat;
				mat1._31 = mat1._32 = -0.5f;
				Matrix4 mat2 = MathConst::IdentityMat;
				float c=cosf(m_p->curRot);
				float s=sinf(m_p->curRot);
				mat2._11=c;
				mat2._12=s;
				mat2._21=-s;
				mat2._22=c;
				Matrix4 mat3 = MathConst::IdentityMat;
				mat3._31 = mat3._32 = 0.5f;
			
				mat1 *= mat2;
				mat1 *= mat3;
				
				pUVRot->SetMatrix( mat1 );
				m_pMtl->InvalidateRenderCache();
			}
		}
		m_p->m_curState = curState;
	}

	void EffectSceneDecal::BeginBuild(const tagDecalProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_nCurState=0;

		m_p->m_prop=prop;
	}

	void EffectSceneDecal::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectSceneDecal::EndBuild()
	{
		m_p->Destroy();
		//-- 创建Mesh对象
		m_p->m_pRMesh=new DynamicMesh;
		m_p->m_pRMesh->SetZBias( true );
		SetDir( -m_p->m_vNormal, m_p->m_vUpVec );
		m_p->m_bForceUpdataMesh = true;
		ResetPlayState();
	}

	void EffectSceneDecal::ResetPlayState()
	{
		m_p->m_nCurState=0;
		m_p->m_stateTime=0;

		m_p->curRot=_DegToRad(m_p->m_prop.initRot);
		SetDir( -m_p->m_vNormal, m_p->m_vUpVec );
		m_p->m_bForceUpdataMesh = true;
		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);
		EffectBase::ResetPlayState();
	}

	const TCHAR* EffectSceneDecal::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectSceneDecal::tagDecalProp EffectSceneDecal::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectSceneDecal::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectSceneDecal::tagState EffectSceneDecal::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectSceneDecal::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectSceneDecal::CreateMtl(const tagMtlEffectDesc& desc)
	{
		EffectBase::CreateMtl(desc);

		if(m_pMtl!=NULL)
		{
			m_pMtl->m_addressU=m_pMtl->m_addressV=m_pMtl->m_addressW=ETAddress_Clamp;
			MMUVTrans *pUVRot=new MMUVTrans;
			m_pMtl->AttachModifier(pUVRot);
		}
	}

	void EffectSceneDecal::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	bool EffectSceneDecal::GetBox(AABBox& out) const
	{
		Vector2 vMaxSize(0.0f,0.0f);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			if( vMaxSize.x < state.size.x )
				vMaxSize.x = state.size.x;
			if( vMaxSize.y < state.size.y )
				vMaxSize.y = state.size.y;
		}
		float fHalfMaxSize = max( vMaxSize.x, vMaxSize.y ) * 0.5f;
		out.min = Vector3( -fHalfMaxSize, -fHalfMaxSize, -fHalfMaxSize ) + m_p->m_prop.offset;
		out.max = Vector3(  fHalfMaxSize,  fHalfMaxSize,  fHalfMaxSize ) + m_p->m_prop.offset;
		return true;
	}

	int EffectSceneDecal::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}

	void EffectSceneDecal::SetUserPos(const Vector3& pos)
	{
		m_p->m_bUserPos=true;
		m_p->m_userPos=pos;
	}

	void EffectSceneDecal::SetDir( const Vector3& vDir, const Vector3& vUpVec )
	{
		D3DXVec3Normalize( &m_p->m_vNormal, &(-vDir) );
		D3DXVec3Cross( &m_p->m_vRightVec, &vUpVec, &m_p->m_vNormal );
		D3DXVec3Normalize( &m_p->m_vRightVec, &m_p->m_vRightVec );
		D3DXVec3Cross( &m_p->m_vUpVec, &m_p->m_vNormal, &m_p->m_vRightVec );
		m_p->m_pRMesh->SetDrawLimit( 0 );
		m_p->m_bForceUpdataMesh = true;
	}

	const Vector2& EffectSceneDecal::GetDecalSize() const
	{
		return m_p->m_curState.size;
	}

	float EffectSceneDecal::GetFrontRange() const
	{
		return m_p->m_prop.rangeFront;
	}
	float EffectSceneDecal::GetBackRange() const
	{
		return m_p->m_prop.rangeBack;
	}
}//namespace Cool3D