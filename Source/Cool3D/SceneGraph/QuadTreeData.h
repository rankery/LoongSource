#pragma once
#include "..\Util\FArchiveAdv.h"

class IFS;
namespace Cool3D
{
	class SGQuadTreeNode;
	/**	\class QuadTreeData
		\brief 管理QuadTree Scene Graph的磁盘数据,可以随时load某个node
	*/
	class QuadTreeData
	{
	protected:
		struct tagItem
		{
			int		id;	//QuadTreeNode的id
			DWORD	offset;//此Node在文件中的offset
		};
		struct tagHeader
		{
			BYTE	magic[4];
			BYTE	ver;
			DWORD	numItem;
			int		numNode;
			DWORD	itemOffset;
		};
	public:
		QuadTreeData(void);
		virtual ~QuadTreeData(void);

		void OpenFromFile(IFS* pFS,const TCHAR* szFileName);
		SGQuadTreeNode*	LoadNode(int id);
		void Close();

		int GetNumNode(){ return m_numNode;}
	protected:
		FArchiveAdv			m_ar;
		map<int,DWORD>		m_itemMap;//id,offset
		int					m_numNode;
	};
}//namespace Cool3D