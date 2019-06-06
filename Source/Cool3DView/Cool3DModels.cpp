#include "StdAfx.h"
#include ".\Cool3Dmodels.h"
#include "MainFrm.h"
#include "..\Cool3D\public\iplugeditor.h"

#include "Cool3DView.h"
#include "Cool3DViewDoc.h"
#include "Cool3DViewWnd.h"
#include "Cool3DViewWnd.h"
#include "..\Cool3D\Util\FMemory.h"
#include "SkelDlg.h"

#include "..\Cool3D\Image\ImageTGA.h"
#include "..\Cool3D\ResSys\ResStaticMesh.h"
#include "..\Cool3D\ResSys\ResSceneMesh.h"
#include "..\Cool3D\ResSys\ResKeyFrame.h"
#include "..\Cool3D\ResSys\ResSkin.h"
#include "..\Cool3D\RenderMtl\MGeneral.h"
#include "..\Cool3D\RenderMtl\MDyeTex.h"
#include "..\Cool3D\RenderMtl\MSEMTex.h"
#include "..\Cool3D\SpecialEffect\SGAttachableEffectNode.h"
#include "..\Cool3D\Util\FileUtil.h"
#include "..\Cool3D\Math\math.h"

#include "..\Cool3D\Public\StaticMeshFile.h"

#include "ModelSnapshoter.h"

const tstring g_configFile=_T(".\\config\\Cool3DViewCfg.txt");
const float g_cameraNear=50;
const float g_cameraFar=500000;
class Cool3DModels::Member
{
public:
	bool			m_bCreated;
	//两种node, 同时只有一个有效
	EntityNode		*m_pEntity;//static mesh,vert key frame ani
	AvatarNode		*m_pAvatar;//skeleton ani
	CameraEuler		m_camera;
	ArcBall			m_newRot;
	SceneGraph		*m_pSG;
	bool			m_bShowNormal;
	bool			m_bShowTangent;
	bool			m_bShowSkeleton;

	//增加两个node，专用于骑乘预览
	AvatarNode		*m_pHorseman;		//骑乘人物
	EntityNode		*m_pHorse;			//骑乘坐骑
	
	// 用于拍照
	ModelSnapshoter	m_Snapshoter;
	bool            m_bEnableSnapshoter;
	CString         m_strCurMdlPath;
	ConfigFile		m_cfgfile;


	Member()
		: m_bCreated(false)
		, m_pEntity(0)
		, m_pAvatar(0)
		, m_bShowNormal(false)
		, m_bShowSkeleton(true)
		, m_bShowTangent(false)
		, m_pHorseman(0)
		, m_pHorse(0)
		, m_bEnableSnapshoter(false)
		, m_cfgfile(g_configFile.c_str())
	{
		m_pSG=new NullSceneGraph;

#if 0
		SGLightNode *pTestLgt=new SGLightNode;
		tagLight lgt;
		lgt.ambient=Color4f(0,0,0,0);
		lgt.attenuation=Vector3(0,1,0);
		lgt.diffuse=Color4f(0,1,0,0);
		lgt.pos=Vector3(0,100,0);
		lgt.range=5000;
		lgt.type=Light_Point;
		pTestLgt->SetLight(&lgt);
		m_pSG->AttachDynamicNode(pTestLgt);
#endif
	}

	SceneNode* GetActiveNode()
	{
		if(m_pEntity)
			return m_pEntity;
		else if(m_pAvatar)
			return m_pAvatar;
		else if (m_pHorse) 
			return m_pHorse;
		else if (m_pHorseman) 
			return m_pHorseman;
		return NULL;
	}

	SceneNode* GetActiveAvatarNode()
	{
		if (m_pAvatar)
			return m_pAvatar;
		else if (m_pHorseman)
			return m_pHorseman;

		return NULL;
	}

	Vector3 GetActiveNodeCenter()
	{
		Vector3 center(0,0,0);
		SceneNode *pNode=GetActiveNode();
		if(pNode)
		{
			AABBox box;
			pNode->GetBox(box);
			center=(box.max+box.min)*0.5f;
		}
		return center;
	};

	void ResetCamera()
	{
		m_newRot.ClearRotate();

		SceneNode *pNode=GetActiveNode();
		if(pNode)
		{
			Vector3 center=GetActiveNodeCenter();
			m_camera.Set(Vector3(0,12,0),0,0,0,1000);
			m_camera.SetLookAtPos(center);
		}
	}

	SceneNode* FindAvatarChild(const TCHAR* szMdlName)
	{
		if(m_pAvatar==NULL)
			return NULL;

		SceneNode* pRet=NULL;

		for(int i=0;i<m_pAvatar->GetNumChildren();i++)
		{
			SceneNode* pChild=(SceneNode*)m_pAvatar->GetChild(i);

			ResBase* pRes=NULL;
			if(pChild->IS_KIND_OF(SGSkinNode))
			{
				SGSkinNode* pSkin=(SGSkinNode*)pChild;
				pRes=(ResBase*)pSkin->GetResSkin();
			}
			else if(pChild->IS_KIND_OF(EntityNode))
			{
				EntityNode* pEntity=(EntityNode*)pChild;
				pRes=pEntity->GetResObj();
			}

			if(pRes!=NULL)
			{
				Filename fname=pRes->GetName();
				fname=fname.NoPath();
				fname=fname.NoExtension();
				if(_tcsicmp(szMdlName,fname.c_str())==0)
				{
					pRet=pChild;
					break;
				}
			}
		}

		return pRet;
	}

