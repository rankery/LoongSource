#include "StdAfx.h"
#include ".\Cool3Dmodels.h"


Cool3DModels::UsefulMember::UsefulMember()
: m_created(false)
, m_pEntity(0)
, m_pAvatar(0)
{
	m_pSceneGraph = new NullSceneGraph;

	m_pTestLgt=new SGLightNode;
	tagLight lgt;
	lgt.ambient=Color4f(0,0,0,0);
	lgt.attenuation=Vector3(0,1,0);
	lgt.diffuse=Color4f(0,1,0,0);
	lgt.pos=Vector3(0,100,0);
	lgt.range=5000;
	lgt.type=Light_Point;
	m_pTestLgt->SetLight(&lgt);

	m_pSceneGraph->AttachDynamicNode(m_pTestLgt);
}

Cool3DModels::UsefulMember::~UsefulMember()
{
	m_pSceneGraph->DetachDynamicNode(m_pTestLgt);
	if(m_pTestLgt)
	{
		delete m_pTestLgt;
	}

	if(m_pSceneGraph)
	{
		delete m_pSceneGraph;
	}
}

Vector3 Cool3DModels::UsefulMember::GetActiveNodeCenter()
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
}

void Cool3DModels::UsefulMember::ResetCamera()
{
	m_arcBall.ClearRotate();

	SceneNode *pNode=GetActiveNode();
	if(pNode)
	{
		Vector3 center=GetActiveNodeCenter();
		m_cameraEuler.Set(Vector3(0,12,0),0,0,0,1000);
		m_cameraEuler.SetLookAtPos(center);
	}
}

SceneNode * Cool3DModels::UsefulMember::GetActiveNode()
{
	if(m_pEntity)
		return m_pEntity;
	else if(m_pAvatar)
		return m_pAvatar;
	return NULL;
}

bool Cool3DModels::UsefulMember::TRACEEffectPropertyListCounter(const TCHAR * functionName)
{
	int counter = m_EffectList.size();
	TCHAR reportString[128];
	_stprintf(reportString, _T("%s()====EffectProperty has %d nodes!\r\n"), functionName, counter);
	TRACE(reportString);
	return false;
}

//=======================================================================
Cool3DModels::Cool3DModels(void)
{
}

Cool3DModels::~Cool3DModels(void)
{
}

bool Cool3DModels::initialEngine(HWND iRenderHWND)
{
	if( m_UsefulMember.m_created )
	{
		return true;
	}

	HDC hDC=::GetDC(iRenderHWND);
	int screenColorBits=GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(iRenderHWND,hDC);

	Kernel::Inst()->CreateEngine((tagEngineCfg*)NULL);
	tagDeviceCfg deviceConfig;
	deviceConfig.bWindowed = true;
	deviceConfig.hWnd = iRenderHWND;
	deviceConfig.depthFormat=EDF_D24S8;
	deviceConfig.nWidth=800;
	deviceConfig.nHeight=600;
	if( screenColorBits==32 )
		deviceConfig.pixelFormat=EPF_X8R8G8B8;
	else
		deviceConfig.pixelFormat=EPF_R5G6B5;
	Kernel::Inst()->CreateRenderDevice(&deviceConfig);
	Device()->SetClearParam(true,true,true,0xffafafaf,1.0f,0);
	ResMgr::Inst()->EnableIOAsync(false);

	m_UsefulMember.m_cameraEuler.Set(Vector3(0,12,0),0,0,0,1000);
	UINT clientWidth=Device()->GetClientWidth(), ClientHeight=Device()->GetClientHeight();
	m_UsefulMember.m_cameraEuler.SetPerspective((float)clientWidth, (float)ClientHeight, _DegToRad(44.98f),50,50000);
	m_UsefulMember.m_pSceneGraph->BindCamera(&(m_UsefulMember.m_cameraEuler));

	m_UsefulMember.m_created = true;

	return true;
}

