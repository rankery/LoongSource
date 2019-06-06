#include "StdAfx.h"
#include "NavCollider_HitFly.h"
#include "NavColliderPrivateDef.h"
#include "NavCollider.h"
#include "NavCarrierNode.h"


namespace WorldBase
{
	NavCollider_HitFly::NavCollider_HitFly(void):m_state(EState_None)
	{
	}

	NavCollider_HitFly::~NavCollider_HitFly(void)
	{
	}

	void NavCollider_HitFly::Init( const Vector3& startPos,const Vector3& xzDir,const Vector3& roleSize,float xzSpeed,float ySpeed,float g,float cdTimeOut,bool bTrnOnly,bool bIgnoreCarrier )
	{
		//--
		m_input.roleSize=roleSize;
		m_input.g=g;
		m_input.cdTimeOut=cdTimeOut;
		m_input.bTrnOnly=bTrnOnly;
		m_input.bIgnoreCarrier=bIgnoreCarrier;

		//--
		m_jumpCollider.Init(startPos,xzDir,roleSize,xzSpeed,ySpeed,g,cdTimeOut,bTrnOnly,bIgnoreCarrier);
		m_state=EState_Jump;
	}


	ENavResult NavCollider_HitFly::Update( NavCollider* pCollider,float startTime,float deltaTime,BitMap* pSpecCangoMap,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,bool bOnWaterCheck/*=false*/ )
	{
		switch(m_state)
		{
		case EState_Jump:
			{
				float cdCarrierTime=0.0f;
				ENavResult result=m_jumpCollider.Update(pCollider,startTime,deltaTime,pSpecCangoMap,outPos,endTime,dwCarrierObjID,cdCarrierTime,bOnWaterCheck);
				if(result==ENR_WillVDrop)
				{
					//--
					m_state=EState_VDrop;

					//--
					Vector3 xzDir(0.0f,0.0f,0.0f);
					float xzSpeed=0.0f;
					float ySpeed=0.0f;
					if(dwCarrierObjID!=-1)
					{
						NavCarrierNode* pCarrier=pCollider->FindCarrier(dwCarrierObjID);
						ASSERT(pCarrier!=NULL&&pCarrier->IsRunning());

						pCarrier->AdjustJumpDirAndSpeed(cdCarrierTime,xzDir,xzSpeed,ySpeed);
					}

					//--
					m_jumpCollider.Init(outPos,xzDir,m_input.roleSize,xzSpeed,ySpeed,m_input.g,m_input.cdTimeOut,m_input.bTrnOnly,m_input.bIgnoreCarrier);

					//--
					deltaTime-=(endTime-startTime);
					if(deltaTime<0)
						deltaTime=0.0f;
					return m_jumpCollider.Update(pCollider,endTime,deltaTime,pSpecCangoMap,outPos,endTime,dwCarrierObjID,cdCarrierTime,bOnWaterCheck);
				}
				else
				{
					return result;
				}
			}
			break;
		case EState_VDrop:
			{
				float cdCarrierTime=0.0f;
				return m_jumpCollider.Update(pCollider,startTime,deltaTime,pSpecCangoMap,outPos,endTime,dwCarrierObjID,cdCarrierTime,bOnWaterCheck);
			}
			break;
		}

		ASSERT(0);
		return ENR_Failed;
	}

}//namespace WorldBase