	void DrawSkinNodeNormalTangent( const SGSkinNode* pSkin, const bool bNormal )
	{
		ASSERT( NULL != pSkin );
		ASSERT( NULL != m_pAvatar );
		const ResSkin* pRes = pSkin->GetResSkin();
		if( NULL == pRes || !pRes->IsCreated() )
			return;

		const vector<SkinVert>& vecSkinVert = pRes->GetSkinVerts();
		for(UINT nVert=0;nVert<vecSkinVert.size();nVert++)
		{
			const SkinVert& vert = vecSkinVert[nVert];
			Vector3 pos(0,0,0),pos1(0,0,0);
			if( bNormal )
			{
				pos=vert.pos;
				pos1=pos+vert.normal*40.0f;
				pos=pos*m_pAvatar->GetWorldMat();
				pos1=pos1*m_pAvatar->GetWorldMat();
				DrawX::Draw3DLine(pos,pos1,0xFFFF00FF);
			}
			else
			{
				pos=vert.pos;
				pos1=pos+vert.tangent*40.0f;
				pos=pos*m_pAvatar->GetWorldMat();
				pos1=pos1*m_pAvatar->GetWorldMat();
				DrawX::Draw3DLine(pos,pos1,0xFFFF0000);
			}
			
		}
	}
	void DrawEntityNodeNormalTangent( const EntityNode* pEntity, const bool bNormal )
	{
		ASSERT( NULL != pEntity );

		VertStream* pVS=NULL;
		ResRenderable* pRes=pEntity->GetResObj();
		if(pRes!=NULL && pRes->IsCreated())
		{
			if(pRes->IS_STATIC_CLASS(ResStaticMesh))
			{
				ResStaticMesh* pMesh=(ResStaticMesh*)pRes;
				pVS=pMesh->GetVertStream();
			}
			else if(pRes->IS_STATIC_CLASS(ResSceneMesh))
			{
				ResSceneMesh* pMesh=(ResSceneMesh*)pRes;
				pVS=pMesh->GetVertStream();
			}
			else if(pRes->IS_STATIC_CLASS(ResKeyFrame))
			{
				//todo
				ResKeyFrame* pMesh = (ResKeyFrame*)pRes;
				VertexKeyFrame* pKF = pMesh->GetFrame(0);
				pVS = pKF->m_pStream;
			}
		}

		//--
		if(pVS!=NULL)
		{
			Vector3 pos(0,0,0),pos1(0,0,0),dir(0,0,0);
			for(UINT i=0;i<pVS->GetNumVert();i++)
			{
				if(!pVS->GetPos(i,pos))
					continue;
				if( bNormal )
				{
					if(!pVS->GetNormal(i,dir))
						continue;
					pos1=pos+dir*10.0f;
					pos=pos*pEntity->GetWorldMat();
					pos1=pos1*pEntity->GetWorldMat();
					DrawX::Draw3DLine(pos,pos1,0xFFFF00FF);
				}
				else
				{
					if(!pVS->GetTangent(i,dir))
						continue;
					pos1=pos+dir*40.0f;
					pos=pos*pEntity->GetWorldMat();
					pos1=pos1*pEntity->GetWorldMat();
					DrawX::Draw3DLine(pos,pos1,0xFFFF0000);
				}
			}
		}
	}
	void DrawNormalTangent( const bool bNormal )
	{	
		if( NULL != m_pAvatar )
		{
			const int numChild = m_pAvatar->GetNumChildren();
			for( int i = 0; i < numChild; i++ )
			{
				const SceneNode* pChild = m_pAvatar->GetChild( i );
				if( pChild->IS_STATIC_CLASS(SGSkinNode) )
				{
					DrawSkinNodeNormalTangent( (const SGSkinNode*)pChild, bNormal );
				}
				else if( pChild->IS_KIND_OF(EntityNode) )
				{
					DrawEntityNodeNormalTangent( (const EntityNode*)pChild, bNormal );
				}
			}
		}
		else if( NULL != m_pEntity )
		{
			DrawEntityNodeNormalTangent( m_pEntity, bNormal );
		}
	}
};

Cool3DModels::Cool3DModels(void)
: m_pCDTree(NULL)
{
	m_p=new Member;
}

Cool3DModels::~Cool3DModels(void)
{
	delete m_p;
}

Cool3DModels g_Cool3DMdls;
Cool3DModels* Cool3DModels::Inst()
{
	return &g_Cool3DMdls;
}

SceneGraph* Cool3DModels::GetSceneGraph()
{
	return m_p->m_pSG;
}

#include "..\Cool3D\Terrain\SGRiverPatch.h"

tstring& replace_all(tstring& str,const tstring& old_value,const tstring& new_value)
{  
	while( true )
	{
		tstring::size_type pos(0);
		if( (pos=str.find(old_value)) != tstring::npos )
			str.replace(pos,old_value.length(),new_value);
		else
			break;
	}  
	return str;
}

tstring& replace_all_distinct(tstring& str,const tstring& old_value,const tstring& new_value)
{
	for( tstring::size_type pos(0); pos!=tstring::npos; pos+=new_value.length())
	{
		if( (pos=str.find(old_value,pos)) != tstring::npos )
			str.replace(pos,old_value.length(),new_value);
		else
			break;
	}
	return str;
}

void Cool3DModels::InitEngine(HWND hDrawWnd)
{
	if(m_p->m_bCreated)
		return;

	HDC hDC=::GetDC(hDrawWnd);
	int screenColorBits=GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(hDrawWnd,hDC);

	if( m_p->m_cfgfile.GetBool(_T("GPUSkin")) )
	{
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_NormalMap,Cool3DOption::ENM_Open);
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_HWSkin,Cool3DOption::EHWS_Open);
	}
	else
	{
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_NormalMap,Cool3DOption::ENM_None);
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_HWSkin,Cool3DOption::EHWS_None);
	}
	Cool3DOption::Inst()->SetOption(Cool3DOption::EO_DebugInfo,1);

	int texLevel = m_p->m_cfgfile.GetInt(_T("TexLevel"));
	if( texLevel == 0 )
	{
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TexDetail, ETexDetail_High);
	}
	else if( texLevel == 1 )
	{
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TexDetail, ETexDetail_Mid);
	}
	else
	{
		Cool3DOption::Inst()->SetOption(Cool3DOption::EO_TexDetail, ETexDetail_Low);
	}

	Kernel::Inst()->CreateEngine((const ConfigFile*)NULL);


	//const TCHAR *szNum = Kernel::Inst()->GetWorkPath();
	//TCHAR szNum[100];
	//_stprintf(szNum,_T("%03d"),8000);
	//tstring szChild = _T("D:\\loong_client\\Game\\data\\map\\default\\lightmap\\trn_000.tga");
	//Image childImg;
	//childImg.LoadFromFile( szChild.c_str() );
	//Image blurImg;
	//blurImg.Create(2*256,2*256,EPF_A8R8G8B8);
	//blurImg.Blit(&childImg,0,256);
	//blurImg.WriteToFile( _T("D:\\loong_client\\Game\\data\\map\\default\\lightmap\\test.tga") );

	//--Create Device
	tagDeviceCfg cfg;
	cfg.bWindowed=true;
	cfg.depthFormat=EDF_D24S8;
	cfg.hWnd=hDrawWnd;
	cfg.nWidth=800;
	cfg.nHeight=600;
	if(screenColorBits==32)
		cfg.pixelFormat=EPF_X8R8G8B8;
	else
		cfg.pixelFormat=EPF_R5G6B5;

	//cfg.multiSampleType=EMT_4x;

	Kernel::Inst()->CreateRenderDevice(&cfg);
	DWORD bkColor = 0xFF000000;
	bkColor += m_p->m_cfgfile.GetInt(_T("DefaultBKColor"));
	Device()->SetClearParam(true,true,true,bkColor,1.0f,0);
	ResMgr::Inst()->EnableIOAsync(false);

	m_p->m_camera.Set(Vector3(0,12,0),0,0,0,1000);
	m_p->m_camera.SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight(),3.14f/4,g_cameraNear,g_cameraFar);
	m_p->m_pSG->BindCamera(&m_p->m_camera);

	//--set flag
	m_p->m_bCreated=true;

//#include "EngineTest.h"
	//ImageTGA testBase;
	//testBase.LoadFromFile(Kernel::Inst()->DefaultFS(),
	//	"System\\AvatarBoy\\defaultTex\\SP_PiFu_01.tga");

	//ImageTGA test;
	//test.LoadFromFile(Kernel::Inst()->DefaultFS(),
	//	"System\\AvatarBoy\\defaultTex\\SP_YiFu.tga");

	//testBase.AlphaBlend(&test);

	//IDeviceTex *pTex=Device()->NewTexture();
	//pTex->CreateFromData(testBase.GetWidth(),testBase.GetHeight(),
	//	EPF_A8R8G8B8,testBase.GetData(),1);
	//pTex->WriteToFile("d:\\test.tga");

	//SoundMgr::Inst()->Init();
	//SoundMgr::Inst()->PlayMP3("d:\\jules.mp3",true);
}

