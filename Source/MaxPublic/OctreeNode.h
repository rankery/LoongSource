#pragma once
#include <vector>
using namespace std;
#include "MathBaseDefine.h"
#include "..\MeshExporter\Math\GeomDefine.h"

namespace Cool3D
{
	class OctreeNode
	{
	public:
		OctreeNode();
		virtual ~OctreeNode();

		int GetID() const				{ return m_id;  }
		const AABBox& GetBox() const	{ return m_box; }

		void WriteFile(FILE *fp);

		static int maxContent;
		static int boxCount;
		static int boxTotal;
		static int maxLevel;
		static int noLeafCount;

	protected:
		int				m_id;
		int				m_lv;
		AABBox			m_box;
		bool			m_childrenID[2];

		//--
		vector<WORD>	m_content;
		OctreeNode*		m_pChildren[2];
	};
}//namespace Cool3D