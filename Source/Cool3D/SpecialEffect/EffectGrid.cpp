#include "StdAfx.h"
#include ".\effectgrid.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\Math\Noise.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "..\Math\Transform.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectGrid,EffectBase);


	class EffectGrid::Member
	{
	public:
		tagGridProp			m_prop;
		DynamicMeshIndexed	*m_pRMesh;

		vector<tagState>	m_keyStates;	//每个关键帧的状态
		int					m_curState;		//当前的关键帧状态index
		float				m_stateTime;	//当前状态保持了多少时间

		Vector3				curRot;	//当前自转
		bool				m_bActive;//现在是否已经激活

	public:
		Member():curRot(0,0,0)
		{
			m_pRMesh=NULL;
			m_curState=0;
			m_stateTime=0;
			m_bActive=true;
		}

		~Member()
		{
			SAFE_DELETE(m_pRMesh);
		}

		void UpdateMesh(const Vector3& pos,const Vector2& size,const Vector3& rot,DWORD color,
			const Vector2& uvScale,float waveSpeed,const Vector3& wavePower,float runTime,
			CameraBase *pCamera,const Matrix4& worldMat)
		{
			if(m_pRMesh==NULL || m_pRMesh->GetVertStream()==NULL)
				return;
			VertStream *pVStream=m_pRMesh->GetVertStream();
			Vert_PTD *pVert=(Vert_PTD*)pVStream->GetRawBuffer();

			int wRes=m_prop.xDiv;
			int hRes=m_prop.yDiv;//for short

			float uStep=uvScale.x/(wRes-1);
			float vStep=uvScale.y/(hRes-1);

			float minX=-size.x*0.5f;
			float minY=-size.y*0.5f;
			float xStep=size.x/wRes;
			float yStep=size.y/hRes;

			Matrix4 mdlView=worldMat;
			mdlView*=(*pCamera->GetView());

			Vector3 viewRight(mdlView._11,mdlView._21,mdlView._31);
			Vector3 viewUp(mdlView._12,mdlView._22,mdlView._32);
			Vector3 viewForward(mdlView._13,mdlView._23,mdlView._33);

			//--计算变换矩阵
			Transform trans;
			trans.Translate(pos);
			trans.Rotate(rot.y,rot.x,rot.z);

			int x,y;
			for(y=0;y<hRes;y++)
			{
				for(x=0;x<wRes;x++)
				{
					float fx=x+1.0f;
					float fy=y+1.0f;
			
					float fsin=sinf((runTime+fx)*waveSpeed+fy+fx);
					float fcos=cosf((runTime+fy)*waveSpeed+fx+fy);
					if(m_prop.faceType==2 ||
						m_prop.faceType==3)//公告板
					{
						Vector3 xaxis=viewRight;
						Vector3 yaxis=MathConst::AxisY;
						if(m_prop.faceType==2)
							yaxis=viewUp;
						float xOffset=x*xStep+minX	+wavePower.x*(fsin-fcos);
						float yOffset=y*yStep+minY	+wavePower.y*(fsin+fcos);
						float zOffset=wavePower.z*fsin;;

						pVert->pos=pos+xaxis*xOffset+yaxis*yOffset+viewForward*zOffset;
					}
					else//平面或者双曲面
					{
						pVert->pos.x=x*xStep+minX	+wavePower.x*(fsin-fcos);
						pVert->pos.y=y*yStep+minY	+wavePower.y*(fsin+fcos);
						pVert->pos.z=wavePower.z*fsin;

						if(m_prop.faceType==1)//双曲面
						{
							pVert->pos.z+=sqrt(pVert->pos.x*pVert->pos.x
								+pVert->pos.y*pVert->pos.y);
						}

						trans.TransVec3(pVert->pos);
					}


					pVert->uv.x=x*uStep;
					pVert->uv.y=1.0f-y*vStep;

					pVert->diffuse=color;
					pVert++;
				}
			}//endof for(y)
			//todo setdraw limit
		}
	};
	

	EffectGrid::EffectGrid(void)
	{
		m_p=new Member;
	}

	EffectGrid::~EffectGrid(void)
	{
		delete m_p;
	}

	void EffectGrid::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectGrid::Deserialize(FArchive& ar)
	{
		tagGridProp prop;
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

	RenderNode* EffectGrid::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectGrid::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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

				D3DXVec4Lerp((Vector4*)&curState.color,(Vector4*)&curState.color,(Vector4*)&ts.color,r);
				D3DXVec2Lerp(&curState.uvScale,&curState.uvScale,&ts.uvScale,r);
				D3DXVec2Lerp(&curState.size,&curState.size,&ts.size,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);

				curState.waveSpeed=LinearLerp(curState.waveSpeed,ts.waveSpeed,r);
				D3DXVec3Lerp(&curState.wavePower,&curState.wavePower,&ts.wavePower,r);

			}//endof if(bLerp)
		}

		m_p->curRot+=curState.rotVel*deltaTime;

		EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
		Vector3 pos=m_p->m_prop.offset;
		if(pPath)
			pos+=pPath->GetCurPos();

		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.color.A*=m_closingFade;
		}

		Color4ub color=curState.color;
		m_p->UpdateMesh(pos,curState.size,m_p->curRot,color,
			curState.uvScale,curState.waveSpeed,curState.wavePower,runTime,
			pCamera,pSGNode->GetWorldMat());

		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	
	void EffectGrid::ResetPlayState()
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

	const TCHAR* EffectGrid::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectGrid::tagGridProp EffectGrid::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectGrid::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectGrid::tagState EffectGrid::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectGrid::BeginBuild(const tagGridProp& prop)
	{
		m_p->m_prop=prop;
		m_p->m_keyStates.clear();
	}

	void EffectGrid::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectGrid::EndBuild()
	{
		//--build mesh
		SAFE_DELETE(m_p->m_pRMesh);
		m_p->m_pRMesh=new DynamicMeshIndexed;
		
		//创建VertStream
		int wRes=m_p->m_prop.xDiv;
		int hRes=m_p->m_prop.yDiv;//for short
		VertStream *pVStream=new VertStream;
		m_p->m_pRMesh->AttachVertStream(pVStream);
		pVStream->Create(EVType_PTD,hRes*wRes);


		//创建Index buffer
		vector<WORD> indices;
		int x,y;
		for(y=1;y<hRes-1;y+=2)
			for(x=1;x<wRes-1;x+=2)
			{
				indices.push_back(y*wRes+x);
				indices.push_back((y-1)*wRes+(x+1));
				indices.push_back(y*wRes+(x+1));

				indices.push_back(y*wRes+x);
				indices.push_back((y-1)*wRes+x);
				indices.push_back((y-1)*wRes+(x+1));

				indices.push_back(y*wRes+x);
				indices.push_back((y-1)*wRes+(x-1));
				indices.push_back((y-1)*wRes+x);

				indices.push_back(y*wRes+x);
				indices.push_back(y*wRes+(x-1));
				indices.push_back((y-1)*wRes+x-1);

				indices.push_back(y*wRes+x);
				indices.push_back((y+1)*wRes+(x-1));
				indices.push_back(y*wRes+x-1);	

				indices.push_back(y*wRes+x);
				indices.push_back((y+1)*wRes+x);
				indices.push_back((y+1)*wRes+x-1);	

				indices.push_back(y*wRes+x);
				indices.push_back((y+1)*wRes+x+1);
				indices.push_back((y+1)*wRes+x);	

				indices.push_back(y*wRes+x);
				indices.push_back(y*wRes+x+1);
				indices.push_back((y+1)*wRes+x+1);	
			}//endof for

		m_p->m_pRMesh->CreateDirect(EDraw_TriangleList,
			0,m_p->m_prop.m_groupID,
			hRes*wRes,sizeof(WORD)*indices.size(),&(*indices.begin()));

		//--set play state
		ResetPlayState();

		//--init mesh
		/*ASSERT(m_p->m_keyStates.size()>0);
		tagState& initState=m_p->m_keyStates[0];
		Color4ub tc=initState.color;
		m_p->UpdateMesh(m_p->m_prop.offset,initState.size,m_p->curRot,tc,initState.uvScale,
			initState.waveSpeed,initState.wavePower,0);*/
	}

	void EffectGrid::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}
	void EffectGrid::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	float EffectGrid::GetPlayTime()
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

	bool EffectGrid::GetBox(AABBox& out) const
	{
		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			Vector2 vHalfSize = state.size * 0.5f;

			AABBox box;
			box.min.x = -vHalfSize.x - state.wavePower.x;
			box.min.y = -vHalfSize.y - state.wavePower.y;
			box.min.z = -state.wavePower.z;
			box.max.x = vHalfSize.x + state.wavePower.x;
			box.max.y = vHalfSize.y + state.wavePower.y;
			box.max.z = state.wavePower.z;
			Transform trans;
			trans.Translate(m_p->m_prop.offset);
			vRot += state.rotVel * state.lastTime;
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectGrid::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D