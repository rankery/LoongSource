#include "StdAfx.h"
#include <deque>
#include ".\effectbeam.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"


namespace Cool3D
{
	IMP_RTTI_DYNC(EffectBeam,EffectBase);

	class EffectBeam::Member
	{
	public:
		DynamicMesh			*m_pRMesh;
		tagBeamProp			m_prop;
		vector<tagState>	m_keyStates;
		int					m_curState;
		float				m_stateTime;
		float				m_sampleDely;	//离上次采样的间隔时间 
		bool				m_bActive;//现在是否已经激活

		deque<Vector3>		m_polyLine;
	public:
		Member()	
		{
			m_pRMesh=NULL;
			m_curState=0;
			m_stateTime=0;
			m_sampleDely=0;
			m_bActive=true;
		}
		~Member()	
		{
			SAFE_DELETE(m_pRMesh);
		}

		void AddSamplePos(const Vector3& pos)
		{
			if((int)m_polyLine.size()>=m_prop.div)
			{
				m_polyLine.pop_front();
			}
			m_polyLine.push_back(pos);	
		}

		void UpdateMesh(const Vector3& eyePos,const tagState& curState)
		{
			//--得到顶点缓冲
			VertStream *pVStream=m_pRMesh->GetVertStream();
			ASSERT(pVStream);
			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
			ASSERT(pVert);
			int vertIndex=0;

			//--填充顶点缓冲
			Vector3 Z,T,C;
			float v=0;

			int div=m_prop.div;
			int numPt=m_polyLine.size();
			ASSERT(numPt<=div);
			for(int i=0;i<numPt-1;
				i++)
			{
				const Vector3& pt=m_polyLine[i];
				const Vector3& nextPt=m_polyLine[i+1];
				Z=eyePos-pt;
				D3DXVec3Normalize(&Z,&Z);
				T=nextPt-pt;
				D3DXVec3Normalize(&T,&T);

				D3DXVec3Cross(&C,&T,&Z);
								
				float w=curState.width+curState.lenWidthInc*i;
				v+=m_prop.divVScale;

				Color4f c;
				D3DXVec4Lerp((Vector4*)&c,
					(Vector4*)&curState.headColor,(Vector4*)&curState.endColor,
					1.0f-(float)i/div);
				Color4ub color=c;

				
				pVert[vertIndex].pos=pt+w*C;
				pVert[vertIndex].uv.x=0;
				pVert[vertIndex].uv.y=v;
				pVert[vertIndex].diffuse=color;
				vertIndex++;

				pVert[vertIndex].pos=pt-w*C;
				pVert[vertIndex].uv.x=1;
				pVert[vertIndex].uv.y=v;
				pVert[vertIndex].diffuse=color;
				vertIndex++;

				//加入尾部两个顶点
				if(i==numPt-2)
				{
					pVert[vertIndex].pos=nextPt+w*C;
					pVert[vertIndex].uv.x=0;
					pVert[vertIndex].uv.y=v;
					pVert[vertIndex].diffuse=color;
					vertIndex++;

					pVert[vertIndex].pos=nextPt-w*C;
					pVert[vertIndex].uv.x=1;
					pVert[vertIndex].uv.y=v;
					pVert[vertIndex].diffuse=color;
					vertIndex++;

					i++;
				}

			}//endof for

			//--如果有剩余的空间,则需要填充成最后的顶点
			//int lastVert=vertIndex-1;
			//if(lastVert>1)
			//{
			//	Vert_PTD v1=pVert[lastVert-1];
			//	Vert_PTD v2=pVert[lastVert];
			//	for(;i<div;i++)
			//	{
			//		pVert[vertIndex++]=v1;
			//		pVert[vertIndex++]=v2;
			//	}
			//}//endof if
			if(vertIndex>2)
				m_pRMesh->SetDrawLimit(vertIndex-2);
			else
				m_pRMesh->SetDrawLimit(0);
		}//endof UpdateMesh()
	};

