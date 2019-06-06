#include "ActorKeyFrameFile.h"
#include "..\..\MaxPublic\MathBaseDefine.h"
#include "..\MsgBox.h"
#include "..\..\F3DMaxExp\SubMeshOptimize.h"
#include "..\ExpMtl.h"
#include "..\..\MaxPublic\StaticMeshFile.h"
#include <assert.h>

namespace Cool3D
{
	ActorKeyFrameFile::ActorKeyFrameFile()
		: m_firstFrame(0)
		, m_lastFrame(0)
		, m_bExportFirst(true)
	{
		
	}
	
	ActorKeyFrameFile::~ActorKeyFrameFile()
	{
		
	}

	VertFrame* ActorKeyFrameFile::LoadFrame(FILE *fp, UINT numVert, const VertFrame* preFrame)
	{
		VertFrame *pFrm=new VertFrame;
		pFrm->ReadFile(fp,preFrame,m_vertType);
		assert(pFrm->vertices.size()/GetVertSize(m_vertType)==numVert);
		return pFrm;
	}

	bool ActorKeyFrameFile::PreLoad(const TCHAR *szPath)
	{
		AKHeader header;
		FILE *fp=_tfopen(szPath,_T("rb"));
		if(fp == NULL)
			return false;
		fread(&header,sizeof(header),1,fp);
		
		assert(wcscmp(header.magic,AK_MAGIC) == 0);
		//skip load mesh section and mtl, so export mesh section every time
		m_vertType=header.vertType;

		VertFrame* pFrm=NULL;
		fseek(fp,header.offsetFrame,SEEK_SET);
		for(DWORD i=0;i<header.numFrame;i++)
		{
			const VertFrame *preFrame=NULL;
			if (i>0)
				preFrame=m_frameBuf[i-1];
			pFrm=LoadFrame(fp, header.numVert, preFrame);
			m_frameBuf.push_back(pFrm);
		}

		fclose(fp);

		return true;
	}
	
	void ActorKeyFrameFile::MergeActionFromSceneContext(DWORD action, INode *pParent, TimeValue start, TimeValue end, 
		int sampleRate)
	{
		RemoveAction(action);

		m_firstFrame = m_frameBuf.size();

		TimeValue step=SecToTicks(1.0f/sampleRate);
		float frmTime=1.0f/sampleRate;//TicksToSec(step);
		TimeValue time;
		for(time=start; time+step<end; time+=step)
		{
			if(time > end)
			{
				frmTime=TicksToSec(step - (end-time));
				assert(frmTime > 0);
			}
			AddFrameFromSceneContext(action,pParent,time,frmTime);
		}

		//强制输出最后一帧
		if(time < end)
		{
			frmTime=TicksToSec(end-time);
			// 最后一帧应该从time算起，经过frmTime，到达end [2/15/2008 leili]
			//AddFrameFromSceneContext(action,end,frmTime);
			AddFrameFromSceneContext(action, pParent, time, frmTime);
		}
		//AddFrameFromSceneContext(action, pParent, end, 1.0f/sampleRate);

		m_lastFrame = m_frameBuf.size() - 1;

		g_msgBox.ShowMsg(
			_T("Action=%s,numFrames=%d"),
			_FourCC2Str(action).c_str(),
			m_frameBuf.size());
	}

