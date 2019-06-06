#include "..\stdafx.h"
#include ".\MaxSkin.h"
#include "..\CSExport.h"

MaxSkin::MaxSkin(INode *pNode,TimeValue time):MaxMesh(pNode,time)
{
	m_pModifier=Max_FindPhysiqueModifier(pNode);
	m_maxVertBoneInf=0;
	m_pPhysiqueExport=NULL;
	m_pContextExport=NULL;
}

MaxSkin::~MaxSkin()
{

}

void MaxSkin::BeginPhysiqueExport()
{
	if(m_pModifier==NULL)
	{
		g_msgBox.ShowMsg(_T("WARNING:Modifier not found,node name=%s.\r\n"),m_pINode->GetName());
		return;
	}
	// create a physique export interface
	m_pPhysiqueExport=(IPhysiqueExport *)m_pModifier->GetInterface(I_PHYINTERFACE);
	if(m_pPhysiqueExport==0)
	{
		g_msgBox.ShowMsg(_T("Physique modifier interface not found.\r\n"));
		return;
	}
	
	// create a context export interface
	m_pContextExport = (IPhyContextExport *)m_pPhysiqueExport->GetContextInterface(m_pINode);
	if(m_pContextExport == 0)
	{
		m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pPhysiqueExport);
		g_msgBox.ShowMsg(_T("Context export interface not found.\r\n"));
		return;
	}
	
	// set the flags in the context export interface
	m_pContextExport->ConvertToRigid(TRUE);
	m_pContextExport->AllowBlending(TRUE);
}

void MaxSkin::EndPhysiqueExport()
{
	// release all interfaces
	if(m_pPhysiqueExport!=NULL)
	{
		m_pPhysiqueExport->ReleaseContextInterface(m_pContextExport);
		m_pPhysiqueExport=NULL;
	}
	if(m_pModifier!=NULL)
	{
		m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pPhysiqueExport);
		m_pModifier=NULL;
	}
}

void MaxSkin::Vert_GetBoneInf(int vertexId,BoneInfArray& binfArray)
{
	if(m_pPhysiqueExport==NULL
		|| m_pContextExport==NULL)
		return;
	
	//--
	binfArray.clear();	
	
	// get the vertex export interface
	IPhyVertexExport *pVertexExport;
	pVertexExport=(IPhyVertexExport *)m_pContextExport->GetVertexInterface(vertexId);
	if(pVertexExport==0)
	{
		m_pPhysiqueExport->ReleaseContextInterface(m_pContextExport);
		m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pPhysiqueExport);
		g_msgBox.ShowMsg(_T("Vertex export interface not found.\r\n"));
		return;
	}
	
	// get the vertex type
	int vertexType;
	vertexType = pVertexExport->GetVertexType();
	// handle the specific vertex type
	if(vertexType==RIGID_TYPE)
	{
		// typecast to rigid vertex
		IPhyRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyRigidVertex *)pVertexExport;
		
		BoneInfluence binf;
		binf.weight=1.0f;
		binf.szBoneName=pTypeVertex->GetNode()->GetName();
		binf.offset=pTypeVertex->GetOffsetVector();
		
		// 顶点只受一块骨头影响 [12/19/2007 leili]
		binfArray.push_back(binf);
	}
	else if(vertexType==RIGID_BLENDED_TYPE)
	{
		// typecast to blended vertex
		IPhyBlendedRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;
		
		// loop through all influencing bones
		int nodeId;
		int numNodes=pTypeVertex->GetNumberNodes();
		for(nodeId=0;nodeId<numNodes;nodeId++)
		{
			// add the influence to the vertex candidate
			BoneInfluence binf;
			binf.weight=pTypeVertex->GetWeight(nodeId);
			binf.szBoneName=pTypeVertex->GetNode(nodeId)->GetName();
			binf.offset=pTypeVertex->GetOffsetVector(nodeId);
			AddBoneInf(binfArray,binf);
		}
	}
	else if(vertexType == DEFORMABLE_TYPE)
	{
		// 可变形顶点
	}
	else
	{
		g_msgBox.ShowMsg(_T("WARNING:Unknown physique vertex type.\r\n"));
	}
	
	if(binfArray.size()>m_maxVertBoneInf)
		m_maxVertBoneInf=binfArray.size();
}

void MaxSkin::AddBoneInf(BoneInfArray& binfArray,BoneInfluence& binf)
{
	// 顶点受多块骨头影响 [12/19/2007 leili]
	// offset 可能不同???
	for(size_t i=0;i<binfArray.size();i++)
	{
		BoneInfluence& t=binfArray[i];
		if(t.szBoneName==binf.szBoneName)
		{
			t.weight+=binf.weight;
			return;
		}
	}
	binfArray.push_back(binf);
}