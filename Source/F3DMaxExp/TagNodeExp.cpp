#include "stdafx.h"
#include "TagNodeExp.h"
#include "MaxMath.h"

TagNodeExp::TagNodeExp()
{}

TagNodeExp::~TagNodeExp()
{}

void TagNodeExp::AddMaxNode(INode *pNode)
{
	m_maxNodes.push_back(pNode);
}

void TagNodeExp::Clear()
{
	m_maxNodes.clear();
}

class TagNodeSortFunc
{
public:
	bool operator() (INode *pMaxNode1,INode *pMaxNode2)
	{
		const TCHAR* szName=pMaxNode1->GetName();
		const TCHAR* szName2=pMaxNode2->GetName();
		return _tcscmp(szName,szName2)>0;
	}
};

void TagNodeExp::ExportFrameAni(TagNodeFrame& out,INode *pParent,TimeValue time,bool expColor)
{
	//--sort tagNodes by name
	TagNodeSortFunc sortFunc;
	std::sort(m_maxNodes.begin(),m_maxNodes.end(),sortFunc);
	//--
	out.nodes.clear();

	for(vector<INode*>::iterator iter=m_maxNodes.begin();iter!=m_maxNodes.end();++iter)
	{
		INode *pMaxNode=(*iter);
		tagTagNode tagNode;
		const TCHAR* szNodeName=pMaxNode->GetName();
		assert(_tcslen(szNodeName)>=5);//Ó¦¸ÃÊÇ"tag_XXXX"
		tagNode.name=_MakeFourCCA(tstring(szNodeName+4));
		tagNode.bMirrored=Max_GetNodeLocalAni(pMaxNode,pParent,time,tagNode.trans,tagNode.rotat);
		if(expColor)
		{
			DWORD dwC=pMaxNode->GetWireColor();
			tagNode.rotat.x=GetRValue(dwC)/255.0f;
			tagNode.rotat.y=GetGValue(dwC)/255.0f;
			tagNode.rotat.z=GetBValue(dwC)/255.0f;
			tagNode.rotat.w=1.0f;
		}
		out.nodes.push_back(tagNode);
	}
}