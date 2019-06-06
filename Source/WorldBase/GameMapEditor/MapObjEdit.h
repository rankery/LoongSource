#pragma once
#include "..\WorldBase.h"
#include "..\..\WorldDefine\MapAttDefine.h"
#include "..\NavMap\NavEntityNode.h"

struct tagMapCarrierEx
{
	tagMapCarrier	carrier;
	Vector3			vPos;
};

namespace WorldBase
{
	
	class GameMapEditor;
	class NavEntityNode;
	class NavNullSceneGraph;
	class WORLD_BASE_API MapObjEdit
	{
	public:
		enum EType
		{
			None = -1,
			NPC = 0,			//生物
			StaticObj,			//静态地物
			DynamicBlock,		//动态障碍物
			PointLight,			//点光源
			MapRect,			//地图区域
			PathPoint,			//寻路导航点
			SpawnPoint,			//刷怪点
			EventTrigger,		//事件触发器
			MapDoor = 13,		//门
			MapCarrier = 14,	//载具
			MapTrrigerEffect = 15,//触发特效
		};

	public:
		MapObjEdit(void);
		virtual ~MapObjEdit(void);

		bool Create(const tagStaticMapObj& data,GameMapEditor *pMap);
		bool Create(const tagMapNPC& data,GameMapEditor *pMap);
		bool Create(const tagMapPointLight& data, GameMapEditor *pMap);
		bool Create(const tagMapArea& data, GameMapEditor *pMap);				//创建地图区域
		bool Create(const tagMapPathPoint&data, GameMapEditor *pMap);			//创建寻路导航点
		bool Create(const tagMapSpawnPoint&data, GameMapEditor* pMap);			//创建刷怪点
		bool Create(const tagMapDynamicBlock&data, GameMapEditor* pMap);		//创建动态障碍物
		bool Create(const tagMapEventTrigger&data, GameMapEditor* pMap);		//创建事件触发器
		bool Create(const tagMapDoor&data, GameMapEditor* pMap);
		bool Create(const tagMapCarrier&data, GameMapEditor* pMap);
		bool Create(const tagMapTriggerEffect&data, GameMapEditor* pMap);

		EType GetType() const;
		void GetData(tagStaticMapObj& out) const;

		void GetData(tagMapNPC& out) const;
		void GetData(tagMapPointLight& out)const;
		void GetData(tagMapArea& out)const;
		void GetData(tagMapPathPoint& out)const;
		void GetData(tagMapSpawnPoint& out)const;
		void GetData(tagMapDynamicBlock& out)const;
		void GetData(tagMapEventTrigger& out)const;
		void GetData(tagMapDoor& out)const;
		void GetData(tagMapCarrier& out)const;
		void GetData(tagMapTriggerEffect& out)const;

		const tagStaticMapObj* GetStaticMapObj() const { return &m_staticObj;}
		const tagMapNPC* GetMapNPC() const { return &m_npc;}
		const tagMapPointLight* GetPointLgtObj()const{ return &m_PointLgtobj;}
		const tagMapArea* GetMapRect()const { return &m_MapRect;}
		const tagMapPathPoint* GetPathPoint()const { return &m_PathPoint;}
		const tagMapSpawnPoint* GetSpawnPoint()const { return &m_SpawnPoint;}
		const tagMapDynamicBlock* GetDynamicBlock()const { return &m_DynamicBlock; }
		const tagMapEventTrigger* GetEventTrigger()const { return &m_EventTrigger; }
		const tagMapDoor* GetMapDoor()const { return &m_MapDoor; }
		const tagMapCarrier* GetMapCarrier()const { return &m_MapCarrier.carrier; }
		const tagMapTriggerEffect* GetMapTriggerEffect()const { return &m_MapTriggerEffect; }
		
		DWORD GetMapID() const;
		DWORD GetTypeID() const;
		void SetObjID(DWORD id);
		FLOAT GetRadius()const;
		EntityNode* GetSGNode() const;
		SGEffectNode* GetSGEffectNode() const;
		SGLightNode* GetSGLightNode() const;
		NavEntityNode* GetNavEntityNode()const;
		void LoadMdl(const TCHAR* szMdlPath);
		void LoadSkin(const TCHAR* szSkinPath);
		void LoadLightMdl();


