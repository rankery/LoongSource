#include "StdAfx.h"
#include "AvatarEquipElement.h"
#include "AvatarEquipNode.h"
#include "ItemProtoData.h"
#include "AvatarCfg.h"

// 变形部分挂载点名称
const tstring g_strAttachTagNodeName[EEAP_Num] =
{
	_T(""),
	_T("tag_Hat"),
	_T("tag_Back"),
	_T("tag_RSld"),
	_T("tag_LSld"),
	_T("tag_Wep6"),
};

// 变形部分模型文件名后缀
const tstring g_strAttachMdlAdd[EEAP_Num] =
{
	_T(""),
	_T("_Hat"),
	_T("_Back"),
	_T("_RSld"),
	_T("_LSld"),
	_T("_Wep6"),
};

// 武器非持械状态挂载点名称[右][左]
const tstring g_strWeaponPosTagNodeName[2][X_WEAPONPOS_NUM] =
{
	{
		_T(""),
		_T("tag_Wep1"),
		_T("tag_Wep3"),
		_T("tag_Wep5"),
		_T("tag_RHd"),
	},
	{
		_T(""),
		_T("tag_Wep2"),
		_T("tag_Wep4"),
		_T("tag_Wep5"),
		_T("tag_LHd"),
	},

};

// 武器持械状态挂载点名称[右][左]
const tstring g_strWeaponHandUpPosTagNodeName[2] =
{
	_T("tag_RHd"),
	_T("tag_LHd"),
};

// 装备展开状态的动画名称
const tstring g_strOpenStateTrackName[AvatarEquipElement::EOS_Num] =
{
	_T("A1"),
	_T("A2"),
	_T("A3"),
	_T("A4"),
};

AvatarEquipElement::AvatarEquipElement( AvatarEquipNode* pFather, EAvatarElement eElement ) :
m_pFather( pFather ),
m_eElement( eElement ),
m_eOpenState( EOS_Closed ),
m_bTexChanged( false )
{
	ZeroMemory( &m_equipDisplay, sizeof( tagEquipDisplay ) );
}

AvatarEquipElement::~AvatarEquipElement()
{
	Destroy();
}

void AvatarEquipElement::Destroy()
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		tagPartData& data = m_vecPartData[i];
		SceneNode* pPartFather = (SceneNode*)data.pSGNode->GetParent();

		if( NULL != pPartFather )
			pPartFather->DetachChild( data.pSGNode );
	}

	for( size_t i = 0; i < m_vecPartData.size(); i++ )
		SAFE_DELETE( m_vecPartData[i].pSGNode );

	m_vecPartData.clear();
	ZeroMemory( &m_equipDisplay, sizeof( tagEquipDisplay ) );

	m_eOpenState = EOS_Closed;
	m_strGlowName = _T("");
	m_strLongHunEffName = _T("");
	m_bTexChanged = false;
}

void AvatarEquipElement::ReloadAtt( const tagAvatarAtt& att, const bool bFashion )
{
	Destroy();

	// 护腕的内衣也分左右
	if( EAE_Glove == m_eElement )
	{
		tstring strFileName = GetAttFileName( att, bFashion, true );
		LoadOneSkin( EPT_Main, true, strFileName );
		strFileName = GetAttFileName( att, bFashion, false );
		LoadOneSkin( EPT_Main, false, strFileName );
	}
	else
	{
		tstring strFileName = GetAttFileName( att, bFashion );
		LoadOneSkin( EPT_Main, true, strFileName );
	}
	
}