bool Cool3DModels::loadModel(const TCHAR * iModelFilePath)
{
	ASSERT(iModelFilePath!=NULL);

	Filename szFileName(iModelFilePath);
	tstring ext=szFileName.GetExtension();

	if(_tcsicmp(ext.c_str(),_T("fskin"))==0)
	{
		if(m_UsefulMember.m_pAvatar==NULL)
		{
			AfxMessageBox(_T("在加载Skin之前,必须先加载一个骨骼(*.fskel)"));
		}
		else
		{
			SGSkinNode *pNewSkin=new SGSkinNode;
			pNewSkin->SetUserData(iModelFilePath);
			pNewSkin->LoadRes(iModelFilePath);
			m_UsefulMember.m_pAvatar->AttachChild(pNewSkin);
		}
		return false;
	}

	//删除所有现有的东西
	if(m_UsefulMember.m_pEntity)
	{
		m_UsefulMember.m_pSceneGraph->DetachDynamicNode(m_UsefulMember.m_pEntity);
		delete m_UsefulMember.m_pEntity;
		m_UsefulMember.m_pEntity=NULL;
	}
	if(m_UsefulMember.m_pAvatar)
	{
		m_UsefulMember.m_pSceneGraph->DetachDynamicNode(m_UsefulMember.m_pAvatar);
		delete m_UsefulMember.m_pAvatar;
		m_UsefulMember.m_pAvatar=NULL;
	}


	if(_tcsicmp(ext.c_str(),_T("fskel"))==0)
	{
		m_UsefulMember.m_pAvatar=new AvatarNode;
		m_UsefulMember.m_pAvatar->LoadRes(iModelFilePath);
		m_UsefulMember.m_pSceneGraph->AttachDynamicNode(m_UsefulMember.m_pAvatar);
		m_UsefulMember.m_AvatarPath = iModelFilePath;
	}
	else
	{
		m_UsefulMember.m_pEntity=new EntityNode;
		m_UsefulMember.m_pEntity->LoadRes(iModelFilePath);
		m_UsefulMember.m_pSceneGraph->AttachDynamicNode(m_UsefulMember.m_pEntity);
		//RTTIObj *pObj=m_UsefulMember.m_pEntity->GetResObj();
		//_ShowNote(pObj->GetClassName());

		/*	m_UsefulMember.m_pEntity->Update(1.0,m_UsefulMember.m_pSceneGraph);
		if(m_UsefulMember.m_pEntity->GetNumMtl()>0)
		{
		IMaterial *pMtl=m_UsefulMember.m _pEntity->GetMtl(0);
		MMColorWriteControl *pCC=new MMColorWriteControl;
		pCC->SetName("ColorMask");
		pCC->m_dwColorMask=COLORWRITEENABLE_RED|COLORWRITEENABLE_GREEN|COLORWRITEENABLE_ALPHA;
		pMtl->AttachModifier(pCC);
		pMtl->InvalidateRenderCache();
		}*/

		m_UsefulMember.m_EntityPath = iModelFilePath;
	}

	m_UsefulMember.ResetCamera();

	return false;
}