	void ActorKeyFrameFile::Prepare()
	{
		//--如果是新建, 则需要创建Sub Meshes
		if(m_subMeshs.size() == 0)
			GetSubMeshs();

		//--导出碰撞盒
		if( F3DExpMtl::s_bExportBox )
		{
			m_octree.SetBoxSize(F3DExpMtl::s_fCollideBoxSize);

			int index = 0;
			if( m_bExportFirst )
				index = m_firstFrame;
			else
				index = m_lastFrame;

			VertFrame *pFrm=m_frameBuf[index];
			m_octree.Begin(&(*pFrm->vertices.begin()), m_vertType);
			int oldMtlID=-1;
			for(UINT i=0;i<m_subMeshs.size();i++)
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
	
	bool ActorKeyFrameFile::Write(const TCHAR *szPath, INode *pNode)
	{
		if(m_frameBuf.size() <= 0)
			return false;

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

				for( size_t frameIndex=0; frameIndex<m_frameBuf.size(); frameIndex++ )
				{
					VertFrame* pVertFrame=m_frameBuf[frameIndex];

					UINT vertSize=GetVertSize(m_vertType);
					UINT vertCount=pVertFrame->vertices.size()/vertSize;
					switch( m_vertType )
					{
					case EVType_PNT:
						{
							Vert_PNT *tmp=NULL;
							for(UINT i=0;i<vertCount;i++)
							{
								tmp=(Vert_PNT *)(&pVertFrame->vertices[i*vertSize]);
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
								tmp=(Vert_PNGT *)(&pVertFrame->vertices[i*vertSize]);
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
								tmp=(Vert_PNT2 *)(&pVertFrame->vertices[i*vertSize]);
								tmp->pos=tmp->pos*mat;
								tmp->normal=tmp->normal*rotateMat;
								D3DXVec3Normalize(&tmp->normal,&tmp->normal);
							}
						}
						break;
					}
				}
			}
		}

		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp == NULL)
			return false;
		AKHeader header;
		memset(&header,0,sizeof(header));
		wcscpy(header.magic,AK_MAGIC);
		header.vertType=m_vertType;
		header.lightmapSize=F3DExpMtl::s_iLightMapSize;

		if( F3DExpMtl::s_bExportLMUV
			&& header.lightmapSize!=0
			&& header.lightmapSize!=32
			&& header.lightmapSize!=64
			&& header.lightmapSize!=128
			&& header.lightmapSize!=256
			&& header.lightmapSize!=512 )
		{
			g_msgBox.ShowMsg(_T("lightmap的大小必须是0、32、64、128、256、512中的一个值！\r\n"));
			return false;
		}

		VertFrame *pFrm=m_frameBuf[0];
		header.numVert=pFrm->vertices.size()/GetVertSize(m_vertType);

		fwrite(&header,sizeof(header),1,fp);

		DWORD i;
		//--sub meshes
		header.numMesh=m_subMeshs.size();
		header.offsetMesh=ftell(fp);
		for(i=0;i<header.numMesh;i++)
			m_subMeshs[i].WriteFile(fp);

		//--frames
		header.numFrame=m_frameBuf.size();
		header.offsetFrame=ftell(fp);
		if(header.numFrame > 0)
			GetBox(m_frameBuf[0],header.max,header.min);
		for(i=0;i<header.numFrame;i++)
		{
			pFrm=m_frameBuf[i];
			const VertFrame *preFrame=NULL;
			if (i>0)
				preFrame=m_frameBuf[i-1];
			pFrm->WriteFile(fp,preFrame,m_vertType);
			assert(header.numVert == pFrm->vertices.size()/GetVertSize(m_vertType));
		}
		
		//--mtl
		header.offsetMtl=ftell(fp);
		header.numMtl=WriteMtlFromSceneContext(fp,szPath);

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

				navHeader.ver=1;
				fwrite(&navHeader,sizeof(navHeader),1,fp);

				navHeader.offsetOctree=ftell(fp);
				OctreeNode::maxLevel=0;
				OctreeNode::noLeafCount=0;
				OctreeNode::boxCount=0;
				OctreeNode::maxContent=0;
				OctreeNode::boxTotal=0;
				m_octree.WriteFile(fp);
				g_msgBox.ShowMsg(_T("八叉树统计信息---------------------------------------\r\n"));
				g_msgBox.ShowMsg(_T("八叉树--碰撞盒总数：%d\r\n"),m_octree.GetNumBox());
				g_msgBox.ShowMsg(_T("八叉树--最大层数：%d\r\n"),OctreeNode::maxLevel);
				g_msgBox.ShowMsg(_T("八叉树--非叶子节点总数：%d\r\n"),OctreeNode::noLeafCount);
				g_msgBox.ShowMsg(_T("八叉树--叶子节点总数：%d\r\n"),OctreeNode::boxCount);
				g_msgBox.ShowMsg(_T("八叉树--叶子节点拥有的最大AABBox数量：%d\r\n"),OctreeNode::maxContent);
				g_msgBox.ShowMsg(_T("八叉树--引用索引总数：%d\r\n"),OctreeNode::boxTotal);
				g_msgBox.ShowMsg(_T("八叉树统计信息---------------------------------------\r\n"));