void AvatarEquipElement::ReloadEquip( const BYTE bySex, const tagEquipDisplay& display, const bool bFashion )
{
	ASSERT( P_VALID( display.dwTypeID ) );

	// 没有变化，直接返回（武器，强制重新加载，因为时装/装备模式切换的时候武器ID不会改变，但有时也需要重新加载（如拢手））
	if( EAE_RWeapon != m_eElement && EAE_LWeapon != m_eElement && 0 == memcmp( &m_equipDisplay, &display, sizeof( tagEquipDisplay ) ) )
		return;

	Destroy();

	m_equipDisplay = display;

	const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay( display.dwTypeID );
	const tagEquipProto* pProto = ItemProtoData::Inst()->FindEquipProto( display.dwTypeID );

	if( !P_VALID( pInfo ) || !P_VALID( pProto ) )
	{
		TRACE( _T("Can't find this equip's data.TypeID=%d"), display.dwTypeID );
		return;
	}

	// 主模型
	tstring strRightMdlFileName;
	tstring strLeftMdlFileName;
	if( 0 == bySex )
	{
		strRightMdlFileName = GetEquipFileName( bySex, pInfo->szFemaleRightMod, pInfo->eModelType );
		strLeftMdlFileName = GetEquipFileName( bySex, pInfo->szFemaleLeftMod, pInfo->eModelType );
	}
	else
	{
		strRightMdlFileName = GetEquipFileName( bySex, pInfo->szMaleRightMod, pInfo->eModelType );
		strLeftMdlFileName = GetEquipFileName( bySex, pInfo->szMaleLeftMod, pInfo->eModelType );
	}

	if( EAE_RWeapon != m_eElement && EAE_LWeapon != m_eElement )
	{
		if( !strRightMdlFileName.empty() )
			LoadOnePart( EPT_Main, true, strRightMdlFileName, (EEquipMdlType)pInfo->eModelType, g_strWeaponPosTagNodeName[0][pProto->eWeaponPos] );

		if( !strLeftMdlFileName.empty() )
			LoadOnePart( EPT_Main, false, strLeftMdlFileName, (EEquipMdlType)pInfo->eModelType, g_strWeaponPosTagNodeName[1][pProto->eWeaponPos] );
	}
	else
	{
		int nTag = EAE_RWeapon == m_eElement ? 1 : 0;
		// 刀的挂载位置相反
		if( EITE_Blade == pProto->eTypeEx )
			nTag = EAE_RWeapon == m_eElement ? 0 : 1;

		if( !strRightMdlFileName.empty() )
			LoadOnePart( EPT_Main, true, strRightMdlFileName, (EEquipMdlType)pInfo->eModelType, g_strWeaponPosTagNodeName[nTag][pProto->eWeaponPos] );

		// 武器附属模型
		// 箭袋的挂载位置相反
		if( EITE_Bow == pProto->eTypeEx )
			nTag = EAE_RWeapon == m_eElement ? 0 : 1;
		tstring strPertainMdlFileName = GetEquipFileName( bySex, pInfo->szPertainMod, EEMT_FSM );
		if( !strPertainMdlFileName.empty() )
			LoadOnePart( EPT_Pertain, true, strPertainMdlFileName, EEMT_FSM, g_strWeaponPosTagNodeName[nTag][pInfo->ePertainModPos] );


		// 时装模式装备拢手需要加载一节胳膊
		if( bFashion && EITE_Faqi == pProto->eTypeEx && GetMainMdl()->IS_STATIC_CLASS(SGSkinNode) )
		{
			tstring strMendFileName = strRightMdlFileName;
			if( 0 == bySex )
				strMendFileName = _T("data\\model\\weapon\\arm\\f_arm.fskin");
			else
				strMendFileName = _T("data\\model\\weapon\\arm\\m_arm.fskin");
			LoadOnePart( EPT_Mend, true, strMendFileName, EEMT_FSKIN, tstring(_T("")) );
		}
	}

	// 时装颜色
	if( EIT_Fashion != pProto->eType || !pProto->bCanDye )
	{
		m_equipDisplay.n8ColorID = EC_Null;
	}

	// 挂载变形模型
	tstring strAttachFileName;
	for( int i = 0; i < MAX_EQUIP_ATTACH_NUM; i++ )
	{
		if( EEAP_NULL != pInfo->eAttachModelPos[i] )
		{
			if( !strRightMdlFileName.empty() )
			{
				strAttachFileName = strRightMdlFileName;
				tstring::size_type pos = strAttachFileName.rfind( _T('.') );
				ASSERT( tstring::npos != pos );
				strAttachFileName.erase( pos );
				strAttachFileName += g_strAttachMdlAdd[pInfo->eAttachModelPos[i]];
				strAttachFileName += GetMdlExt( EEMT_FAK );
				LoadOnePart( EPT_Attach, true, strAttachFileName, EEMT_FAK, g_strAttachTagNodeName[pInfo->eAttachModelPos[i]] );
			}
		}
	}

	// 刀光(只有主手才加载)
	if( EAE_RWeapon == m_eElement )
	{
		tstring strTagNodeName;
		if( EITE_Faqi == pProto->eTypeEx && GetMainMdl()->IS_STATIC_CLASS(SGSkinNode) )
			strTagNodeName = _T("tag_RHd");
		LoadOneSFX( EPT_BladeFlare, true, pInfo->szBladeFlareSFX, strTagNodeName, false );
	}
	Open( false );

	// 光晕
	if( _T('\0') != pInfo->szBladeGlowSFX[0] )
	{
		m_strGlowName = GetGlowName( pInfo->szBladeGlowSFX, display.byFlareVal );
	}
}

