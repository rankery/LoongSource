#include "StaticMeshWriter.h"
#include <assert.h>
#include "..\MsgBox.h"
#include "..\Math\Max2D3D.h"
#include "..\..\MaxPublic\StaticMeshFile.h"
#include "..\..\F3DMaxExp\SubMeshOptimize.h"
#include "..\ExpMtl.h"

namespace Cool3D
{
	StaticMeshWriter::StaticMeshWriter()
		: m_pOriginNode(0)
	{
		max.x=max.y=max.z=-FLOAT_MAX;
		min.x=min.y=min.z=FLOAT_MAX;
	}
	
	StaticMeshWriter::~StaticMeshWriter()
	{
		m_pOriginNode=NULL;
	}
	
	void StaticMeshWriter::Prepare()
	{
		m_vertBuf.clear();
		m_subMeshs.clear();

		//--
		for(int i=0;i<SceneContext::Inst()->GetNumMeshs();i++)
		{
			ExpMesh *pMesh=SceneContext::Inst()->GetMesh(i);
			WORD baseIndex=m_vertBuf.size()/GetVertSize(m_vertType);
			GetVertices(pMesh);
			GetSubMeshs(baseIndex,pMesh->GetNumVert(),pMesh);
		}

		SceneContext::Inst()->GetTagNodeExp()->ExportFrameAni(m_tagNodeFrm,m_pOriginNode,0);

		if( F3DExpMtl::s_bExportBox )
		{
			m_octree.SetBoxSize(F3DExpMtl::s_fCollideBoxSize);

			m_octree.Begin(&(*m_vertBuf.begin()), m_vertType);
			int oldMtlID=-1;
			for(i=0;i<m_subMeshs.size();i++)
			{
				LodSubMesh& subMesh=m_subMeshs[i];
				//--镂空材质不生成octree和aabbtree
				if(subMesh.mtlID>=0)
				{
					tagMtlDesc mtl;
					SceneContext::Inst()->GetMtl(subMesh.mtlID)->GetMtlDesc(mtl);
					if(mtl.m_bOpcityMap
						&& !F3DExpMtl::s_bExportOpcity)
						continue;

					if( subMesh.mtlID <= oldMtlID )//不同对象，使用不同的顶点集合
					{
						m_octree.AddSubMesh(&(*subMesh.baseFaces.begin()),subMesh.baseFaces.size(),subMesh.numVert);
					}
					else
					{
						oldMtlID = subMesh.mtlID;
						m_octree.AddSubMesh(&(*subMesh.baseFaces.begin()),subMesh.baseFaces.size(),0);
					}
				}
			}

			g_msgBox.ShowMsg(_T("Begin Export Octree........\r\n"));
			m_octree.End();
			g_msgBox.ShowMsg(_T("End Export Octree.\r\n"));
		}
	}
	
