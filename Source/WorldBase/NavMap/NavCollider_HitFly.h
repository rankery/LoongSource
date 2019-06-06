#pragma once
#include "NavColliderPublicDef.h"
#include "NavCollider_Jump.h"

namespace WorldBase
{
	class NavCollider;
	class NavCarrierNode;
	/** \class NavCollider_HitFly
		\brief »÷·ÉÅö×²¼ì²âÆ÷
	*/
	class WORLD_BASE_API NavCollider_HitFly
	{
		enum EState						//×´Ì¬
		{
			EState_None,				//Î´½øÐÐÈÎºÎ²Ù×÷
			EState_Jump,				//ÌøÔ¾
			EState_VDrop,				//´¹Ö±µôÂä
		};

		struct tagInput
		{
			Vector3 roleSize;
			float	g;
			float	cdTimeOut;
			bool	bTrnOnly;
			bool	bIgnoreCarrier;
		};

	public:
		NavCollider_HitFly(void);
		~NavCollider_HitFly(void);

		/** ³õÊ¼»¯
		*/
		void Init(const Vector3& startPos,const Vector3& xzDir,const Vector3& roleSize,float xzSpeed,float ySpeed,float g=1200.0f,float cdTimeOut=60.0f,bool bTrnOnly=false,bool bIgnoreCarrier=false);
		/** »÷·É
		*/
		ENavResult Update(NavCollider* pCollider,float startTime,float deltaTime,BitMap* pSpecCangoMap,Vector3& outPos,float& endTime,DWORD& dwCarrierObjID,bool bOnWaterCheck=false);

	private:
		tagInput			m_input;
		EState				m_state;
		NavCollider_Jump	m_jumpCollider;
	};
}//namespace WorldBase