void AvatarEquipElement::Updata()
{
	// 更新变形状态
	if( EOS_Opening == m_eOpenState && IsKeyFrameTrackEnd( g_strOpenStateTrackName[EOS_Opening] ) )
	{
		m_eOpenState = EOS_Opened;
		PlayKeyFrameTrack( g_strOpenStateTrackName[m_eOpenState], true );
	}
	else if( EOS_Closing == m_eOpenState && IsKeyFrameTrackEnd( g_strOpenStateTrackName[EOS_Closing] ) )
	{
		m_eOpenState = EOS_Closed;
		PlayKeyFrameTrack( g_strOpenStateTrackName[m_eOpenState], true );
	}

	// 更新特效矩阵
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		tagPartData& data = m_vecPartData[i];
		if( EPT_BladeFlare == data.eType && NULL != data.pSGNode->GetParent() )
		{
			data.pSGNode->SetWorldMat( &data.pSGNode->GetParent()->GetWorldMat() );
		}
	}
}

bool AvatarEquipElement::ForceUpdate(float deltaTime,SceneGraph *pSG,const tagAvatarAtt& att)
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		tagPartData& data = m_vecPartData[i];
		if( NULL != data.pSGNode )
			data.pSGNode->ForceUpdate( deltaTime, pSG );
	}

	if( IsResourceReady() )
	{
		if( m_bTexChanged )
			return true;
		else
		{
			if( !P_VALID( m_equipDisplay.dwTypeID ) )
			{
				if( EAE_Head == m_eElement )
					SetHairColorTex( att.bySex, att.wHairMdlID, att.wHairTexID );
				else if( EAE_Face == m_eElement )
					SetFaceDetailTex( att.bySex, att.wFaceDetailTexID );
			}
			SetColor( m_equipDisplay.n8ColorID );
			LoadAttachEffect( EPT_Glow, m_strGlowName, false );
			// 龙魂特效
			const BYTE byEquipEffect = m_equipDisplay.byEquipEffect;
			m_equipDisplay.byEquipEffect = 0;
			UpdataLongHunEffect( byEquipEffect, false );

			m_bTexChanged = true;
		}
	}
	return false;
}

void AvatarEquipElement::Open( const bool bOpen )
{
	if( !HaveKeyFrame() )
	{
		m_eOpenState = ( bOpen ? EOS_Opened : EOS_Closed );
		return;
	}

	if( EOS_Closed == m_eOpenState && bOpen )
	{
		m_eOpenState = EOS_Opening;
		PlayKeyFrameTrack( g_strOpenStateTrackName[m_eOpenState], false );
	}
	else if( EOS_Opened == m_eOpenState && !bOpen )
	{
		m_eOpenState = EOS_Closing;
		PlayKeyFrameTrack( g_strOpenStateTrackName[m_eOpenState], false );
	}
}

const SceneNode* AvatarEquipElement::GetMainMdl() const
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		if( EPT_Main == m_vecPartData[i].eType )
			return m_vecPartData[i].pSGNode;
	}
	return NULL;
}