	bool StaticMeshWriter::Write(const TCHAR *szPath, INode *pNode)
	{
		m_pOriginNode=pNode;
		Prepare();

		if( pNode!=NULL )	// 变换模型顶点的矩阵
		{
			D3DXMATRIX mat;
			if( SceneContext::Inst()->GetMat(pNode,mat) )
			{
				D3DXMATRIX rotateMat=mat;
				rotateMat.m[3][0]=0.0f;
				rotateMat.m[3][1]=0.0f;
				rotateMat.m[3][2]=0.0f;

				UINT vertSize=GetVertSize(m_vertType);
				UINT vertCount=m_vertBuf.size()/vertSize;
				switch( m_vertType )
				{
				case EVType_PNT:
					{
						Vert_PNT *tmp=NULL;
						for(UINT i=0;i<vertCount;i++)
						{
							tmp=(Vert_PNT *)(&m_vertBuf[i*vertSize]);
							tmp->pos=tmp->pos*mat;
							tmp->normal=tmp->normal*rotateMat;
							D3DXVec3Normalize(&tmp->normal,&tmp->normal);
						}
					}
					break;
				case EVType_PNGT:
					{
						Vert_PNGT *tmp=NULL;
						for(UINT i=0;i<vertCount;i++)
						{
							tmp=(Vert_PNGT *)(&m_vertBuf[i*vertSize]);
							tmp->pos=tmp->pos*mat;
							tmp->normal=tmp->normal*rotateMat;
							tmp->tangent=tmp->tangent*rotateMat;
							D3DXVec3Normalize(&tmp->normal,&tmp->normal);
							D3DXVec3Normalize(&tmp->tangent,&tmp->tangent);
						}
					}
					break;
				case EVType_PNT2:
					{
						Vert_PNT2 *tmp=NULL;
						for(UINT i=0;i<vertCount;i++)
						{
							tmp=(Vert_PNT2 *)(&m_vertBuf[i*vertSize]);
							tmp->pos=tmp->pos*mat;
							tmp->normal=tmp->normal*rotateMat;
							D3DXVec3Normalize(&tmp->normal,&tmp->normal);
						}
					}
					break;
				}
			}
		}

		size_t i;
		int sum=0;
		for(i=0;i<m_subMeshs.size();i++)
		{
			sum+=m_subMeshs[i].baseFaces.size();
		}
		g_msgBox.ShowMsg(_T("Simple Mesh: numVert=%d,numSubMeshs=%d,numFaces=%d,numTagNodes=%d\r\n")
			,m_vertBuf.size()/GetVertSize(m_vertType),m_subMeshs.size(),sum,m_tagNodeFrm.nodes.size());

		FILE* fp=_tfopen(szPath,_T("wb"));
		if(fp == NULL)
			return false;

		ComputeAABBox();
		
		StaticMeshHeader header;
		memset(&header,0,sizeof(header));
		wcscpy(header.magic,StaticMesh_Magic);
		header.ver=1;
		header.vertType=m_vertType;
		header.numVert=m_vertBuf.size()/GetVertSize(m_vertType);
		header.numMesh=m_subMeshs.size();
		header.numTagNode=m_tagNodeFrm.nodes.size();
		header.min=min;
		header.max=max;
		header.lightmapSize=F3DExpMtl::s_iLightMapSize;

		if( F3DExpMtl::s_bExportLMUV
			&& header.lightmapSize!=0
			&& header.lightmapSize!=32
			&& header.lightmapSize!=64
			&& header.lightmapSize!=128
			&& header.lightmapSize!=256
			&& header.lightmapSize!=512
			&& header.lightmapSize!=1024)
		{
			g_msgBox.ShowMsg(_T("lightmap的大小必须是0、32、64、128、256、512中、1024的一个值！\r\n"));
			return false;
		}

		//--pre write header
		fwrite(&header,sizeof(header),1,fp);
		
		//--context
		header.offsetVert=ftell(fp);
		fwrite(&(*m_vertBuf.begin()),GetVertSize(header.vertType),header.numVert,fp);

		header.offsetMesh=ftell(fp);
		for(i=0;i<header.numMesh;i++)
			m_subMeshs[i].WriteFile(fp);

		header.offsetMtl=ftell(fp);
		header.numMtl=WriteMtlFromSceneContext(fp,szPath);

		header.offsetTagNode=ftell(fp);
		m_tagNodeFrm.WriteFile(fp);
				
		//--header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);

		fclose(fp);

		//--导出Nav用于碰撞检测
		if( F3DExpMtl::s_bExportBox )
		{
			if( m_octree.GetNumBox() == 0 )//没有碰撞盒
			{
				g_msgBox.ShowMsg(_T("该模型没有碰撞盒，可能全部是镂空材质，需要勾选选项后重新导出！\r\n"));
				return false;
			}
			else
			{
				TCHAR szDrive[512];
				TCHAR szDir[512];
				TCHAR szFilePath[512];
				TCHAR szNavPath[512];
				_tsplitpath(szPath,szDrive,szDir,szFilePath,NULL);
				_stprintf(szNavPath,_T("%s%s%s.Nav"),szDrive,szDir,szFilePath);
				fp=_tfopen(szNavPath,_T("wb"));
				if(fp == NULL)
					return false;

				SceneMeshNavHeader navHeader;
				memset(&navHeader,0,sizeof(navHeader));
				wcscpy(navHeader.magic,SceneMeshNav_Magic);

				//navHeader.ver=1;
				navHeader.ver=2;
				fwrite(&navHeader,sizeof(navHeader),1,fp);

				navHeader.offsetOctree=ftell(fp);
				OctreeNode::maxLevel=0;
				OctreeNode::noLeafCount=0;
				OctreeNode::boxCount=0;
				OctreeNode::maxContent=0;
				OctreeNode::boxTotal=0;
				m_octree.WriteFile(fp);
				//g_msgBox.ShowMsg(_T("八叉树统计信息---------------------------------------\r\n"));
				//g_msgBox.ShowMsg(_T("八叉树--碰撞盒总数：%d\r\n"),m_octree.GetNumBox());
				//g_msgBox.ShowMsg(_T("八叉树--最大层数：%d\r\n"),OctreeNode::maxLevel);
				//g_msgBox.ShowMsg(_T("八叉树--非叶子节点总数：%d\r\n"),OctreeNode::noLeafCount);
				//g_msgBox.ShowMsg(_T("八叉树--叶子节点总数：%d\r\n"),OctreeNode::boxCount);
				//g_msgBox.ShowMsg(_T("八叉树--叶子节点拥有的最大AABBox数量：%d\r\n"),OctreeNode::maxContent);
				//g_msgBox.ShowMsg(_T("八叉树--引用索引总数：%d\r\n"),OctreeNode::boxTotal);
				//g_msgBox.ShowMsg(_T("八叉树统计信息---------------------------------------\r\n"));
				g_msgBox.ShowMsg(_T("二叉树统计信息---------------------------------------\r\n"));
				g_msgBox.ShowMsg(_T("二叉树--碰撞盒总数：%d\r\n"),m_octree.GetNumBox());
				g_msgBox.ShowMsg(_T("二叉树--最大层数：%d\r\n"),OctreeNode::maxLevel);
				g_msgBox.ShowMsg(_T("二叉树--非叶子节点总数：%d\r\n"),OctreeNode::noLeafCount);
				g_msgBox.ShowMsg(_T("二叉树--叶子节点总数：%d\r\n"),OctreeNode::boxCount);
				g_msgBox.ShowMsg(_T("二叉树--叶子节点拥有的最大AABBox数量：%d\r\n"),OctreeNode::maxContent);
				g_msgBox.ShowMsg(_T("二叉树--引用索引总数：%d\r\n"),OctreeNode::boxTotal);
				g_msgBox.ShowMsg(_T("二叉树统计信息---------------------------------------\r\n"));

				//--header
				fseek(fp,0,SEEK_SET);
				fwrite(&navHeader,sizeof(navHeader),1,fp);

				fclose(fp);
			}
		}
		
		return true;
	}
	
