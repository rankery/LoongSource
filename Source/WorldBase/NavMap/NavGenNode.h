#pragma once

namespace WorldBase
{
	class NavGenNode
	{
		enum { EChildNum = 2 };
	public:
		NavGenNode();
		virtual ~NavGenNode();

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
		bool			m_childrenID[EChildNum];

		//--
		vector<WORD>	m_content;
		NavGenNode*		m_pChildren[EChildNum];
	};
}// WorldBase