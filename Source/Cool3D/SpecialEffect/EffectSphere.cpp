#include "StdAfx.h"
#include ".\effectsphere.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectSphere,EffectBase);

	class EffectSphere::Member
	{
	public:
		tagSphereProp		m_prop;
		DynamicMeshIndexed	*m_pRMesh;
		VertStream			*m_pOriginVert;	//没有经过transform的vert 
		vector<tagState>	m_keyStates;	//每个关键帧的状态
		int					m_curState;		//当前的关键帧状态index
		float				m_stateTime;	//当前状态保持了多少时间
		bool				m_bActive;//现在是否已经激活

		Vector3 curRot;	//当前自转
		

		Member():curRot(0,0,0)
		{
			m_curState=0;
			m_stateTime=0;
			m_pRMesh=NULL;
			m_pOriginVert=NULL;
			m_bActive=true;
		}
		~Member()	
		{
			Destroy();
		}

		void Destroy()
		{
			SAFE_DELETE(m_pRMesh);
			SAFE_DELETE(m_pOriginVert);
		}

		void InitSphereMesh(DWORD initColor)
		{
			int slices=m_prop.slices;
			float radius=m_prop.radius;
			//--
			if(slices<=0)
				slices=1;
			int numVerts=(slices+1)*(slices+1);
			
			
			VertStream *pVStream=new VertStream;
			pVStream->Create(EVType_PTD,numVerts);
			Vert_PTD *pVB=(Vert_PTD *)pVStream->GetRawBuffer();

			Vector2 pitchRange(_DegToRad(m_prop.pitchRange.x),_DegToRad(m_prop.pitchRange.y));
			Vector2 yawRange(_DegToRad(m_prop.yawRange.x),_DegToRad(m_prop.yawRange.y));
			int i=0, j=0;
			float pitch,yaw;
			int vi=0;
			for(i=0;i<=slices;i++)
			{
				pitch=(pitchRange.y-pitchRange.x)/slices*(slices-i)+pitchRange.x;
				for(j=0;j<=slices;j++)
				{
					yaw=(yawRange.y-yawRange.x)/slices*j+yawRange.x;
					Vert_PTD& vert=pVB[vi];
					vert.pos.x=radius*sinf(pitch)*cosf(yaw);
					vert.pos.y=radius*cosf(pitch);
					vert.pos.z=radius*sinf(pitch)*sinf(yaw);

					vert.uv.x=yaw/(yawRange.y-yawRange.x)*m_prop.uvScale.x;
					vert.uv.y=pitch/(pitchRange.y-pitchRange.x)*m_prop.uvScale.y;
					
					/*sphere map
					Vector3 normal=vert.pos;
					Vec3Normalize(normal);
					vert.uv.x=normal.x*0.5f+0.5f;
					vert.uv.y=normal.y*0.5f+0.5f;
					*/
					
					vert.diffuse=initColor;

					vi++;
				}
			}
			ASSERT(vi==pVStream->GetNumVert());
			m_pOriginVert=pVStream->Clone(true);

			vector<WORD> indexBuf;
			int k=0,l=0;
			for(j=0;j<slices+2;j++)
			{
				for(i=0;i<slices;i++)
				{
					indexBuf.push_back(l+i);
					indexBuf.push_back(k+i);
				}
				indexBuf.push_back(l);
				indexBuf.push_back(k);

				k=l;
				l+=slices;
			}

			//--转换成list
			vector<WORD> indexBuf2;
			indexBuf2.push_back(indexBuf[0]);
			indexBuf2.push_back(indexBuf[1]);
			indexBuf2.push_back(indexBuf[2]);
			int indexBufSize=(int)indexBuf.size();
			for(i=3;i<indexBufSize;i++)
			{
				indexBuf2.push_back(indexBuf[i-2]);
				indexBuf2.push_back(indexBuf[i-1]);
				indexBuf2.push_back(indexBuf[i]);
			}

			//--
			m_pRMesh=new DynamicMeshIndexed;
			m_pRMesh->CreateDirect(EDraw_TriangleList,
				0,m_prop.m_groupID,pVStream->GetNumVert(),sizeof(WORD)*indexBuf2.size(),
				&(*indexBuf2.begin()));
			m_pRMesh->AttachVertStream(pVStream);
			//todo m_pRMesh->SetDrawLimit(-1);
		}

		void UpdateVert(DWORD color,Vector3 rot,Vector3 scale,Vector3 pos)
		{
			VertStream *pVStream=m_pRMesh->GetVertStream();
			Vert_PTD *pVB=(Vert_PTD *)pVStream->GetRawBuffer();

			Vert_PTD *pOVB=(Vert_PTD *)m_pOriginVert->GetRawBuffer();

			Transform trans;
			trans.Translate(pos);
			trans.Rotate(rot.y,rot.x,rot.z);
			trans.Scale(scale.x,scale.y,scale.z);
			
			int numVert=pVStream->GetNumVert();
			for(int i=0;i<numVert;i++)
			{
				Vert_PTD& vert=pVB[i];
				Vert_PTD& vertO=pOVB[i];
				vert.diffuse=color;
				vert.pos=vertO.pos;
				trans.TransVec3(vert.pos);
			}
		}
	};

	EffectSphere::EffectSphere(void)
	{
		m_p=new Member;
	}

	EffectSphere::~EffectSphere(void)
	{
		delete m_p;
	}

	void EffectSphere::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectSphere::Deserialize(FArchive& ar)
	{
		tagSphereProp prop;
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

	RenderNode* EffectSphere::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectSphere::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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
		m_p->m_curState=AdvanceState(m_p->m_curState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_curState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_curState>=0 
			&& m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];

		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			size_t nextState=m_p->m_curState+1;
			bool bLerp=true;
			if(nextState>=m_p->m_keyStates.size())
			{
				if(!m_p->m_prop.m_loop)//已经播放到最后一个状态,而且不是循环播放,则不需插值
					bLerp=false;
				nextState=0;
			}

			if(bLerp)
			{
				tagState& ts=m_p->m_keyStates[nextState];
				float r=m_p->m_stateTime/curState.lastTime;

				//lerp state
				D3DXVec4Lerp((Vector4*)&curState.color,(Vector4*)&curState.color,(Vector4*)&ts.color,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
				D3DXVec3Lerp(&curState.sacle,&curState.sacle,&ts.sacle,r);
			}
		}

		m_p->curRot+=curState.rotVel*deltaTime;

		Vector3 pos=m_p->m_prop.offset;
		EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
		if(pPath)
			pos+=pPath->GetCurPos();

		if(m_bClosing)//如果正在关闭,则将alpha降低
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.color.A*=m_closingFade;
		}

		Color4ub cc=curState.color;
		m_p->UpdateVert(cc,m_p->curRot,curState.sacle,pos);

		//--update mtl
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectSphere::BeginBuild(const tagSphereProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_curState=0;

		m_p->m_prop=prop;
	}

	void EffectSphere::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectSphere::EndBuild()
	{
		m_p->Destroy();

		ResetPlayState();
		ASSERT(m_p->m_keyStates.size()>0);
		tagState& initState=m_p->m_keyStates[0];
		Color4ub cc=initState.color;
		m_p->InitSphereMesh(cc);
		m_p->UpdateVert(cc,m_p->curRot,initState.sacle,MathConst::Zero3);
	}

	void EffectSphere::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;

		m_p->curRot=Deg2Rad3f(m_p->m_prop.initRot);

		m_p->m_bActive=m_p->m_prop.m_bActive;
		//todo
		//if(!m_p->m_bActive)
		//	m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}
	const TCHAR* EffectSphere::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectSphere::tagSphereProp EffectSphere::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectSphere::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectSphere::tagState EffectSphere::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectSphere::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectSphere::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	float EffectSphere::GetPlayTime()
	{
		if(m_p->m_prop.m_loop)
			return -1;

		float time=0;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			time+=m_p->m_keyStates[i].lastTime;
		}
		return time;
	}

	bool EffectSphere::GetBox(AABBox& out) const
	{
		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			AABBox box;
			box.min = Vector3( -m_p->m_prop.radius, -m_p->m_prop.radius, -m_p->m_prop.radius );
			box.max = Vector3(  m_p->m_prop.radius,  m_p->m_prop.radius,  m_p->m_prop.radius );
			vRot += state.rotVel * state.lastTime;
			Transform trans;
			trans.Translate( m_p->m_prop.offset );
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			trans.Scale( state.sacle.x, state.sacle.y, state.sacle.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectSphere::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D