				//--header
				fseek(fp,0,SEEK_SET);
				fwrite(&navHeader,sizeof(navHeader),1,fp);

				fclose(fp);
			}
		}

		return true;
	}

	void ActorKeyFrameFile::GetTrackSet(set<tstring>& out)
	{
		VertFrame *pFrm;
		for(size_t i=0;i<m_frameBuf.size();i++)
		{
			pFrm=m_frameBuf[i];
			out.insert(_FourCC2StrA(pFrm->name).c_str());
		}
	}
	
	void ActorKeyFrameFile::RemoveAction(DWORD action)
	{
		VertFrame *pFrm;
		for(size_t i=0;i<m_frameBuf.size();)
		{
			pFrm=m_frameBuf[i];
			if(pFrm->name == action)
				m_frameBuf.erase(m_frameBuf.begin()+i);
			else
				i++;
		}
		
	}
	
	void ActorKeyFrameFile::AddFrameFromSceneContext(DWORD action, INode *pParent, TimeValue time, float frmTime)
	{
		SceneContext::Inst()->ClearExportData();
		SceneContext::Inst()->ExportData(time);
		VertFrame *pFrm=new VertFrame;
		pFrm->name=action;
		pFrm->time=frmTime;
		for(int i=0;i<SceneContext::Inst()->GetNumMeshs();i++)
		{
			ExpMesh *pMesh=SceneContext::Inst()->GetMesh(i);
			GetVert(pMesh, pFrm->vertices);
		}
		SceneContext::Inst()->GetTagNodeExp()->ExportFrameAni(pFrm->tagNodes,pParent,time);
		m_frameBuf.push_back(pFrm);
	}

	void ActorKeyFrameFile::GetSubMeshs()
	{
		std::vector<SubMesh>	tmpMeshs;
		int i;
		WORD baseIndex=0;
		for(i=0;i<SceneContext::Inst()->GetNumMeshs();i++)
		{
			ExpMesh *pMesh=SceneContext::Inst()->GetMesh(i);
			SceneWriter::GetSubMeshs(baseIndex,pMesh->GetNumVert(),pMesh,tmpMeshs);
			baseIndex+=pMesh->GetNumVert();
		}

		//--
		m_subMeshs.clear();
		for(size_t j=0;j<tmpMeshs.size();j++)
		{
			LodSubMesh tm;
			SceneWriter::BuildLodSubMesh(tmpMeshs[j],tm);
			SubMeshOptimize(&tm);
			m_subMeshs.push_back(tm);
		}
		
	}

	void ActorKeyFrameFile::GetBox(VertFrame *pFrm,Vector3& max,Vector3& min)
	{
		DWORD vertCount=pFrm->vertices.size()/GetVertSize(m_vertType);

		max.x=max.y=max.z=-FLOAT_MAX;
		min.x=min.y=min.z=FLOAT_MAX;

		Vector3 pos;
		for(size_t i=0;i<vertCount;i++)
		{
			GetPos(&(*pFrm->vertices.begin()), i, pos);
			if(pos.x>max.x)
				max.x=pos.x;
			if(pos.y>max.y)
				max.y=pos.y;
			if(pos.z>max.z)
				max.z=pos.z;

			if(pos.x<min.x)
				min.x=pos.x;
			if(pos.y<min.y)
				min.y=pos.y;
			if(pos.z<min.z)
				min.z=pos.z;
		}
	}
	
}//namespace Cool3D
