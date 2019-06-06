#include "StdAfx.h"
#include ".\MapObjEdit.h"
#include "GameMapEditor.h"
#include "..\NavMap\NavNullSceneGraph.h"
#include "..\..\WorldBase\GameMapEditor\CreatureDataReader.h"
namespace WorldBase
{
	MapObjEdit::MapObjEdit(void)
	{
		m_type=None;
		m_pMap=NULL;
		m_pSGNode=NULL;
		m_pSGEffectNode=NULL;
		m_pSGLightNode=NULL;
		m_pNavSGNode=NULL;
	}

	MapObjEdit::~MapObjEdit(void)
	{
		if(m_pSGNode)
		   m_pMap->GetSceneGraph()->DetachDynamicNode(m_pSGNode);
		if(m_pSGEffectNode)
		   m_pMap->GetSceneGraph()->DetachDynamicNode(m_pSGEffectNode);
		if(m_pSGLightNode)
			((NullSceneGraph*)m_pMap->GetSceneGraph())->DetachStaticNode(m_pSGLightNode);
		if(m_pNavSGNode)
			m_pMap->GetNavSG()->DetachNode(m_pNavSGNode);
		m_pMap=NULL;
		SAFE_DELETE(m_pSGNode);
		SAFE_DELETE(m_pSGEffectNode);
		SAFE_DELETE(m_pSGLightNode);
		SAFE_DELETE(m_pNavSGNode);
	}

	void MapObjEdit::LoadMdl(const TCHAR* szMdlPath)
	{
		ASSERT(m_pMap!=NULL);
		//--
		TCHAR szDir[_MAX_DIR];
		TCHAR szFName[_MAX_FNAME];
		TCHAR szExt[_MAX_EXT];

		_tsplitpath(szMdlPath,NULL,szDir,szFName,szExt);

		if(_tcsicmp(szExt,_T(".sfx"))==0)
		{
		   m_pMap->GetSceneGraph()->DetachDynamicNode(m_pSGEffectNode);
		   SAFE_DELETE(m_pSGEffectNode);
		   m_pSGEffectNode=new SGEffectNode;

		   FArchive ar;
		   ar.OpenForRead(Kernel::Inst()->DefaultFS(),szMdlPath);
		   m_pSGEffectNode->Deserialize(ar);
		   ar.Close();
		   m_pMap->GetSceneGraph()->AttachDynamicNode(m_pSGEffectNode);
		}
		else
		{
			m_pMap->GetSceneGraph()->DetachDynamicNode(m_pSGNode);
			SAFE_DELETE(m_pSGNode);
			m_pSGNode=new EntityNode;
			m_pSGNode->LoadRes(szMdlPath);
			m_pSGNode->SetMtlDiffuseFactor(m_staticObj.fMtlDiffuseFactor);
			m_pMap->GetSceneGraph()->AttachDynamicNode(m_pSGNode);

			//¼ÓÔØÅö×²SceneNode
			TCHAR szNavPath[512];
			_stprintf(szNavPath,_T("%s%s.nav"),szDir,szFName);
			if(_taccess(szNavPath,4)!=-1)
			{
				m_pMap->GetNavSG()->DetachNode(m_pNavSGNode);
				SAFE_DELETE(m_pNavSGNode);
				m_pNavSGNode=new NavEntityNode;
				m_pNavSGNode->LoadRes(szNavPath);
				m_pMap->GetNavSG()->AttachNode(m_pNavSGNode);
				m_pMap->GetNavSG()->SetCurNavEntityNode(m_pNavSGNode);
			}
		}	
	}

	void MapObjEdit::LoadSkin( const TCHAR* szSkinPath )
	{
		ASSERT(m_pMap!=NULL);

		bool bIOAsync = ResMgr::Inst()->IsIOAsync();
		ResMgr::Inst()->EnableIOAsync(false);

		SGSkinNode *pNewSkin = new SGSkinNode;
		pNewSkin->LoadRes( szSkinPath );
		m_pSGNode->AttachChild( pNewSkin );

		ResMgr::Inst()->EnableIOAsync(bIOAsync);
	}

