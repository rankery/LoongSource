#include "..\stdafx.h"
#include <assert.h>
#include ".\ExpSkeleton.h"
#include "..\CSExport.h"
#include "..\..\MaxPublic\MathBaseDefine.h"

// --From CS SDK
// This function can be used to set the non-uniform scale of a biped.
// The node argument should be a biped node.
// If the scale argument is non-zero the non-uniform scale will be removed from the biped.
// Remove the non-uniform scale before exporting biped nodes and animation data
// If the scale argument is zero the non-uniform scaling will be reapplied to the biped.
// Add the non-uniform scaling back on the biped before exporting skin data
//***********************************************************************************
void ScaleBiped(INode* node, int scale)
{
	if (node->IsRootNode()) return;

	// Use the class ID to check to see if we have a biped node
	Control* c = node->GetTMController();
    if ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
         (c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
         (c->ClassID() == FOOTPRINT_CLASS_ID))
    {

        // Get the Biped Export Interface from the controller 
        IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);

        // Either remove the non-uniform scale from the biped, 
		// or add it back in depending on the boolean scale value
        BipIface->RemoveNonUniformScale(scale);
		Control* iMaster = (Control *) c->GetInterface(I_MASTER);
		iMaster->NotifyDependents(FOREVER, PART_TM, REFMSG_CHANGE);
		
		// Release the interfaces
		c->ReleaseInterface(I_MASTER,iMaster);
		c->ReleaseInterface(I_BIPINTERFACE,BipIface);
	}
}

//--------------------------------------------
class BoneEnumProc : public ITreeEnumProc  
{
public:
	BoneEnumProc(ExpSkeleton *pExp)
	{
		m_pExp=pExp;
	}
	~BoneEnumProc()	{}
	virtual int callback( INode *pNode )
	{
		if(IsTagNode(pNode))
		{
			//m_pExp->AddTagNode(pNode);
			//都按照bone来处理
			m_pExp->AddBoneNode(pNode);
		}
		else
		{	
			if(MaxNode_IsBone(pNode) || MaxNode_IsDummy(pNode))
			{
				m_pExp->AddBoneNode(pNode);
			}
		}
		return TREE_CONTINUE;
	}
private:
	ExpSkeleton	*m_pExp;
};
//-------------------------------------
ExpSkeleton::ExpSkeleton()
{
	m_bShowInfo=true;
}

ExpSkeleton::~ExpSkeleton()
{

}

int ExpSkeleton::BuildFromMaxExportInterface(ExpInterface *pEI)
{
	Clear();

	//--
	BoneEnumProc enumProc(this);
	pEI->theScene->EnumTree(&enumProc);

	//--
	BuildBoneHierarchy();

	//--find out root bones
	for(size_t i=0;i<m_expBones.size();i++)
	{
		const ExpBone& bone=m_expBones[i];
		if(bone.m_parentId==-1)
		{
			m_rootNodes.push_back(i);
			g_msgBox.ShowMsg(_T("Root node:%s\r\n"),bone.m_pMaxNode->GetName());
		}
	}

	return m_maxBones.size();
}

void ExpSkeleton::AddTagNode(INode *pNode)
{
	if(pNode==NULL)
		return;

	//--Tag node单独输出的话, tag node则不能有child是需要输出的Bone,
	//否则, child bone会找不到parent node. 如果实在不行,则可以把tag作为bone再输出一次
	m_tagNodeExp.AddMaxNode(pNode);
	g_msgBox.ShowMsg(_T("tagNode:%s\r\n"),pNode->GetName());
}

void ExpSkeleton::AddBoneNode(INode *pNode)
{
	if(pNode==NULL)
		return;
	//ScaleBiped(pNode,1);
	m_maxBones.push_back(pNode);

	Matrix3 tt=pNode->GetNodeTM(0);
	bool bMirrored=Max_TMNegParity(tt);
	if(m_bShowInfo)
	{
		if(bMirrored)
			g_msgBox.ShowMsg(_T("Bone:%s,Mirrored!\r\n"),pNode->GetName());
		else
			g_msgBox.ShowMsg(_T("Bone:%s\r\n"),pNode->GetName());
	}
	
}

void ExpSkeleton::Clear()
{
	m_maxBones.clear();
	m_expBones.clear();
	m_rootNodes.clear();
	m_tagNodeExp.Clear();
}

int ExpSkeleton::FindNodeByName(const TCHAR *szName)
{
	for(size_t i=0;i<m_maxBones.size();i++)
	{
		INode *pNode=m_maxBones[i];
		if(_tcscmp(pNode->GetName(),szName)==0)
			return i;
	}
	return -1;
}

