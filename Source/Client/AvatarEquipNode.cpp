#include "StdAfx.h"
#include "AvatarEquipNode.h"
#include "AvatarEquipElement.h"
#include "AvatarCfg.h"
#include "ItemProtoData.h"

const tstring g_strResPath[2] = 
{
	_T("data\\animation\\avatargirl\\std1.fskel"),
	_T("data\\animation\\avatarboy\\std1.fskel"),
};

IMP_RTTI(AvatarEquipNode,AvatarNode);

AvatarEquipNode::AvatarEquipNode(void) :
m_dwSuitID( GT_INVALID ),
m_bWeaponHandUp( false ),
m_pSuitEffect( NULL ),
m_bWeaponHide( false ),
m_bWeaPonPertainHide(false),
m_bPlaySuitEffect(false),
m_bNeedCull( false )
{
	memset( &m_Att, 0, sizeof(m_Att) );
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		m_pEquipElement[i] = new AvatarEquipElement( this, (EAvatarElement)i );
		m_pLoadingElement[i] = new AvatarEquipElement( this, (EAvatarElement)i );
	}
	ZeroMemory( &m_edHead, sizeof( tagEquipDisplay ) );
}

AvatarEquipNode::~AvatarEquipNode(void)
{
	DetachChild( m_pSuitEffect );
	SAFE_DELETE( m_pSuitEffect );
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		SAFE_DELETE( m_pLoadingElement[i] );
		SAFE_DELETE( m_pEquipElement[i] );
	}
}

bool AvatarEquipNode::IsInFrustum(const Frustum *pFrumstum) const
{
	if( m_bNeedCull && !IsAllResourceLoaded() )
		return false;

	return AvatarNode::IsInFrustum( pFrumstum );
}

void AvatarEquipNode::Update(float deltaTime,SceneGraph *pSG)
{
	for( int i = EAE_Start; i <= EAE_End; i++ )
		m_pEquipElement[i]->Updata();

	if( P_VALID( m_pSuitEffect ) )
	{
		m_pSuitEffect->SetWorldMat( &GetWorldMat() );
	}

	AvatarNode::Update( deltaTime, pSG );
}

void AvatarEquipNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
{
	// 检查资源加载情况
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( !m_pLoadingElement[i]->IsEmpty() )
		{
			// 加载完成，切换过去
			if( m_pLoadingElement[i]->ForceUpdate( deltaTime, pSG, m_Att ) )
			{
				AvatarEquipElement* pElement = m_pEquipElement[i];
				pElement->Destroy();
				m_pEquipElement[i] = m_pLoadingElement[i];
				m_pEquipElement[i]->AttachToSG();
				m_pLoadingElement[i] = pElement;

				if( EAE_RWeapon == i )
				{
					if( m_bWeaponHandUp )
					{
						m_pEquipElement[EAE_RWeapon]->HandUp( true );
						m_pEquipElement[EAE_RWeapon]->Open( true );
					}
					if( m_bWeaponHide )
					{
						m_pEquipElement[EAE_RWeapon]->SetInvisible( true, false, AvatarEquipElement::EPT_Main );
					}
					
					if (m_bWeaPonPertainHide)
					{
						m_pEquipElement[EAE_RWeapon]->SetInvisible(true, false, AvatarEquipElement::EPT_Pertain);
					}

				}
				CheckOverlap();
				SetDisplaySet( m_displaySet, false );
			}
		}
	}

	AvatarNode::ForceUpdate( deltaTime, pSG );
}

void AvatarEquipNode::LoadDefault()
{
	ASSERT( NULL == GetResource() );

	LoadRes( g_strResPath[0].c_str() );

	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		m_pLoadingElement[i]->ReloadAtt( m_Att, false );
	}
	m_edHead = m_pLoadingElement[EAE_Head]->GetDisplay();
	m_bNeedCull = true;
}