void Cool3DModels::Destroy()
{
	m_p->m_Snapshoter.Destroy();
	delete m_p->m_pSG;
	m_p->m_pSG=NULL;
	Kernel::Inst()->FinalDestroy();
}

void Cool3DModels::SetBkColor(DWORD color)
{
	Device()->SetClearParam(true,true,true,color,1.0f,0);
}

float Cool3DModels::GetBunsHeight()
{
	float height = 0.0f;

	if(m_p->m_pHorseman!=NULL)
	{
		ASSERT( m_p->m_pHorseman->IS_KIND_OF( AvatarNode ) );

		DWORD bunsBoneIndex=-1;
		ResSkeleton *pSkel = const_cast<ResSkeleton*>(((AvatarNode*)m_p->m_pHorseman)->GetResource());
		if(pSkel!=NULL && pSkel->IsCreated())
		{
			// 不同的性别取不同的骨骼ID
			bunsBoneIndex = pSkel->GetBoneID(_T("Bip01 Pelvis"));

			if (bunsBoneIndex == -1)
				bunsBoneIndex = pSkel->GetBoneID(_T("Bip01 Pelvis01"));
		}
		
		if (bunsBoneIndex != -1)
		{
			NodeAnimationCtrl *pAniCtrl = ((AvatarNode*)m_p->m_pHorseman)->GetSkeletonAniCtrl();
			if(pAniCtrl != NULL)
			{
				float y = pAniCtrl->GetBoneSpace(bunsBoneIndex)->_42;
				if(y>0)
				{
					height=y-10.0f;
				}
			}
		}

	}//end if

	return height;
}

void Cool3DModels::BindToHorse()
{
	if ( !m_p->m_pHorseman || !m_p->m_pHorse )
		return;

	float grade=0;

	//--找出挂载点旋转与位移变换矩阵
	bool bOK = false;
	Matrix4 matTag;
	AnimationCtrl* pAniCtrl = m_p->m_pHorse->GetAniCtrl();
	if(pAniCtrl && pAniCtrl->IS_STATIC_CLASS(NodeAnimationCtrl))
	{
		NodeAnimationCtrl *pNodeAniCtrl = (NodeAnimationCtrl*)pAniCtrl;
		matTag = *(pNodeAniCtrl->GetBoneSpace(_T("tag_Ride")));
		matTag._11 = 1.0f;	matTag._12 = 0.0f;	matTag._13 = 0.0f;
		matTag._21 = 0.0f;	matTag._22 = 1.0f;	matTag._23 = 0.0f;
		matTag._31 = 0.0f;	matTag._32 = 0.0f;	matTag._33 = 1.0f;
		bOK=true;
	}

	if(!bOK)
		return;

	//--设置SceneNode世界矩阵
	{
		Transform trans(Transform::ECM_ToRight);
		//旋转
		trans.LoadMatrix(&matTag);
		//位移
		trans.Translate(Vector3(0,-GetBunsHeight(),0));

		m_p->m_pHorseman->SetWorldMat(trans.GetMatrix());
	}
}

void Cool3DModels::LoadHorsemanModel(const TCHAR* szPath)
{
	ASSERT(szPath!=NULL);
	Filename szFileName(szPath);
	tstring ext=szFileName.GetExtension();

	if(m_p->m_pEntity)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pEntity);
		delete m_p->m_pEntity;
		m_p->m_pEntity=NULL;
	}
	if(m_p->m_pAvatar)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pAvatar);
		delete m_p->m_pAvatar;
		m_p->m_pAvatar=NULL;
	}

	//删除骑乘人物
	if (m_p->m_pHorseman) 
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pHorseman);
		delete m_p->m_pHorseman;
		m_p->m_pHorseman=NULL;
	}

	if(_tcsicmp(ext.c_str(),_T("fskel"))==0)
	{
		m_p->m_pHorseman=new AvatarNode;
		m_p->m_pHorseman->LoadRes(szPath);
		m_p->m_pSG->AttachDynamicNode(m_p->m_pHorseman);
		_ShowNote(_T("Skeleton"));
	}
}


void Cool3DModels::LoadHorseModel(const TCHAR* szPath)
{
	ASSERT(szPath!=NULL);
	Filename szFileName(szPath);
	tstring ext=szFileName.GetExtension();

	if(m_p->m_pEntity)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pEntity);
		delete m_p->m_pEntity;
		m_p->m_pEntity=NULL;
	}
	if(m_p->m_pAvatar)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pAvatar);
		delete m_p->m_pAvatar;
		m_p->m_pAvatar=NULL;
	}

	//删除骑乘坐骑
	if (m_p->m_pHorse) 
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pHorse);
		delete m_p->m_pHorse;
		m_p->m_pHorse=NULL;
	}

	if(_tcsicmp(ext.c_str(),_T("fskel"))==0)
	{
		m_p->m_pHorse=new EntityNode;
		m_p->m_pHorse->LoadRes(szPath);
		m_p->m_pSG->AttachDynamicNode(m_p->m_pHorse);
		RTTIObj *pObj=m_p->m_pHorse->GetResObj();
		_ShowNote(pObj->GetClassName());

		TCHAR szFind[512];
		vector<tstring> skins;
		_stprintf(szFind,_T("%s\\*.fskin"),szFileName.GetPath().c_str());
		SearchFiles(szFind,skins);
		for( UINT i=0; i<skins.size(); ++i )
		{
			SGSkinNode *pSkin=new SGSkinNode;
			pSkin->SetUserData(skins[i].c_str());
			pSkin->LoadRes(skins[i].c_str());
			m_p->m_pHorse->AttachChild(pSkin);
		}
	}

	CString szCSVPath = szPath;
	int result  = szCSVPath.Find(_T(".fskel"));
	if(result != -1)
		szCSVPath.Replace(_T(".fskel"), _T(".csv"));
	result = szCSVPath.Find(_T(".FSKEL"));
	if(result != -1)
		szCSVPath.Replace(_T(".FSKEL"), _T(".csv"));
	result = szCSVPath.Find(_T(".FAK"));
	if(result != -1)
		szCSVPath.Replace(_T(".FAK"), _T(".csv"));
	result = szCSVPath.Find(_T(".fak"));
	if(result != -1)
		szCSVPath.Replace(_T(".fak"), _T(".csv"));

	if(szCSVPath.Find(_T(".csv")) != -1)
	{
		if(g_viewWnd->GetDocument()->GetCSVFilePath() != szCSVPath)
		{
			g_viewWnd->GetDocument()->m_aniMsgTable.Clear();
			g_viewWnd->m_actMsgDlg.ClearAll();
		}
		g_viewWnd->GetDocument()->m_aniMsgTable.LoadFromFile(Kernel::Inst()->DefaultFS(), szCSVPath);
		g_viewWnd->GetDocument()->SetCSVFilePath(szCSVPath);
	}
}

