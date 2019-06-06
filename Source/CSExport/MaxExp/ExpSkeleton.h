#pragma once

#include <vector>
#include "..\..\F3DMaxExp\MaxNodeType.h"
#include "..\..\F3DMaxExp\MaxMath.h"
#include "..\..\F3DMaxExp\TagNodeExp.h"
#include "..\..\MaxPublic\SkinBoneDefine.h"

using namespace std;
using namespace Cool3D;

inline bool IsTagNode(INode *pNode)
{
	TCHAR *szName=pNode->GetName();
	if(_tcslen(szName)<4)
		return false;
	//--name==tag*;
	if(szName[0]==_T('t')
		&& szName[1]==_T('a')
		&& szName[2]==_T('g'))
		return true;
	return false;
}

class ExpBone
{
public:
	ExpBone()	{}
	~ExpBone()	{}
	ExpBone(const ExpBone& other)	{	Copy(other);	}
	const ExpBone& operator = (const ExpBone& other)
	{
		if(this==&other)
			return *this;
		Copy(other);
		return *this;
	}
	
	int			m_id;
	int			m_parentId;
	vector<int>	m_children;
	INode*		m_pMaxNode;
	bool		m_bMirrored;
private:
	void Copy(const ExpBone& other)
	{
		m_id		=	other.m_id;
		m_parentId	=	other.m_parentId;
		m_children	=	other.m_children;
		m_pMaxNode	=	other.m_pMaxNode;
		m_bMirrored	=	other.m_bMirrored;
	}
};

class ExpSkeleton  
{
public:
	ExpSkeleton();
	virtual ~ExpSkeleton();


	void SetShowInfo(bool show)	{	m_bShowInfo=show;}
	/**	build bone array from max export interface
	*/
	int BuildFromMaxExportInterface(ExpInterface *pEI);
	int GetNumRoot()	{	return m_rootNodes.size();}
	INode *GetRootNode(int i) { assert(i<m_rootNodes.size()); return m_expBones[m_rootNodes[i]].m_pMaxNode; }
	SkeletonFrame* CreateFrame(const TCHAR* szFrmName,TimeValue time);
	void CreateSkeletonStruct(SkeletonStruct& stut);
	
public:
	void Clear();
	void AddBoneNode(INode *pNode);
	void AddTagNode(INode *pNode);
	int FindNodeByName(const TCHAR *szName);
	INode *GetNode(int i);
protected:
	/**	build m_expBones
	*/
	void BuildBoneHierarchy();
	void ExpBone2F3DBone(size_t expBoneIndex,Cool3D::Bone& fbone,TimeValue time);
protected:
	bool			m_bShowInfo;
	vector<INode*>	m_maxBones;
	vector<ExpBone>	m_expBones;
	vector<int>		m_rootNodes;
	TagNodeExp		m_tagNodeExp;

};