	EffectBeam::EffectBeam(void)
	{
		m_p=new Member;
	}

	EffectBeam::~EffectBeam(void)
	{
		delete m_p;
	}

	void EffectBeam::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectBeam::Deserialize(FArchive& ar)
	{
		tagBeamProp prop;
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

	RenderNode* EffectBeam::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectBeam::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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

		//--更新Poly line
		m_p->m_sampleDely+=deltaTime;
		if(m_p->m_sampleDely>m_p->m_prop.sampleTime)//是否达到了采样时机
		{
			m_p->m_sampleDely = 0.0f;

			EffectPathCtrl *pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
			if(pPath)
			{
				m_p->AddSamplePos(pPath->GetCurPos());
				m_p->m_pRMesh->SetIdentityWorldMat(false);
			}
			else
			{
				const Matrix4& mat = pSGNode->GetWorldMat();
				m_p->AddSamplePos(Vector3(mat._41,mat._42,mat._43));
				m_p->m_pRMesh->SetIdentityWorldMat(true);
			}
		}


		//--插值出当前的状态
		m_p->m_stateTime+=deltaTime;
		m_p->m_curState=AdvanceState(m_p->m_curState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_curState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_curState>=0 
			&& m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];

		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			int nextState=m_p->m_curState+1;

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

				curState.width	=		LinearLerp(curState.width,ts.width,r);
				curState.lenWidthInc=	LinearLerp(curState.lenWidthInc,ts.lenWidthInc,r);

				D3DXVec4Lerp((Vector4*)&curState.headColor,(Vector4*)&curState.headColor,(Vector4*)&ts.headColor,r);
				D3DXVec4Lerp((Vector4*)&curState.endColor,(Vector4*)&curState.endColor,(Vector4*)&ts.endColor,r);
			}//endof if(bLerp)
		}//endof if

		//--如果正在关闭,则将alpha降低
		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.headColor.A*=m_closingFade;
			curState.endColor.A*=m_closingFade;
		}

		//--update mesh
		Vector3 vEyePos = pCamera->GetPos();
		Matrix4 mat;
		D3DXMatrixInverse( &mat, NULL, &pSGNode->GetWorldMat() );
		vEyePos = vEyePos * mat;
		m_p->UpdateMesh( vEyePos,curState);

		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	const TCHAR* EffectBeam::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectBeam::tagBeamProp EffectBeam::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectBeam::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectBeam::tagState EffectBeam::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectBeam::BeginBuild(const tagBeamProp& prop)
	{
		m_p->m_prop=prop;
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
	}

	void EffectBeam::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectBeam::EndBuild()
	{
		SAFE_DELETE(m_p->m_pRMesh);

		m_p->m_pRMesh=new DynamicMesh;

		m_p->m_pRMesh->Create(m_p->m_prop.m_groupID,
			EDraw_TriangleStrip,EVType_PTD,
			m_p->m_prop.div*2);//每次分割产生一条边

		m_p->m_pRMesh->SetDrawLimit(0);

		ResetPlayState();
	}

	void EffectBeam::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;
		m_p->m_sampleDely=0;
		//--
		m_p->m_polyLine.clear();
		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}

	void EffectBeam::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectBeam::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	float EffectBeam::GetPlayTime()
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

	bool EffectBeam::GetBox(AABBox& out) const
	{
		float fMaxWidth = 0.0f;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			float fWidth = state.width + m_p->m_prop.div * state.lenWidthInc;
			if( fMaxWidth < fWidth )
				fMaxWidth = fWidth;
		}
		const float fHalfMaxWidth = fMaxWidth * 0.5f;
		out.min = Vector3( -fHalfMaxWidth, -fHalfMaxWidth, -fHalfMaxWidth );
		out.max = Vector3( fHalfMaxWidth, fHalfMaxWidth, fHalfMaxWidth );
		return true;
	}

	int EffectBeam::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D