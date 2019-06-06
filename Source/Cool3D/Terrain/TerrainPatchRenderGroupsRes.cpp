#include "StdAfx.h"
#include ".\terrainpatchrendergroupsres.h"
#include "..\Util\Exception.h"
#include "..\VFS\FileUtil.h"
#include ".\TPatchTriangulationStatic.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI(TerrainPatchRenderGroupsRes,ResRenderable);

	TerrainPatchRenderGroupsRes::TerrainPatchRenderGroupsRes(const TCHAR* szName):ResRenderable(szName)
	{
		m_pIB=NULL;
	}

	TerrainPatchRenderGroupsRes::~TerrainPatchRenderGroupsRes(void)
	{
		SAFE_DELETE(m_pIB);
	}

	UINT TerrainPatchRenderGroupsRes::CreateContent(DWORD param)
	{
		IFS *pFS=GetSafeFS();
		const TCHAR* szName=GetName();
		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("Terrain patch load faied"),szName);
		
		//--read header
		tagHeader header;
		pFS->Read(&header,sizeof(tagHeader),hFile);

		//--load index
		pFS->Seek(hFile,header.indexOffset,FILE_BEGIN);
		vector<WORD> indices;
		FReadValVector(pFS,hFile,indices);

		//--load render groups
		pFS->Seek(hFile,header.renderGroupOffset,FILE_BEGIN);
		int i;
		for(i=0;i<(int)header.numRenderGroup;i++)
		{
			tagRenderGroup renderGroup;
			FReadValVector(pFS,hFile,renderGroup.grids);
			FReadValVector(pFS,hFile,renderGroup.layers);
			FReadString(pFS,hFile,renderGroup.szAlphaMap);
			FReadValue(pFS,hFile,renderGroup.startIndex);
			FReadValue(pFS,hFile,renderGroup.numFace);
			m_renderGroups.push_back(renderGroup);
		}

		//--´´½¨IndexBuffer
		m_pIB=Device()->NewIB();
		m_pIB->CreateWord(indices.size()*sizeof(WORD),EB_StaticWriteOnly);
		m_pIB->CopyFromMem(&indices[0],indices.size()*sizeof(WORD));

		//--close file
		pFS->Close(hFile);

		return ResRenderable::CreateContent(param);
	}

	IIndexBuffer* TerrainPatchRenderGroupsRes::GetIB() const
	{
		return m_pIB;
	}

	int TerrainPatchRenderGroupsRes::GetNumRenderGroups() const
	{
		return (int)m_renderGroups.size();
	}

	const TerrainPatchRenderGroupsRes::tagRenderGroup& TerrainPatchRenderGroupsRes::GetRenderGroup(int group) const
	{
		ASSERT(group>=0 && group<(int)m_renderGroups.size());
		return m_renderGroups[group];
	}

	UINT TerrainPatchRenderGroupsRes::GetNumRenderNode() const
	{
		return 0;
	}

	RenderNode *TerrainPatchRenderGroupsRes::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const AABBox *TerrainPatchRenderGroupsRes::GetAABBox() const
	{
		return NULL;
	}

	bool TerrainPatchRenderGroupsRes::EditorSave(const TCHAR* szPath,RECT myRect,
		vector<tagRenderGroup>& renderGroups,TerrainEditor *pEditor)
	{
		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		tagHeader header;
		memset(&header,0,sizeof(tagHeader));
		_tcscpy((TCHAR*)header.magic,_T("RG"));

		fwrite(&header,sizeof(header),1,fp);

		int i;

		//--index
		vector<WORD> indices;
		BuildIndices(renderGroups,indices);
		header.indexOffset=ftell(fp);
		FWriteValVector(fp,indices);

		//--render groups
		header.numRenderGroup=(int)renderGroups.size();
		header.renderGroupOffset=ftell(fp);
		for(i=0;i<(int)renderGroups.size();i++)
		{
			tagRenderGroup& renderGroup=renderGroups[i];
			FWriteValVector(fp,renderGroup.grids);
			FWriteValVector(fp,renderGroup.layers);
			FWriteString(fp,renderGroup.szAlphaMap.c_str());
			FWriteValue(fp,renderGroup.startIndex);
			FWriteValue(fp,renderGroup.numFace);
		}

		//»ØÐ´Header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		
		fclose(fp);
		return true;
	}

	void TerrainPatchRenderGroupsRes::BuildIndices(vector<tagRenderGroup>& renderGroups,vector<WORD>& indices)
	{
		for(size_t i=0;i<renderGroups.size();i++)
		{
			tagRenderGroup& renderGroup=renderGroups[i];
			renderGroup.startIndex=indices.size();
			TPatchTriangulationStatic::Inst()->GetIndices(0,renderGroup.grids,indices,renderGroup.numFace);
		}
	}

}//namespace Cool3D