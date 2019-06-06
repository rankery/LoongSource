#pragma once

namespace WorldBase
{
	class NavGenNodeBuilder;

	class NavGenBuilder
	{
	public:
		struct tagNode 
		{
			Vector3	a;
			Vector3	b;
			Vector3 c;
			AABBox	box;
		};
		enum EBoxSize
		{
			EBS_Common	= 0,	// ∆’Õ®
			EBS_Small,			// –°
			EBS_Big,			// ¥Û
		};
	public:
		NavGenBuilder();
		virtual ~NavGenBuilder();

		void Build(VertStream *pVB,vector<WORD>& ib);

		void SetBoxSize(EBoxSize eBoxSize) { m_boxSize = eBoxSize; }

		void WriteFile(LPCTSTR szFile);

		//--
		int GetNumBox() { return m_boxes.size(); }

	private:
		void GetPos(UINT i,Vector3& out);
		void BuildAABBox(WORD *pIB,tagNode& node);

	public:
		vector<AABBox>		m_boxes;

	private:
		vector<tagNode>		m_nodes;
		NavGenNodeBuilder	*m_pRoot;
		VertStream			*m_pVB;
		EBoxSize			m_boxSize;
	};
}