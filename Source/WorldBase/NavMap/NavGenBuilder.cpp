#include "stdafx.h"
#include "NavGenBuilder.h"
#include "NavGenNodeBuilder.h"
#include "..\..\Cool3D\Public\StaticMeshFile.h"

namespace WorldBase
{
	NavGenBuilder::NavGenBuilder()
		: m_pVB(0)
		, m_boxSize(EBS_Common)
		, m_pRoot(0)
	{

	}

	NavGenBuilder::~NavGenBuilder()
	{
		SAFE_DELETE(m_pRoot);
	}

	void NavGenBuilder::Build( VertStream *pVB,vector<WORD>& ib )
	{
		int numTri = ib.size()/3;
		ASSERT( numTri*3==ib.size() );

		m_pVB = pVB;

		m_pRoot = new NavGenNodeBuilder;
		for(int i=0; i<numTri; i++)
		{
			tagNode node;
			BuildAABBox( &ib[i*3], node );
			m_nodes.push_back(node);
		}

		m_pRoot->Build(m_nodes,m_boxes,0,0,m_boxSize);
	}

	void NavGenBuilder::BuildAABBox( WORD *pIB,tagNode& node )
	{
		node.box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		node.box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		GetPos(pIB[0],node.a);
		GetPos(pIB[1],node.b);
		GetPos(pIB[2],node.c);

		Vector3 pos;
		for(int j=0;j<3;j++)
		{
			GetPos(pIB[j], pos);
			if(pos.x > node.box.max.x)
				node.box.max.x=pos.x;
			if(pos.y > node.box.max.y)
				node.box.max.y=pos.y;
			if(pos.z > node.box.max.z)
				node.box.max.z=pos.z;

			if(pos.x < node.box.min.x)
				node.box.min.x=pos.x;
			if(pos.y < node.box.min.y)
				node.box.min.y=pos.y;
			if(pos.z < node.box.min.z)
				node.box.min.z=pos.z;
		}

		//--盒子的最小尺寸为1
		if(fabsf(node.box.max.x-node.box.min.x)<1.0e-6f)
			node.box.max.x=node.box.min.x+1.0f;
		if(fabsf(node.box.max.y-node.box.min.y)<1.0e-6f)
			node.box.max.y=node.box.min.y+1.0f;
		if(fabsf(node.box.max.z-node.box.min.z)<1.0e-6f)
			node.box.max.z=node.box.min.z+1.0f;
	}

	void NavGenBuilder::GetPos( UINT i,Vector3& out )
	{
		switch(m_pVB->GetType())
		{
		case EVType_P:
			{
				Vert_P *pV=(Vert_P *)m_pVB->GetRawBuffer();
				pV+=i;
				out=pV->pos;
			}
			break;
		}
	}

	void NavGenBuilder::WriteFile( LPCTSTR szFile )
	{
		FILE* fp=_tfopen(szFile,_T("wb"));
		if(fp == NULL)
			return;

		SceneMeshNavHeader navHeader;
		memset(&navHeader,0,sizeof(navHeader));
		wcscpy(navHeader.magic,SceneMeshNav_Magic);

		navHeader.ver=2;
		fwrite(&navHeader,sizeof(navHeader),1,fp);

		navHeader.offsetOctree=ftell(fp);

		FWriteValVector(fp,m_boxes);
		m_pRoot->WriteFile(fp);

		//--header
		fseek(fp,0,SEEK_SET);
		fwrite(&navHeader,sizeof(navHeader),1,fp);

		fclose(fp);
	}
}