void AvatarEquipElement::HandUp( const bool bHandUp )
{
	if( !P_VALID( m_equipDisplay.dwTypeID ) )
		return;

	// 按照策划要求:没有双持,拿出/收回都是主手武器
	if( EAE_RWeapon != m_eElement )
		return;

	const tagEquipProto* pProto = ItemProtoData::Inst()->FindEquipProto( m_equipDisplay.dwTypeID );

	if( !P_VALID( pProto ) )
	{
		TRACE( _T("Can't find this equipProto's data.TypeID=%d"), m_equipDisplay.dwTypeID );
		return;
	}

	if( !bHandUp )
		PlayBladeFlare( bHandUp );

	// 法器手持挂载位置不变
	if( EITE_Faqi == pProto->eTypeEx )
		return;

	if( bHandUp )
	{
		for( size_t i = 0; i < m_vecPartData.size(); i++ )
		{
			if( m_vecPartData[i].pSGNode->GetParent() == m_pFather && 
				m_vecPartData[i].pSGNode->IS_STATIC_CLASS(SGAvatarAttachment) &&
				EPT_Main == m_vecPartData[i].eType )
			{ 
				SGAvatarAttachment* pAttach = (SGAvatarAttachment*)m_vecPartData[i].pSGNode;
				// 弓和琴挂在左手
				if( EITE_Bow == pProto->eTypeEx || EITE_Qin == pProto->eTypeEx )
					pAttach->BindToTagNode( g_strWeaponHandUpPosTagNodeName[1].c_str() );
				else
					pAttach->BindToTagNode( g_strWeaponHandUpPosTagNodeName[0].c_str() );
			}
		}
	}
	else
	{
		for( size_t i = 0; i < m_vecPartData.size(); i++ )
		{
			if( m_vecPartData[i].pSGNode->GetParent() == m_pFather && 
				m_vecPartData[i].pSGNode->IS_STATIC_CLASS(SGAvatarAttachment) &&
				EPT_Main == m_vecPartData[i].eType )
			{ 
				SGAvatarAttachment* pAttach = (SGAvatarAttachment*)m_vecPartData[i].pSGNode;
				// 刀的挂载位置相反
				if( EITE_Blade == pProto->eTypeEx )
					pAttach->BindToTagNode( g_strWeaponPosTagNodeName[0][pProto->eWeaponPos].c_str() );
				else
					pAttach->BindToTagNode( g_strWeaponPosTagNodeName[1][pProto->eWeaponPos].c_str() );
			}
		}
	}
}

void AvatarEquipElement::LoadOnePart( EPartType ePartType, const bool bRight, const tstring& strFileName, const EEquipMdlType eMdlType, const tstring& strTagNodeName )
{
	// FOR DEBUG??
	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Model");
	DWORD dwFileSize = pIFS->GetSize( strFileName.c_str() );
	if( 0 == dwFileSize || GT_INVALID == dwFileSize )
	{
		LOG( _T("%s have not found.\r\n"), strFileName.c_str() );
		ASSERT( 0 );
		return;
	}

	tagPartData data( ePartType, bRight );
	switch( eMdlType )
	{
		case EEMT_FSKIN:
			{
				SGSkinNode *pNewSkin = new SGSkinNode;
				pNewSkin->LoadRes( strFileName.c_str() );
				data.pSGNode = pNewSkin;
				m_vecPartData.push_back( data );
			}
			break;
		case EEMT_FAK:
		case EEMT_FSM:
			{
				SGAvatarAttachment *pAttach = new SGAvatarAttachment;
				pAttach->LoadRes( strFileName.c_str() );
				if( !strTagNodeName.empty() )
					pAttach->BindToTagNode( strTagNodeName.c_str() );
				data.pSGNode = pAttach;
				m_vecPartData.push_back( data );
			}
			break;
		default: break;
	}
}

void AvatarEquipElement::LoadOneSkin( EPartType ePartType, const bool bRight, const tstring& strFileName )
{
	if( strFileName.empty() )
		return;

	// FOR DEBUG??
	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Model");
	DWORD dwFileSize = pIFS->GetSize( strFileName.c_str() );
	if( 0 == dwFileSize || GT_INVALID == dwFileSize )
	{
		LOG( _T("%s have not found.\r\n"), strFileName.c_str() );
		ASSERT( 0 );
		return;
	}

	tagPartData data( ePartType, bRight );

	SGSkinNode *pNewSkin = new SGSkinNode;
	pNewSkin->LoadRes( strFileName.c_str() );
	data.pSGNode = pNewSkin;
	m_vecPartData.push_back( data );
}

void AvatarEquipElement::LoadOneSFX( EPartType ePartType, const bool bRight, const tstring& strFileName, const tstring& strTagNodeName, const bool bAttchToSG )
{
	if( strFileName.empty() )
		return;

	SceneNode* pMainMdl = (SceneNode*)GetMainMdl();
	if( NULL == pMainMdl )
		return;

	tstring strEffectPath = _T("data\\system\\effect\\combat\\");
	strEffectPath += strFileName;
	strEffectPath += _T(".sfx");

	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	DWORD dwFileSize = pIFS->GetSize( strEffectPath.c_str() );
	if( 0 == dwFileSize || GT_INVALID == dwFileSize )
	{
		TRACE( _T("SFX file have not found.file=%s\r\n"), strEffectPath.c_str() );
		return;
	}

	tagPartData data( ePartType, bRight );
	SGAttachableEffectNode* pEffect = new SGAttachableEffectNode;
	pEffect->LoadRes( strEffectPath.c_str() );

	if( !strTagNodeName.empty() )
		pEffect->BindToTagNode( strTagNodeName.c_str() );

	data.pSGNode = pEffect;
	m_vecPartData.push_back( data );

	if( bAttchToSG )
		AttachPartNode( pMainMdl, pEffect );
}

