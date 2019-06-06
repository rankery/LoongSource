#pragma once
#include <vector>
using namespace std;
#include "MathBaseDefine.h"
#include "..\MeshExporter\Math\GeomDefine.h"

namespace Cool3D
{
	class BintreeNode
	{
	public:
		BintreeNode();
		virtual ~BintreeNode();

		const AABBox& GetBox() const	{ return m_box; }

		void WriteFile(FILE *fp);

		static int maxContent;
		static int boxCount;
		static int boxTotal;
		static int maxLevel;
		static int noLeafCount;

	protected:
		int				m_lv;
		AABBox			m_box;
		bool			m_childrenID[8];

		//--
		vector<WORD>	m_content;
		BintreeNode*	m_pChildren[8];
	};
}//namespace Cool3D