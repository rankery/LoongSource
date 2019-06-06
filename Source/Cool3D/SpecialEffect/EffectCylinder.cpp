#include "StdAfx.h"
#include ".\effectcylinder.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "EffectTarget.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectCylinder,EffectBase);

	class EffectCylinder::Member
	{
	public:
		DynamicMesh			*m_pRMesh;
		tagCylinderProp		m_prop;
		vector<tagState>	m_keyStates;
		int					m_curState;
		float				m_stateTime;
		bool				m_bActive;//现在是否已经激活
		
		Vector3				curRot;
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

		void UpdateMesh(const tagState& state,const Vector3& rot,SGEffectNode* pSGNode)
		{
			//--得到顶点缓冲
			VertStream *pVStream=m_pRMesh->GetVertStream();
			ASSERT(pVStream);
			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
			ASSERT(pVert);
			int vertIndex=0;

			//--变化颜色
			Color4ub tmpC(state.topColor);
			DWORD topC=tmpC;
			tmpC=state.bottomColor;
			DWORD bottomC=tmpC;

			//--计算变换矩阵
			Transform trans;
			trans.Translate(m_prop.offset);
			trans.Rotate(rot.y,rot.x,rot.z);

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

			//--填充顶点缓冲
			int div=m_prop.div;
			float angle=0;
			float angleStep=(FLOAT_PI*2*state.numCircle)/div;

			bool bShrink = true;	// 收缩周期
			float fSin, fCos;
			float topR,bottomR;
			float topH,bottomH;
			int numVertUsed=0;
			float u=0;
			float uStep=m_prop.uScale/div;
			int i;
			for(i=0;i<div;i++)
			{
				angle=i*angleStep;
				fSin=sinf(angle);
				fCos=cosf(angle);

				topR=state.topRadius+angle*state.divTopRadiusInc;
				bottomR=state.bottomRadius+angle*state.divBottomRadiusInc;

				topH=state.high+angle*state.divTopRise;
				bottomH=angle*state.divBottomRise;

				u=i*uStep;
				
				if(bShrink)
				{
					topR-=state.topShrink;
					bottomR-=state.bottomShrink;
				}
				bShrink=!bShrink;


				//增加一条边上的两个顶点
				//添加顶部顶点
				pVert[numVertUsed].diffuse=topC;
				pVert[numVertUsed].pos.x=topR*fSin;
				pVert[numVertUsed].pos.y=topH;
				pVert[numVertUsed].pos.z=topR*fCos;
				trans.TransVec3(pVert[numVertUsed].pos);
				pVert[numVertUsed].uv.x=u;
				pVert[numVertUsed].uv.y=0.0f;
				numVertUsed++;
				//添加底部顶点
				pVert[numVertUsed].diffuse=bottomC;
				pVert[numVertUsed].pos.x=bottomR*fSin;
				pVert[numVertUsed].pos.y=bottomH;
				pVert[numVertUsed].pos.z=bottomR*fCos;
				trans.TransVec3(pVert[numVertUsed].pos);
				pVert[numVertUsed].uv.x=u;
				pVert[numVertUsed].uv.y=1.0f;
				numVertUsed++;
			}//endof for

			if(state.divTopRise>0.0f ||state.divBottomRise>0.0f)
			{
				//如果是带状,则不形成封闭形
				int n=numVertUsed-1;
				pVert[numVertUsed++]=pVert[n-1];
				pVert[numVertUsed++]=pVert[n];
			}
			else
			{
				//形成封闭形
				float lastU=uStep*i;
				pVert[numVertUsed]=pVert[0];
				pVert[numVertUsed].uv.x=lastU;
				numVertUsed++;
				pVert[numVertUsed]=pVert[1];
				pVert[numVertUsed].uv.x=lastU;
				numVertUsed++;
			}

			ASSERT(numVertUsed <= (int)pVStream->GetNumVert());
			m_pRMesh->SetDrawLimit(-1);

		}
	};

	EffectCylinder::EffectCylinder(void)
	{
		m_p=new Member;
	}

	EffectCylinder::~EffectCylinder(void)
	{
		delete m_p;
	}

	void EffectCylinder::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectCylinder::Deserialize(FArchive& ar)
	{
		tagCylinderProp prop;
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

	RenderNode* EffectCylinder::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectCylinder::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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

				curState.numCircle	=	LinearLerp(curState.numCircle,ts.numCircle,r);
				curState.high		=	LinearLerp(curState.high,ts.high,r);
				curState.topRadius	=	LinearLerp(curState.topRadius,ts.topRadius,r);
				curState.bottomRadius=	LinearLerp(curState.bottomRadius,ts.bottomRadius,r);
				curState.divTopRadiusInc=LinearLerp(curState.divTopRadiusInc,ts.divTopRadiusInc,r);
				curState.divBottomRadiusInc=LinearLerp(curState.divBottomRadiusInc,ts.divBottomRadiusInc,r);
				curState.topShrink	=	LinearLerp(curState.topShrink,ts.topShrink,r);
				curState.bottomShrink=	LinearLerp(curState.bottomShrink,ts.bottomShrink,r);
				curState.divTopRise	=	LinearLerp(curState.divTopRise,ts.divTopRise,r);
				curState.divBottomRise	=	LinearLerp(curState.divBottomRise,ts.divBottomRise,r);

				D3DXVec4Lerp((Vector4*)&curState.topColor,(Vector4*)&curState.topColor,(Vector4*)&ts.topColor,r);
				D3DXVec4Lerp((Vector4*)&curState.bottomColor,(Vector4*)&curState.bottomColor,(Vector4*)&ts.bottomColor,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
			}//endof if(bLerp)
		}
		
		m_p->curRot+=curState.rotVel*deltaTime;

		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			curState.topColor.A*=m_closingFade;
			curState.bottomColor.A*=m_closingFade;
		}

		m_p->UpdateMesh(curState,m_p->curRot,pSGNode);

		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectCylinder::BeginBuild(const tagCylinderProp& prop)
	{
		m_p->m_prop=prop;
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
	}

	void EffectCylinder::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectCylinder::EndBuild()
	{
		SAFE_DELETE(m_p->m_pRMesh);

		m_p->m_pRMesh=new DynamicMesh;
		m_p->m_pRMesh->Create(m_p->m_prop.m_groupID,
			EDraw_TriangleStrip,EVType_PTD,
			m_p->m_prop.div*2+2);//顶部,底部各一个顶点,加上起始边的两个顶点(重复,为了形成封闭形)

		m_p->m_pRMesh->SetDrawLimit(0);

		ResetPlayState();
	}

	void EffectCylinder::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;
		
		m_p->curRot=Deg2Rad3f(m_p->m_prop.initRot);
				
		ASSERT(m_p->m_keyStates.size()>0);
		tagState st=m_p->m_keyStates[0];
		m_p->UpdateMesh(st,m_p->curRot,NULL);

		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}

	void EffectCylinder::CreateMtl(const tagMtlEffectDesc& desc)
	{
		EffectBase::CreateMtl(desc);
		if(m_pMtl)
		{
			m_pMtl->m_addressU=ETAddress_Wrap;//水平方向需要重复
			m_pMtl->m_addressV=ETAddress_Clamp;//垂直方向不重复
		}
	}

	const TCHAR* EffectCylinder::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectCylinder::tagCylinderProp EffectCylinder::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectCylinder::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectCylinder::tagState EffectCylinder::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectCylinder::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectCylinder::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	float EffectCylinder::GetPlayTime()
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

	bool EffectCylinder::GetBox(AABBox& out) const
	{
		bool bShrink = true;
		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			float angle=FLOAT_PI*2*state.numCircle;

			float topR=state.topRadius;
			if( state.divTopRadiusInc > 0.0f )
				topR+=angle*state.divTopRadiusInc;

			float bottomR=state.bottomRadius;
			if( state.divBottomRadiusInc > 0.0f )
				bottomR+=angle*state.divBottomRadiusInc;

			float topH=state.high+angle*state.divTopRise;

			if(bShrink)
			{
				topR-=state.topShrink;
				bottomR-=state.bottomShrink;
			}
			bShrink=!bShrink;
			float fR=max(topR,bottomR);
			AABBox box;
			box.min = Vector3( -fR, 0.0f, -fR );
			box.max = Vector3( fR, topH, fR );
			Transform trans;
			trans.Translate(m_p->m_prop.offset);
			vRot += state.rotVel * state.lastTime;
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectCylinder::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D