bool AvatarEquipElement::LoadAttachEffect( const EPartType eType, const tstring& strFileName, const bool bAttchToSG )
{
	if( strFileName.empty() )
		return true;

	const SceneNode* pMainNode = GetMainMdl();
	ASSERT( NULL != pMainNode );
	if( pMainNode->IS_STATIC_CLASS( SGAvatarAttachment ) )
	{
		SGAvatarAttachment* pEntityAttach = (SGAvatarAttachment*)pMainNode;
		if( !pEntityAttach->IsLoaded() )
			return false;

		for( int i = 0; i < 8; i++ )
		{
			tstringstream stream;
			stream<<_T("tag_BG")<<i;
			if( !pEntityAttach->HaveTagNode( stream.str().c_str() ) )
				continue;

			LoadOneSFX( eType, true, strFileName, stream.str(), bAttchToSG );
		}
		return true;
	}
	else if( pMainNode->IS_STATIC_CLASS( SGSkinNode ) )
	{
		tstring strTagNodeName = GetAttatchEffectBindBoneName( true );
		LoadOneSFX( eType, true, strFileName, strTagNodeName, bAttchToSG );

		// 防具光晕有左右2个
		if( EAE_RWeapon != m_eElement && EAE_LWeapon != m_eElement )
		{
			strTagNodeName = GetAttatchEffectBindBoneName( false );
			LoadOneSFX( eType, false, strFileName, strTagNodeName, bAttchToSG );
		}
		return true;
	}
	else
	{
		return true;
	}
}

bool AvatarEquipElement::HaveKeyFrame()
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		if( !m_vecPartData[i].pSGNode->IS_KIND_OF( EntityNode ) )
			continue;

		EntityNode* pEntity = (EntityNode*)m_vecPartData[i].pSGNode;
		AnimationCtrl* pAni = pEntity->GetAniCtrl();
		if( NULL == pAni )
			continue;
		return true;
	}
	return false;
}

void AvatarEquipElement::PlayKeyFrameTrack( const tstring& strTrackName, bool bLoop )
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		if( !m_vecPartData[i].pSGNode->IS_KIND_OF( EntityNode ) )
			continue;

		EntityNode* pEntity = (EntityNode*)m_vecPartData[i].pSGNode;
		AnimationCtrl* pAni = pEntity->GetAniCtrl();
		if( NULL == pAni )
			continue;
		pAni->OpenTrack( strTrackName.c_str(), bLoop );
	}
}

bool AvatarEquipElement::IsKeyFrameTrackEnd( const tstring& strTrackName )
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		if( !m_vecPartData[i].pSGNode->IS_KIND_OF( EntityNode ) )
			continue;

		EntityNode* pEntity = (EntityNode*)m_vecPartData[i].pSGNode;
		AnimationCtrl* pAni = pEntity->GetAniCtrl();
		if( NULL == pAni )
			continue;
		if( !pAni->IsTrackEnd( strTrackName.c_str() ) )
			return false;
	}
	return true;
}

const tstring AvatarEquipElement::GetMdlExt( const EEquipMdlType eType ) const
{
	switch( eType )
	{
		case EEMT_FSKIN:	return _T(".fskin");
		case EEMT_FAK:		return _T(".fak");
		case EEMT_FSM:		return _T(".fsm");
		default: ASSERT( 0 && _T("invalid equip mdl type!") ); return _T("");
	}
}