	void MapObjEdit::LoadLightMdl()
	{
		ASSERT(m_pMap->GetSceneGraph()->IS_STATIC_CLASS(NullSceneGraph));

		NullSceneGraph *pSG=(NullSceneGraph*)m_pMap->GetSceneGraph();
		pSG->DetachStaticNode(m_pSGLightNode);
		SAFE_DELETE(m_pSGLightNode);

		m_pSGLightNode=new SGLightNode;

		tagLight light;
		light.type=Light_Point;
		light.pos.x=m_PointLgtobj.fPos[0];
		light.pos.y=m_PointLgtobj.fPos[1];
		light.pos.z=m_PointLgtobj.fPos[2];
		light.range=m_PointLgtobj.range;
		Color4f diffuse=m_PointLgtobj.diffuse;
		diffuse.R*=m_PointLgtobj.fIncModulus;
		diffuse.G*=m_PointLgtobj.fIncModulus;
		diffuse.B*=m_PointLgtobj.fIncModulus;
		light.diffuse=diffuse;
		Color4f ambient=m_PointLgtobj.ambient;
		light.ambient=ambient;
		light.specular.A = m_PointLgtobj.specular[0];
		light.specular.R = m_PointLgtobj.specular[1];
		light.specular.G = m_PointLgtobj.specular[2];
		light.specular.B = m_PointLgtobj.specular[3];
		light.attenuation.x=m_PointLgtobj.attenuation[0];
		light.attenuation.y=m_PointLgtobj.attenuation[1];
		light.attenuation.z=m_PointLgtobj.attenuation[2];
		m_pSGLightNode->SetLight(&light);
		m_pSGLightNode->SetDynamic(m_PointLgtobj.bDynamicPL);
		m_pSGLightNode->SetNoBlockRange(m_PointLgtobj.fnoBlockRange);
		pSG->AttachStaticNode(m_pSGLightNode);
	}

	bool MapObjEdit::Create(const tagStaticMapObj& data,GameMapEditor *pMap)
	{
		m_type=StaticObj;
		m_staticObj=data;
		m_pMap = pMap;
		LoadMdl(m_staticObj.szMdlPath);
		m_pMap->AddMapObjEdit(this);
		if(m_pSGEffectNode)
		   m_pSGEffectNode->Play();
		return true;
	}

	bool MapObjEdit::Create(const tagMapDoor&data, GameMapEditor* pMap)
	{
		m_type=MapDoor;
		m_MapDoor = data;
		m_pMap = pMap;
		LoadMdl(m_MapDoor.szModelPath);
		m_pMap->AddMapObjEdit(this);
		AnimationCtrl* pCtrl = m_pSGNode->GetAniCtrl();
		if( pCtrl!=NULL )
		{
			if( m_MapDoor.bInitState )
				pCtrl->OpenTrack(_T("open"), false);
			else
				pCtrl->OpenTrack(_T("clos"), false);
		}
		if(m_pSGEffectNode)
		{
			m_pSGEffectNode->Play();
		}
		return true;
	}

	bool MapObjEdit::Create(const tagMapCarrier&data, GameMapEditor* pMap)
	{
		m_type = MapCarrier;
		m_MapCarrier.carrier = data;
		m_pMap = pMap;
		LoadMdl(m_MapCarrier.carrier.szModelPath);
		m_pMap->AddMapObjEdit(this);
		if(m_pSGEffectNode)
			m_pSGEffectNode->Play();
		return true;
	}

	bool MapObjEdit::Create(const tagMapTriggerEffect&data, GameMapEditor* pMap)
	{
		m_type = MapTrrigerEffect;
		m_MapTriggerEffect = data;
		m_pMap = pMap;
		LoadMdl( m_MapTriggerEffect.szMdlPath );
		m_pMap->AddMapObjEdit(this);
		if(m_pSGEffectNode)
			m_pSGEffectNode->Play();
		return true;
	}
	
	bool MapObjEdit::Create(const tagMapDynamicBlock&data, GameMapEditor* pMap)
	{
		m_type = DynamicBlock;
		m_DynamicBlock = data;
		m_pMap = pMap;
		LoadMdl(m_DynamicBlock.szModelPath);
		m_pMap->AddMapObjEdit(this);
		if(m_pSGEffectNode)
			m_pSGEffectNode->Play();
		return true;
	}

	bool MapObjEdit::Create(const tagMapPointLight& data, GameMapEditor *pMap)
	{
		m_type = PointLight;
		m_PointLgtobj = data;
		m_pMap = pMap;
		LoadLightMdl();
		m_pMap->AddPointLight(this);
		return true;
	}

	bool MapObjEdit::Create(const tagMapArea& data, GameMapEditor *pMap)
	{
		m_type = MapRect;
		m_MapRect = data;
		m_pMap = pMap;
		m_pMap->AddMapRect(this);
		return true;
	}

	bool MapObjEdit::Create(const tagMapPathPoint&data, GameMapEditor *pMap)
	{
		m_type = PathPoint;
		m_PathPoint = data;
		m_pMap = pMap;
		m_pMap->AddPathPoint(this);
		return true;
	}

	bool MapObjEdit::Create(const tagMapNPC& data,GameMapEditor *pMap)
	{
		m_type = NPC;
		m_npc = data;
		m_npc.fYaw=0.0f;
		m_pMap = pMap;
		const tagCreatureProto* pAtt = pMap->GetAttDataBase()->FindCreatureAtt(m_npc.dwTypeID);
		if(pAtt == NULL)
			return false;
		tstring szMdlPath = CreatureDataReader::GetCreatureMdlPath(pAtt);
		LoadMdl(szMdlPath.c_str());

		if( pAtt->eAnimationType == EAT_Skeleton )
		{
			int i=1;
			tstring szSkinPath = CreatureDataReader::GetCreatureSkinPath(pAtt,i++);
			DWORD fileSize = Cool3D::Kernel::Inst()->DefaultFS()->GetSize(szSkinPath.c_str());
			while( 0 != fileSize && -1 != fileSize )
			{
				LoadSkin(szSkinPath.c_str());
				szSkinPath = CreatureDataReader::GetCreatureSkinPath(pAtt,i++);
				fileSize = Cool3D::Kernel::Inst()->DefaultFS()->GetSize(szSkinPath.c_str());
			}
		}

		m_pMap->AddMapObjEdit(this);
		return true;
	}