void Cool3DModels::LoadModel(const TCHAR* szPath)
{
	ASSERT(szPath!=NULL);
	Filename szFileName(szPath);
	tstring ext=szFileName.GetExtension();
	
	if(_tcsicmp(ext.c_str(),_T("fskin"))==0)
	{
		if(m_p->m_pAvatar==NULL && m_p->m_pHorseman==NULL)
		{
			AfxMessageBox(_T("在加载Skin之前,必须先加载一个骨骼(*.fskel)"));
		}
		else
		{
			SGSkinNode *pNewSkin=new SGSkinNode;
			pNewSkin->SetUserData(szPath);//使用Path作为标识
			pNewSkin->LoadRes(szPath);
			if (m_p->m_pAvatar)
				m_p->m_pAvatar->AttachChild(pNewSkin);
			else
				m_p->m_pHorseman->AttachChild(pNewSkin);
		}
		return;
	}

	if(_tcsicmp(ext.c_str(),_T("fcloth"))==0)
	{
		if(m_p->m_pAvatar==NULL && m_p->m_pHorseman==NULL)
		{
			AfxMessageBox(_T("在加载Cloth之前,必须先加载一个骨骼(*.fskel)"));
		}
		else
		{
			SGClothNode *pNewCloth=new SGClothNode;
			pNewCloth->SetUserData(szPath);//使用Path作为标识
			pNewCloth->LoadRes(szPath);
			if (m_p->m_pAvatar)
				m_p->m_pAvatar->AttachChild(pNewCloth);
			else
				m_p->m_pHorseman->AttachChild(pNewCloth);
		}
		return;
	}

	
	//删除所有现有的东西
	if(m_p->m_pEntity)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pEntity);
		delete m_p->m_pEntity;
		m_p->m_pEntity=NULL;
	}
	if(m_p->m_pAvatar)
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pAvatar);
		delete m_p->m_pAvatar;
		m_p->m_pAvatar=NULL;
	}

	//删除骑乘人物和骑乘坐骑
	if (m_p->m_pHorseman) 
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pHorseman);
		delete m_p->m_pHorseman;
		m_p->m_pHorseman=NULL;
	}
	if (m_p->m_pHorse) 
	{
		m_p->m_pSG->DetachDynamicNode(m_p->m_pHorse);
		delete m_p->m_pHorse;
		m_p->m_pHorse=NULL;
	}
	
	if(_tcsicmp(ext.c_str(),_T("fskel"))==0)
	{
		m_p->m_pAvatar=new AvatarNode;
		m_p->m_pAvatar->LoadRes(szPath);

		m_p->m_pSG->AttachDynamicNode(m_p->m_pAvatar);
		_ShowNote(_T("Skeleton"));

		m_p->m_strCurMdlPath = szPath;
	}
	else
	{
		m_p->m_pEntity=new EntityNode;
		m_p->m_pEntity->LoadRes(szPath);

		ResRenderable *pRes=m_p->m_pEntity->GetResObj();

		m_p->m_pSG->AttachDynamicNode(m_p->m_pEntity);
		RTTIObj *pObj=m_p->m_pEntity->GetResObj();
		_ShowNote(pObj->GetClassName());

		m_p->m_strCurMdlPath = szPath;

	/*	m_p->m_pEntity->Update(1.0,m_p->m_pSG);
		if(m_p->m_pEntity->GetNumMtl()>0)
		{
			IMaterial *pMtl=m_p->m_pEntity->GetMtl(0);
			MMColorWriteControl *pCC=new MMColorWriteControl;
			pCC->SetName("ColorMask");
			pCC->m_dwColorMask=COLORWRITEENABLE_RED|COLORWRITEENABLE_GREEN|COLORWRITEENABLE_ALPHA;
			pMtl->AttachModifier(pCC);
			pMtl->InvalidateRenderCache();
		}*/
	}

	m_p->ResetCamera();

	//ResMgr::Inst()->DoGC();
}

ArcBall* Cool3DModels::GetArcBall()
{
	return &m_p->m_newRot;
}

CameraEuler* Cool3DModels::GetCamera()
{
	return &m_p->m_camera;
}

bool GetBonePos(const TCHAR* szBoneName,SceneNode* pSGNode,Vector3& out)
{
	bool bOK=false;
	int boneIndex = -1;

	if(pSGNode!=NULL)
	{
		if( pSGNode->IS_KIND_OF( AvatarNode ) )
		{
			if(boneIndex == -1)
			{
				ResSkeleton *pSkel = const_cast<ResSkeleton*>(((AvatarNode*)pSGNode)->GetResource());
				if(pSkel!=NULL
					&& pSkel->IsCreated())
				{
					boneIndex = pSkel->GetBoneID(szBoneName);
				}
			}

			if(boneIndex != -1)
			{
				NodeAnimationCtrl *pAniCtrl = ((AvatarNode*)pSGNode)->GetSkeletonAniCtrl();
				if(pAniCtrl != NULL)
				{
					Transform tn;
					tn.LoadMatrix(pAniCtrl->GetBoneSpace(boneIndex));
					tn.CombineTransRight(&pSGNode->GetWorldMat());
					out=Vector3(0,0,0);
					tn.TransVec3(out);
					bOK=true;
				}
			}
		}	
	}

	return bOK;
}

void Cool3DModels::Render()
{
	//--更新引擎时钟
	Kernel::Inst()->Tick();

	//--渲染当前的模型
	if(!m_p->m_bCreated)
		return;

	SceneNode *pNode=m_p->GetActiveNode();
	if(pNode==NULL)
		return;

	IDevice *pDev=Device();
	pDev->Clear();
	pDev->Begin();

	Vector3 center=m_p->GetActiveNodeCenter();
	Matrix4 trans,transInv;
	D3DXMatrixTranslation(&trans,center.x,center.y,center.z);
	D3DXMatrixInverse(&transInv,NULL,&trans);
	Matrix4 rotate;
	m_p->m_newRot.GetRotateMat(&rotate);
	//rotate=transInv*rotate*trans;
	pNode->SetWorldMat(&rotate);
	
	if (m_p->m_pHorseman && m_p->m_pHorse)
		BindToHorse();

	m_p->m_camera.UpdateViewMatrix();		
	m_p->m_pSG->Render(Kernel::Inst()->GetDeltaTime());


	//--draw axis
	pDev->MatLoad(EMS_Prj,m_p->m_camera.GetPrj());
	pDev->MatLoad(EMS_View,m_p->m_camera.GetView());
	pDev->FFlushMatrix(false,true,true,false);
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(10000,0,0),Color4ub(255,255,0,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,10000,0),Color4ub(255,0,255,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,0,10000),Color4ub(255,0,0,255));

#if 0
	//draw skeleton if necessary
	if(pNode->IS_STATIC_CLASS(AvatarNode))
	{
		AvatarNode *pAvatar=(AvatarNode *)pNode;

		SkeletonAniCtrl *pAni=pAvatar->GetSkeletonAniCtrl();
		if(pAni)
			pAni->DrawSkeleton();
	}