void AvatarEquipNode::LoadAvatar( const tagAvatarAtt& att, const tagAvatarEquip& ep )
{	
	ASSERT( NULL == GetResource() );

	LoadRes( g_strResPath[att.bySex].c_str() );
	
	m_Att = att;
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( EAE_Head == i )
		{
			m_edHead = ep.AvatarEquip[EAE_Head];
			ReloadHead( m_displaySet.bHideHead, m_displaySet.bFashionDisplay );
			continue;
		}
		if( !P_VALID( ep.AvatarEquip[i].dwTypeID ) )
			m_pLoadingElement[i]->ReloadAtt( att, m_displaySet.bFashionDisplay );
		else
			m_pLoadingElement[i]->ReloadEquip( att.bySex, ep.AvatarEquip[i], m_displaySet.bFashionDisplay );
	}
	UpdataSuitEffect( ep.dwSuitEffectID );
	m_bNeedCull = true;
}

void AvatarEquipNode::UpdateAllAtt( const tagAvatarAtt& att )
{
	// 头,发色
	if( att.wHairMdlID != m_Att.wHairMdlID || att.wHairTexID != m_Att.wHairTexID )
	{
		m_pLoadingElement[EAE_Head]->ReloadAtt( att, m_displaySet.bFashionDisplay );
	}

	// 面部,面部细节
	if( att.wFaceMdlID != m_Att.wFaceMdlID || att.wFaceDetailTexID != m_Att.wFaceDetailTexID )
	{
		m_pLoadingElement[EAE_Face]->ReloadAtt( att, m_displaySet.bFashionDisplay );
	}

	// 保存数据
	m_Att = att;
	m_bNeedCull = false;
}

void AvatarEquipNode::UpdateAllEquip( const tagAvatarEquip& ep )
{
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( EAE_Head == i )
		{
			m_edHead = ep.AvatarEquip[EAE_Head];
			ReloadHead( m_displaySet.bHideHead, m_displaySet.bFashionDisplay );
			continue;
		}
		if( !P_VALID( ep.AvatarEquip[i].dwTypeID ) )
			m_pLoadingElement[i]->ReloadAtt( m_Att, m_displaySet.bFashionDisplay );
		else
			m_pLoadingElement[i]->ReloadEquip( m_Att.bySex, ep.AvatarEquip[i], m_displaySet.bFashionDisplay );

		// 卸装，直接删除显示的装备
		if( m_pLoadingElement[i]->IsEmpty() )
		{
			m_pEquipElement[i]->Destroy();
			CheckOverlap();
		}
	}
	UpdataSuitEffect( ep.dwSuitEffectID );
	m_bNeedCull = false;
}

void AvatarEquipNode::SetEquip( const EAvatarElement eAvaEnt, const tagEquipDisplay& ed )
{
	if( eAvaEnt == EAE_Head )
	{
		m_edHead = ed;
		ReloadHead( m_displaySet.bHideHead, m_displaySet.bFashionDisplay );
		return;
	}

	if( !P_VALID( ed.dwTypeID ) )
		m_pLoadingElement[eAvaEnt]->ReloadAtt( m_Att, m_displaySet.bFashionDisplay );
	else
		m_pLoadingElement[eAvaEnt]->ReloadEquip( m_Att.bySex, ed, m_displaySet.bFashionDisplay );

	// 卸装，直接删除显示的装备
	if( m_pLoadingElement[eAvaEnt]->IsEmpty() )
	{
		m_pEquipElement[eAvaEnt]->Destroy();
		CheckOverlap();
	}

	m_bNeedCull = false;
}
 
const tagEquipDisplay& AvatarEquipNode::GetEquip( const EAvatarElement eAvaEnt ) const
{
	if( EAE_Head == eAvaEnt && m_displaySet.bHideHead )
		return m_edHead;
	
	if( !m_pLoadingElement[eAvaEnt]->IsEmpty() )
		return m_pLoadingElement[eAvaEnt]->GetDisplay();

	return m_pEquipElement[eAvaEnt]->GetDisplay();
}

const SceneNode* AvatarEquipNode::GetElementMainMdl( const EAvatarElement eAvaEnt ) const
{
	return m_pEquipElement[eAvaEnt]->GetMainMdl();
}

