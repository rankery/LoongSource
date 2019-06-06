#include "StdAfx.h"
#include ".\effectcross.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "EffectTarget.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectCross,EffectBase);

	class EffectCross::Member
	{
	public:
		tagCrossProp		m_prop;
		DynamicMesh			*m_pRMesh;
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
			m_bActive=true;
		}
		~Member()	
		{
			Destroy();
		}

		void Destroy()
		{
			SAFE_DELETE(m_pRMesh);
		}

		void AddQuad(Vert_PTD *pVBuf,int& index,Vert_PTD quadVert[4])
		{
			pVBuf[index++]=quadVert[0];
			pVBuf[index++]=quadVert[1];
			pVBuf[index++]=quadVert[2];

			pVBuf[index++]=quadVert[2];
			pVBuf[index++]=quadVert[3];
			pVBuf[index++]=quadVert[0];
		}

		void UpdateMesh(const tagState& state,SGEffectNode* pSGNode)
		{
			VertStream *pVStream=m_pRMesh->GetVertStream();
			ASSERT(pVStream);
			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
			
			int vertIndex=0;
			int i,j;
			Color4ub tmpc=state.color;
			Vert_PTD quadVert[4];
			quadVert[0].uv=Vector2(0,0);
			quadVert[0].diffuse=tmpc;
			quadVert[1].uv=Vector2(0,1);
			quadVert[1].diffuse=tmpc;
			quadVert[2].uv=Vector2(1,1);
			quadVert[2].diffuse=tmpc;
			quadVert[3].uv=Vector2(1,0);
			quadVert[3].diffuse=tmpc;

			Transform trans;
			trans.Translate(m_prop.offset);
			trans.Rotate(curRot.y,curRot.x,curRot.z);

			//变换到路径的空间
			if(NULL!=pSGNode)
			{
				EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_prop.szPathName);
				if(pPath)
				{
					Transform transPath;
					Vector3 rotPath=pPath->GetCurRot();
					transPath.Rotate(rotPath.y,rotPath.x,rotPath.z);
					Vector3 posPath=pPath->GetCurPos();
					transPath.Translate(posPath);
					trans.CombineTransRight(transPath.GetMatrix());
				}
			}

			//先沿着X,Z轴建立一对基本的face
			//x
			quadVert[0].pos.x=-state.size.x;
			quadVert[0].pos.y=state.size.y;
			quadVert[0].pos.z=0;

			quadVert[1].pos.x=-state.size.x;
			quadVert[1].pos.y=-state.size.y;
			quadVert[1].pos.z=0;

			quadVert[2].pos.x=state.size.x;
			quadVert[2].pos.y=-state.size.y;
			quadVert[2].pos.z=0;

			quadVert[3].pos.x=state.size.x;
			quadVert[3].pos.y=state.size.y;
			quadVert[3].pos.z=0;
			for(j=0;j<4;j++)
				trans.TransVec3(quadVert[j].pos);
			AddQuad(pVert,vertIndex,quadVert);

			//z
			quadVert[0].pos.z=-state.size.x;
			quadVert[0].pos.y=state.size.y;
			quadVert[0].pos.x=0;

			quadVert[1].pos.z=-state.size.x;
			quadVert[1].pos.y=-state.size.y;
			quadVert[1].pos.x=0;

			quadVert[2].pos.z=state.size.x;
			quadVert[2].pos.y=-state.size.y;
			quadVert[2].pos.x=0;

			quadVert[3].pos.z=state.size.x;
			quadVert[3].pos.y=state.size.y;
			quadVert[3].pos.x=0;
			for(j=0;j<4;j++)
				trans.TransVec3(quadVert[j].pos);
			AddQuad(pVert,vertIndex,quadVert);

			if(m_prop.bHFace)
			{
				//y
				quadVert[0].pos.x=-state.size.x;
				quadVert[0].pos.z=state.size.y;
				quadVert[0].pos.y=0;

				quadVert[1].pos.x=-state.size.x;
				quadVert[1].pos.z=-state.size.y;
				quadVert[1].pos.y=0;

				quadVert[2].pos.x=state.size.x;
				quadVert[2].pos.z=-state.size.y;
				quadVert[2].pos.y=0;

				quadVert[3].pos.x=state.size.x;
				quadVert[3].pos.z=state.size.y;
				quadVert[3].pos.y=0;
				for(j=0;j<4;j++)
					trans.TransVec3(quadVert[j].pos);
				AddQuad(pVert,vertIndex,quadVert);
			}

			//建立扩展face序列
			for(i=0;i<m_prop.numFace;i++)
			{
				float dist=state.faceDist+(state.faceDist+state.faceDistInc)*i;
				Vector2 size=state.size+state.faceSizeInc*(float)i;
				Color4f color=state.color;
				color.A-=state.faceAlphaDec*i;
				if(color.A<0.0f)
					color.A=0;
				tmpc=color;
				//沿z正向复制x方向的面
				quadVert[0].pos.x=-size.x;
				quadVert[0].pos.y=size.y;
				quadVert[0].pos.z=dist;

				quadVert[1].pos.x=-size.x;
				quadVert[1].pos.y=-size.y;
				quadVert[1].pos.z=dist;

				quadVert[2].pos.x=size.x;
				quadVert[2].pos.y=-size.y;
				quadVert[2].pos.z=dist;

				quadVert[3].pos.x=size.x;
				quadVert[3].pos.y=size.y;
				quadVert[3].pos.z=dist;
				for(j=0;j<4;j++)
				{
					trans.TransVec3(quadVert[j].pos);
					quadVert[j].diffuse=tmpc;
				}
				AddQuad(pVert,vertIndex,quadVert);

				//沿z反向复制x方向的面
				quadVert[0].pos.x=-size.x;
				quadVert[0].pos.y=size.y;
				quadVert[0].pos.z=-dist;

				quadVert[1].pos.x=-size.x;
				quadVert[1].pos.y=-size.y;
				quadVert[1].pos.z=-dist;

				quadVert[2].pos.x=size.x;
				quadVert[2].pos.y=-size.y;
				quadVert[2].pos.z=-dist;

				quadVert[3].pos.x=size.x;
				quadVert[3].pos.y=size.y;
				quadVert[3].pos.z=-dist;
				for(j=0;j<4;j++)
				{
					trans.TransVec3(quadVert[j].pos);
					quadVert[j].diffuse=tmpc;
				}
				AddQuad(pVert,vertIndex,quadVert);

				//x正向
				quadVert[0].pos.z=-size.x;
				quadVert[0].pos.y=size.y;
				quadVert[0].pos.x=dist;

				quadVert[1].pos.z=-size.x;
				quadVert[1].pos.y=-size.y;
				quadVert[1].pos.x=dist;

				quadVert[2].pos.z=size.x;
				quadVert[2].pos.y=-size.y;
				quadVert[2].pos.x=dist;

				quadVert[3].pos.z=size.x;
				quadVert[3].pos.y=size.y;
				quadVert[3].pos.x=dist;
				for(j=0;j<4;j++)
				{
					trans.TransVec3(quadVert[j].pos);
					quadVert[j].diffuse=tmpc;
				}
				AddQuad(pVert,vertIndex,quadVert);

				//x反向
				quadVert[0].pos.z=-size.x;
				quadVert[0].pos.y=size.y;
				quadVert[0].pos.x=-dist;

				quadVert[1].pos.z=-size.x;
				quadVert[1].pos.y=-size.y;
				quadVert[1].pos.x=-dist;

				quadVert[2].pos.z=size.x;
				quadVert[2].pos.y=-size.y;
				quadVert[2].pos.x=-dist;

				quadVert[3].pos.z=size.x;
				quadVert[3].pos.y=size.y;
				quadVert[3].pos.x=-dist;
				for(j=0;j<4;j++)
				{
					trans.TransVec3(quadVert[j].pos);
					quadVert[j].diffuse=tmpc;
				}
				AddQuad(pVert,vertIndex,quadVert);

				if(m_prop.bHFace)
				{
					//y正向
					quadVert[0].pos.x=-state.size.x;
					quadVert[0].pos.z=state.size.y;
					quadVert[0].pos.y=dist;

					quadVert[1].pos.x=-state.size.x;
					quadVert[1].pos.z=-state.size.y;
					quadVert[1].pos.y=dist;

					quadVert[2].pos.x=state.size.x;
					quadVert[2].pos.z=-state.size.y;
					quadVert[2].pos.y=dist;

					quadVert[3].pos.x=state.size.x;
					quadVert[3].pos.z=state.size.y;
					quadVert[3].pos.y=dist;
					for(j=0;j<4;j++)
						trans.TransVec3(quadVert[j].pos);
					AddQuad(pVert,vertIndex,quadVert);

					//y反向
					quadVert[0].pos.x=-state.size.x;
					quadVert[0].pos.z=state.size.y;
					quadVert[0].pos.y=-dist;

					quadVert[1].pos.x=-state.size.x;
					quadVert[1].pos.z=-state.size.y;
					quadVert[1].pos.y=-dist;

					quadVert[2].pos.x=state.size.x;
					quadVert[2].pos.z=-state.size.y;
					quadVert[2].pos.y=-dist;

					quadVert[3].pos.x=state.size.x;
					quadVert[3].pos.z=state.size.y;
					quadVert[3].pos.y=-dist;
					for(j=0;j<4;j++)
						trans.TransVec3(quadVert[j].pos);
					AddQuad(pVert,vertIndex,quadVert);
				}//endof if(h face)
			}//endof for
			m_pRMesh->SetDrawLimit(-1);
		}
	};

	EffectCross::EffectCross(void)
	{
		m_p=new Member;
	}

	EffectCross::~EffectCross(void)
	{
		delete m_p;
	}

	void EffectCross::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectCross::Deserialize(FArchive& ar)
	{
		tagCrossProp prop;
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

	RenderNode* EffectCross::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectCross::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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
				D3DXVec2Lerp(&curState.size,&curState.size,&ts.size,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
				curState.faceDist=LinearLerp(curState.faceDist,ts.faceDist,r);
				curState.faceDistInc=LinearLerp(curState.faceDistInc,ts.faceDistInc,r);
				D3DXVec2Lerp(&curState.faceSizeInc,&curState.faceSizeInc,&ts.faceSizeInc,r);
			}
		}//endof if

		m_p->curRot+=curState.rotVel*deltaTime;

		if(m_bClosing)//如果正在关闭,则将alpha降低
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.color.A*=m_closingFade;
		}
		m_p->UpdateMesh(curState,pSGNode);

		//--update mtl
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectCross::BeginBuild(const tagCrossProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_curState=0;

		m_p->m_prop=prop;
	}

	void EffectCross::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectCross::EndBuild()
	{
		m_p->Destroy();

		//-- 创建face序列需要的Mesh
		m_p->m_pRMesh=new DynamicMesh;
		int numFace=0;
		if(m_p->m_prop.bHFace)
			numFace=m_p->m_prop.numFace*6+3;
		else
			numFace=m_p->m_prop.numFace*4+2;
		
		m_p->m_pRMesh->Create(m_p->m_prop.m_groupID,
			EDraw_TriangleList,EVType_PTD,
			numFace*6);//每个face为一个quad,需要两个三角形
		VertStream *pVStream=m_p->m_pRMesh->GetVertStream();
		memset(pVStream->GetRawBuffer(),0,pVStream->GetBufferSize());

		ResetPlayState();

		ASSERT(m_p->m_keyStates.size()>0);
		tagState& initState=m_p->m_keyStates[0];
		m_p->UpdateMesh(initState,NULL);
	}

	void EffectCross::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;

		m_p->curRot=Deg2Rad3f(m_p->m_prop.initRot);

		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}
	const TCHAR* EffectCross::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectCross::tagCrossProp EffectCross::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectCross::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectCross::tagState EffectCross::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectCross::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectCross::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	float EffectCross::GetPlayTime()
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

	bool EffectCross::GetBox(AABBox& out) const
	{
		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			Vector2 size(0.0f,0.0f);
			float dist=0.0f;
			if( m_p->m_prop.numFace > 0 )
			{
				dist=state.faceDist+(state.faceDist+state.faceDistInc)*(float)(m_p->m_prop.numFace-1);
				size=state.size+state.faceSizeInc*(float)(m_p->m_prop.numFace-1);
			}
			else
			{
				dist=state.faceDist;
				size=state.size;
			}
			Vector3 vHalfSize(0.0f,0.0f,0.0f);
			if( m_p->m_prop.bHFace )
			{
				vHalfSize.x = max( size.x, dist );
				vHalfSize.y = max( size.y, dist );
				vHalfSize.z = max( max( size.x, state.size.y ), dist );
			}
			else
			{
				vHalfSize.x = max( size.x, dist );
				vHalfSize.y = size.y;
				vHalfSize.z = max( size.x, dist );
			}
			AABBox box;
			box.min = -vHalfSize;
			box.max =  vHalfSize;
			Transform trans;
			trans.Translate(m_p->m_prop.offset);
			vRot += state.rotVel * state.lastTime;
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectCross::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D