		void SetMdlScale(const Vector3& scale)
		{
			Transform trans;
			switch(m_type)
			{
			case StaticObj:
				{
					trans.Scale(scale.x,scale.y,scale.z);
					trans.Rotate(m_staticObj.fRotate[0],m_staticObj.fRotate[1],m_staticObj.fRotate[2]);
					trans.Translate(Vector3(m_staticObj.fPos[0],m_staticObj.fPos[1],m_staticObj.fPos[2]));
				}
				break;
			case DynamicBlock:
				{
					trans.Scale(scale.x,scale.y,scale.z);
					trans.Rotate(m_DynamicBlock.fYaw,0,0);
					trans.Translate(m_DynamicBlock.vPos);			
				}
				break;
			case MapDoor:
				{
					trans.Scale(scale.x,scale.y,scale.z);
					trans.Rotate(m_MapDoor.fYaw,0,0);
					trans.Translate(m_MapDoor.vPos);
				}
				break;
			case MapTrrigerEffect:
				{
					trans.Scale(scale.x,scale.y,scale.z);
					trans.Rotate(m_MapTriggerEffect.fRotate[0],m_MapTriggerEffect.fRotate[1],m_MapTriggerEffect.fRotate[2]);
					trans.Translate(Vector3(m_MapTriggerEffect.fPos[0],m_MapTriggerEffect.fPos[1],m_MapTriggerEffect.fPos[2]));
				}
				break;
			default:
				break;
			}
			if(m_pSGNode)
				m_pSGNode->SetWorldMat(trans.GetMatrix());
			if(m_pSGEffectNode)
				m_pSGEffectNode->SetWorldMat(trans.GetMatrix());
			if(m_pNavSGNode)
			{
				m_pNavSGNode->SetWorldMat(*trans.GetMatrix());
				m_pNavSGNode->SetScale(scale.x,scale.y,scale.z);
			}
		}

		Vector3 GetWorldPos()
		{
			switch(m_type)
			{
			case StaticObj:
				return Vector3(m_staticObj.fPos[0],m_staticObj.fPos[1],m_staticObj.fPos[2]);
				break;
			case NPC:
				return m_npc.vPos;
				break;
			case PointLight:
				return	Vector3(m_PointLgtobj.fPos[0], m_PointLgtobj.fPos[1], m_PointLgtobj.fPos[2]);
				break;
			case DynamicBlock:
				return m_DynamicBlock.vPos;
				break;
			case MapDoor:
				return m_MapDoor.vPos;
				break;
			case MapCarrier:
				return m_MapCarrier.vPos;
				break;
			case MapTrrigerEffect:
				return Vector3(m_MapTriggerEffect.fPos[0],m_MapTriggerEffect.fPos[1],m_MapTriggerEffect.fPos[2]);
				break;
			}
			return Vector3(0,0,0);
		}

		void SetWorldPos(const Vector3& pos)
		{
			Transform trans;
			switch(m_type)
			{
			case StaticObj:
				m_staticObj.fPos[0]=pos.x;
				m_staticObj.fPos[1]=pos.y;
				m_staticObj.fPos[2]=pos.z;
				
				trans.Scale(m_staticObj.fScale[0],m_staticObj.fScale[1],m_staticObj.fScale[2]);
				trans.Rotate(m_staticObj.fRotate[0],m_staticObj.fRotate[1],m_staticObj.fRotate[2]);
				trans.Translate(pos);
				break;
			case NPC:
				m_npc.vPos = pos;
				trans.Translate(pos);
				trans.Rotate(m_npc.fYaw*(D3DX_PI/180.0f),0,0);
				break;
			case DynamicBlock:
				m_DynamicBlock.vPos = pos;
				trans.Scale(m_DynamicBlock.fScale[0],m_DynamicBlock.fScale[1],m_DynamicBlock.fScale[2]);
				trans.Rotate(m_DynamicBlock.fYaw,0,0);
				trans.Translate(pos);
				break;
			case MapDoor:
				m_MapDoor.vPos = pos;
				trans.Scale(m_MapDoor.fScale[0],m_MapDoor.fScale[1],m_MapDoor.fScale[2]);
				trans.Rotate(m_MapDoor.fYaw,0,0);
				trans.Translate(pos);
				break;
			case MapCarrier:
				m_MapCarrier.vPos = pos;
				trans.Rotate(m_MapCarrier.carrier.fYaw,0,0);
				trans.Translate(pos);
				break;
			case MapTrrigerEffect:
				m_MapTriggerEffect.fPos[0]=pos.x;
				m_MapTriggerEffect.fPos[1]=pos.y;
				m_MapTriggerEffect.fPos[2]=pos.z;

				trans.Scale(m_MapTriggerEffect.fScale[0],m_MapTriggerEffect.fScale[1],m_MapTriggerEffect.fScale[2]);
				trans.Rotate(m_MapTriggerEffect.fRotate[0],m_MapTriggerEffect.fRotate[1],m_MapTriggerEffect.fRotate[2]);
				trans.Translate(pos);
				break;
			}
            if(m_pSGNode)
			    m_pSGNode->SetWorldMat(trans.GetMatrix());
			if(m_pSGEffectNode)
				m_pSGEffectNode->SetWorldMat(trans.GetMatrix());
			if(m_pNavSGNode)
				m_pNavSGNode->SetWorldMat(*trans.GetMatrix());
			
		}