void AvatarEquipNode::HandUpWeapon( const bool bHandUp )
{
	if( bHandUp == m_bWeaponHandUp )
		return;

	if( !m_pLoadingElement[EAE_RWeapon]->IsEmpty() )
		m_pLoadingElement[EAE_RWeapon]->HandUp( bHandUp );

	m_pEquipElement[EAE_RWeapon]->HandUp( bHandUp );
	m_bWeaponHandUp = bHandUp;

	CheckOverlap();
}

void AvatarEquipNode::HideWeapon( const bool bHide )
{
	if( bHide == m_bWeaponHide )
		return;

	if( !m_pLoadingElement[EAE_RWeapon]->IsEmpty() )
		m_pLoadingElement[EAE_RWeapon]->SetInvisible( bHide, false, AvatarEquipElement::EPT_Main );
	if( !m_pLoadingElement[EAE_LWeapon]->IsEmpty() )
		m_pLoadingElement[EAE_LWeapon]->SetInvisible( bHide, false, AvatarEquipElement::EPT_Main );

	m_pEquipElement[EAE_RWeapon]->SetInvisible( bHide, false, AvatarEquipElement::EPT_Main );
	m_pEquipElement[EAE_LWeapon]->SetInvisible( bHide, false, AvatarEquipElement::EPT_Main );

	/*!todo:由于拢手是蒙皮模型, 需要特殊处理,直接隐藏的时候，会少掉一截手臂
	*/
	
	m_bWeaponHide = bHide;
}

void AvatarEquipNode::HideWeaponPertain(bool bHide)
{
	if (bHide == m_bWeaPonPertainHide)
		return;

	if (!m_pLoadingElement[EAE_RWeapon]->IsEmpty())
		m_pLoadingElement[EAE_RWeapon]->SetInvisible(bHide, false, AvatarEquipElement::EPT_Pertain);
	if (!m_pLoadingElement[EAE_LWeapon]->IsEmpty())
		m_pLoadingElement[EAE_LWeapon]->SetInvisible(bHide, false, AvatarEquipElement::EPT_Pertain);

	m_pEquipElement[EAE_RWeapon]->SetInvisible(bHide, false, AvatarEquipElement::EPT_Pertain);
	m_pEquipElement[EAE_LWeapon]->SetInvisible(bHide, false, AvatarEquipElement::EPT_Pertain);

	m_bWeaPonPertainHide = bHide;
}


void AvatarEquipNode::OpenAllEquip( const bool bOpen )
{
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( EAE_LWeapon == i )
			continue;

		if( !m_pLoadingElement[i]->IsEmpty() )
			m_pLoadingElement[i]->Open( bOpen );
		m_pEquipElement[i]->Open( bOpen );
	}
}

bool AvatarEquipNode::IsAllEquipOpened()
{
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( EAE_LWeapon == i )
			continue;
		if( AvatarEquipElement::EOS_Opened != m_pEquipElement[i]->GetOpenState() )
			return false;
	}
	return true;
}

bool AvatarEquipNode::IsAllEquipClosed()
{
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( EAE_LWeapon == i )
			continue;
		if( AvatarEquipElement::EOS_Closed != m_pEquipElement[i]->GetOpenState() )
			return false;
	}
	return true;
}

void AvatarEquipNode::PlayBladeFlare( const bool bPlay )
{
	m_pEquipElement[EAE_RWeapon]->PlayBladeFlare( bPlay );
}