void Cool3DModels::render(void)
{
//--更新引擎时钟
	Kernel::Inst()->Tick();

	//--渲染当前的模型
	if(!m_UsefulMember.m_created)
		return;

	SceneNode *pNode=m_UsefulMember.GetActiveNode();
	if(pNode==NULL)
		return;
	
	IDevice *pDev=Device();
	pDev->Clear();
	pDev->Begin();

	/*Vector3 center=m_UsefulMember.GetActiveNodeCenter();
	Matrix4 trans,transInv;
	D3DXMatrixTranslation(&trans,center.x,center.y,center.z);
	D3DXMatrixInverse(&transInv,NULL,&trans);
	Matrix4 rotate;
	m_UsefulMember.m_newRot.GetRotateMat(&rotate);
	rotate=transInv*rotate*trans;
	pNode->SetWorldMat(&rotate);
	*/

	m_UsefulMember.m_cameraEuler.UpdateViewMatrix();
	m_UsefulMember.m_pSceneGraph->Render(Kernel::Inst()->GetDeltaTime());

	//--darw axis
	pDev->MatLoad(EMS_Prj,m_UsefulMember.m_cameraEuler.GetPrj());
	pDev->MatLoad(EMS_View,m_UsefulMember.m_cameraEuler.GetView());
	pDev->FFlushMatrix(false,true,true,false);
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(10000,0,0),Color4ub(255,255,0,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,10000,0),Color4ub(255,0,255,0));
	DrawX::Draw3DLine(Vector3(0,0,0),Vector3(0,0,10000),Color4ub(255,0,0,255));

	////draw skeleton if necessory
	//if(pNode->IS_STATIC_CLASS(AvatarNode))
	//{
	//	AvatarNode *pAvatar=(AvatarNode *)pNode;

	//	NodeAnimationCtrl *pAni=pAvatar->GetSkeletonAniCtrl();
	//	if(pAni)
	//		pAni->DrawSkeleton();
	//}//endof if

	//--draw tag nodes
	AABBox tagBox;
	float size=8;
	tagBox.max=Vector3(size,size,size);
	tagBox.min=Vector3(-size,-size,-size);
	DWORD boxColor=Color4ub(255,5,5,250);
	DWORD selColor=Color4ub(255,255,0,0);
	AnimationCtrl *pAni=GetSceneNodeAni();
	if( pAni != NULL )
	{
		if( pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
		{
			NodeAnimationCtrl *pNodeCtrl = static_cast<NodeAnimationCtrl*>(pAni);

			vector<Matrix4> bonesMat;
			pNodeCtrl->GetAllBoneMat(bonesMat);
			pDev->MatPush(EMS_World);
			for( UINT i=0; i<bonesMat.size(); i++ )
			{
				pDev->MatLoad(EMS_World, &bonesMat[i]);
				pDev->FFlushMatrix(true,false,false,false);

				if( m_selNode == i )
					DrawX::DrawAABBox(tagBox,selColor);
				else
					DrawX::DrawAABBox(tagBox,boxColor);

			}
			pDev->MatPop(EMS_World);
			pDev->FFlushMatrix(true,false,false,false);
		}
		else if( pAni->IS_STATIC_CLASS(VertexAnimationCtrl) )
		{
			VertexAnimationCtrl *pVertexCtrl = static_cast<VertexAnimationCtrl*>(pAni);

			const vector<tagTagNodeSpace>& tagNodes=pVertexCtrl->GetTagNodes();
			pDev->MatPush(EMS_World);
			for(vector<tagTagNodeSpace>::const_iterator iter=tagNodes.begin();
				iter!=tagNodes.end();++iter)
			{
				pDev->MatLoad(EMS_World,iter->trans.GetMatrix());
				pDev->FFlushMatrix(true,false,false,false);
				DrawX::DrawAABBox(tagBox,boxColor);
			}
			pDev->MatPop(EMS_World);
			pDev->FFlushMatrix(true,false,false,false);
		}
	}

#if 1
	//--draw aabbtree of scene mesh
	if(m_UsefulMember.m_pEntity)
	{
		ResBase *pRes=m_UsefulMember.m_pEntity->GetResObj();
		if(pRes->IS_STATIC_CLASS(ResSceneMesh))
		{
			((ResSceneMesh*)pRes)->DrawAABBTree();
		}
	}
#endif

	pDev->End();
	pDev->Present();

	//--更新界面显示
	Sleep(2);
}

