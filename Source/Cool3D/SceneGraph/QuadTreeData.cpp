#include "StdAfx.h"
#include ".\quadtreedata.h"
#include "..\VFS\IFS.h"

#define VFS_READ
#include "..\Public\FileUtil.h"

#include "SGQuadTreeNode.h"

namespace Cool3D
{
	QuadTreeData::QuadTreeData(void)
	{
		m_numNode=0;
	}

	QuadTreeData::~QuadTreeData(void)
	{
	}

	void QuadTreeData::OpenFromFile(IFS* pFS,const TCHAR* szFileName)
	{
		m_ar.OpenForRead(pFS,szFileName);
		DWORD hFile=m_ar.GetReadFile();

		//--read header
		tagHeader header;
		FReadValue(pFS,hFile,header);
		m_numNode=header.numNode;

		//--read all items
		pFS->Seek(hFile,header.itemOffset,FILE_BEGIN);
		for(int i=0;i<(int)header.numItem;i++)
		{
			tagItem item;
			FReadValue(pFS,hFile,item);
			m_itemMap[item.id]=item.offset;
		}
	}

	SGQuadTreeNode*	QuadTreeData::LoadNode(int id)
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
		SGQuadTreeNode *pNewNode=new SGQuadTreeNode;
		pNewNode->Deserialize(m_ar);
		return pNewNode;
	}

	void QuadTreeData::Close()
	{
		m_ar.Close();
		m_itemMap.clear();
	}
}//namespace Cool3D