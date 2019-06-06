#include "StdAfx.h"
#include "EffectMgr.h"
#include "MapMgr.h"
#include "RoleMgr.h"
#include "Role.h"
#include "GameFrameMgr.h"
#include "NPC.h"
#include "CreatureData.h"
#include "SFXData.h"
#include "GameSet.h"

const Vector3 g_vStdEffectRefSize( 50.0f, 175.0f, 50.0f);    // 特效的标准参考大小，用来缩放特效
const float g_fDefaultRoleEffYOffset = 2.0f / 3.0f;          // 默认角色特效播放的Y方向偏移角色高度的2/3

EffectMgr::EffectMgr(void) :
m_Trunk( this ),
m_dwCurEffectID( EFFECT_START_ID )
{
}

EffectMgr::~EffectMgr(void)
{
}

EffectMgr g_effectMgr;
EffectMgr* EffectMgr::Inst()
{
	return &g_effectMgr;
}

VOID EffectMgr::Init()
{
	m_pGameFrameMgr = TObjRef<GameFrameMgr>();
	m_pGameFrameMgr->RegisterEventHandle( _T("CloseMap"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&EffectMgr::OnCloseMap));

	srand( timeGetTime() );
}

VOID EffectMgr::Destroy()
{
	m_pGameFrameMgr->UnRegisterEventHandler( _T("CloseMap"), (FRAMEEVENTPROC)m_Trunk.sfp1(&EffectMgr::OnCloseMap));
	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end(); itr++ )
	{
		DeleteEffect( itr->second );
	}
	m_mapEffect.clear();
}

VOID EffectMgr::Update()
{
	SceneGraph* pSG = MapMgr::Inst()->GetSceneGraph();
	if( NULL == pSG )
		return;

	const float fTimeDelta = Kernel::Inst()->GetDeltaTime();

	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end();  )
	{
		tagBaseEffectData* pData = itr->second;
		if( GT_INVALID != pData->dwBindRoleID )
		{
			BOOL bBindRoleExist = UpdataRoleEffect( pData );
			// 绑定的角色已经不存在了
			if( !bBindRoleExist )
			{
				pData->pSGNode = NULL;
				DeleteEffect( pData );
				itr = m_mapEffect.erase( itr );
				continue;
			}
		}

		if( EET_Launch == pData->eType )
		{
			UpdataTargetPath( (tagLaunchEffectData*)pData, fTimeDelta );
		}

		// FOR DEBUG?? 特效在水面下看不到，如果在水下，把它放到水面上
		if( pData->bPullOnWater )
		{
			PullOnWater( pData );
		}

		// 更新特效
		pData->pSGNode->UserUpdate( fTimeDelta, pSG );

		// 删除已经关闭的特效
		if( pData->pSGNode->IsClosed() )
		{
			DeleteEffect( pData );
			itr = m_mapEffect.erase( itr );
			continue;
		}
		itr++;
	}
}