INode *ExpSkeleton::GetNode(int i)
{
	assert(i<m_maxBones.size());
	return m_maxBones[i];
}

SkeletonFrame* ExpSkeleton::CreateFrame(const TCHAR* szFrmName,TimeValue time)
{
	SkeletonFrame *pNewFrame=new SkeletonFrame;
	pNewFrame->m_name=_MakeFourCCA(tstring(szFrmName));

	for(size_t i=0;i<m_expBones.size();i++)
	{
		Cool3D::Bone bone;
		ExpBone2F3DBone(i,bone,time);
		pNewFrame->m_boneArray.push_back(bone);
	}

	m_tagNodeExp.ExportFrameAni(pNewFrame->m_tagNodes,NULL,time);

	return pNewFrame;
}

void ExpSkeleton::CreateSkeletonStruct(SkeletonStruct& stut)
{
	stut.m_boneNames.clear();
	stut.m_boneStruct.resize(m_maxBones.size());
	for(size_t i=0;i<m_maxBones.size();i++)
	{
		INode *pNode=m_maxBones[i];
		stut.m_boneNames.push_back(CharToWide(pNode->GetName()));

		BoneStruct& boneStruct=stut.m_boneStruct[i];
		boneStruct.id=i;
		boneStruct.parentID=-1;
	}
	stut.m_rootBones=m_rootNodes;

	//--建立父子关系
	for(i=0;i<m_maxBones.size();i++)
	{
		BoneStruct& boneStruct=stut.m_boneStruct[i];
		INode *pParentNode=m_maxBones[i]->GetParentNode();
		boneStruct.parentID=FindNodeByName(pParentNode->GetName());
		if(boneStruct.parentID!=-1)
		{
			assert(boneStruct.parentID!=boneStruct.id);
			assert(boneStruct.parentID>=0 && boneStruct.parentID<m_maxBones.size());
			BoneStruct& parentBone=stut.m_boneStruct[boneStruct.parentID];
			parentBone.children.push_back(i);
		}
	}
}

void ExpSkeleton::BuildBoneHierarchy()
{
	size_t i;
	//--按照max bone数组的顺序建立exp bone数组
	for(i=0;i<m_maxBones.size();i++)
	{
		INode *pNode=m_maxBones[i];
		ExpBone bone;
		bone.m_id=i;
		bone.m_pMaxNode=pNode;
		bone.m_parentId=-1;
		//--check mirrored bones
		Matrix3 tt=pNode->GetNodeTM(0);
		bone.m_bMirrored=Max_TMNegParity(tt);
	
		m_expBones.push_back(bone);
	}

	//--建立父子关系
	for(i=0;i<m_expBones.size();i++)
	{
		ExpBone& bone=m_expBones[i];
		INode *pParentNode=bone.m_pMaxNode->GetParentNode();
		bone.m_parentId=FindNodeByName(pParentNode->GetName());
		if(bone.m_parentId!=-1)
		{
			assert(bone.m_parentId!=bone.m_id);
			assert(bone.m_parentId>=0 && bone.m_parentId<m_expBones.size());
			ExpBone& parentBone=m_expBones[bone.m_parentId];
			parentBone.m_children.push_back(i);
		}
	}
}

inline Matrix3 Max_MatMax2D3D()
{
	Matrix3 rot=RotateXMatrix(-Cool3D::FLOAT_PI*0.5f);
	Matrix3 flipZ=ScaleMatrix(Point3(1,1,-1));
	return rot*flipZ;
}

inline Matrix3 Max_MatAlign2D3D(const Matrix3& src)
{
    Matrix3 toD3D, toD3D_neg;
    toD3D=Max_MatMax2D3D();

    toD3D_neg = Inverse( toD3D );

    toD3D_neg *= src;
    toD3D_neg *= toD3D;
    return toD3D_neg;   //final_matrix = Inverse(R) * TransformMat * R
}


void ExpSkeleton::ExpBone2F3DBone(size_t expBoneIndex,Cool3D::Bone& fbone,TimeValue time)
{
	assert(expBoneIndex>=0 && expBoneIndex<m_expBones.size());
	const ExpBone& eBone=m_expBones[expBoneIndex];

	fbone.m_bMirrored	=	eBone.m_bMirrored;

	//--get transform
	INode *pParentNode=NULL;
	if(eBone.m_parentId!=-1)
	{
		assert(eBone.m_parentId>=0 && eBone.m_parentId<m_expBones.size());
		const ExpBone& parentBone=m_expBones[eBone.m_parentId];
		pParentNode=parentBone.m_pMaxNode;
	}
	
	fbone.m_bMirrored=Max_GetNodeLocalAni(eBone.m_pMaxNode,pParentNode,time,fbone.m_trans,fbone.m_rotat);
}