void AvatarEquipNode::PlaySuitEffect(bool bPlay)
{
	if (m_bPlaySuitEffect == bPlay)
		return;
	if (!P_VALID(m_dwSuitID))
		return;

	m_bPlaySuitEffect = bPlay;
	
	DetachChild(m_pSuitEffect);
	SAFE_DELETE(m_pSuitEffect);

	if (m_bPlaySuitEffect)
	{
		const tagSuitProtoClient *pSuitProto = ItemProtoData::Inst()->FindSuitProto(m_dwSuitID);
		if (!P_VALID(pSuitProto))
		{
			TRACE( _T("Can't find SuitProto.TypeID=%d\n"), m_dwSuitID );
			return;
		}
		if( _T('\0') == pSuitProto->szSpecEffectMod )
			return;

		tstring strEffectPath = _T("data\\system\\effect\\combat\\");
		strEffectPath += pSuitProto->szSpecEffectMod;
		strEffectPath += _T(".sfx");
		
		IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
		DWORD dwFileSize = pIFS->GetSize( strEffectPath.c_str() );
		if( 0 == dwFileSize || GT_INVALID == dwFileSize )
		{
			TRACE( _T("Suit SFX file have not found.file=%s\r\n"), strEffectPath.c_str() );
			return;
		}
		
		m_pSuitEffect = new SGAttachableEffectNode;
		m_pSuitEffect->LoadRes(strEffectPath.c_str());
		AttachChild(m_pSuitEffect);
	}
}



void AvatarEquipNode::UpdataLongHunEffect( const EAvatarElement eAvaEnt, const BYTE byEquipEffect )
{
	if( !m_pLoadingElement[eAvaEnt]->IsEmpty() )
		m_pLoadingElement[eAvaEnt]->UpdataLongHunEffect( byEquipEffect, false );
	else
		m_pEquipElement[eAvaEnt]->UpdataLongHunEffect( byEquipEffect, true );
}

void AvatarEquipNode::UpdataSuitEffect( const DWORD dwSuitID )
{
	if( dwSuitID == m_dwSuitID )
		return;

	DetachChild( m_pSuitEffect );
	SAFE_DELETE( m_pSuitEffect );
	m_bPlaySuitEffect = false;

	if( P_VALID( dwSuitID ) )
	{
		const tagSuitProtoClient* pSuitProto = ItemProtoData::Inst()->FindSuitProto( dwSuitID );
		if( !P_VALID(pSuitProto) )
		{
			TRACE( _T("Can't find SuitProto.TypeID=%d\n"), dwSuitID );
			return;
		}
		if( _T('\0') == pSuitProto->szSpecEffectMod )
			return;

		tstring strEffectPath = _T("data\\system\\effect\\combat\\");
		strEffectPath += pSuitProto->szSpecEffectMod;
		strEffectPath += _T(".sfx");

		IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
		DWORD dwFileSize = pIFS->GetSize( strEffectPath.c_str() );
		if( 0 == dwFileSize || GT_INVALID == dwFileSize )
		{
			TRACE( _T("Suit SFX file have not found.file=%s\r\n"), strEffectPath.c_str() );
			return;
		}

		m_pSuitEffect = new SGAttachableEffectNode;
		m_pSuitEffect->LoadRes( strEffectPath.c_str() );
		AttachChild( m_pSuitEffect );
		m_bPlaySuitEffect = true;
	}
	m_dwSuitID = dwSuitID;
}

EItemTypeEx AvatarEquipNode::GetRWeaponType() const
{
	DWORD dwTypeID = GT_INVALID;
	if( !m_pLoadingElement[EAE_RWeapon]->IsEmpty() )
		dwTypeID = m_pLoadingElement[EAE_RWeapon]->GetDisplay().dwTypeID;
	else
		dwTypeID = m_pEquipElement[EAE_RWeapon]->GetDisplay().dwTypeID;

	const tagEquipDisplay& displayRWeapon = m_pEquipElement[EAE_RWeapon]->GetDisplay();
	if( !P_VALID( dwTypeID ) )
		return EITE_Null;

	const tagEquipProto* pProtoRWeapon = ItemProtoData::Inst()->FindEquipProto( dwTypeID );
	ASSERT( P_VALID( pProtoRWeapon ) );
	
	return pProtoRWeapon->eTypeEx;
}

EItemTypeEx AvatarEquipNode::GetLWeaponType() const
{
	DWORD dwTypeID = GT_INVALID;
	if( !m_pLoadingElement[EAE_LWeapon]->IsEmpty() )
		dwTypeID = m_pLoadingElement[EAE_LWeapon]->GetDisplay().dwTypeID;
	else
		dwTypeID = m_pEquipElement[EAE_LWeapon]->GetDisplay().dwTypeID;

	if( !P_VALID( dwTypeID ) )
		return EITE_Null;

	const tagEquipProto* pProtoLWeapon = ItemProtoData::Inst()->FindEquipProto( dwTypeID );
	ASSERT( P_VALID( pProtoLWeapon ) );

	return pProtoLWeapon->eTypeEx;
}