DWORD EffectMgr::PlayEffect(const TCHAR* szPath, const Vector3& vPos, const Vector3& vScale, const Vector3& vRot )
{
	SGAttachableEffectNode* pNode = LoadSFXFile( szPath, NULL );
	if( NULL == pNode )
		return GT_INVALID;

	Transform trans;
	trans.Scale( vScale.x, vScale.y, vScale.z );
	trans.Rotate( vRot.y, vRot.x, vRot.z );
	trans.Translate( vPos );
	pNode->SetWorldMat( trans.GetMatrix() );
	pNode->Play();
	tagBaseEffectData* pData = new tagBaseEffectData(EET_Common);
	pData->pSGNode = pNode;
	pData->dwBindRoleID = GT_INVALID;
	const DWORD dwID = AddEffect( pData, NULL, NULL );
	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

DWORD EffectMgr::PlayEffect(const Vector3& vPos,float yaw,const TCHAR* szEffectName, const Vector3* pSize/*=NULL*/, EEffectScaleType eScaleType/* = EEST_Null*/, const TCHAR* szFolderName/* = _T("combat")*/)
{
	SGAttachableEffectNode* pNode = LoadSFXFile( szEffectName, szFolderName );
	if( NULL == pNode )
		return GT_INVALID;

	Transform trans;
	if( NULL != pSize )
	{
		float fScale = GetEffectScale( eScaleType, *pSize );
		if( fScale > 3.0f )
			fScale = 3.0f; 
		trans.Scale( fScale, fScale, fScale );
	}
	trans.Translate( vPos );
	trans.RotateAxis('Y',yaw);
	pNode->SetWorldMat( trans.GetMatrix() );
	pNode->Play();
	tagBaseEffectData* pData = new tagBaseEffectData(EET_Common);
	pData->bPullOnWater = true;
	pData->pSGNode = pNode;
	pData->dwBindRoleID = GT_INVALID;
	const DWORD dwID = AddEffect( pData, NULL, NULL );
	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

DWORD EffectMgr::PlayRoleEffect(DWORD dwRoleID,const TCHAR* szEffectName,const TCHAR* szTagNodeName/*=NULL*/, EEffectScaleType eScaleType/* = EST_Y*/, const TCHAR* szFolderName/* = _T("combat")*/, const bool bPullOnWater/* = false*/ )
{
	Role* pRole = RoleMgr::Inst()->FindRole( dwRoleID );
	if( !P_VALID(pRole) || !pRole->IsAttributeInited() )
		return GT_INVALID;

	SGAttachableEffectNode* pNode = LoadSFXFile( szEffectName, szFolderName );
	if( NULL == pNode )
		return GT_INVALID;

	pNode->Play();
	
	tagRoleEffectData* pData = new tagRoleEffectData;
	pData->bPullOnWater = bPullOnWater;
	pData->pSGNode = pNode;
	pData->eScaleType = eScaleType;

	const bool bCanMove = SFXCanMove( szEffectName );
	DWORD dwID = GT_INVALID;

	if( !bCanMove )
	{
		// 不可移动特效设置矩阵
		if( NULL != szTagNodeName && szTagNodeName[0] != _T('\0') )
		{
			Matrix4 mat = MathConst::IdentityMat;
			pRole->GetBoneSpace( szTagNodeName, mat );
			pData->pSGNode->SetWorldMat( &mat );
		}
		else
		{
			UpdataEffectMatrix( pData, pRole );
		}
		pData->dwBindRoleID = GT_INVALID;
		dwID = AddEffect( pData, NULL, NULL );
	}
	else
	{
		pData->dwBindRoleID = dwRoleID;
		dwID = AddEffect( pData, pRole, szTagNodeName );
	}
	
	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

DWORD EffectMgr::PlayLaunchEffect(const tagHitTargetEvent& evt,const TCHAR* szEffectName,const TCHAR* szTagNodeName/*=NULL*/)
{
	Role* pSrcRole = RoleMgr::Inst()->FindRole( evt.dwSrcRoleID );
	if( NULL == pSrcRole || NULL == pSrcRole->GetSceneNode() )
		return GT_INVALID;

	Role* pTarRole = RoleMgr::Inst()->FindRole( evt.dwTarRoleID );
	if( NULL == pTarRole || !pTarRole->IsAttributeInited() )
		return GT_INVALID;


	SGAttachableEffectNode* pNode = LoadSFXFile( szEffectName );
	if( NULL == pNode )
		return GT_INVALID;
	
	Vector3 vDest = GetBeAttackPos( pTarRole );

	// 起始点可以在绑定的骨骼处（如果没有绑定骨骼则使用默认位置）
	Vector3 vSrc = pSrcRole->GetPos();
	Matrix4 matBone = MathConst::IdentityMat;
	if( pSrcRole->GetBoneSpace( szTagNodeName, matBone ) )
	{
		vSrc.x = matBone._41;
		vSrc.y = matBone._42;
		vSrc.z = matBone._43;
	}
	else
	{
		AABBox boxSrc;
		pSrcRole->GetSceneNode()->GetBox( boxSrc );
		vSrc.y += ( boxSrc.max.y - boxSrc.min.y ) * g_fDefaultRoleEffYOffset;
	}
	pNode->ResetTargetPath( vSrc, vDest );

	pNode->Play();
	pNode->SetBoxCenter( pSrcRole->GetPos() );

	tagLaunchEffectData* pData = new tagLaunchEffectData;
	pData->pSGNode = pNode;
	pData->evtHitTarget = evt;

	const DWORD dwID = AddEffect( pData, NULL, NULL );
	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

DWORD EffectMgr::PlayBeAttackEffect(DWORD dwRoleID,const TCHAR* szEffectBaseName,const int nNumEffect, EEffectScaleType eScaleType /*= EEST_YOutOnly*/ )
{
	if( IsEffectNumFull() )
		return GT_INVALID;

	if( !P_VALID(szEffectBaseName) || _T('\0') == szEffectBaseName[0] || nNumEffect <= 0 )
		return GT_INVALID;

	Role* pRole = RoleMgr::Inst()->FindRole( dwRoleID );
	if( NULL == pRole || ! pRole->IsAttributeInited() )
		return GT_INVALID;

	TCHAR szEffectName[X_SHORT_NAME];
	int nRand = 0;
	if( nNumEffect > 1 )
		nRand = rand() % nNumEffect;
	_sntprintf( szEffectName, X_SHORT_NAME, _T("%s_%02d"), szEffectBaseName, nRand + 1 );

	SGAttachableEffectNode* pNode = LoadSFXFile( szEffectName );
	if( NULL == pNode )
		return GT_INVALID;

	pNode->Play();

	tagBeAttackEffectData* pData = new tagBeAttackEffectData;
	pData->pSGNode = pNode;
	pData->dwBindRoleID = GT_INVALID;
	pData->eScaleType = eScaleType;

	Vector3 vEffPos = GetBeAttackPos( pRole );
	Vector3 vScale = GetRoleEffectScale( pData->eScaleType, pRole );
	Transform trans;
	trans.Scale( vScale.x, vScale.y, vScale.z );
	trans.Rotate( pRole->GetDestYaw(), 0, 0 );
	trans.Translate( vEffPos );
	pData->pSGNode->SetWorldMat( trans.GetMatrix() );

	const DWORD dwID = AddEffect( pData, NULL, NULL );
	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

DWORD EffectMgr::PlayBuffEffect( DWORD dwRoleID,DWORD dwBuffID,const TCHAR* szEffectName, EEffectScaleType eScaleType /*= EEST_Y*/  )
{
	Role* pRole = RoleMgr::Inst()->FindRole( dwRoleID );
	if( !P_VALID(pRole) || !pRole->IsAttributeInited() )
		return GT_INVALID;

	// 如果有相同的Buff特效在播放则不再增加
	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end(); itr++ )
	{
		if( EET_Buff != itr->second->eType )
			continue;
		tagBuffEffectData* pData = (tagBuffEffectData*)itr->second;
		if( dwRoleID == pData->dwBindRoleID && dwBuffID == pData->dwBuffID && NULL != pData->pSGNode )
		{
			pData->pSGNode->Play();
			return itr->first;
		}
	}

	SGAttachableEffectNode* pNode = LoadSFXFile( szEffectName );
	if( NULL == pNode )
		return GT_INVALID;

	pNode->Play();

	tagBuffEffectData* pData = new tagBuffEffectData;
	pData->pSGNode = pNode;
	
	pData->dwBuffID = dwBuffID;
	pData->eScaleType = eScaleType;

	const bool bCanMove = SFXCanMove( szEffectName );
	DWORD dwID = GT_INVALID;

	if( !bCanMove )
	{
		// 不可移动特效设置矩阵
		UpdataEffectMatrix( pData, pRole );
		pData->dwBindRoleID = GT_INVALID;
		dwID = AddEffect( pData, NULL, NULL );
	}
	else
	{
		pData->dwBindRoleID = dwRoleID;
		dwID = AddEffect( pData, pRole, NULL );
	}

	if( GT_INVALID == dwID )
	{
		SAFE_DEL( pData->pSGNode );
		SAFE_DEL( pData );
	}
	return dwID;
}

VOID EffectMgr::StopBuffEffect( DWORD dwRoleID,DWORD dwBuffID )
{
	Role* pRole = RoleMgr::Inst()->FindRole( dwRoleID );
	if( NULL == pRole )
		return;

	// 所有相同的BUFF特效都停止
	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end(); itr++ )
	{
		if( EET_Buff != itr->second->eType )
			continue;
		tagBuffEffectData* pData = (tagBuffEffectData*)itr->second;
		if( dwRoleID == pData->dwBindRoleID && dwBuffID == pData->dwBuffID && NULL != pData->pSGNode )
		{
			if( GT_INVALID != pData->dwBindRoleID )
			{
				// 已经被删掉了
				Role* pRole = RoleMgr::Inst()->FindRole( pData->dwBindRoleID );
				if( !P_VALID( pRole ) || !P_VALID( pRole->GetSceneNode() ) )
					break;
			}
			pData->pSGNode->Close();
		}
	}
}

VOID EffectMgr::StopEffect( const DWORD dwEffectID )
{
	EffecMap::iterator itr = m_mapEffect.find( dwEffectID );
	if( itr != m_mapEffect.end() )
	{
		tagBaseEffectData* pData = itr->second;

		// 已经被删掉了
		if( GT_INVALID != pData->dwBindRoleID )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pData->dwBindRoleID );
			if( !P_VALID( pRole ) || !P_VALID( pRole->GetSceneNode() ) )
				return;
		}
		pData->pSGNode->Close();
	}
}

SGAttachableEffectNode* EffectMgr::LoadSFXFile( const TCHAR* szEffectName, const TCHAR* szFolderName/* = _T("combat")*/ )
{
	if( !P_VALID(szEffectName) || _T('\0') == szEffectName[0] )
		return NULL;

	// 如果传了文件夹名称就构造路径，否则直接使用szEffectName作为路径
	tstring strEffectPath;
	if( NULL != szFolderName )
	{
		strEffectPath = _T("data\\system\\effect\\");
		strEffectPath += szFolderName;
		strEffectPath += _T("\\");
		strEffectPath += szEffectName;
		strEffectPath += _T(".sfx");
	}
	else
	{
		strEffectPath = szEffectName;
	}

	// FOR DEBUG??
	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	DWORD dwFileSize = pIFS->GetSize( strEffectPath.c_str() );
	if( 0 == dwFileSize || GT_INVALID == dwFileSize )
	{
		TRACE( _T("SFX file have not found.file=%s\r\n"), strEffectPath.c_str() );
		return NULL;
	}

	SGAttachableEffectNode *pEffect=new SGAttachableEffectNode;
	pEffect->SetUserWorldMat( true );
	pEffect->LoadRes( strEffectPath.c_str() );
	pEffect->SetUserUpdate( true );
	pEffect->SetUserPlay( true );
	return pEffect;
}

DWORD EffectMgr::AddEffect( tagBaseEffectData* pData, Role* pBindRole, const TCHAR* szTagNodeName )
{
	// 没有绑定角色
	if( !P_VALID( pBindRole ) )
	{
		if( NULL == MapMgr::Inst()->GetSceneGraph() )
			return GT_INVALID;

		pData->bBindTagNode = false;
		MapMgr::Inst()->GetSceneGraph()->AttachDynamicNode( pData->pSGNode );
	}
	else
	{
		SceneNode* pRoleSGNode = pBindRole->GetSceneNode();
		ASSERT( NULL != pRoleSGNode );

		// 绑定骨骼
		if( NULL != szTagNodeName && _T('\0') != szTagNodeName[0] )
		{
			pData->bBindTagNode = true;
			pData->pSGNode->BindToTagNode( szTagNodeName );
		}
		pRoleSGNode->AttachChild( pData->pSGNode );
	}
	m_mapEffect[m_dwCurEffectID] = pData;
	return m_dwCurEffectID++;
}

VOID EffectMgr::DeleteEffect( tagBaseEffectData* pData )
{
	if( P_VALID( pData->pSGNode ) )
	{
		// 已经被删掉了
		if( GT_INVALID != pData->dwBindRoleID )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( pData->dwBindRoleID );
			if( !P_VALID( pRole ) || !P_VALID( pRole->GetSceneNode() ) )
			{
				SAFE_DEL( pData );
				return;
			}
		}

		SceneNode* pFather = (SceneNode*)pData->pSGNode->GetParent();
		if( NULL != pFather  )
			pFather->DetachChild( pData->pSGNode );
		else
			MapMgr::Inst()->GetSceneGraph()->DetachDynamicNode( pData->pSGNode );
		SAFE_DEL( pData->pSGNode );
	}
	SAFE_DEL( pData );
}