const tstring AvatarEquipElement::GetEquipFileName( const BYTE bySex, const TCHAR* szMdlName, const EEquipMdlType eMdlType ) const
{
	if( NULL == szMdlName || _T('\0') == szMdlName[0] )
		return _T("");

	tstring strFileName = _T("data\\model");

	if( EAE_RWeapon == m_eElement || EAE_LWeapon == m_eElement )
	{
		strFileName += _T("\\weapon\\");
		strFileName += szMdlName;
		if( EEMT_FSKIN == eMdlType )
		{
			// 蒙皮武器特殊处理
			tstring::size_type pos = strFileName.rfind( _T('_') );
			if( tstring::npos != pos )
				strFileName.erase( pos );
		}
	}
	else
	{
		if( 0 == bySex )
			strFileName += _T("\\AvatarGirl");
		else
			strFileName += _T("\\AvatarBoy");
		switch( m_eElement )
		{
			case EAE_Head:     strFileName += _T("\\hat");      break;
			case EAE_Face:	   strFileName += _T("\\face");     break;
			case EAE_Upper:    strFileName += _T("\\upper");	break;
			case EAE_Lower:    strFileName += _T("\\lower");	break;
			case EAE_Glove:    strFileName += _T("\\glove");	break;
			case EAE_Boot:     strFileName += _T("\\boot");		break;
			case EAE_Back:     strFileName += _T("\\mantle");	break;
			default: ASSERT( 0 ); break;
		}
	}
	strFileName += _T("\\");
	strFileName += szMdlName;
	strFileName += GetMdlExt( eMdlType );

	return strFileName;
}

const tstring AvatarEquipElement::GetAttFileName( const tagAvatarAtt& att, bool bFashion, bool bRight ) const
{
	switch( m_eElement )
	{
		case EAE_Head:   return AvatarCfg::Inst()->GetHairTypeMdlPath( att.bySex, att.wHairMdlID );
		case EAE_Face:   return AvatarCfg::Inst()->GetFaceMdlPath( att.bySex, att.wFaceMdlID );
		case EAE_Upper:  return AvatarCfg::Inst()->GetUpperUWMdlPath( att.bySex, bFashion );
		case EAE_Lower:  return AvatarCfg::Inst()->GetLowerUWMdlPath( att.bySex, bFashion );
		case EAE_Glove:  return AvatarCfg::Inst()->GetGloveUWMdlPath( att.bySex, bRight, bFashion );
		case EAE_Boot:   return AvatarCfg::Inst()->GetBootUWMdlPath( att.bySex, bFashion );
		default:		 return _T("");
	}
}

const tstring AvatarEquipElement::GetGlowName( const TCHAR* szBaseName, const BYTE byVal ) const
{
	if( NULL == szBaseName || _T('\0') == szBaseName[0] )
		return _T("");

	int n = 0;
	if( byVal < 4 )	return _T("");
	else if( byVal < 10 )	n = 1;
	else if( byVal < 18 )	n = 2;
	else if( byVal < 25 )	n = 3;
	else if( byVal < 30 )	n = 4;
	else if( byVal < 35 )	n = 5;
	else if( byVal < 40 )	n = 6;
	else if( byVal < 50 )	n = 7;
	else if( byVal < 60 )	n = 8;
	else if( byVal < 70 )	n = 9;
	else if( byVal < 80 )	n = 10;
	else if( byVal < 100 )	n = 11;
	else if( byVal == 100 )	n = 12;
	else	return _T("");

	TCHAR szBuff[X_SHORT_NAME];
	_sntprintf( szBuff, X_SHORT_NAME, _T("%s_%02d"), szBaseName, n );
	return szBuff;
}

const tstring AvatarEquipElement::GetAttatchEffectBindBoneName( const bool bRight ) const
{
	switch( m_eElement )
	{
		case EAE_Upper:		return bRight ? _T("Bip01 R Clavicle") : _T("Bip01 L Clavicle");
		case EAE_Glove:		return bRight ? _T("Bip01 R Forearm")  : _T("Bip01 L Forearm");
		case EAE_Boot:		return bRight ? _T("Bip01 R Foot")     : _T("Bip01 L Foot");
		case EAE_RWeapon:
		case EAE_LWeapon:	return _T("tag_RHd"); // 拢手
		default: return _T("");
	}
}

void AvatarEquipElement::SetInvisible( const bool bInvisible, const bool bRightMdlOnly/* = false */, const EPartType eType/* = EPT_Num*/ )
{
	SceneNode* pMainMdl = (SceneNode*)GetMainMdl();
	if( NULL == pMainMdl )
		return;

	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		tagPartData& data = m_vecPartData[i];
		if( EPT_Num != eType && eType != data.eType )
			continue;

		SceneNode* pPartFather = (SceneNode*)data.pSGNode->GetParent();
		if( bInvisible )
		{
			if( pPartFather == NULL )
				continue;
			if( !bRightMdlOnly || ( bRightMdlOnly && true == data.bRight ) )
				pPartFather->DetachChild( data.pSGNode );
		}
		else
		{
			if( NULL != pPartFather )
				continue;
			if( !bRightMdlOnly || ( bRightMdlOnly && true == data.bRight ) )
				AttachPartNode( pMainMdl, data.pSGNode );
		}
	}
}