	void StaticMeshWriter::GetSubMeshs(WORD baseIndex,WORD numVert,ExpMesh *pMesh)
	{
		std::vector<SubMesh> tmp;
		SceneWriter::GetSubMeshs(baseIndex,numVert,pMesh,tmp);
		for(size_t i=0;i<tmp.size();i++)
		{
			LodSubMesh tm;
			BuildLodSubMesh(tmp[i],tm);
			SubMeshOptimize(&tm);
			m_subMeshs.push_back(tm);
		}
	}
	
	void StaticMeshWriter::GetVertices(ExpMesh *pMesh)
	{
		GetVert(pMesh,m_vertBuf);
	}
	
	void StaticMeshWriter::ComputeAABBox()
	{
		//以这样的顶点格式作为依据计算：位置坐标总是在格式的最前面
		max.x=max.y=max.z=-FLOAT_MAX;
		min.x=min.y=min.z=FLOAT_MAX;

		DWORD vertCount=m_vertBuf.size()/GetVertSize(m_vertType);

		Vector3 pos;
		for(DWORD i=0;i<vertCount;i++)
		{
			GetPos((BYTE *)(&(*m_vertBuf.begin())), i, pos);
			if(pos.x > max.x)
				max.x=pos.x;
			if(pos.y > max.y)
				max.y=pos.y;
			if(pos.z > max.z)
				max.z=pos.z;

			if(pos.x < min.x)
				min.x=pos.x;
			if(pos.y < min.y)
				min.y=pos.y;
			if(pos.z < min.z)
				min.z=pos.z;
		}
	}
}//namespace Cool3D