AnimationCtrl* Cool3DModels::GetSceneNodeAni()
{
	SceneNode *pNode=m_UsefulMember.GetActiveNode();
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

SceneGraph* Cool3DModels::GetSceneGraph()
{
	return this->m_UsefulMember.m_pSceneGraph;
}

bool Cool3DModels::AttachEffect(const TCHAR * iEffectPath, const TCHAR * iAttachPointName)
{
	IFS *pFS = (IFS *)Kernel::Inst()->DefaultFS();
	DWORD fileSize=pFS->GetSize(iEffectPath);
	if(fileSize==0 )//文件不存在
	{
		LOG(_T("特效文件没有找到,file=%s\r\n"),iEffectPath);
		return false;
	}
	//--读取对象
	FArchive ar;
	ar.OpenForRead(pFS,iEffectPath);
	Cool3DModels::EffectProperty *	currentEffectProperty = new Cool3DModels::EffectProperty;//声名一个特效属性类的实例,用以保存当前要添加的特效,及其相关属性.
	//currentEffectProperty->m_Effect = new Cool3D::SGAttachableEffectNode;
	currentEffectProperty->m_Effect->Deserialize(ar); //添加特效
	ar.Close();

	currentEffectProperty->m_Effect->BindToTagNode(iAttachPointName); //绑定到挂在点

	if(m_UsefulMember.m_pEntity!=NULL)
	{
		m_UsefulMember.m_pEntity->AttachChild(currentEffectProperty->m_Effect); //设置为人物模型的子接点
	}
	else if(m_UsefulMember.m_pAvatar!=NULL)
	{
		m_UsefulMember.m_pAvatar->AttachChild(currentEffectProperty->m_Effect); //设置为人物模型的子接点
	}
	else
	{
		return false;
	}

	currentEffectProperty->m_AttachPointName = iAttachPointName;//设置加载点的名称
	currentEffectProperty->m_EffectPath = iEffectPath;//设置特效文件路径

	//记录文件路径
	tstring effectPath = iEffectPath;
	m_UsefulMember.m_EffectList.push_back(*currentEffectProperty);//添加一个特效属性类实例

	//--启动播放
	// del by bwen 2006.4.11
	//Cool3DModels::Inst()->GetSceneGraph()->AttachDynamicNode(m_UsefulMember.m_EffectList.back().m_Effect);
	m_UsefulMember.m_EffectList.back().m_Effect->SetTimeScale(1.0);
	m_UsefulMember.m_EffectList.back().m_Effect->Play();

	m_UsefulMember.TRACEEffectPropertyListCounter(_T("AttachEffect"));

	return true;
}
bool Cool3DModels::AttachEffectThroughFileContent(tstring * iEffectAttachingFileContent)
{
	//首先要清空所有的特效
	list<EffectProperty>::iterator  effectListIterator = m_UsefulMember.m_EffectList.begin();
	while (effectListIterator!=m_UsefulMember.m_EffectList.end())
	{
		SceneGraph * pSG = GetSceneGraph();
		pSG->DetachDynamicNode(effectListIterator->m_Effect);
		if( m_UsefulMember.m_pEntity != NULL )
			m_UsefulMember.m_pEntity->DetachChild(effectListIterator->m_Effect);
		if( m_UsefulMember.m_pAvatar != NULL )
			m_UsefulMember.m_pAvatar->DetachChild(effectListIterator->m_Effect);

		++effectListIterator;
	}
	m_UsefulMember.m_EffectList.clear();

	tstring attachPointName, filePath;
	while(iEffectAttachingFileContent->find(_T(",")) != tstring::npos)
	{	
		tstring::size_type commaPosition = iEffectAttachingFileContent->find(_T(","));
		attachPointName = iEffectAttachingFileContent->substr(0,commaPosition);
		iEffectAttachingFileContent->erase(0,commaPosition+1);

		commaPosition = iEffectAttachingFileContent->find(_T(","));
		if(commaPosition == tstring::npos)
		{
			filePath = *iEffectAttachingFileContent;
			iEffectAttachingFileContent->erase();
		}
		else
		{
			filePath = iEffectAttachingFileContent->substr(0,commaPosition);
			iEffectAttachingFileContent->erase(0,commaPosition+1);
		}

		if(!AttachEffect(filePath.c_str(), attachPointName.c_str()))
			return false;
	}

	
	return true;
}

bool Cool3DModels::DetachEffect(const TCHAR * iEffectFilePath)
{
	list<EffectProperty>::iterator effectListIterator = m_UsefulMember.m_EffectList.begin();
		
	while(effectListIterator != m_UsefulMember.m_EffectList.end())
	{
		if(_tcsicmp(effectListIterator->m_EffectPath.c_str(), iEffectFilePath)==0)
		{
				SceneGraph *pSG = Inst()->GetSceneGraph();
				//effectListIterator->m_Effect->Close();
				pSG->DetachDynamicNode(effectListIterator->m_Effect);

				if( m_UsefulMember.m_pEntity != NULL )
					m_UsefulMember.m_pEntity->DetachChild(effectListIterator->m_Effect);//取消子接点的连接
				if( m_UsefulMember.m_pAvatar != NULL )
					m_UsefulMember.m_pAvatar->DetachChild(effectListIterator->m_Effect);//取消子接点的连接
				if(!effectListIterator->m_Effect->UnbindToTagNode())
					return false;

				m_UsefulMember.m_EffectList.erase(effectListIterator);

				m_UsefulMember.TRACEEffectPropertyListCounter(_T("DetachEffect"));

				return true;
		}
		++effectListIterator;
	}

	return false;
}


bool Cool3DModels::ApplicationClose(void)
{
	//清空所有的特效
	list<EffectProperty>::iterator  effectListIterator = m_UsefulMember.m_EffectList.begin();
	while (effectListIterator!=m_UsefulMember.m_EffectList.end())
	{
		SceneGraph * pSG = GetSceneGraph();
		pSG->DetachDynamicNode(effectListIterator->m_Effect);
		if( m_UsefulMember.m_pEntity != NULL )
			m_UsefulMember.m_pEntity->DetachChild(effectListIterator->m_Effect);
		if( m_UsefulMember.m_pAvatar != NULL )
			m_UsefulMember.m_pAvatar->DetachChild(effectListIterator->m_Effect);

		++effectListIterator;
	}
	m_UsefulMember.m_EffectList.clear();

	//清除m_pEntity和m_pAvatar
	if(m_UsefulMember.m_pEntity)
	{
		m_UsefulMember.m_pSceneGraph->DetachDynamicNode(m_UsefulMember.m_pEntity);
		delete m_UsefulMember.m_pEntity;
		m_UsefulMember.m_pEntity=NULL;
	}
	if(m_UsefulMember.m_pAvatar)
	{
		m_UsefulMember.m_pSceneGraph->DetachDynamicNode(m_UsefulMember.m_pAvatar);
		delete m_UsefulMember.m_pAvatar;
		m_UsefulMember.m_pAvatar=NULL;
	}

	if(m_UsefulMember.m_pAvatar==NULL && m_UsefulMember.m_pEntity==NULL && m_UsefulMember.m_EffectList.empty()==true)
		return true;

	return false;
}

void Cool3DModels::SetSelectedNode( const TCHAR* szSelNode )
{
	AnimationCtrl *pAni=GetSceneNodeAni();
	if( pAni != NULL )
	{
		if( pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
		{
			ResSkeleton *pRes = (ResSkeleton*)pAni->GetRes();
			const SkeletonStruct& ss = pRes->GetStruct();
			for( UINT i=0; i<ss.m_boneNames.size(); i++ )
			{
				const tstring& boneName = ss.m_boneNames[i];
				if( boneName == szSelNode )
				{
					m_selNode = i;
					break;
				}
			}
		}
		//else if( pAni->IS_STATIC_CLASS(VertexAnimationCtrl) )
		//{
		//	VertexAnimationCtrl *pVertexCtrl = static_cast<VertexAnimationCtrl*>(pAni);
		//}
	}
}