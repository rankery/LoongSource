#include "StdAfx.h"
#include ".\navquadtreedata.h"

#define VFS_READ
#include "..\..\Cool3D\Public\FileUtil.h"

#include "NavSGQuadTreeNode.h"
#include "NavQuadTreeSG.h"

namespace WorldBase
{
	NavQuadTreeData::NavQuadTreeData(void)
	{
	}

	NavQuadTreeData::~NavQuadTreeData(void)
	{
	}

	void NavQuadTreeData::OpenFromFile(IFS* pFS,const TCHAR* szFileName)
	{
		m_ar.OpenForRead(pFS,szFileName);
		DWORD hFile=m_ar.GetReadFile();

		//--read header
		tagHeader header;
		FReadValue(pFS,hFile,header);

		//--read all items
		pFS->Seek(hFile,header.itemOffset,FILE_BEGIN);
		for(int i=0;i<(int)header.numItem;i++)
		{
			tagItem item;
			FReadValue(pFS,hFile,item);
			m_itemMap[item.id]=item.offset;
		}
	}

	NavSGQuadTreeNode*	NavQuadTreeData::LoadNode(int id,NavQuadTreeSG* pSG)
	{
		map<int,DWORD>::iterator iter=m_itemMap.find(id);
		if(iter==m_itemMap.end())
			return NULL;//not find id
		
		//--seek archive
		DWORD offset=iter->second;
		IFS *pFS=m_ar.GetReadFS();
		DWORD hFile=m_ar.GetReadFile();
		ASSERT(pFS!=NULL);
		ASSERT(hFile!=NULL);
		pFS->Seek(hFile,offset,FILE_BEGIN);

		//--load form archive
		NavSGQuadTreeNode *pNewNode=new NavSGQuadTreeNode(pSG);
		pNewNode->Deserialize(m_ar);
		return pNewNode;
	}

	void NavQuadTreeData::Close()
	{
		m_ar.Close();
		m_itemMap.clear();
	}
}//namespace WorldBase