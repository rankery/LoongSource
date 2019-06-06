#include "SceneEnumProc.h"
#include "SceneContext.h"
using namespace Cool3D;
#include <CS\BipExp.h>
#include <CS\PhyExp.h>
#include "stdmat.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "Max.h"
#if MAX_RELEASE >= 4000
#include "iskin.h"
#endif
#include "decomp.h"
#include "..\F3DMaxExp\MaxNodeType.h"
#include "..\CSExport\MaxExp\ExpSkeleton.h"

SceneEnumProc::SceneEnumProc()
{

}

SceneEnumProc::~SceneEnumProc()
{

}

int SceneEnumProc::callback(INode *node)
{
	if(IsTagNode(node))
	{
		SceneContext::Inst()->AddBoneNode(node);
	}
	if(MaxNode_IsBone(node) || MaxNode_IsDummy(node))
	{
		SceneContext::Inst()->AddBoneNode(node);
	}

	if(node->IsRootNode())
		return TREE_CONTINUE;
	if(node->IsHidden())
		return TREE_CONTINUE;

	if(MaxNode_IsDummy(node)
		||MaxNode_IsBone(node))
		return TREE_CONTINUE;

	if(IsMeshNode(node))
		SceneContext::Inst()->AddMeshNode(node);
	//else if(IsLightNode(node))
	//	SceneContext::Inst()->AddLightNode(node);

	return TREE_CONTINUE;
}

bool SceneEnumProc::IsMeshNode(INode *pNode)
{
	if(pNode==NULL)
		return false;
	if(pNode->IsHidden())
		return false;

	ObjectState os=pNode->EvalWorldState(0);
	if(os.obj == NULL)
		return false;
	
	if(os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return false;
	if(os.obj->ClassID() == Class_ID(TARGET_CLASS_ID,0))// should be Light and Camera target objects
		return false;
	if(!os.obj->CanConvertToType(triObjectClassID))
		return false;

	//¹ýÂËµô0face,0vertµÄnode
	bool haveGeom=false;
	TriObject* pTriObj=static_cast<TriObject*>(os.obj->ConvertToType(0,triObjectClassID));
	if(pTriObj->mesh.numFaces > 0
		&& pTriObj->mesh.numVerts > 0)
		haveGeom=true;

	if(os.obj != pTriObj)
		delete pTriObj;

	return haveGeom;
}

bool SceneEnumProc::IsLightNode(INode *pNode)
{
	if(pNode==NULL)
		return false;
	if(pNode->IsHidden())
		return false;
	
	ObjectState os=pNode->EvalWorldState(0);
	if(os.obj == NULL)
		return false;

	//-- if light is on
	if(os.obj->SuperClassID() != LIGHT_CLASS_ID)
		return false;

	GenLight *pMaxLight=static_cast<GenLight*>(os.obj);
	LightState lgtState;
	
	Interval valid = FOREVER;
	pMaxLight->EvalLightState(0, valid, &lgtState);

	return lgtState.on!=0;
}