VOID EffectMgr::OnCloseMap( tagGameEvent* )
{
	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end();  )
	{
		tagBaseEffectData* pData = itr->second;
		if( GT_INVALID == pData->dwBindRoleID )
		{
			DeleteEffect( pData );
			itr = m_mapEffect.erase( itr );
		}
		else
		{
			itr++;
		}
	}
}

VOID EffectMgr::ShowEffect( const DWORD dwEffectID, const bool bShow )
{
	EffecMap::iterator itr = m_mapEffect.find( dwEffectID );
	if( itr != m_mapEffect.end() )
	{
		tagBaseEffectData* pData = itr->second;
		pData->pSGNode->Show( bShow );
	}
}

VOID EffectMgr::ShowBindRoleEffect( const DWORD dwBindRoleID, const bool bShow )
{
	if( !P_VALID( dwBindRoleID ) )
		return;

	for( EffecMap::iterator itr = m_mapEffect.begin(); itr != m_mapEffect.end(); itr++ )
	{
		tagBaseEffectData* pData = itr->second;
		if( dwBindRoleID == pData->dwBindRoleID )
		{
			pData->pSGNode->Show( bShow );
		}
	}	
}

BOOL EffectMgr::UpdataRoleEffect( tagBaseEffectData* pData )
{
	Role* pRole = RoleMgr::Inst()->FindRole( pData->dwBindRoleID );
	if( !P_VALID( pRole ) || !P_VALID( pRole->GetSceneNode() ) )
		return FALSE;

	if( pData->bBindTagNode )
		return TRUE;

	// 更新
	UpdataEffectMatrix( pData, pRole );

	return TRUE;
}

