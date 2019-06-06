#include "stdafx.h"
#include "EditorDecoFile.h"
#define VFS_READ
#include "..\VFS\IFS.h"
#include "..\Kernel.h"
#include "..\Public\FileUtil.h"
#include "..\Util\DebugMisc.h"
#include "..\Math\math.h"

namespace Cool3D
{
	EditorDecoFile::EditorDecoFile()
	{
		//
	}

	EditorDecoFile::~EditorDecoFile()
	{
		Clear();
	}

	void EditorDecoFile::Clear()
	{
		for(NodeIter iter=m_decoNodeMap.begin();iter!=m_decoNodeMap.end();++iter)
		{
			DecoNode *pNode=iter->second;
			SAFE_DELETE(pNode);
		}
		m_decoNodeMap.clear();
	}

	bool EditorDecoFile::Save( const TCHAR *szFile )
	{
		FILE *fp=_tfopen(szFile,_T("wb"));
		if(fp!=NULL)
		{
			UINT numNode=m_decoNodeMap.size();
			tagHead header;
			header.numDeco=0;
			FWriteValue(fp,header);

			if(numNode>0)
			{
				FWriteValue(fp,numNode);
				for(NodeIter iter=m_decoNodeMap.begin();
					iter!=m_decoNodeMap.end();++iter)
				{
					DecoNode *pNode=iter->second;
					ASSERT(pNode!=NULL);

					UINT numChild=pNode->childMap.size();
					if(numChild>0)
					{
						FWriteValue(fp,iter->first);
						FWriteValue(fp,numChild);
						for(ChildIter childIter=pNode->childMap.begin();
							childIter!=pNode->childMap.end();++childIter)
						{
							FWriteValue(fp,childIter->first);
							FWriteValue(fp,childIter->second);
						}

						header.numDeco+=numChild;
					}
				}
			}

			//--回写tagHead信息
			fseek(fp,0,SEEK_SET);
			FWriteValue(fp,header);

			fclose(fp);
		}
		else
		{
			Kernel::Inst()->SetLastError(_T("deco map save failed,name=%s"),szFile);
			return false;
		}

		return true;
	}

	void EditorDecoFile::Load( IFS *pFS,const TCHAR *szFile )
	{
		ASSERT(pFS!=NULL);

		Clear();

		DWORD hFile=pFS->Open(szFile);
		if(hFile!=NULL)
		{
			tagHead header;
			FReadValue(pFS,hFile,header);
			if(header.numDeco>0)
			{
				UINT numNode;
				FReadValue(pFS,hFile,numNode);
				for(UINT i=0;i<numNode;i++)
				{
					float zVal;
					FReadValue(pFS,hFile,zVal);
					DecoNode *pNode=new DecoNode;
					m_decoNodeMap.insert(NodePair(zVal,pNode));

					UINT numChild;
					FReadValue(pFS,hFile,numChild);
					for(UINT j=0;j<numChild;j++)
					{
						float xVal;
						tagSRInfo srInfo;
						FReadValue(pFS,hFile,xVal);
						FReadValue(pFS,hFile,srInfo);
						pNode->childMap.insert(ChildPair(xVal,srInfo));
					}
				}
			}
			pFS->Close(hFile);
		}
		else
		{
			Kernel::Inst()->SetLastError(_T("deco density map load failed,name=%s"),szFile);
		}
	}

	bool EditorDecoFile::GetGrass( const Vector2& leftTop,const Vector2& rightBottom,vector<tagEditorDecoInfo>& out )
	{
		float top=leftTop.y;
		float bottom=rightBottom.y;
		float left=leftTop.x;
		float right=rightBottom.x;
		ASSERT(top<bottom || left<right);

		NodeIter topIter=m_decoNodeMap.lower_bound(top);
		NodeIter bottomIter=m_decoNodeMap.lower_bound(bottom);
		if(topIter==bottomIter)
			return false;

		for(NodeIter iter=topIter;iter!=bottomIter;++iter)
		{
			DecoNode *pNode=iter->second;
			ChildIter leftIter=pNode->childMap.lower_bound(left);
			ChildIter rightIter=pNode->childMap.lower_bound(right);
			if(leftIter==rightIter)
				continue;

			for(ChildIter childIter=leftIter;childIter!=rightIter;++childIter)
			{
				tagEditorDecoInfo decoInfo;
				decoInfo.pos.x=childIter->first;
				decoInfo.pos.y=iter->first;
				decoInfo.pos.z=childIter->second.height;
				decoInfo.sizeRandom=childIter->second.sizeRandom;
				decoInfo.rotateRandom=childIter->second.rotateRandom;
				out.push_back(decoInfo);
			}
		}

		return true;
	}