		void SetMtlDiffuseFactor(float factor)
		{
			if( m_type==StaticObj )
			{
				if(m_pSGNode)
					m_pSGNode->SetMtlDiffuseFactor(factor);
			}
		}

		void SetYawForNPC(float fYaw)
		{
			if(m_type==NPC)
			{
				m_npc.fYaw=fYaw;
				Transform trans;
				//trans.Translate(Vector3(m_npc.fYaw, 0, 0));
				trans.Translate(m_npc.vPos);
				fYaw=fYaw*(D3DX_PI/180.0f);
				trans.Rotate(fYaw,0,0);
				if(m_pSGNode)
					m_pSGNode->SetWorldMat(trans.GetMatrix());
				if(m_pSGEffectNode)
					m_pSGEffectNode->SetWorldMat(trans.GetMatrix());
				if(m_pNavSGNode)
					m_pNavSGNode->SetWorldMat(*trans.GetMatrix());
			}
		}

		bool GetYawForNPC(float &fYaw)
		{
			if(m_type==NPC)
			{
				fYaw= m_npc.fYaw;
				return true;
			}
			return false;
		}

		void PlayAnimationTrack()
		{
			switch (m_type)
			{
			case MapDoor:
				{	
					if(m_pSGNode)
					{
						AnimationCtrl* pCtrl = m_pSGNode->GetAniCtrl();
						if( pCtrl!=NULL )
						{
							if( m_MapDoor.bInitState )
								pCtrl->OpenTrack(_T("open"), false);
							else
								pCtrl->OpenTrack(_T("clos"), false);
						}
					}
				}
				break;
			}
		}

		
		void SetYaw(float fYaw)
		{
			Transform trans(Transform::ECM_ToRight);
			switch (m_type)
			{
			case StaticObj:
				m_staticObj.fRotate[0] = fYaw;
				trans.Scale(m_staticObj.fScale[0],m_staticObj.fScale[1],m_staticObj.fScale[2]);
				trans.Rotate(m_staticObj.fRotate[0],m_staticObj.fRotate[1],m_staticObj.fRotate[2]);
				trans.Translate(Vector3(m_staticObj.fPos[0],m_staticObj.fPos[1],m_staticObj.fPos[2]));
				break;
			case DynamicBlock:
				m_DynamicBlock.fYaw = fYaw;
				trans.Rotate(m_DynamicBlock.fYaw, 0, 0);
				trans.Translate(m_DynamicBlock.vPos);
				break;
			case MapDoor:
				m_MapDoor.fYaw = fYaw;
				trans.Rotate(m_MapDoor.fYaw, 0, 0);
				trans.Translate(m_MapDoor.vPos);
				break;
			case MapCarrier:
				m_MapCarrier.carrier.fYaw = fYaw;
				trans.Rotate(m_MapCarrier.carrier.fYaw, 0, 0);
				trans.Translate(m_MapCarrier.vPos);
				break;
			case MapTrrigerEffect:
				m_MapTriggerEffect.fRotate[0] = fYaw;
				trans.Scale(m_MapTriggerEffect.fScale[0],m_MapTriggerEffect.fScale[1],m_MapTriggerEffect.fScale[2]);
				trans.Rotate(m_MapTriggerEffect.fRotate[0],m_MapTriggerEffect.fRotate[1],m_MapTriggerEffect.fRotate[2]);
				trans.Translate(Vector3(m_MapTriggerEffect.fPos[0],m_MapTriggerEffect.fPos[1],m_MapTriggerEffect.fPos[2]));
				break;
			}
			if(m_pSGNode)
				m_pSGNode->SetWorldMat(trans.GetMatrix());
			if(m_pSGEffectNode)
				m_pSGEffectNode->SetWorldMat(trans.GetMatrix());
			if(m_pNavSGNode)
				m_pNavSGNode->SetWorldMat(*trans.GetMatrix());
		}
	protected:
		//void LoadMdl(const TCHAR* szMdlPath);

	protected:
		EType				m_type;
		GameMapEditor		*m_pMap;

		//--编辑数据,只有一个是有效的,根据type
		tagStaticMapObj		m_staticObj;
		tagMapNPC			m_npc;
		tagMapPointLight	m_PointLgtobj;
		tagMapArea			m_MapRect;
		tagMapPathPoint		m_PathPoint;
		tagMapSpawnPoint	m_SpawnPoint;
		tagMapDynamicBlock	m_DynamicBlock;
		tagMapEventTrigger  m_EventTrigger;
		tagMapDoor			m_MapDoor;
		tagMapCarrierEx		m_MapCarrier;
		tagMapTriggerEffect m_MapTriggerEffect;
		
		//--用来画图
		EntityNode			*m_pSGNode;
		SGEffectNode        *m_pSGEffectNode;
		SGLightNode			*m_pSGLightNode;

		//--用来做碰撞检测
		NavEntityNode		*m_pNavSGNode;
	};
}//namespace WorldBase

