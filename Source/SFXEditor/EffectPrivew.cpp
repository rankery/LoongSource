#include "StdAfx.h"
#include ".\effectprivew.h"
#include "..\Cool3D\SpecialEffect\EffectAll.h"
#include "..\Cool3D\ResSys\ResStaticMesh.h"
#include "..\Cool3D\ResSys\ResKeyFrame.h"

EffectPrivew::EffectPrivew(void)
{
	m_pEftNode=NULL;
	m_pAvatarNode=NULL;
	m_bDrawBox=false;
}

EffectPrivew::~EffectPrivew(void)
{
}

void EffectPrivew::RenderView(bool bAdaptive)
{
	m_camera.UpdateViewMatrix();
	
	IDevice *pDev=Device();
	//--darw axis first
	pDev->MatLoad(EMS_Prj,m_camera.GetPrj());
	pDev->MatLoad(EMS_View,m_camera.GetView());
	pDev->FFlushMatrix(false,true,true,false);
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(10000,0,0),Color4ub(255,255,0,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,10000,0),Color4ub(255,0,255,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,0,10000),Color4ub(255,0,0,255));

	//--draw sg
	m_timer.Update();
	m_pSG->Render(m_timer.GetDelta());
	if( m_bDrawBox && NULL != m_pEftNode )
	{
		AABBox box;
		m_pEftNode->ReBuildLocalBox();
		m_pEftNode->GetBox( box );
		DrawX::DrawAABBox( box );
	}
}

void EffectPrivew::Init(EffectBase *pEft)
{
	if(pEft)
	{
		if(m_pEftNode)
		{
			m_pSG->DetachDynamicNode(m_pEftNode);
			SAFE_DELETE(m_pEftNode);
		}
		m_pEftNode=new SGEffectNode;
		m_pEftNode->AddEffect(pEft);
		m_pSG->AttachDynamicNode(m_pEftNode);
	}
	m_timer.Init();
}

void EffectPrivew::LoadDefaultAvatar(BYTE bySex)
{
	if(m_pAvatarNode!=NULL)
	{
		m_pSG->DetachDynamicNode(m_pAvatarNode);
		SAFE_DELETE(m_pAvatarNode);
	}

	m_pAvatarNode=_LoadDefaultAvatar(bySex);
	m_pSG->AttachDynamicNode(m_pAvatarNode);
}