	void EditorDecoFile::AddGrass( const Vector2& leftTop,const Vector2& rightBottom,UINT density,float height )
	{
		//浓度==0时，不变
		if(density==0)
			return;

		//在指定范围(leftTop,rightBottom)内根据density随机生成装饰物的位置，插入到m_decoNodeMap
		//返回指定范围内装饰物的位置的集合
		float top=leftTop.y;
		float bottom=rightBottom.y;
		float left=leftTop.x;
		float right=rightBottom.x;
		ASSERT(top<bottom || left<right);
		float tileW=right-left;
		float tileH=bottom-top;

		//浓度>0时，随机生成DecoNode加入m_decoNodeMap
		UINT num=GetRandom(0,15-density/17);
		if (num>0)
			return;
		num=GetRandom(0,1+density/85);
		for(UINT i=0;i<num;i++)
		{
			float x=right+RandUnit()*tileW;
			float z=top+RandUnit()*tileH;

			DecoNode *pNode=NULL;
			NodeIter findIter=m_decoNodeMap.find(z);
			if(findIter!=m_decoNodeMap.end())
			{
				pNode=findIter->second;
			}
			else
			{
				pNode=new DecoNode;
				pair<NodeIter,bool> retNode=m_decoNodeMap.insert(NodePair(z,pNode));
				ASSERT(retNode.second==true);
			}
			tagSRInfo srInfo;
			srInfo.sizeRandom.x=RandUnit();
			srInfo.sizeRandom.y=RandUnit();
			srInfo.height=height;
			srInfo.rotateRandom=RandUnitVec3();
			pNode->childMap.insert(ChildPair(x,srInfo));
		}
	}

	void EditorDecoFile::DeleteGrass( const Vector2& leftTop,const Vector2& rightBottom,UINT density )
	{
		//在指定范围(leftTop,rightBottom)内根据density随机生成装饰物的位置，从m_decoNodeMap删除
		//返回指定范围内草的位置的集合
		float top=leftTop.y;
		float bottom=rightBottom.y;
		float left=leftTop.x;
		float right=rightBottom.x;
		ASSERT(top<bottom || left<right);
		float tileW=right-left;
		float tileH=bottom-top;

		//浓度==255时，从m_decoNodeMap删除区域内所有DecoNode
		//浓度<255时，从m_decoNodeMap删除区域内随机个DecoNode
		NodeIter topIter=m_decoNodeMap.lower_bound(top);
		NodeIter bottomIter=m_decoNodeMap.lower_bound(bottom);
		if(topIter==bottomIter)
			return;

		if(density==255)
		{
			for(NodeIter iter=topIter;iter!=bottomIter;++iter)
			{
				DecoNode *pNode=iter->second;
				ChildIter leftIter=pNode->childMap.lower_bound(left);
				ChildIter rightIter=pNode->childMap.lower_bound(right);
				if(leftIter==rightIter)
					continue;

				pNode->childMap.erase(leftIter,rightIter);
			}
		}
		else if(density>0)
		{
			UINT num=GetRandom(0,15-density/17);
			if (num>0)
				return;

			num=GetRandom(0,1+density/85);
			for(NodeIter iter=topIter;iter!=bottomIter;++iter)
			{
				DecoNode *pNode=iter->second;
				ChildIter leftIter=pNode->childMap.lower_bound(left);
				ChildIter rightIter=pNode->childMap.lower_bound(right);
				if(leftIter==rightIter)
					continue;

				for(ChildIter childIter=leftIter;childIter!=rightIter;)
				{
					if(num==0)
						break;
					childIter=pNode->childMap.erase(childIter);
					num--;
				}
			}
		}
	}

	bool EditorDecoFile::HaveDeco()
	{
		for(NodeIter iter=m_decoNodeMap.begin();iter!=m_decoNodeMap.end();++iter)
		{
			DecoNode *pNode=iter->second;
			if(pNode->childMap.begin()!=pNode->childMap.end())
				return true;
		}

		return false;
	}

	bool EditorDecoFile::HaveDeco( const Vector2& leftTop,const Vector2& rightBottom )
	{
		float top=leftTop.y;
		float bottom=rightBottom.y;
		float left=leftTop.x;
		float right=rightBottom.x;
		ASSERT(top<bottom || left<right);

		NodeIter topIter=m_decoNodeMap.lower_bound(top);
		NodeIter bottomIter=m_decoNodeMap.lower_bound(bottom);
		if(topIter==bottomIter)
			return false;

		for(NodeIter iter=topIter;iter!=bottomIter;++iter)
		{
			DecoNode *pNode=iter->second;
			ChildIter leftIter=pNode->childMap.lower_bound(left);
			ChildIter rightIter=pNode->childMap.lower_bound(right);
			if(leftIter!=rightIter)
				return true;
		}

		return false;
	}

	void EditorDecoFile::ChangeGrassHeight( const Vector2& leftTop,const Vector2& rightBottom,float height )
	{
		float top=leftTop.y;
		float bottom=rightBottom.y;
		float left=leftTop.x;
		float right=rightBottom.x;
		ASSERT(top<bottom || left<right);

		NodeIter topIter=m_decoNodeMap.lower_bound(top);
		NodeIter bottomIter=m_decoNodeMap.lower_bound(bottom);
		if(topIter==bottomIter)
			return;

		for(NodeIter iter=topIter;iter!=bottomIter;++iter)
		{
			DecoNode *pNode=iter->second;
			ChildIter leftIter=pNode->childMap.lower_bound(left);
			ChildIter rightIter=pNode->childMap.lower_bound(right);
			if(leftIter==rightIter)
				continue;

			for(ChildIter childIter=leftIter;childIter!=rightIter;++childIter)
			{
				childIter->second.height=height;
			}
		}
	}
}//namespace Cool3D