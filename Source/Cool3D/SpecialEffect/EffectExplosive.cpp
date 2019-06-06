#include "StdAfx.h"
#include ".\effectexplosive.h"
#include "EftQuadDef.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "SGeffectNode.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectExplosive,EffectBase);

	class EffectExplosive::Member
	{
	public:
		tagExpProp	m_prop;

		int			m_numQuad;
		DynamicMesh	*m_pRMesh;
		tagEftQuad	*m_quadBuf;
		Vector3		*m_quadVel;//每个Quad的速度,与上面的数组一一对应
		Vector3		*m_quadRotVel;//每个Quad的自转速度
		float		*m_quadFadeSpeed;//每个Quad的alpha降低的速度

		bool		m_bActive;//现在是否已经激活
		bool		m_bStart;//是否已经启动了爆炸
		
		Member()
		{
			m_bStart=false;
			m_numQuad=0;

			m_pRMesh=NULL;
			m_quadBuf=NULL;
			m_quadVel=NULL;
			m_quadRotVel=NULL;
			m_quadFadeSpeed=NULL;
			m_bActive=true;
		}
		~Member()
		{
			Destroy();
		}

		void Destroy()
		{
			SAFE_DELETE(m_pRMesh);
			SAFE_DELETEA(m_quadBuf);
			SAFE_DELETEA(m_quadVel);
			SAFE_DELETEA(m_quadRotVel);
			SAFE_DELETEA(m_quadFadeSpeed);
		}
		void AllocData(int numQuad)
		{
			ASSERT(m_quadBuf==NULL);
			m_quadBuf=new tagEftQuad[numQuad];
			m_quadVel=new Vector3[numQuad];
			m_quadRotVel=new Vector3[numQuad];
			m_quadFadeSpeed=new float[numQuad];
			
			m_pRMesh=new DynamicMesh;
		}

		void StartExplosive(const Vector3& pos,const Vector3& vel)
		{
			int x,y;

			ASSERT(m_quadBuf!=NULL);

			float uStep=m_prop.uvScale.x/(m_prop.xDiv);
			float vStep=m_prop.uvScale.y/(m_prop.yDiv);
			if(m_prop.bodyType==1)//sphere
			{
				float radius=m_prop.bodyParam[0];

				Vector2 quadSize(radius/m_prop.xDiv,radius/m_prop.xDiv);

				float phi,theta;
				float dPhi=(FLOAT_PI*2)/m_prop.yDiv;
				float dTheta=(FLOAT_PI*2)/m_prop.xDiv;

				int quadIndex=0;
				for(y=0;y<m_prop.yDiv;y++)
				{
					phi=dPhi*y;
					for(x=0;x<m_prop.xDiv;x++)
					{
						theta=dTheta*x;

						tagEftQuad& quad=m_quadBuf[quadIndex];
						quad.color=0xFFFFFFFF;
						quad.halfSize=quadSize;
						quad.pos.x= radius * sin(phi) * cos(theta);
						quad.pos.y= radius * sin(phi) * sin(theta);
						quad.pos.z= radius * cos(phi);

						quad.pos+=pos;

						quad.sUV.x=x*uStep;
						quad.sUV.y=y*vStep;

						quad.eUV.x=quad.sUV.x+uStep;
						quad.eUV.y=quad.sUV.y-vStep;

						float power=GetRandom(m_prop.minPower,m_prop.maxPower);
						m_quadVel[quadIndex]=RandUnitVec3()*power+m_prop.dir+vel;
						m_quadRotVel[quadIndex]=RandUnitVec3()*FLOAT_PI*2;
						quad.rot=RandUnitVec3()*FLOAT_PI;
						m_quadFadeSpeed[quadIndex]=1.0f/GetRandom(m_prop.minLifeTime,m_prop.maxLifeTime);
						//--
						quadIndex++;
					}
				}
				ASSERT(quadIndex==m_numQuad);
			}
			else
			{
				Vector2 step;
				Vector2 quadSize;
				quadSize.x=m_prop.bodyParam[0]/m_prop.xDiv;
				quadSize.y=m_prop.bodyParam[1]/m_prop.yDiv;
				step=quadSize;
				quadSize*=0.5f;//转换成half size

				Transform trans;
				trans.Translate(pos);
				trans.Rotate(_DegToRad(m_prop.rot.x),
					_DegToRad(m_prop.rot.y),_DegToRad(m_prop.rot.z));
				Vector3 startPos(m_prop.bodyParam[0]*-0.5f,m_prop.bodyParam[1]*-0.5f,0);
				Vector3 centerPos(0,0,0);
				float maxDist=Vec3Dist(startPos,centerPos);
				int quadIndex=0;
				for(y=0;y<m_prop.yDiv;y++)
				{
					for(x=0;x<m_prop.xDiv;x++)
					{
						tagEftQuad& quad=m_quadBuf[quadIndex];
						quad.color=0xFFFFFFFF;
						quad.halfSize=quadSize;
						quad.pos.x=step.x*x+startPos.x;
						quad.pos.y=step.y*y+startPos.y;
						quad.pos.z=0;
						trans.TransVec3(quad.pos);

						quad.sUV.x=x*uStep;
						quad.sUV.y=y*vStep;

						quad.eUV.x=quad.sUV.x+uStep;
						quad.eUV.y=quad.sUV.y-vStep;

						float powerDec=Vec3Dist(quad.pos,centerPos)/maxDist;
						powerDec=sinf(powerDec)*m_prop.powerDec;
						float power=GetRandom(0.0f,m_prop.maxPower)*powerDec+m_prop.minPower;
						

						m_quadVel[quadIndex]=RandUnitVec3()*power+m_prop.dir+vel;
						m_quadRotVel[quadIndex]=RandUnitVec3()*FLOAT_PI*2;
						//quad.rot=RandUnitVec3()*FLOAT_PI;
						quad.rot=Vector3(0,0,0);
						m_quadFadeSpeed[quadIndex]=1.0f/GetRandom(m_prop.minLifeTime,m_prop.maxLifeTime);
						//--
						quadIndex++;
					}
				}
				ASSERT(quadIndex==m_numQuad);
			}//endof if(plane)
			m_pRMesh->SetDrawLimit(m_numQuad*2);
		}

		void UpdateExplosive(float deltaTime)
		{
			Vector3 airAcc;
			Vector3 gAcc;
			Color4ub cc;
			Color4f cf;
			for(int i=0;i<m_numQuad;i++)
			{
				tagEftQuad& quad=m_quadBuf[i];
				quad.pos+=m_quadVel[i]*deltaTime;
				quad.rot+=m_quadRotVel[i]*deltaTime;

				cf=quad.color;
				cf.A-=m_quadFadeSpeed[i]*deltaTime;
				if(cf.A<0.0f)
					cf.A=0.0f;
				cc=cf;
				quad.color=cc;

				airAcc=m_quadVel[i]*m_prop.airForce;
				gAcc=MathConst::AxisY*m_prop.gForce;
				m_quadVel[i]-=airAcc*deltaTime;
				m_quadVel[i]+=gAcc*deltaTime;
			}
		}
	};

	EffectExplosive::EffectExplosive(void)
	{
		m_p=new Member;
	}

	EffectExplosive::~EffectExplosive(void)
	{
		delete m_p;
	}

	void EffectExplosive::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		
		EffectBase::Serialize(ar);
	}

	void EffectExplosive::Deserialize(FArchive& ar)
	{
		tagExpProp prop;
		ar.ValueRead(prop);
		
		//build
		Build(prop);
		
		EffectBase::Deserialize(ar);
	}

	RenderNode* EffectExplosive::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectExplosive::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
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

		if(!m_p->m_bStart)
		{
			Vector3 pos=m_p->m_prop.offset;
			Vector3 vel(0,0,0);
			EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
			if(pPath)
			{
				pos+=pPath->GetCurPos();
				vel=pPath->GetCurVel();
			}
			m_p->StartExplosive(pos,vel);
			m_p->m_bStart=true;
		}
		else
			m_p->UpdateExplosive(deltaTime);

		//--copy to mesh
		CopyQuadArrayToMesh(m_p->m_pRMesh,m_p->m_quadBuf,m_p->m_numQuad,
			pCamera,pSGNode->GetWorldMat(),m_p->m_prop.faceType);

		if( runTime > m_p->m_prop.maxLifeTime + m_p->m_prop.m_startTime )
		{
			m_bClosing = true;
		}
		if(m_bClosing)
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
		}
		//--update mtl
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);
	}

	void EffectExplosive::Build(const tagExpProp& prop)
	{
		m_p->Destroy();

		m_p->m_prop=prop;

		m_p->m_numQuad=prop.xDiv*prop.yDiv;
		m_p->AllocData(m_p->m_numQuad);
		m_p->m_pRMesh->Create(m_p->m_prop.m_groupID,
			EDraw_TriangleList,EVType_PTD,
			m_p->m_numQuad*6);//每个quad,需要两个三角形
		VertStream *pVStream=m_p->m_pRMesh->GetVertStream();
		memset(pVStream->GetRawBuffer(),0,pVStream->GetBufferSize());
		m_p->m_pRMesh->SetDrawLimit(0);
	}

	EffectExplosive::tagExpProp EffectExplosive::GetProperty() const
	{
		return m_p->m_prop;
	}

	void EffectExplosive::ResetPlayState()
	{
		m_p->m_bStart=false;

		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);
		
		EffectBase::ResetPlayState();
	}
	const TCHAR* EffectExplosive::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	void EffectExplosive::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}
	void EffectExplosive::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	bool EffectExplosive::GetBox(AABBox& out) const
	{
		float fMaxHalfSize=0.0f;
		if(m_p->m_prop.bodyType==1)//sphere
			fMaxHalfSize=m_p->m_prop.bodyParam[0];
		else
			fMaxHalfSize=max(m_p->m_prop.bodyParam[0],m_p->m_prop.bodyParam[1])*0.5f;

		out.min=Vector3(-fMaxHalfSize,-fMaxHalfSize,-fMaxHalfSize)+m_p->m_prop.offset;
		out.max=Vector3( fMaxHalfSize, fMaxHalfSize, fMaxHalfSize)+m_p->m_prop.offset;
		return true;
	}

	int EffectExplosive::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D