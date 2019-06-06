#ifndef MAX_NODE_TYPE_H
#define MAX_NODE_TYPE_H

inline bool MaxNode_IsMesh(INode *pINode)
{
	// check for invalid nodes
	if(pINode == 0) 
		return false;

	// check for root node
	if(pINode->IsRootNode()) 
		return false;

	// check for mesh node
	ObjectState os;
	os = pINode->EvalWorldState(0);
	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) 
		return true;

	return false;
}

inline bool MaxNode_IsDummy(INode *pINode)
{
	if(pINode==NULL)
		return false;
	// check for root node
	if(pINode->IsRootNode())
		return false;

	// check for dummy node
	ObjectState os;
	os = pINode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return true;
	return false;
}

inline bool MaxNode_IsBone(INode *pINode)
{
	if(pINode==NULL)
		return false;
	// check for root node
	if(pINode->IsRootNode())
		return false;

	// check for bone node
	ObjectState os;
	os = pINode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0)) 
		return true;

#if MAX_RELEASE >= 4000
	if(os.obj->ClassID() == BONE_OBJ_CLASSID)
		return true;
#endif

	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) 
		return false;

	// check for biped node
	Control *pControl;
	pControl = pINode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) 
		|| (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) 
		return true;

	return false;
}

inline bool MaxNode_IsBipedBone(INode *pNode)
{
	// check for invalid nodes
	if(pNode == 0) 
		return false;

	// check for root node
	if(pNode->IsRootNode()) 
		return false;

	// check for bone node
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	// check for biped node
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) 
		|| (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) 
		return true;

	return false;
}

//----------------------------------------------------------------------------//
// Find the physique modifier for a given node                                //
//----------------------------------------------------------------------------//

inline Modifier *Max_FindPhysiqueModifier(INode *pINode)
{
	// get the object reference of the node
	Object *pObject;
	pObject = pINode->GetObjectRef();
	if(pObject == 0) 
		return 0;

	// loop through all derived objects
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);

		// loop through all modifiers
		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			// get the modifier
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);

			// check if we found the physique modifier
			if(pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return pModifier;
		}

		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return 0;
}

//----------------------------------------------------------------------------//
// Find the skin modifier for a given node                                    //
//----------------------------------------------------------------------------//

inline Modifier *Max_FindSkinModifier(INode *pINode)
{
#if MAX_RELEASE >= 4000
	// get the object reference of the node
	Object *pObject;
	pObject = pINode->GetObjectRef();
	if(pObject == 0)
		return 0;
	
	// loop through all derived objects
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);
		
		// loop through all modifiers
		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			// get the modifier
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);
			
			// check if we found the skin modifier
			if(pModifier->ClassID() == SKIN_CLASSID)
				return pModifier;
		}
		
		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}
#endif
	
	return 0;
}

#endif//#ifndef MAX_NODE_TYPE_H