#endif

	if (m_p->m_pAvatar) 
	{
		pDev->MatPush(EMS_World);

		pDev->MatLoad(EMS_World,&m_p->m_pAvatar->GetWorldMat());
		pDev->FFlushMatrix(true,false,false,false);

		AvatarNode *pAvatar=(AvatarNode *)m_p->m_pAvatar;

		NodeAnimationCtrl *pAni=pAvatar->GetSkeletonAniCtrl();
		if(pAni && m_p->m_bShowSkeleton)
			pAni->DrawSkeleton();

		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,false,false,false);
	}
	else if (m_p->m_pHorseman) 
	{
		pDev->MatPush(EMS_World);

		pDev->MatLoad(EMS_World,&m_p->m_pHorseman->GetWorldMat());
		pDev->FFlushMatrix(true,false,false,false);

		AvatarNode *pAvatar=(AvatarNode *)m_p->m_pHorseman;

		NodeAnimationCtrl *pAni=pAvatar->GetSkeletonAniCtrl();
		if(pAni && m_p->m_bShowSkeleton)
			pAni->DrawSkeleton();

		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,false,false,false);
	}

	//--draw scene mesh octree box
	if(m_p->m_pEntity!=NULL)
	{
		ResRenderable* pRes=m_p->m_pEntity->GetResObj();
		if( pRes!=NULL && pRes->IsCreated()
			&& m_p->m_bShowSkeleton )
		{
			if( pRes->IS_STATIC_CLASS(ResSceneMesh) )
			{
				ResSceneMesh* pMesh=static_cast<ResSceneMesh*>(pRes);
				pMesh->DrawOctree();
			}
			else if( pRes->IS_STATIC_CLASS(ResStaticMesh) )
			{
				ResStaticMesh* pMesh=static_cast<ResStaticMesh*>(pRes);
				pMesh->DrawOctree();
			}
			else if( pRes->IS_STATIC_CLASS(ResKeyFrame) )
			{
				ResKeyFrame *pMesh=static_cast<ResKeyFrame*>(pRes);
				pMesh->DrawOctree();
			}

			AABBox boxEntity;
			m_p->m_pEntity->GetBox( boxEntity );
			DrawX::DrawAABBox( boxEntity, Color4ub(255,255,0,0) );
		}
	}

	//--draw tag nodes
#if 0
	AniCtrl *pAni=GetSceneNodeAni();
	if(pAni)
	{
		AABBox tagBox;
		float size=8;
		tagBox.max=Vector3(size,size,size);
		tagBox.min=Vector3(-size,-size,-size);
		DWORD boxColor=Color4ub(255,5,5,250);
		const vector<tagTagNodeSpace>& tagNodes=pAni->GetTagNodes();
		pDev->MatPush(EMS_World);
		for(vector<tagTagNodeSpace>::const_iterator iter=tagNodes.begin();
			iter!=tagNodes.end();++iter)
		{
			//--
			DWORD dwName=iter->name;
			tstring szName=_FourCC2Str(dwName);
			//--
			pDev->MatLoad(EMS_World,iter->trans.GetMatrix());
			pDev->FFlushMatrix(true,false,false,false);
			DrawX::DrawAABBox(tagBox,boxColor);
		}
		pDev->MatPop(EMS_World);
		pDev->FFlushMatrix(true,false,false,false);
	}
#endif

#if 0
	//--draw aabbtree of scene mesh
	if(m_p->m_pEntity)
	{
		ResBase *pRes=m_p->m_pEntity->GetResObj();
		if(pRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			((ResSceneMesh*)pRes)->DrawAABBTree();
		}
	}
#endif

#if 0
	static Vector3 pos(0,80,-9000);
	/*pos.y=sinf(Kernel::Inst()->GetAccumTime())*500;
	Transform tm;
	tm.Translate(pos);
	tm.Rotate(Kernel::Inst()->GetAccumTime(),Kernel::Inst()->GetAccumTime(),Kernel::Inst()->GetAccumTime());
	m_p->m_pEntity->SetWorldMat(tm.GetMatrix());*/

	Ray testRay;
	testRay.dir=Vector3(0,0,1);//向下
	
	pos.x=sinf(Kernel::Inst()->GetAccumTime())*500;
	//pos.y=cosf(Kernel::Inst()->GetAccumTime())*100;

	testRay.origin=pos;

	Vector3 tmp=pos+testRay.dir*1000;
	DrawX::Draw3DLine(pos,tmp,Color4ub(255,255,255,0));

	TResult tr=m_p->m_pEntity->RayCollide(testRay);
	if(tr.first)
	{
		Vector3 c=tr.second*testRay.dir+testRay.origin;
		Vector3 s(10,10,10);
		AABBox box;
		box.max=c+s;
		box.min=c-s;
		DrawX::DrawAABBox(box,Color4ub(255,255,255,250));
		
	}
#endif

	if(m_p->m_bShowNormal)
		m_p->DrawNormalTangent(true);

	if(m_p->m_bShowTangent)
		m_p->DrawNormalTangent(false);

	if( m_p->m_bEnableSnapshoter )
		m_p->m_Snapshoter.Render( m_p->m_pSG, pNode );


	pDev->End();
	pDev->Present();

	//--更新界面显示
	int selIndex=g_viewWnd->m_trackList.GetCurSel();
	if(selIndex==-1)
		return;
	CString szSel;
	g_viewWnd->m_trackList.GetText(selIndex,szSel);
	AnimationCtrl *pAniCtrl=GetSceneNodeAni();
	if(pAniCtrl!=NULL)
	{
		CString szInfo;
		szInfo.Format(_T("%.4f"),pAniCtrl->GetTrackPosition(szSel));
		g_viewWnd->m_hPlayTime.SetWindowText(szInfo);
	}

	//Sleep(2);
}

SceneNode* Cool3DModels::GetSceneNode()
{
	return m_p->GetActiveNode();
}

SceneNode* Cool3DModels::GetAvatarNode()
{
	return m_p->GetActiveAvatarNode();
}

AnimationCtrl* Cool3DModels::GetSceneNodeAni()
{
	SceneNode *pNode=m_p->GetActiveNode();
	if(pNode==NULL)
		return NULL;

	AnimationCtrl *pAni=NULL;
	if(pNode->IS_STATIC_CLASS(EntityNode))
	{
		EntityNode *pEntity=(EntityNode *)pNode;
		pAni=pEntity->GetAniCtrl();
	}
	else if(pNode->IS_STATIC_CLASS(AvatarNode))
	{
		AvatarNode *pAvatar=(AvatarNode *)pNode;
		pAni=pAvatar->GetSkeletonAniCtrl();
	}
	return pAni;
}

CString	Cool3DModels::GetSceneNodeResName()
{
	SceneNode *pNode=m_p->GetActiveNode();
	if(pNode==NULL)
		return _T("");

	if(pNode->IS_STATIC_CLASS(EntityNode))
	{
		EntityNode *pEntity=(EntityNode *)pNode;
		return pEntity->GetResObj()->GetName();
	}
	else if(pNode->IS_STATIC_CLASS(AvatarNode))
	{
		AvatarNode *pAvatar=(AvatarNode *)pNode;
		return pAvatar->GetResource()->GetName();
	}
	return _T("");
}

