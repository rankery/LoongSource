#include "StdAfx.h"
#include ".\edgecollapse.h"

namespace F3D
{
	EdgeCollapse::EdgeCollapse(void)
	{
	}

	EdgeCollapse::~EdgeCollapse(void)
	{
	}

	void EdgeCollapse::BuildTriListMesh(VertStream *pVertStream,WORD *pIndices,UINT numTri)
	{

		m_pVertStream=pVertStream;
	}

	void EdgeCollapse::Simplify(float maxCost)
	{}

	void EdgeCollapse::GetIndices(vector<WORD>& indices)
	{}

	void EdgeCollapse::CalCost(tagEdge& edge)
	{
	}
}//namespace F3D