VOID EffectMgr::UpdataTargetPath( tagLaunchEffectData* pData, const float fTimeDelta )
{
	Role* pSrcRole = RoleMgr::Inst()->FindRole( pData->evtHitTarget.dwSrcRoleID );
	if( NULL == pSrcRole || NULL == pSrcRole->GetSceneNode() )
		return;

	Role* pTarRole = RoleMgr::Inst()->FindRole( pData->evtHitTarget.dwTarRoleID );
	if( NULL == pTarRole || !pTarRole->IsAttributeInited() )
		return;

	// 命中
	if( pData->pSGNode->IsTargetPathArrive() && !pData->bEventSended )
	{
		pData->pSGNode->Close();
		m_pGameFrameMgr->SendEvent( &pData->evtHitTarget );
		pData->bEventSended = true;
	}
	else
	{
		Vector3 vDest = GetBeAttackPos( pTarRole );
		pData->pSGNode->ChangeTargetPathTarget( vDest );
	}
}

VOID EffectMgr::PullOnWater( tagBaseEffectData* pData )
{
	Matrix4 matWorld = pData->pSGNode->GetWorldMat();
	Vector3 vPos( matWorld._41, matWorld._42, matWorld._43 );

	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	if( NULL == pNav )
		return;
	NavCollider* pCollider = pNav->GetCollider();
	if( NULL == pCollider || !pCollider->IsLoaded() )
		return;
	float fWaterH = 0.0f;
	if( pCollider->GetWaterHeight( vPos, fWaterH ) )
	{
		if( matWorld._42 < fWaterH )
		{
			matWorld._42 = fWaterH;
			pData->pSGNode->SetWorldMat( &matWorld );
		}
	}
}