void AvatarEquipNode::CheckOverlap()
{
	// 先取消隐藏
	m_pEquipElement[EAE_Glove]->SetInvisible( false, true );
	m_pEquipElement[EAE_LWeapon]->SetInvisible( false );

	const tagEquipDisplay& displayRWeapon = m_pEquipElement[EAE_RWeapon]->GetDisplay();
	const tagEquipDisplay& displayLWeapon = m_pEquipElement[EAE_LWeapon]->GetDisplay();

	if( !P_VALID( displayRWeapon.dwTypeID ) && !P_VALID( displayLWeapon.dwTypeID ) )
		return;

	const SceneNode* pNodeRWeapon = m_pEquipElement[EAE_RWeapon]->GetMainMdl();
	const SceneNode* pNodeLWeapon = m_pEquipElement[EAE_LWeapon]->GetMainMdl();

	// 只要装备了拢手就隐藏右边护腕
	if( ( NULL != pNodeRWeapon && pNodeRWeapon->IS_STATIC_CLASS(SGSkinNode) ) ||
		( NULL != pNodeLWeapon && pNodeLWeapon->IS_STATIC_CLASS(SGSkinNode) ) )
	{
		m_pEquipElement[EAE_Glove]->SetInvisible( true, true );
	}

	// 主、副手都是拢手隐藏副手
	if( ( NULL != pNodeRWeapon && pNodeRWeapon->IS_STATIC_CLASS(SGSkinNode) ) &&
		( NULL != pNodeLWeapon && pNodeLWeapon->IS_STATIC_CLASS(SGSkinNode) ) )
	{
		m_pEquipElement[EAE_LWeapon]->SetInvisible( true );
	}

	// 主、副手武器挂载点相同隐藏副手
	if( NULL != pNodeRWeapon && pNodeRWeapon->IS_STATIC_CLASS(SGAvatarAttachment) &&
		NULL != pNodeLWeapon && pNodeLWeapon->IS_STATIC_CLASS(SGAvatarAttachment) )
	{
		const SGAvatarAttachment* pAttachRWeapon = (const SGAvatarAttachment*)pNodeRWeapon;
		const SGAvatarAttachment* pAttachLWeapon = (const SGAvatarAttachment*)pNodeLWeapon;
		if( pAttachRWeapon->GetTagNodeName() == pAttachLWeapon->GetTagNodeName() )
			m_pEquipElement[EAE_LWeapon]->SetInvisible( true );
	}
}

void AvatarEquipNode::SetDisplaySet( const tagDisplaySet& ds, const bool bHead/* = true*/ )
{
	if( bHead )
		ReloadHead( ds.bHideHead, ds.bFashionDisplay );
	m_pEquipElement[EAE_Face]->SetInvisible( ds.bHideFace, false, AvatarEquipElement::EPT_Attach );
	m_pEquipElement[EAE_Back]->SetInvisible( ds.bHideBack );

	m_displaySet = ds;
}

void AvatarEquipNode::ReloadHead( const bool bHide, const bool bFashion )
{
	if( bHide || !P_VALID( m_edHead.dwTypeID ) )
		m_pLoadingElement[EAE_Head]->ReloadAtt( m_Att, bFashion );
	else
		m_pLoadingElement[EAE_Head]->ReloadEquip( m_Att.bySex, m_edHead, bFashion );
}

bool AvatarEquipNode::IsAllResourceLoaded() const
{
	const ResSkeleton* pRes = GetResource();
	if( NULL == pRes || !pRes->IsCreated() )
		return false;
	for( int i = EAE_Start; i <= EAE_End; i++ )
	{
		if( !m_pLoadingElement[i]->IsEmpty() )
			return false;
	}
	return true;
}