	bool MapObjEdit::Create(const tagMapSpawnPoint&data, GameMapEditor* pMap)
	{
		m_type = SpawnPoint;
		m_SpawnPoint = data;
		m_pMap = pMap;
		m_pMap->AddSpawnPoint(this);
		return true;
	}

	bool MapObjEdit::Create(const tagMapEventTrigger&data, GameMapEditor* pMap)
	{
		m_type = EventTrigger;
		m_EventTrigger = data;
		m_pMap = pMap;
		m_pMap->AddEventTrigger(this);
		return true;
	}


	MapObjEdit::EType MapObjEdit::GetType() const
	{
		return m_type;
	}

	void MapObjEdit::GetData(tagStaticMapObj& out) const
	{
		out = m_staticObj;
	}

	void MapObjEdit::GetData(tagMapDynamicBlock& out)const
	{
		out = m_DynamicBlock;
	}

	void MapObjEdit::GetData(tagMapEventTrigger& out)const
	{
		out = m_EventTrigger;
	}

	void MapObjEdit::GetData(tagMapNPC& out) const
	{
		out = m_npc;
	}

	void MapObjEdit::GetData(tagMapPointLight& out)const
	{
		out = m_PointLgtobj;
	}

	void MapObjEdit::GetData(tagMapArea& out)const
	{
		out = m_MapRect;
	}

	void MapObjEdit::GetData(tagMapPathPoint&out)const
	{
		out = m_PathPoint;
	}

	void MapObjEdit::GetData(tagMapSpawnPoint& out)const
	{
		out = m_SpawnPoint;	
	}

	void MapObjEdit::GetData(tagMapDoor& out)const
	{
		out = m_MapDoor;
	}

	void MapObjEdit::GetData(tagMapCarrier& out)const
	{
		out = m_MapCarrier.carrier;
	}

	void MapObjEdit::GetData(tagMapTriggerEffect& out)const
	{
		out = m_MapTriggerEffect;
	}


	DWORD MapObjEdit::GetMapID() const
	{
		switch(m_type)
		{
		case StaticObj:
			return m_staticObj.dwMapID;
		case NPC:
			return m_npc.dwObjID;
		case PointLight:
			return m_PointLgtobj.dwID;
		case MapRect:
			return m_MapRect.dwObjID;
		case PathPoint:
			return m_PathPoint.dwObjID;
		case SpawnPoint:
			return m_SpawnPoint.dwObjID;
		case DynamicBlock:
			return m_DynamicBlock.dwObjID;
		case EventTrigger:
			return m_EventTrigger.dwObjID;
		case MapDoor:
			return m_MapDoor.dwObjID;
		case MapCarrier:
			return m_MapCarrier.carrier.dwObjID;
			break;
		case MapTrrigerEffect:
			return m_MapTriggerEffect.dwObjID;
			break;
		}
		return -1;
	}
	DWORD MapObjEdit::GetTypeID() const
	{
		switch(m_type)
		{
		case NPC:
			return m_npc.dwTypeID;
			break;
		}
		return -1;
	}
	FLOAT MapObjEdit::GetRadius()const
	{
		switch(m_type)
		{
		case NPC:
			return m_npc.fRadius;
		}
		return -1;
	}
	void MapObjEdit::SetObjID(DWORD id)
	{
		switch(m_type)
		{
		case StaticObj:
			m_staticObj.dwMapID = id;
		case NPC:
			m_npc.dwObjID = id;
		case PointLight:
			m_PointLgtobj.dwID = id;
		case MapRect:
			m_MapRect.dwObjID = id;
		case PathPoint:
			m_PathPoint.dwObjID = id;
		case SpawnPoint:
			m_SpawnPoint.dwObjID = id;
		case DynamicBlock:
			m_DynamicBlock.dwObjID = id;
		case EventTrigger:
			m_EventTrigger.dwObjID = id;
		case MapDoor:
			m_MapDoor.dwObjID = id;
			break;
		}
	}	
	EntityNode* MapObjEdit::GetSGNode() const
	{
		return m_pSGNode;
	}

	SGEffectNode* MapObjEdit::GetSGEffectNode() const
	{
		return m_pSGEffectNode;
	}
	SGLightNode* MapObjEdit::GetSGLightNode() const
	{
		return m_pSGLightNode;
	}
	NavEntityNode* MapObjEdit::GetNavEntityNode()const
	{
		return m_pNavSGNode;
	}
}//namespace WorldBase