Vector3	EffectMgr::GetRoleEffectScale( const EEffectScaleType eScaleType, Role* pRole )
{
	ASSERT( P_VALID(pRole) && pRole->IsAttributeInited() );

	if( EEST_RoleScale == eScaleType )
	{
		return pRole->GetObjectSpace().GetScale();
	}

	Vector3 vScale = pRole->GetObjectSpace().GetScale();
	if( pRole->IS_KIND_OF(NPC) )
	{
		NPC* pNPC = (NPC*)pRole;
		const tagCreatureMdlData* pMdlData = pNPC->GetMdlData();
		if( NULL != pMdlData )
		{
			vScale *= pMdlData->fBeAttackSfxScale;
		}
	}
	if( vScale.x > 3.0f || vScale.y > 3.0f || vScale.z > 3.0f )
		vScale = Vector3( 3.0f, 3.0f, 3.0f );
	return vScale;
}

float EffectMgr::GetEffectScale( const EEffectScaleType eScaleType, const Vector3& vSize )
{
	float fScale = 1.0f;
	switch( eScaleType )
	{
	case EEST_Y:
		{
			fScale = vSize.y / g_vStdEffectRefSize.y;
		}
		break;
	case EEST_XYZ:
		{
			fScale = ( vSize.x / g_vStdEffectRefSize.x + vSize.y / g_vStdEffectRefSize.y + vSize.z / g_vStdEffectRefSize.z ) / 3.0f;
		}
		break;
	case EEST_XZ:
		{
			fScale = ( vSize.x / g_vStdEffectRefSize.x + vSize.z / g_vStdEffectRefSize.z ) * 0.5f;
		}
		break;
	case EEST_YOutOnly:
		{
			fScale = vSize.y / g_vStdEffectRefSize.y;
			if( fScale < 1.0f )
				fScale = 1.0f;
		}
		break;
	}
	return fScale;
}