void AvatarEquipElement::PlayBladeFlare( const bool bPlay )
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		tagPartData& data = m_vecPartData[i];
		if( EPT_BladeFlare == data.eType && NULL != data.pSGNode && data.pSGNode->IS_KIND_OF(SGEffectNode) )
		{
			SGEffectNode* pEff = (SGEffectNode*)data.pSGNode;
			if( bPlay )
				pEff->Play();
			else
				pEff->Close();
		}
	}
}

void AvatarEquipElement::UpdataLongHunEffect( const BYTE byEquipEffect, const bool bAttachToSG )
{
	if( byEquipEffect == m_equipDisplay.byEquipEffect )
		return;

	// 先删除
	for( vector<tagPartData>::iterator itr = m_vecPartData.begin(); itr != m_vecPartData.end(); )
	{
		if( EPT_LongHunEffect == itr->eType )
		{
			SceneNode* pPartFather = (SceneNode*)itr->pSGNode->GetParent();
			if( NULL != pPartFather )
				pPartFather->DetachChild( itr->pSGNode );
			SAFE_DELETE(itr->pSGNode);
			itr = m_vecPartData.erase( itr );
		}
		else
		{
			itr++;
		}
	}
	m_strLongHunEffName = _T("");

	const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay( m_equipDisplay.dwTypeID );
	ASSERT( P_VALID(pInfo) );

	if( _T('\0') == pInfo->szAttachSFX[0] )
		return;

	if( byEquipEffect > EEE_Null && byEquipEffect < EEE_All )
	{
		TCHAR szFileName[X_SHORT_NAME];
		_sntprintf( szFileName, X_SHORT_NAME, _T("%s_%02d"), pInfo->szAttachSFX,byEquipEffect );
		m_strLongHunEffName = szFileName;
		LoadAttachEffect( EPT_LongHunEffect, m_strLongHunEffName, bAttachToSG );
	}
	m_equipDisplay.byEquipEffect = byEquipEffect;
}

void AvatarEquipElement::ChangeTexture( SceneNode* pNode, const int nMtlID, const TCHAR* szNewTexPath )
{
	ASSERT( NULL != pNode && NULL != szNewTexPath );
	IMaterial* pMtl = pNode->GetMtl( nMtlID );
	if( NULL != pMtl && pMtl->IS_KIND_OF(MGeneral) )
	{
		MGeneral* pMGl = (MGeneral*)pMtl;
		ASSERT( pMGl->m_pDiffuse!=NULL );
		pMGl->m_pDiffuse->SetTexture( nMtlID, szNewTexPath );
		pMGl->InvalidateRenderCache();
	}
}

bool AvatarEquipElement::SetColor( const int nColor )
{
	if( EC_Null == nColor || EC_Start == nColor )
		return true;

	ASSERT( nColor > EC_Start && nColor<= EC_End );
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		const tagPartData& data = m_vecPartData[i];
		if( EPT_Main == data.eType )
		{
			// 资源没有加载，返回false
			ASSERT( NULL != data.pSGNode && data.pSGNode->IS_STATIC_CLASS(SGSkinNode) );
			SGSkinNode* pSkin = (SGSkinNode*)data.pSGNode;
			const ResSkin* pRes = pSkin->GetResSkin();
			if( NULL == pRes || !pRes->IsCreated() )
				return false;

			// 材质还没有创建，返回false
			const int nNumMtl = pRes->GetNumMtl();
			if( pSkin->GetNumMtl() != nNumMtl )
				return false;

			// 替换贴图
			for( int nMtl = 0; nMtl < nNumMtl; nMtl++ )
			{
				const tagMtlDesc& mtlDesc = pRes->GetMtlDesc( nMtl );
				Filename FileName = mtlDesc.m_map[EMap_Diffuse];
				tstring::size_type pos = FileName.rfind( _T('_') );
				ASSERT( tstring::npos != pos );
				tstring strExt = FileName.GetExtension();
				FileName.erase( pos );
				tstringstream stream;
				stream<<FileName<<_T('_')<<nColor+1<<_T('.')<<strExt;
				ChangeTexture( pSkin, nMtl, stream.str().c_str() );
			}
		}
	}
	return true;
}