void Cool3DModels::SetCameraFov(float rad)
{
	m_p->m_camera.SetPerspective((float)Device()->GetClientWidth(),(float)Device()->GetClientHeight(),
		rad,g_cameraNear,g_cameraFar);
}

void Cool3DModels::ShowSkelDlg()
{
	if(m_p->m_pAvatar)
	{
		CSkelDlg dlg;
		dlg.m_pRes=const_cast<ResSkeleton*>(m_p->m_pAvatar->GetResource());
		dlg.DoModal();
	}
}

void Cool3DModels::AttachMdl(const TCHAR* szMdlPath,const TCHAR* szTagNodeName)
{
	if(m_p->m_pAvatar==NULL)
		return;

	Cool3D::SGAvatarAttachment *pAttach=new SGAvatarAttachment;
	pAttach->LoadRes(szMdlPath);
	pAttach->BindToTagNode(szTagNodeName);

	m_p->m_pAvatar->AttachChild(pAttach);

	//SGBladeFlare *pFlare=NULL;
	//pFlare=new SGBladeFlare;
	//pAttach->AttachChild(pFlare);
	//pFlare->Init(2);
	//pFlare->Enable(true);
	//pFlare->SetColor(Color4ub(255,55,97,2));
}

void Cool3DModels::UpdateUpperSkeAni(const TCHAR* szTrackName)
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true,0.0f,0.0f,BPT_Upper);
}

void Cool3DModels::UpdateLowerSkeAni(const TCHAR* szTrackName)
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true,0.0f,0.0f,BPT_Lower);
}

void Cool3DModels::UpdateAllSkeAni(const TCHAR* szTrackName)
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true);
	//pAni->OpenTrack(szTrackName,false,-1.0f);
}

void Cool3DModels::BlendUpperSkeAni( const TCHAR *szTrackName,float weight )
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true,0.0f,0.0f,BPT_Upper,true,weight);
}

void Cool3DModels::BlendLowerSkeAni( const TCHAR *szTrackName,float weight )
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true,0.0f,0.0f,BPT_Lower,true,weight);
}

void Cool3DModels::BlendAllSkeAni( const TCHAR *szTrackName,float weight )
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->OpenTrack(szTrackName,true,0.0f,0.0f,BPT_All,true,weight);
}

void Cool3DModels::RotateUpperSkeAni(float yaw, float pitch, float roll)
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->RotateUpper(yaw, pitch, roll);
}

void Cool3DModels::RotateHeadSkeAni(float yaw, float pitch, float roll)
{
	if (m_p->m_pAvatar==NULL)
	{
		return;
	}

	NodeAnimationCtrl *pAni=static_cast<NodeAnimationCtrl*>(m_p->m_pAvatar->GetSkeletonAniCtrl());
	if(pAni==NULL)
		return;
	pAni->RotateHead(yaw, pitch, roll);
}