Vector3	EffectMgr::GetBeAttackPos( Role* pRole )
{
	ASSERT( P_VALID(pRole) && (pRole->IsAttributeInited()) );

	tstring strBoneName;
	if( pRole->GetSceneNode()->IS_KIND_OF(AvatarNode) )
	{
		strBoneName = _T("Bip01 Spine2");
	}
	else if( pRole->IS_KIND_OF(NPC) )
	{
		NPC* pNPC = (NPC*)pRole;
		strBoneName = CreatureData::Inst()->FindNpcBoneName( pNPC->GetTypeID(), _T("tag_Back") );
	}

	Matrix4 matBone = MathConst::IdentityMat;
	if( pRole->GetBoneSpace( strBoneName.c_str(), matBone ) )
	{
		return Vector3( matBone._41, matBone._42, matBone._43 );
	}
	else
	{
		AABBox boxRole;
		pRole->GetSceneNode()->GetBox( boxRole );
		Vector3 vPos = pRole->GetPos();
		vPos.y += ( boxRole.max.y - boxRole.min.y ) * g_fDefaultRoleEffYOffset;
		return vPos;
	}
}

bool EffectMgr::SFXCanMove( const TCHAR* szFileName )
{
	if( !P_VALID(szFileName) || _T('\0') == szFileName[0] )
		return false;

	tstring strFileName = szFileName;
	tstring::size_type pos1 = strFileName.find_last_of( _T('\\') );
	tstring::size_type pos2 = strFileName.rfind( _T('.') );
	if( tstring::npos == pos1 )
		pos1 = 0;
	else
		pos1++;
	if( tstring::npos == pos2 )
		pos2 = strFileName.length();
		
	tstring strSFXName = strFileName.substr( pos1, pos2 - pos1 );
	const tagSFXData* pSFXData = SFXData::Inst()->FindData( strSFXName.c_str() );
	if( NULL == pSFXData )
		return true;

	return pSFXData->bCanMove;
}

VOID EffectMgr::UpdataEffectMatrix( tagBaseEffectData* pData, Role* pRole )
{
	ASSERT( P_VALID( pData ) );
	ASSERT( P_VALID(pRole) && pRole->IsAttributeInited() );

	Transform trans;
	Vector3 vScale = GetRoleEffectScale( pData->eScaleType, pRole );
	trans.Scale( vScale.x, vScale.y, vScale.z );
	trans.Rotate( pRole->GetDestYaw(), 0, 0 );
	trans.Translate( pRole->GetPos() );
	pData->pSGNode->SetWorldMat( trans.GetMatrix() );
}

bool EffectMgr::IsEffectNumFull()
{
	const float r = (float)GameSet::Inst()->Get( EGS_EffectDetail ) / SGEffectNode::MAX_LOD;
	const size_t nMaxEffectNum = (size_t)( MIN_EFFECT_NUM + r * ( MAX_EFFECT_NUM - MIN_EFFECT_NUM ) );
	return m_mapEffect.size() >= nMaxEffectNum;
}