void AvatarEquipElement::SetHairColorTex(BYTE bySex, WORD hairID, WORD colorID)
{
	ASSERT( EAE_Head == m_eElement );
	const tstring strTexPath = AvatarCfg::Inst()->GetHairColorTexPath(bySex, hairID, colorID);
	ASSERT( !strTexPath.empty() );

	SceneNode* pHead = (SceneNode*)GetMainMdl();
	if( NULL == pHead )
		return;

	IMaterial* pMtl = pHead->GetMtl( 0 );
	if( NULL == pMtl )
		return;

	ChangeTexture( pHead, 0, strTexPath.c_str() );
}

void AvatarEquipElement::SetFaceDetailTex(BYTE bySex, WORD id)
{
	ASSERT( EAE_Face == m_eElement );

	const tstring strTexPath = AvatarCfg::Inst()->GetFaceDtlTexPath(bySex, id);
	SceneNode* pFace = (SceneNode*)GetMainMdl();	
	if( NULL == pFace )
		return;

	IMaterial* pMtl = pFace->GetMtl(0);
	if( NULL == pMtl )
		return;

	ASSERT( pMtl->IS_STATIC_CLASS(MSkin) );
	MSkin* pSkin = (MSkin*)pMtl;
	if( strTexPath.empty() )
	{
		if( NULL != pSkin->m_pBlendLayer )
		{
			SAFE_DELETE( pSkin->m_pBlendLayer );
			pSkin->InvalidateRenderCache();
		}
	}
	else
	{
		if( NULL == pSkin->m_pBlendLayer )
			pSkin->m_pBlendLayer = new MBitmap;
		pSkin->m_pBlendLayer->SetTexture(0, strTexPath.c_str(), 0);
		pSkin->InvalidateRenderCache();
	}
}

bool AvatarEquipElement::IsResourceReady() const
{
	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		const tagPartData& data = m_vecPartData[i];
		ASSERT( NULL != data.pSGNode );
		if( data.pSGNode->IS_STATIC_CLASS(SGSkinNode) )
		{
			SGSkinNode* pSkinNode = (SGSkinNode*)data.pSGNode;
			const ResSkin* pResSkin = pSkinNode->GetResSkin();
			if( NULL == pResSkin || !pResSkin->IsCreated() )
				return false;

			const UINT nNumMtls = pSkinNode->GetNumMtl();
			if( nNumMtls != pResSkin->GetNumMtl() )
				return false;

			for( UINT nMtl = 0; nMtl < nNumMtls; nMtl++ )
			{
				IMaterial* pMtl = pSkinNode->GetMtl( nMtl );
				if( NULL == pMtl || !pMtl->IsResourceReady() )
					return false;
			}
		}
		else if( data.pSGNode->IS_KIND_OF(EntityNode) )
		{
			EntityNode* pEntityNode = (EntityNode*)data.pSGNode;
			if( !pEntityNode->IsLoaded() )
				return false;
		}
	}
	return true;
}

void AvatarEquipElement::AttachToSG()
{
	SceneNode* pMainMdl = (SceneNode*)GetMainMdl();
	if( NULL == pMainMdl )
		return;

	for( size_t i = 0; i < m_vecPartData.size(); i++ )
	{
		const tagPartData& data = m_vecPartData[i];
		ASSERT( NULL != data.pSGNode );
		AttachPartNode( pMainMdl, data.pSGNode );
	}
	PlayBladeFlare( false );
}

void AvatarEquipElement::AttachPartNode( SceneNode* pMainNode, SceneNode* pPartNode )
{
	if( NULL == pMainNode || NULL == pPartNode )
		return;

	SceneNode* pPartFather = (SceneNode*)pPartNode->GetParent();
	if( NULL != pPartFather )
		pPartFather->DetachChild( pPartNode );

	if( pPartNode->IS_KIND_OF( SGEffectNode ) )
	{
		if( pMainNode->IS_STATIC_CLASS( SGSkinNode ) )
			m_pFather->AttachChild( pPartNode );
		else
			pMainNode->AttachChild( pPartNode );
	}
	else
	{
		m_pFather->AttachChild( pPartNode );
	}
}