bool Cool3DModels::LoadSkin(SkinType st, unsigned char serialIndex)
{
	SceneNode *pAvatar = NULL;
	if( m_p->m_pAvatar != NULL )
		pAvatar = m_p->m_pAvatar;
	else if( m_p->m_pHorseman != NULL )
		pAvatar = m_p->m_pHorseman;
	else
		return false;

	//删除已有Skin
	int nodeNum=pAvatar->GetNumChildren();
	for(int i=0; i<nodeNum; i++)
	{
		SceneNode *pChild=const_cast<SceneNode*>(pAvatar->GetChild(i));
		pAvatar->DetachChild(pChild);
	}

	//载入配置文件中的皮肤
	tstring szBootSkin;
	tstring szHairSkin;
	tstring szHeadSkin;
	tstring szLowerSkin;
	tstring szUpperSkin;
	tstring szLGloveSkin;
	tstring szRGloveSkin;
	tstring szHatSkin;

	switch (st)
	{
	case ST_BOY:
		szBootSkin		= m_p->m_cfgfile.GetString(_T("BoyBootSkinDir0"));
		szHairSkin		= m_p->m_cfgfile.GetString(_T("BoyHairSkinDir0"));
		szHeadSkin		= m_p->m_cfgfile.GetString(_T("BoyHeadSkinDir0"));
		szLowerSkin		= m_p->m_cfgfile.GetString(_T("BoyLowerSkinDir0"));
		szUpperSkin		= m_p->m_cfgfile.GetString(_T("BoyUpperSkinDir0"));
		szLGloveSkin	= m_p->m_cfgfile.GetString(_T("BoyLGloveSkinDir0"));
		szRGloveSkin	= m_p->m_cfgfile.GetString(_T("BoyRGloveSkinDir0"));
		szHatSkin		= m_p->m_cfgfile.GetString(_T("BoyHatSkinDir0"));
		break;
	case ST_GIRL:
		szBootSkin		= m_p->m_cfgfile.GetString(_T("GirlBootSkinDir0"));
		szHairSkin		= m_p->m_cfgfile.GetString(_T("GirlHairSkinDir0"));
		szHeadSkin		= m_p->m_cfgfile.GetString(_T("GirlHeadSkinDir0"));
		szLowerSkin		= m_p->m_cfgfile.GetString(_T("GirlLowerSkinDir0"));
		szUpperSkin		= m_p->m_cfgfile.GetString(_T("GirlUpperSkinDir0"));
		szLGloveSkin	= m_p->m_cfgfile.GetString(_T("GirlLGloveSkinDir0"));
		szRGloveSkin	= m_p->m_cfgfile.GetString(_T("GirlRGloveSkinDir0"));
		szHatSkin		= m_p->m_cfgfile.GetString(_T("GirlHatSkinDir0"));
		break;
	}

	if( !szBootSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szBootSkin.c_str());
		pSkin->LoadRes(szBootSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szHairSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szHairSkin.c_str());
		pSkin->LoadRes(szHairSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szHeadSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szHeadSkin.c_str());
		pSkin->LoadRes(szHeadSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szLowerSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szLowerSkin.c_str());
		pSkin->LoadRes(szLowerSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szUpperSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szUpperSkin.c_str());
		pSkin->LoadRes(szUpperSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szLGloveSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szLGloveSkin.c_str());
		pSkin->LoadRes(szLGloveSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szRGloveSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szRGloveSkin.c_str());
		pSkin->LoadRes(szRGloveSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	if( !szHatSkin.empty() )
	{
		SGSkinNode *pSkin=new SGSkinNode;
		pSkin->SetUserData(szHatSkin.c_str());
		pSkin->LoadRes(szHatSkin.c_str());
		pAvatar->AttachChild(pSkin);
	}

	return true;
}

void Cool3DModels::ChangeSEMMtl(const TCHAR* szEnvMap)
{
	SceneNode* pNode=NULL;

	if (m_p->m_pEntity)
	{
		pNode=m_p->m_pEntity;
	}
	else
	{
		for (int i=0;i<m_p->m_pAvatar->GetNumChildren();i++)
		{
			pNode=m_p->m_pAvatar->GetChildVar(i);
			if (pNode->IS_KIND_OF(EntityNode))
				break;
		}
	}

	if (!pNode)
		return;
	if(!pNode->IS_STATIC_CLASS(SGSkinNode) && !pNode->IS_KIND_OF(EntityNode))
		return;

	for(UINT i=0;i<pNode->GetNumMtl();i++)
	{
		IMaterial* pMtl=pNode->GetMtl(i);
		if(pMtl->IS_KIND_OF(MGeneral))
		{
			MGeneral* pGM=(MGeneral*)pMtl;
			if (!pGM->m_bEnableReflection)
				continue;

			if(pGM->m_pDiffuse==NULL || pGM->m_pDiffuse->m_pBitmap==NULL)
				continue;

			MSEMTex* pNewMtl=new MSEMTex;
			pNewMtl->SetEnvMap(szEnvMap);

			if(pGM->m_pDiffuse!=NULL)
			{
				pNewMtl->m_pBaseMap=pGM->m_pDiffuse;
				pGM->m_pDiffuse=NULL;
			}

			if(pNode->IS_STATIC_CLASS(SGSkinNode))
				((SGSkinNode*)pNode)->ReplaceMtl(i,pNewMtl);
			else if(pNode->IS_KIND_OF(EntityNode))
				((EntityNode*)pNode)->ReplaceMtl(i,pNewMtl);

		}
		else if(pMtl->IS_STATIC_CLASS(MSEMTex))
		{
			MSEMTex* pSEM=(MSEMTex*)pMtl;
			pSEM->InvalidateRenderCache();
		}
	}
}

void Cool3DModels::ShowNormal()
{
	m_p->m_bShowNormal=!m_p->m_bShowNormal;
}

void Cool3DModels::ShowTangent()
{
	m_p->m_bShowTangent=!m_p->m_bShowTangent;
}

void Cool3DModels::SetShowSkel( bool bShow )
{
	m_p->m_bShowSkeleton=bShow;
}

void Cool3DModels::EnableSnapshoter( bool bEnable )
{
	m_p->m_bEnableSnapshoter = bEnable;
}

bool Cool3DModels::SavePhoto( const TCHAR* szFilename )
{
	return m_p->m_Snapshoter.SaveToFile( szFilename );
}

CString Cool3DModels::GetPhotoFileExt()
{
	return m_p->m_Snapshoter.GetFileNameExt();
}

CString Cool3DModels::GetCurMdlPath()
{
	return m_p->m_strCurMdlPath;
}

void Cool3DModels::ModifyLightmapSize( int size )
{
	if( m_p->m_pEntity!=NULL )
	{
		ResRenderable *pRes=m_p->m_pEntity->GetResObj();
		ASSERT( pRes!=NULL );

		if( pRes->IS_STATIC_CLASS(ResStaticMesh) )
		{
			ResStaticMesh *pResStatic=(ResStaticMesh*)pRes;
			if( !pResStatic->SetLightMapSize(size) )
			{
				AfxMessageBox(_T("修改Lightmap的大小失败！"));
			}
		}
		else if( pRes->IS_STATIC_CLASS(ResSceneMesh) )
		{
			ResSceneMesh *pResScene=(ResSceneMesh*)pRes;
			if( !pResScene->SetLightMapSize(size) )
			{
				AfxMessageBox(_T("修改Lightmap的大小失败！"));
			}
		}
		else if( pRes->IS_STATIC_CLASS(ResKeyFrame) )
		{
			ResKeyFrame *pResKF=(ResKeyFrame*)pRes;
			if( !pResKF->SetLightMapSize(size) )
			{
				AfxMessageBox(_T("修改Lightmap的大小失败！"));
			}
		}
	}
}

int Cool3DModels::GetLightmapSize() const
{
	int lightmapSize=0;

	if( m_p->m_pEntity!=NULL )
	{
		ResRenderable *pRes=m_p->m_pEntity->GetResObj();
		ASSERT( pRes!=NULL );

		if( pRes->IS_STATIC_CLASS(ResStaticMesh) )
		{
			ResStaticMesh *pResStatic=(ResStaticMesh*)pRes;
			lightmapSize=pResStatic->GetLightMapSize();
		}
		else if( pRes->IS_STATIC_CLASS(ResSceneMesh) )
		{
			ResSceneMesh *pResScene=(ResSceneMesh*)pRes;
			lightmapSize=pResScene->GetLightMapSize();
		}
		else if( pRes->IS_STATIC_CLASS(ResKeyFrame) )
		{
			ResKeyFrame *pResKF=(ResKeyFrame*)pRes;
			lightmapSize=pResKF->GetLightMapSize();
		}
	}

	return lightmapSize;
}

void Cool3DModels::SplitSkel()
{
	TCHAR szDir[256];
	TCHAR szPath[256];
	TCHAR szName[256];
	TCHAR szExt[128];
	TCHAR szFile[512];
	_tsplitpath( m_p->m_strCurMdlPath, szDir, szPath, szName, szExt );

	if( _tcsicmp(szExt,_T(".fskel")) == 0 )
	{
		ResSkeleton *pRes = m_p->m_pAvatar->GetResource();

		FILE *fp = NULL;
		FSkelHeader header;
		wcscpy(header.magic,FSKEL_MAGIC);
		header.ver = 1;

		UINT totalFrameNum = pRes->GetNumFrame();
		SkeletonKeyFrame *pFrame = pRes->GetFirstFrame();
		DWORD curName = pFrame->m_name;
		DWORD curFrameNum = 0;
		for( UINT i=0; i<totalFrameNum; i++ )
		{
			pFrame = pRes->GetFrame(i);
			if( pFrame->m_name == curName )
			{
				if( curFrameNum == 0 )//相同动作
				{
					_stprintf( szFile, _T("%s%s%s.FSKEL"), szDir, szPath, _FourCC2Str(curName).c_str() );
					fp =_tfopen(szFile,_T("wb"));
					if( fp == NULL )
						continue;

					FWriteValue(fp,header);

					header.structOffset=ftell(fp);
					pRes->GetStruct().WriteFile(fp);

					header.frameOffset=ftell(fp);
				}

				// 写入文件
				FWriteValue(fp,pFrame->m_name);
				UINT numBone=pFrame->GetNumBone();
				FWriteValue(fp,numBone);
				for(UINT bonei=0;bonei<numBone;bonei++)
					pFrame->m_boneArray[bonei].WriteFile(fp);
				FWriteValue(fp,pFrame->m_time);
				pFrame->m_tagNodes.WriteFile(fp);

				curFrameNum++;
			}
			else//不同动作
			{
				// 结束写入文件
				header.numFrame=curFrameNum;
				fseek(fp,0,SEEK_SET);
				FWriteValue(fp,header);
				fclose(fp);
				fp = NULL;

				i--;
				curFrameNum = 0;
				curName = pFrame->m_name;
			}
		}

		if( fp != NULL )
		{
			header.numFrame=curFrameNum;
			fseek(fp,0,SEEK_SET);
			FWriteValue(fp,header);
			fclose(fp);
		}
	}
}

void FindFiles( LPCTSTR szPath, LPCTSTR szExt, vector<tstring>& out )
{
	TCHAR szFind[512];
	_sntprintf(szFind,512,_T("%s\\*.%s"),szPath,szExt);
	SearchFiles(szFind,out);

	WIN32_FIND_DATA fd;
	_sntprintf(szFind,512,_T("%s\\*.*"),szPath);
	HANDLE hSearch=FindFirstFile(szFind, &fd);
	if(hSearch==INVALID_HANDLE_VALUE) 
		return;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_PATH];
	_tsplitpath_s(szFind,szDrive,_MAX_DRIVE,szDir,_MAX_PATH,NULL,0,NULL,0);

	TCHAR szFullPath[512];
	do 
	{
		if( (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)
			&& _tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")) )//遍历子文件夹
		{
			_sntprintf_s(szFullPath,512,_T("%s%s%s"),szDrive,szDir,fd.cFileName);
			FindFiles( szFullPath, szExt, out );
		}
	} 
	while(FindNextFile(hSearch,&fd)==TRUE);

	FindClose(hSearch);
}

void Cool3DModels::UpdateAABBTree( LPCTSTR szPath )
{
	IFS *pFS = ResMgr::Inst()->SafeGetFS( ResMgr::EFST_Model );

	vector<tstring> fscnFiles;
	FindFiles( szPath, _T("fscn"), fscnFiles );

	for( UINT i=0; i<fscnFiles.size(); i++ )
	{
		if( ReadAABBTree( pFS, fscnFiles[i].c_str() ) )
		{
			m_builder.BeginBuild( m_pVB, m_vertType );
			int index = 0;
			AABBTreeNode *pNode = m_pCDTree->GetNode(index++);
			while( pNode != NULL )
			{
				m_builder.AddFace( pNode );
				pNode = m_pCDTree->GetNode(index++);
			}
			m_builder.EndBuild();

			WriteAABBTree( fscnFiles[i].c_str() );
		}
	}

	::MessageBox(NULL,_T("升级完毕！"),_T("UpdateAABBTree"),MB_OK);
}

BOOL Cool3DModels::ReadAABBTree( IFS *pFS, LPCTSTR szFile )
{
	DWORD hFile = pFS->Open( szFile );
	if( hFile==0 || hFile==-1 )
	{
		errno_t err;
		_get_errno( &err );

		::MessageBox(NULL,szFile,_T("ResSceneMesh open failed"),MB_OK|MB_ICONWARNING);
		return FALSE;
	}

	SceneMeshHeader header;
	pFS->Read(&header,sizeof(SceneMeshHeader),hFile);
	m_vertType = header.vertType;
	if( header.ver > 1 )//已经是最新版本
	{
		pFS->Close(hFile);
		return FALSE;
	}

	pFS->Seek( hFile, header.offsetVert, FILE_BEGIN );
	UINT bufSize = GetVertSize( m_vertType ) * header.numVert;
	m_pVB = (BYTE*)malloc( bufSize );
	pFS->Read( m_pVB, bufSize, hFile );

	pFS->Seek( hFile, header.boxTreeOffset, FILE_BEGIN );
	m_pCDTree = new AABBTreeCollider;
	m_pCDTree->ReadFile( pFS, hFile );

	pFS->Close(hFile);

	return TRUE;
}

void Cool3DModels::WriteAABBTree( LPCTSTR szFile )
{
	FILE *fp =_tfopen( szFile, _T("r+b") );
	if( fp == NULL )
		return;

	SceneMeshHeader header;
	fread( &header, sizeof(SceneMeshHeader), 1, fp );
	header.ver = 2;

	fseek( fp, 0, FILE_BEGIN );
	fwrite( &header, sizeof(SceneMeshHeader), 1, fp );
	fseek( fp, header.boxTreeOffset, FILE_BEGIN );
	m_builder.WriteFile( fp );

	fclose( fp );

	SAFE_DELETE(m_pCDTree);
	SAFE_FREE(m_pVB);	
}

void Cool3DModels::ModifySkin( LPCTSTR szPath )
{
	IFS *pFS = ResMgr::Inst()->SafeGetFS( ResMgr::EFST_Model );

	vector<tstring> skinFiles;
	FindFiles( szPath, _T("fskin"), skinFiles );

	for( UINT i=0; i<skinFiles.size(); i++ )
	{
		FILE *fp =_tfopen( skinFiles[i].c_str(), _T("r+b") );
		if( fp == NULL )
			continue;

		tagFSkinHeader header;
		fread( &header,sizeof(tagFSkinHeader), 1, fp );

		//--read mtls
		vector<tagMtlDesc> mtlDescs;
		fseek( fp, header.mtlOffset, FILE_BEGIN );
		for( DWORD mtli=0; mtli<header.numMtl; mtli++ )
		{
			tagMtlDesc md;
			fread( &md, sizeof(tagMtlDesc), 1, fp );
			md.m_emissive = Vector4(0.0f,0.0f,0.0f,1.0f);
			md.m_specular = Vector4(0.9f,0.9f,0.9f,1.0f);
			mtlDescs.push_back(md);
		}

		fseek( fp, header.mtlOffset, FILE_BEGIN );
		for( DWORD mtli=0; mtli<header.numMtl; mtli++ )
		{
			tagMtlDesc& md = mtlDescs[mtli];
			fwrite(&md,sizeof(tagMtlDesc),1,fp);
		}

		fclose( fp );
	}

	::MessageBox(NULL,_T("修改完毕！"),_T("ModifySkin"),MB_OK);
}

void Cool3DModels::CheckSkel( void )
{
	if( m_p->m_pAvatar != NULL )
	{
		ResSkeleton *pRes = m_p->m_pAvatar->GetResource();

		TCHAR szDrive[255];
		TCHAR szDir[255];
		TCHAR szFile[255];
		TCHAR szExt[255];
		_tsplitpath(pRes->GetName(),szDrive,szDir,szFile,szExt);

		TCHAR szFind[255];
		_stprintf(szFind,_T("%s%s*%s"),szDrive,szDir,szExt);

		vector<tstring> out;
		SearchFiles(szFind,out);

		UINT correctNum = pRes->GetNumBone();
		bool bError = false;
		TCHAR szError[1024]={0};
		for( size_t i=0; i<out.size(); i++ )
		{
			// 读取文件头，判断骨骼数量是否符合要求
			FILE *fp=_tfopen(out[i].c_str(),_T("rb"));

			FSkelHeader header;
			fread(&header,sizeof(header),1,fp);
			fseek(fp,header.structOffset,FILE_BEGIN);
			UINT numBone;
			fread(&numBone,sizeof(UINT),1,fp);

			fclose(fp);

			if( correctNum != numBone )
			{
				_stprintf(szError,_T("%serror:%s\r\n"),szError,out[i].c_str());
				bError = true;
			}
		}

		if( bError )
		{
			::MessageBox(NULL,szError,_T("骨骼检查"),MB_OK);
		}
	}
}