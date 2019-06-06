#include "StdAfx.h"
#include ".\effectparticleemitter.h"
#include "..\RenderSys\DynamicMeshIndexed.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Util\ObjectPool.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"
#include "..\SceneGraph\HugeQuadTreeSG.h"

const int Particle_Max_Per_Effect = 300;
const int Particle_Max_Toal = 2000;
int Particle_Total_Num = 0;
bool EffectParticleEmitter::s_bLimitMaxParticleNum=false;

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectParticleEmitter,EffectBase);

	class Particle
	{
	public:
		float	maxLifeTime;//总共能生存多久
		float	lifeTime;	//生存的时间
		Vector3	pos;	//当前位置
		Vector3 vel;	//当前速度
		Vector3 lastPos;//上一次记录下来的位置
		
		float	swing;	//摇摆幅度,<=0忽略
		float	size;	//当前大小
		float	sizeVel;//大小的变化速度
		Color4f	color;	//当前颜色
		Color4f colorVel;//当前颜色变化速度,alpha小于等于0则粒子死亡
		Vector3	rot;	//当前自转
		Vector3 rotSpeed;//自转速度
	public:
		Particle()
		{	}

		~Particle()
		{}

		void InitState()
		{
			memset(this,0,sizeof(*this));
			color=Color4f(1,1,1,1);
		}

		//返回是否仍然活着
		bool Update(float deltaTime,float gForce,float airForce)
		{
			lifeTime+=deltaTime;
			if(lifeTime>=maxLifeTime)
				return false;
			
			
			Vector3 airAcc=airForce*vel;//空气阻力随速率的增大而线性增大?
			Vector3 gAcc=MathConst::AxisY*gForce;
			
			vel+=(airAcc+gAcc)*deltaTime;
			lastPos=pos;
			pos+=vel*deltaTime;
			if(swing>0)
			{
				pos.x+=sinf(lifeTime)*swing*deltaTime;
				pos.z+=cosf(lifeTime)*swing*deltaTime;
			}
			size+=sizeVel*deltaTime;
			color.R+=colorVel.R*deltaTime;
			color.G+=colorVel.G*deltaTime;
			color.B+=colorVel.B*deltaTime;
			color.A+=colorVel.A*deltaTime;

			rot+=rotSpeed*deltaTime;

			if(color.A<0)//透明度为0,则算做死亡
				return false;
			return true;
		}
	};

	class EffectParticleEmitter::Member
	{
	public:
		tagPEMProp			m_prop;
		vector<tagState>	m_keyStates;	//每个关键帧的状态
		int					m_curState;		//当前的关键帧状态index
		float				m_stateTime;	//当前状态保持了多少时间
		Vector3				m_curRot;		//发射器当前自转
		bool				m_bActive;//现在是否已经激活
		
		DynamicMesh			*m_pMesh;

		float					m_spawnCount;
		list<Particle*>			m_activeParList;//活动的粒子
		ObjectPool<Particle>	*m_pParPool;//粒子对象池

		float					m_parSizeScale; // 单个粒子的缩放
		float					m_emitterScale; // 发射器的缩放

	public:
		Member():m_curRot(0,0,0),m_parSizeScale(1),m_emitterScale(1)
		{
			m_spawnCount = 0;
			m_curState=0;
			m_stateTime=0;
			m_pMesh=NULL;
			m_pParPool=NULL;
			m_bActive=true;
		}

		~Member()
		{
			SAFE_DELETE(m_pMesh);

			FreeAllParticles();
		}

		void FreeAllParticles()
		{
			Particle_Total_Num -= m_activeParList.size();

			for(list<Particle*>::iterator iter=m_activeParList.begin();
				iter!=m_activeParList.end();++iter)
			{
				m_pParPool->FreeObj(*iter);
			}
			m_activeParList.clear();

			SAFE_DELETE(m_pParPool);
		}

		void EmitterOneParticle(tagState emState,const Vector3& emitterPos)
		{
			if(m_pParPool==NULL
				|| m_pParPool->IsEmpty())
				return;

			Particle_Total_Num++;

			Particle *par=m_pParPool->AllocObj();
			par->InitState();
			m_activeParList.push_back(par);

			par->maxLifeTime=GetRandom(emState.parLifeTimeMin,emState.parLifeTimeMax);

			//产生随机位置
			switch(m_prop.shape)
			{
			case 0://球
				{
					float radius=emState.shapeParam.x*m_emitterScale;
					//因为时球形发射器,所以忽略掉当前发射器的自转
					par->pos.x=GetRandom(-radius,radius);
					par->pos.y=GetRandom(-radius,radius);
					par->pos.z=GetRandom(-radius,radius);
				}
				break;
			case 1://box
				{
					Vector3 size(emState.shapeParam.x*m_emitterScale,emState.shapeParam.y*m_emitterScale,emState.shapeParam.z*m_emitterScale);
					par->pos.x=GetRandom(-size.x,size.x);
					par->pos.y=GetRandom(-size.y,size.y);
					par->pos.z=GetRandom(-size.z,size.z);

					Transform trans;
					trans.Rotate(m_curRot.y,m_curRot.x,m_curRot.z);
					trans.TransVec3(par->pos);
				}
				break;
			case 2://cone
				{
					float h=GetRandom(0.0f,emState.shapeParam.z*m_emitterScale);
					float maxR=LinearLerp(emState.shapeParam.x,emState.shapeParam.y,
						h/emState.shapeParam.z);
					float myR=GetRandom(0.0f,maxR);
					float angle=GetRandom(0.0f,FLOAT_PI*2);

					par->pos.x=myR*sinf(angle);
					par->pos.y=h;
					par->pos.z=myR*cosf(angle);

					Transform trans;
					trans.Rotate(m_curRot.y,m_curRot.x,m_curRot.z);
					trans.TransVec3(par->pos);
				}
				break;
			default:
				par->pos.x=emState.shapeParam.x*m_emitterScale;
				par->pos.y=emState.shapeParam.y*m_emitterScale;
				par->pos.z=emState.shapeParam.z*m_emitterScale;
				break;
			}//endof switch

			par->pos+=emitterPos;
			par->lastPos=par->pos;
			//初始化粒子的速度
			float parSpeed=GetRandom(emState.parSpeedMin,emState.parSpeedMax);
			switch(m_prop.velType)
			{
			case 0://离心方向
				{
					Vector3 vel=par->pos-emitterPos;
					D3DXVec3Normalize(&par->vel,&vel);
					par->vel*=parSpeed;
				}
				break;
			case 1://指定方向
				D3DXVec3Normalize(&par->vel,&emState.parVelDir);
				par->vel*=parSpeed;
				break;
			}

			par->swing=GetRandom(emState.parSwingMin,emState.parSwingMax);
			par->size=GetRandom(emState.parSizeMin,emState.parSizeMax);
			par->sizeVel=emState.parSizeVel;

			par->rot = Deg2Rad3f(m_prop.initRot);  // 初始自转置为其公转角度
			par->rotSpeed.x=GetRandom(emState.parRotSpeedMin.x,emState.parRotSpeedMax.x);
			par->rotSpeed.y=GetRandom(emState.parRotSpeedMin.x,emState.parRotSpeedMax.y);
			par->rotSpeed.z=GetRandom(emState.parRotSpeedMin.x,emState.parRotSpeedMax.z);
			if(par->rotSpeed!=Vector3(0,0,0))
			{
				par->rot.x=GetRandom(0.0f,FLOAT_PI*2);
				par->rot.y=GetRandom(0.0f,FLOAT_PI*2);
				par->rot.z=GetRandom(0.0f,FLOAT_PI*2);
			}

			par->color=emState.parColor;
			par->colorVel=emState.parColorVel;
			if(m_prop.bParAutoFade)
				par->colorVel.A=(par->color.A)/par->maxLifeTime*-1.0f;
		}

		void FillQuadVertUV(float startU,float startV,float scaleU,float scaleV,Vert_PTD* quadVert,DWORD color)
		{
			quadVert[0].uv.x=startU;
			quadVert[0].uv.y=startV;
			quadVert[0].diffuse=color;

			quadVert[1].uv.x=startU;
			quadVert[1].uv.y=startV+scaleV;
			quadVert[1].diffuse=color;

			quadVert[2].uv.x=startU+scaleU;
			quadVert[2].uv.y=startV+scaleV;
			quadVert[2].diffuse=color;

			quadVert[3].uv.x=startU+scaleU;
			quadVert[3].uv.y=startV;
			quadVert[3].diffuse=color;
		}

		void CopyParListAsSprite(const Matrix4& worldMat,CameraBase *pCamera,const tagMtlEffectDesc& mtlDesc)
		{
			if(m_pMesh==NULL)
				return;
			//--得到vert stream
			VertStream *pVStream=m_pMesh->GetVertStream();
			if(pVStream == NULL)
				return;

			ASSERT(pVStream);
			ASSERT(pVStream->GetNumVert()>=m_activeParList.size()*2*3);

			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();

			Matrix4 mdlView=worldMat;

			if(m_prop.bToWorld)
				mdlView= (*pCamera->GetView());
			else
				mdlView*=(*pCamera->GetView());

			Vector3 viewRight(mdlView._11,mdlView._21,mdlView._31);
			Vector3 viewUp(mdlView._12,mdlView._22,mdlView._32);
			Vector3 viewForward(mdlView._13,mdlView._23,mdlView._33);
			//--填充vert stream
			Color4ub tmpC;

			Vert_PTD quadVert[4];
			int i=0;
			float startU=0,startV=0;
			float scaleU=1.0f/mtlDesc.frmU;
			float scaleV=1.0f/mtlDesc.frmV;

			Transform trans;
			for(list<Particle*>::iterator iter=m_activeParList.begin();
				iter!=m_activeParList.end();++iter)
			{
				Particle *par=(*iter);

				//计算出当前的动画帧
				CalFrmUV(mtlDesc,par->lifeTime,startU,startV);

				Vector3 xaxis=viewRight;
				Vector3 yaxis=viewUp;
				if(par->rot.x!=0.0f)
				{
					trans.LoadIdentity();
					trans.RotateAxis(viewForward,par->rot.x);
					trans.TransVec3(xaxis);
					trans.TransVec3(yaxis);
				}
				xaxis*=par->size*m_parSizeScale;
				yaxis*=par->size*m_parSizeScale;

				tmpC=par->color;

				quadVert[0].pos=par->pos+ xaxis + yaxis;
				quadVert[1].pos=par->pos+xaxis - yaxis;
				quadVert[2].pos=par->pos- xaxis - yaxis;
				quadVert[3].pos=par->pos- xaxis + yaxis;
				
				FillQuadVertUV(startU,startV,scaleU,scaleV,quadVert,tmpC);
				//{0,3,2,2,1,0}
				pVert[i++]=quadVert[0];
				pVert[i++]=quadVert[3];
				pVert[i++]=quadVert[2];

				pVert[i++]=quadVert[2];
				pVert[i++]=quadVert[1];
				pVert[i++]=quadVert[0];
			}

			m_pMesh->SetDrawLimit(i/3);
		}

		void CalFrmUV(const tagMtlEffectDesc& mtlDesc,float frmTime,float& startU,float& startV)
		{
			if(mtlDesc.frmU>1
				|| mtlDesc.frmV>1)
			{
				int frame=int(frmTime*mtlDesc.fps);
				int numFrm=mtlDesc.frmU*mtlDesc.frmV;
				if(frame>=numFrm)
				{
					if(mtlDesc.bPlayOnce)
						frame=numFrm-1;//停止在最后一帧
					else
						frame=0;
				}
				startV=float(frame/mtlDesc.frmU);
				startU=float(frame%mtlDesc.frmU);
				startU*=1.0f/mtlDesc.frmU;
				startV*=1.0f/mtlDesc.frmV;
			}
		}
		
		void CopyParListAsSpark(const Vector3& cameraPos,const tagMtlEffectDesc& mtlDesc)
		{
			VertStream *pVStream=m_pMesh->GetVertStream();
			ASSERT(pVStream);
			if(pVStream == NULL)
				return;

			
			ASSERT(pVStream->GetNumVert()>=m_activeParList.size()*2*3);

			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();
			
			Color4ub tmpC;

			Vert_PTD quadVert[4];
			int i=0;
			float startU=0,startV=0;
			float scaleU=1.0f/mtlDesc.frmU;
			float scaleV=1.0f/mtlDesc.frmV;

			for(list<Particle*>::iterator iter=m_activeParList.begin();
				iter!=m_activeParList.end();++iter)
			{
				Particle *par=(*iter);
				Vector3 yaxis = par->pos-par->lastPos;
				Vector3 heading = par->pos - cameraPos;
				Vec3Normalize(heading);
				
				//->bwen modify 20060306: 
				// old code:
				/*
				Vector3 parallel = heading;

				float zcomponent = D3DXVec3Dot(&yaxis,&parallel);
				parallel *= zcomponent;
				yaxis -= parallel;
				zcomponent = Vec3Len(&yaxis);

				if (zcomponent>0.0f)
				{
					yaxis *= par->size/zcomponent;
				}
				else
				{
					// Facing straight onto camera; can't determine correct axese.
					yaxis=Vector3(0.0f, par->size, 0.0f);
				}
				*/
				// new code:将yaxis方向由面向摄像机改为lastPos->pos方向
				Vec3Normalize( yaxis );
				yaxis *= par->size*m_parSizeScale;
				//<-bwen modify

				Vector3 xaxis;
				D3DXVec3Cross(&xaxis,&yaxis,&heading);

				//计算出当前的动画帧
				CalFrmUV(mtlDesc,par->lifeTime,startU,startV);

				/**
					0 3
					1 2
				*/

				tmpC=par->color;

				quadVert[0].pos=par->pos+ xaxis + yaxis;
				quadVert[1].pos=par->lastPos+xaxis - yaxis;
				quadVert[2].pos=par->lastPos- xaxis - yaxis;
				quadVert[3].pos=par->pos- xaxis + yaxis;
				FillQuadVertUV(startU,startV,scaleU,scaleV,quadVert,tmpC);
				//{0,3,2,2,1,0}
				pVert[i++]=quadVert[0];
				pVert[i++]=quadVert[3];
				pVert[i++]=quadVert[2];

				pVert[i++]=quadVert[2];
				pVert[i++]=quadVert[1];
				pVert[i++]=quadVert[0];

			}

			m_pMesh->SetDrawLimit(i/3);
		}

		void CopyParListAsQuad(const tagMtlEffectDesc& mtlDesc)
		{
			VertStream *pVStream=m_pMesh->GetVertStream();
			ASSERT(pVStream);
			if(pVStream == NULL)
				return;

			ASSERT(pVStream->GetNumVert()>=m_activeParList.size()*2*3);

			Vert_PTD *pVert=(Vert_PTD *)pVStream->GetRawBuffer();

			Color4ub tmpC;

			Vert_PTD quadVert[4];
			int i=0;
			float startU=0,startV=0;
			float scaleU=1.0f/mtlDesc.frmU;
			float scaleV=1.0f/mtlDesc.frmV;

			Transform trans;
			for(list<Particle*>::iterator iter=m_activeParList.begin();
				iter!=m_activeParList.end();++iter)
			{
				Particle *par=(*iter);

				CalFrmUV(mtlDesc,par->lifeTime,startU,startV);

				trans.LoadIdentity();
				trans.Translate(par->pos);
				trans.Rotate(par->rot.y,par->rot.x,par->rot.z);

				//假定原始Quad是在XY面,计算出经过自转+平移的Quad
				quadVert[0].pos=Vector3(-par->size*m_parSizeScale,par->size*m_parSizeScale,0);
				quadVert[1].pos=Vector3(-par->size*m_parSizeScale,-par->size*m_parSizeScale,0);
				quadVert[2].pos=Vector3(par->size*m_parSizeScale,-par->size*m_parSizeScale,0);
				quadVert[3].pos=Vector3(par->size*m_parSizeScale,par->size*m_parSizeScale,0);
				trans.TransVec3(quadVert[0].pos);
				trans.TransVec3(quadVert[1].pos);
				trans.TransVec3(quadVert[2].pos);
				trans.TransVec3(quadVert[3].pos);

				//--填充颜色
				tmpC=par->color;

				FillQuadVertUV(startU,startV,scaleU,scaleV,quadVert,tmpC);
				//{0,3,2,2,1,0}
				pVert[i++]=quadVert[0];
				pVert[i++]=quadVert[3];
				pVert[i++]=quadVert[2];

				pVert[i++]=quadVert[2];
				pVert[i++]=quadVert[1];
				pVert[i++]=quadVert[0];

			}
			m_pMesh->SetDrawLimit(i/3);
		}
	};

	EffectParticleEmitter::EffectParticleEmitter(void)
	{
		m_p=new Member;
	}

	EffectParticleEmitter::~EffectParticleEmitter(void)
	{
		delete m_p;
	}

	void EffectParticleEmitter::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectParticleEmitter::Deserialize(FArchive& ar)
	{
		tagPEMProp prop;
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
		
	RenderNode* EffectParticleEmitter::GetRenderNode(int i)
	{
		return m_p->m_pMesh;
	}

	void EffectParticleEmitter::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if( 0 == deltaTime )
		{
			EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
			return;
		}

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

				D3DXVec4Lerp(&curState.shapeParam,&curState.shapeParam,&ts.shapeParam,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
				curState.emitSpeed=LinearLerp(curState.emitSpeed,ts.emitSpeed,r);
				curState.parLifeTimeMax=LinearLerp(curState.parLifeTimeMax,ts.parLifeTimeMax,r);
				curState.parLifeTimeMin=LinearLerp(curState.parLifeTimeMin,ts.parLifeTimeMin,r);
				D3DXVec3Lerp(&curState.parVelDir,&curState.parVelDir,&ts.parVelDir,r);
				curState.parSpeedMax=LinearLerp(curState.parSpeedMax,ts.parSpeedMax,r);
				curState.parSpeedMin=LinearLerp(curState.parSpeedMin,ts.parSpeedMin,r);
				
				curState.parSwingMax=LinearLerp(curState.parSwingMax,ts.parSwingMax,r);
				curState.parSwingMin=LinearLerp(curState.parSwingMin,ts.parSwingMin,r);

				D3DXVec3Lerp(&curState.parRotSpeedMax,&curState.parRotSpeedMax,&ts.parRotSpeedMax,r);
				D3DXVec3Lerp(&curState.parRotSpeedMin,&curState.parRotSpeedMin,&ts.parRotSpeedMin,r);

				curState.parSizeMax=LinearLerp(curState.parSizeMax,ts.parSizeMax,r);
				curState.parSizeMin=LinearLerp(curState.parSizeMin,ts.parSizeMin,r);
				curState.parSizeVel=LinearLerp(curState.parSizeVel,ts.parSizeVel,r);
				D3DXVec4Lerp((Vector4*)&curState.parColor,(Vector4*)&curState.parColor,(Vector4*)&ts.parColor,r);
				D3DXVec4Lerp((Vector4*)&curState.parColorVel,(Vector4*)&curState.parColorVel,(Vector4*)&ts.parColorVel,r);
			}
		}

		//更新当前发射器的状态
		m_p->m_curRot+=curState.rotVel*deltaTime;
		Vector3 emitterPos=m_p->m_prop.pathOffset;
		EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
		if(pPath)
			emitterPos+=pPath->GetCurPos();

		if(m_p->m_prop.bToWorld)
		{
			emitterPos = emitterPos * pSGNode->GetWorldMat();
		}
		

		//更新当前所有粒子
		for(list<Particle*>::iterator iter=m_p->m_activeParList.begin();
			iter!=m_p->m_activeParList.end();)
		{
			Particle *par=(*iter);
			if(!par->Update(deltaTime,m_p->m_prop.gForce,m_p->m_prop.airForce))
			{
				//粒子死亡处理
				Particle_Total_Num--;
				m_p->m_pParPool->FreeObj(par);
				iter=m_p->m_activeParList.erase(iter);
			}
			else
				++iter;
		}

		//发射新的粒子
		if(!m_bClosing)
		{
			// 使用LOD缩放发射数目
			float fEmitNumQuotiety = 1.0f - (float)pSGNode->GetCurLOD() / ( SGEffectNode::MAX_LOD + 1 );

			m_p->m_spawnCount+=curState.emitSpeed*deltaTime*fEmitNumQuotiety;
			int numEmit=(int)m_p->m_spawnCount;
			m_p->m_spawnCount-=numEmit;
			
			if(numEmit > 32)
				numEmit = 32;

			for(int i=0;i<numEmit;i++)
			{
				if(Particle_Total_Num < Particle_Max_Toal)
					m_p->EmitterOneParticle(curState,emitterPos);
				else
					break;
			}
		}

		//更新mesh
		switch(m_p->m_prop.polygonType)
		{
		case 1://point sprites
			m_p->CopyParListAsSprite(pSGNode->GetWorldMat(),pCamera,m_mtlDesc);
			break;
		case 2://spark
			{
				Vector3 pos=pCamera->GetPos();
				const Matrix4& mat=pSGNode->GetWorldMat();
				pos.x-=mat._41;
				pos.y-=mat._42;
				pos.z-=mat._43;
				m_p->CopyParListAsSpark(pos,m_mtlDesc);
			}
			break;
		default://普通面片
			m_p->CopyParListAsQuad(m_mtlDesc);
			break;
		}

		
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectParticleEmitter::ResetPlayState()
	{
		m_p->m_spawnCount = 0;
		m_p->m_curState=0;
		m_p->m_stateTime=0;
		m_p->m_curRot=Deg2Rad3f(m_p->m_prop.initRot);

		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}

	const TCHAR* EffectParticleEmitter::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectParticleEmitter::tagPEMProp EffectParticleEmitter::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectParticleEmitter::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectParticleEmitter::tagState EffectParticleEmitter::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectParticleEmitter::BeginBuild(const tagPEMProp& prop)
	{
		m_p->m_prop=prop;
		m_p->m_keyStates.clear();

		srand(Kernel::Inst()->GetAccumTimeDW());
	}

	void EffectParticleEmitter::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectParticleEmitter::EndBuild()
	{
		//--free old
		SAFE_DELETE(m_p->m_pMesh);
		m_p->FreeAllParticles();

		//--alloc all
		//--最大个数限制
		if(s_bLimitMaxParticleNum
			&& m_p->m_prop.maxParticles > Particle_Max_Per_Effect)
		{
			//LOG("粒子系统最大数量超过最大值,%d\r\n",m_p->m_prop.maxParticles);
			m_p->m_prop.maxParticles = Particle_Max_Per_Effect;
		}

		m_p->m_pParPool=new ObjectPool<Particle>(m_p->m_prop.maxParticles);

		ASSERT(m_p->m_keyStates.size()>0);
		tagState& initState=m_p->m_keyStates[0];
		for(int i=0;i<m_p->m_prop.initParNum;i++)
			m_p->EmitterOneParticle(initState,Vector3(0,0,0));

		m_p->m_pMesh=new DynamicMesh;
		
		m_p->m_pMesh->SetIdentityWorldMat(m_p->m_prop.bToWorld);
		m_p->m_pMesh->Create(m_p->m_prop.m_groupID,EDraw_TriangleList,
			EVType_PTD,
			m_p->m_prop.maxParticles*2*3//每个粒子用两个三角形,每个tri有三个顶点
			);
		m_p->m_pMesh->SetDrawLimit(0);
	}

	void EffectParticleEmitter::CreateMtl(const tagMtlEffectDesc& desc)
	{
		//因为每个需要播放自己的贴图动画帧,所以不创建材质动画控制器
		EffectBase::CreateBasicMtl(desc);
	}

	void EffectParticleEmitter::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	bool EffectParticleEmitter::IsClosed()
	{
		return m_bClosing && m_p->m_activeParList.size()<=0;
	}

	void EffectParticleEmitter::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pMesh)
			m_p->m_pMesh->SetMtlID(id);
	}

	void EffectParticleEmitter::SetPathOffset(const Vector3& pos)
	{
		m_p->m_prop.pathOffset=pos;
	}

	float EffectParticleEmitter::GetPlayTime()
	{
		if(m_p->m_prop.m_loop)
			return -1;

		float totalStateTime=0;
		float maxLifeTime=0;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			totalStateTime+=m_p->m_keyStates[i].lastTime;
			if(m_p->m_keyStates[i].parLifeTimeMax>maxLifeTime)
				maxLifeTime=m_p->m_keyStates[i].parLifeTimeMax;
		}

		return totalStateTime+maxLifeTime;
	}

	bool EffectParticleEmitter::GetBox(AABBox& out) const
	{
		switch(m_p->m_prop.shape)
		{
		case 0://球
			{
				Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
				for(int i=0;i<(int)m_p->m_keyStates.size();i++)
				{
					const tagState& state = m_p->m_keyStates[i];
					float fRadius = state.shapeParam.x + state.parSizeMax;
					AABBox box;
					box.min = Vector3( -fRadius, -fRadius, -fRadius );
					box.max = Vector3(  fRadius,  fRadius,  fRadius );
					vRot += state.rotVel * state.lastTime;
					Transform trans;
					trans.Translate( m_p->m_prop.pathOffset );
					trans.Rotate( vRot.y, vRot.x, vRot.z );
					box.Transform( *trans.GetMatrix() );
					out.Merge( box );
				}
			}
			return true;
		case 1://box
			{
				Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
				for(int i=0;i<(int)m_p->m_keyStates.size();i++)
				{
					const tagState& state = m_p->m_keyStates[i];
					Vector3 vHalfSize( state.shapeParam.x + state.parSizeMax, state.shapeParam.y + state.parSizeMax, state.shapeParam.z + state.parSizeMax );
					AABBox box;
					box.min = -vHalfSize;
					box.max =  vHalfSize;
					vRot += state.rotVel * state.lastTime;
					Transform trans;
					trans.Translate( m_p->m_prop.pathOffset );
					trans.Rotate( vRot.y, vRot.x, vRot.z );
					box.Transform( *trans.GetMatrix() );
					out.Merge( box );
				}
			}
			return true;
		case 2://cone
			{
				Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
				for(int i=0;i<(int)m_p->m_keyStates.size();i++)
				{
					const tagState& state = m_p->m_keyStates[i];
					float fR = max( state.shapeParam.x, state.shapeParam.y ) + state.parSizeMax;
					AABBox box;
					box.min = Vector3( -fR, -state.parSizeMax, -fR );
					box.max = Vector3(  fR, state.shapeParam.z + state.parSizeMax, fR );
					vRot += state.rotVel * state.lastTime;
					Transform trans;
					trans.Translate( m_p->m_prop.pathOffset );
					trans.Rotate( vRot.y, vRot.x, vRot.z );
					box.Transform( *trans.GetMatrix() );
					out.Merge( box );
				}
			}
			return true;
		default:
			return false;
		}//endof switch
	}

	int EffectParticleEmitter::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}

	void EffectParticleEmitter::OnSetWorldMat( const Matrix4& matWorld )
	{
		Vector3 scale( 1.0f, 1.0f, 1.0f );
		Quaternion  rot;
		Vector3     offset;
		D3DXMatrixDecompose( &scale, &rot, &offset, &matWorld );
		float maxScale = max( max( abs(scale.x), abs(scale.y) ), abs(scale.z) );
		if( m_p->m_prop.bToWorld )
		{
			m_p->m_parSizeScale = maxScale;
			m_p->m_emitterScale = maxScale;
		}
		else
		{
			if( maxScale > 1.0f )
				m_p->m_parSizeScale = 1.0f / maxScale;
			else
				m_p->m_parSizeScale = 1.0f;
			m_p->m_emitterScale = 1.0f;
		}
	}
}//namespace Cool3D