bool _AddSkin(const TCHAR *szMdlName,AvatarNode *pAvatarNode,CString strEleName)
{
	Cool3D::SGSkinNode *pNewSkin=new SGSkinNode;
	try
	{
		pNewSkin->LoadRes(szMdlName);
	}
	catch(...)
	{
		SAFE_DELETE(pNewSkin);
		return false;
	}
	
	pNewSkin->SetUserData( strEleName );
	pAvatarNode->AttachChild(pNewSkin);
	return true;
}
AvatarNode *_LoadDefaultAvatar(BYTE bySex)
{
	Cool3D::AvatarNode *pSGNode=new AvatarNode;
	if(1==bySex)
	{
		pSGNode->LoadRes(_T("data\\animation\\AvatarGirl\\std1.fskel"));
		_AddSkin( _T("data\\model\\avatargirl\\hair\\Fhair01_1_hair.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Head) );
		_AddSkin( _T("data\\model\\avatargirl\\face\\Fface01_face.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Face) );
		_AddSkin( _T("data\\model\\avatargirl\\upper\\XSY1_upper.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Upper) );
		_AddSkin( _T("data\\model\\avatargirl\\glove\\XSW1_r_glove.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Glove) );
		_AddSkin( _T("data\\model\\avatargirl\\glove\\XSW1_l_glove.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Glove) );
		_AddSkin( _T("data\\model\\avatargirl\\lower\\XSK1_lower.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Lower) );
		_AddSkin( _T("data\\model\\avatargirl\\boot\\XSX1_boot.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Boot) );
	}
	else
	{
		pSGNode->LoadRes(_T("data\\animation\\AvatarBoy\\std1.fskel"));
		_AddSkin( _T("data\\model\\AvatarBoy\\hair\\Mhair01_1_hair.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Head) );
		_AddSkin( _T("data\\model\\AvatarBoy\\face\\Mface01_face.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Face) );
		_AddSkin( _T("data\\model\\AvatarBoy\\upper\\XSY1_upper.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Upper) );
		_AddSkin( _T("data\\model\\AvatarBoy\\glove\\XSW1_r_glove.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Glove) );
		_AddSkin( _T("data\\model\\AvatarBoy\\glove\\XSW1_l_glove.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Glove) );
		_AddSkin( _T("data\\model\\AvatarBoy\\lower\\XSK1_lower.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Lower) );
		_AddSkin( _T("data\\model\\AvatarBoy\\boot\\XSX1_boot.fskin"), pSGNode, _GetAvatarElementName(EEPAE_Boot) );
	}

	return pSGNode;
}


AvatarNode *_LoadNPC(const TCHAR* szPath)
{
	if( NULL == szPath || szPath[0] == _T('\0') )
		return NULL;

	Cool3D::AvatarNode *pSGNode=new AvatarNode;
	pSGNode->LoadRes(szPath);
	Filename strPath = szPath;
	CString szFindPath = strPath.GetPath().c_str();
	szFindPath += _T("*.fskin");
	CFileFind finder;
	BOOL bWorking = finder.FindFile(szFindPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		_AddSkin( finder.GetFilePath(), pSGNode, finder.GetFileName() );
	}

	return pSGNode;
}


void _LoadAvatarActionList(AvatarNode* pNode,CComboBox *pCmb)
{
	pCmb->ResetContent();
	if(pNode==NULL)
		return;
	AnimationCtrl *pAni=pNode->GetSkeletonAniCtrl();

	if(pAni)
	{
		vector<tstring> tracks;
		pAni->GetTrackSet(tracks);
		for(vector<tstring>::iterator iter=tracks.begin();
			iter!=tracks.end();iter++)
		{
			pCmb->AddString(iter->c_str());
		}
		pCmb->SetCurSel(0);
	}

	Filename strPath = pNode->GetResource()->GetName();
	Filename strName = strPath.NoPath();
	CString szCurTrack = strName.NoExtension().c_str();
	CString szFindPath = strPath.GetPath().c_str();
	szFindPath += _T("*.fskel");

	CFileFind finder;
	BOOL bWorking = finder.FindFile(szFindPath);
	int i = 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		CString szActName = finder.GetFileTitle();
		pCmb->AddString(szActName);
		if( szActName == szCurTrack )
			pCmb->SetCurSel( i );
		i++;
	}

}

void _PlayAvatarAction(AvatarNode* pNode,const TCHAR* szActName,bool bLoop)
{
	if(pNode==NULL)
		return;

	pNode->GetSkeletonAniCtrl()->OpenTrack(szActName,bLoop);
}

void EffectPrivew::UpdateActList(CComboBox *pCmb)
{
	_LoadAvatarActionList(m_pAvatarNode,pCmb);
}

void EffectPrivew::PlayAvatarAction(const TCHAR* szActName)
{
	_PlayAvatarAction(m_pAvatarNode,szActName);
}

void EffectPrivew::SetTimeScale(float s)
{
	if(m_pAvatarNode!=NULL)
		m_pAvatarNode->GetSkeletonAniCtrl()->SetTimeFactor(s);
	if(m_pEftNode!=NULL)
		m_pEftNode->SetTimeScale(s);
}

void EffectPrivew::SetShowMdl(bool bShow)
{
	if(bShow)
	{
		if(m_pAvatarNode)
			m_pSG->AttachDynamicNode(m_pAvatarNode);
	}
	else
	{
		if(m_pAvatarNode)
			m_pSG->DetachDynamicNode(m_pAvatarNode);
	}
}

CString _GetAvatarElementName( const int nEle )
{
	ASSERT( nEle >= 0 && nEle < EEPAE_Num );
	static CString strEle[EEPAE_Num] =
	{
		_T("hair"),
		_T("face"),
		_T("upper"),
		_T("lower"),
		_T("glove"),
		_T("boot")
	};
	return strEle[nEle];
}

void _LoadAvatarElementList(CComboBox *pCmb)
{
	pCmb->ResetContent();
	for( int i = 0; i < EEPAE_Num; i++ )
	{
		pCmb->AddString( _GetAvatarElementName(i) );
	}
	pCmb->SetCurSel(0);
}

void _LoadBoneTagNodeList(CComboBox* pCmb)
{
	const int TagNodeNum = 13;
	static CString strBoneTagNodeName[TagNodeNum] =
	{
		_T("tag_Face"),
		_T("tag_Hat"),
		_T("tag_Back"),
		_T("tag_RSld"),
		_T("tag_LSld"),
		_T("tag_RHd"),
		_T("tag_LHd"),
		_T("tag_Wep1"),
		_T("tag_Wep2"),
		_T("tag_Wep3"),
		_T("tag_Wep4"),
		_T("tag_Wep5"),
		_T("tag_Wep6")
	};
	pCmb->ResetContent();
	for( int i = 0; i < TagNodeNum; i++ )
	{
		pCmb->AddString( strBoneTagNodeName[i] );
	}
	pCmb->SetCurSel(0);
}

bool _LoadEquipTagNodeList(SGAvatarAttachment* pNode, CComboBox* pCmb)
{
	ResRenderable* pRes = pNode->GetResObj();
	if( NULL == pRes || !pRes->IsCreated() )
		return false;

	pCmb->ResetContent();
	if( pRes->IS_STATIC_CLASS( ResStaticMesh ) )
	{
		ResStaticMesh* pResStaticMesh = (ResStaticMesh*)pRes;
		const TagNodeFrame& NodeFrame = pResStaticMesh->GetTagNodes();
		for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
		{
			CString strName = _T("tag_");
			strName += _FourCC2Str( NodeFrame.nodes[i].name ).c_str();
			pCmb->AddString( strName );
		}
	}
	else if( pRes->IS_STATIC_CLASS( ResKeyFrame ) )
	{
		ResKeyFrame* pResKeyFrame = (ResKeyFrame*)pRes;
		const vector<TagNodeFrame>& NodeFrameArray = pResKeyFrame->GetTagFrameArray();
		if( NodeFrameArray.empty() )
			return false;

		// 取最后一帧的挂载点
		const TagNodeFrame& NodeFrame = NodeFrameArray[NodeFrameArray.size()-1];
		for( size_t i = 0; i < NodeFrame.nodes.size(); i++ )
		{
			CString strName = _T("tag_");
			strName += _FourCC2Str( NodeFrame.nodes[i].name ).c_str();
			pCmb->AddString( strName );
		}
	}
	pCmb->SetCurSel(0);
	return true;
}

bool _ChangeSkin(AvatarNode* pNode,CString strEleName,CString strFileName)
{
	int numChildren = pNode->GetNumChildren();
	for(int i=0; i<numChildren; i++)
	{
		SceneNode* pChild = const_cast<SceneNode*>( pNode->GetChild(i) );
		if( NULL != pChild && pChild->IS_STATIC_CLASS(SGSkinNode) && pChild->GetUserData() != NULL )
		{
			CString strUserData = pChild->GetUserData();
			if( strEleName == strUserData )
			{
				pNode->DetachChild( pChild );
				SAFE_DELETE(pChild);
				if( !_AddSkin( strFileName, pNode, strEleName ) )
					return false;
				return true;
			}
		}
	}
	return false;
}