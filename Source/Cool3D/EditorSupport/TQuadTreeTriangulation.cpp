#include "StdAfx.h"
#include ".\tquadtreetriangulation.h"
#include "..\Terrain\Heightmap.h"
#include "..\Util\SpeedAnalyser.h"

namespace Cool3D
{
	float TQuadTreeTriangulation::m_minResolution=1.0f;

	TQuadTreeTriangulation::TQuadTreeTriangulation(void)
	{
		m_pMap=NULL;
		memset(&m_rect,0,sizeof(m_rect));
		m_quadMatrix=NULL;
		m_bForceTri=false;
	}

	TQuadTreeTriangulation::~TQuadTreeTriangulation(void)
	{
	}

	void TQuadTreeTriangulation::GenerateIndices(float desireResolution)
	{
		GenerateIndices(desireResolution,desireResolution);
	}

	void TQuadTreeTriangulation::GenerateIndices(float desireResolution,float higherRes)
	{
		ASSERT(m_pMap!=NULL);

		m_desireResolution=desireResolution;
		m_higherRes=higherRes;
		if(m_desireResolution>=1.0f)//最大分辨率,则使用强制切分?
			m_bForceTri=true;
		//--reset
		m_indices.clear();
		SAFE_FREE(m_quadMatrix);

		//--build matrix
		m_matW=m_rect.right-m_rect.left;
		m_matH=m_rect.bottom-m_rect.top;
		ASSERT(m_matW > 0);
		ASSERT(m_matH > 0);
		//ASSERT(m_matW % 2==0);//长宽必须能被2整除,否则会有crack
		//ASSERT(m_matH % 2==0);
		

		m_quadMatrix=(BYTE*)malloc(m_matW*m_matH);
		memset(m_quadMatrix,0,m_matW*m_matH);

		RECT rect=m_rect;
		BuildMatrixR(rect);

		//--build index buffer
		BuildTriListR(rect);

		//--clear
		SAFE_FREE(m_quadMatrix);
	}
	
	UINT TQuadTreeTriangulation::GetNumIndices()
	{
		return m_indices.size();
	}
	
	WORD* TQuadTreeTriangulation::GetIndices()
	{
		return &(*m_indices.begin());
	}

	void TQuadTreeTriangulation::GetChildrenRect(RECT myRect,RECT *pRect)
	{
		/*将myRect切分成4个子rect
		--------|--------
		|       |		|
		|   0   |	1	|
		|-------|-------|
		|       |		|
		|   3   |	2	|
		|_______|_______|
		*/
		int nodeW=myRect.right-myRect.left;
		int nodeH=myRect.bottom-myRect.top;
		int child0W=nodeW/2;
		int child0H=nodeH/2;
		RECT childRc;
		//0
		childRc.left=myRect.left;
		childRc.right=childRc.left+child0W;
		childRc.top=myRect.top;
		childRc.bottom=childRc.top+child0H;
		pRect[0]=childRc;

		//1
		childRc.right=myRect.right;
		childRc.left=childRc.right-(nodeW-child0W);
		childRc.top=myRect.top;
		childRc.bottom=childRc.top+child0H;
		pRect[1]=childRc;

		//2
		childRc.right=myRect.right;
		childRc.left=childRc.right-(nodeW-child0W);
		childRc.bottom=myRect.bottom;
		childRc.top=myRect.bottom-(nodeH-child0H);
		pRect[2]=childRc;

		//3
		childRc.left=myRect.left;
		childRc.right=childRc.left+child0W;
		childRc.bottom=myRect.bottom;
		childRc.top=myRect.bottom-(nodeH-child0H);
		pRect[3]=childRc;
	}

	void TQuadTreeTriangulation::BuildMatrixR(RECT myRect)
	{
		if(IfNeedSplit(m_pMap,myRect))
		{
			UINT centerX=(myRect.left+myRect.right)/2;
			UINT centerY=(myRect.top+myRect.bottom)/2;
			SetMatrix(centerX,centerY,1);//设置标记

			//向下细分
			RECT childRc[4];
			GetChildrenRect(myRect,childRc);
			for(int i=0;i<4;i++)
				BuildMatrixR(childRc[i]);
		}
	}

	void TQuadTreeTriangulation::BuildTriListR(RECT myRect)
	{
		UINT centerX=(myRect.left+myRect.right)/2;
		UINT centerY=(myRect.top+myRect.bottom)/2;
		if(GetMatrix(centerX,centerY))
		{
			//向下细分
			RECT childRc[4];
			GetChildrenRect(myRect,childRc);
			for(int i=0;i<4;i++)
				BuildTriListR(childRc[i]);
		}
		else
		{
			//顺时针产生triangle fan
			vector<WORD> fan;
			fan.push_back(GetVertIndex(centerX,centerY));
			
			fan.push_back(GetVertIndex(myRect.left,myRect.top));
			FixCrackTopR(myRect,fan);
			fan.push_back(GetVertIndex(myRect.right,myRect.top));
			FixCrackRightR(myRect,fan);
			fan.push_back(GetVertIndex(myRect.right,myRect.bottom));
			FixCrackBottomR(myRect,fan);
			fan.push_back(GetVertIndex(myRect.left,myRect.bottom));
			FixCrackLeftR(myRect,fan);
			fan.push_back(GetVertIndex(myRect.left,myRect.top));

			AppendTriFan(fan);
		}
	}

	void TQuadTreeTriangulation::FixCrackTopR(RECT myRect,vector<WORD>& fan,bool first)
	{
		if(first)
		{
			//向上偏移,找到与此节点相邻的同级节点
			int offset=myRect.bottom-myRect.top;
			myRect.top-=offset;
			myRect.bottom-=offset;
		}
		int centerX=(myRect.left+myRect.right)/2;
		int centerY=(myRect.top+myRect.bottom)/2;
		
		if(centerX>=m_rect.right
			|| centerX<m_rect.left
			|| centerY>=m_rect.bottom
			|| centerY<m_rect.top)
			return;
		if(!GetMatrix(centerX,centerY))
			return;
		//寻找下一级相邻的节点
		//此时myRect为上面相邻的节点的RECT
		RECT childRc[4];
		GetChildrenRect(myRect,childRc);
		
		FixCrackTopR(childRc[3],fan,false);//左侧一个
		fan.push_back(GetVertIndex(centerX,myRect.bottom));
		FixCrackTopR(childRc[2],fan,false);//右侧一个
	}

	void TQuadTreeTriangulation::FixCrackLeftR(RECT myRect,vector<WORD>& fan,bool first)
	{
		if(first)
		{
			//向左偏移
			int offset=myRect.right-myRect.left;
			myRect.left-=offset;
			myRect.right-=offset;
		}
		int centerX=(myRect.left+myRect.right)/2;
		int centerY=(myRect.top+myRect.bottom)/2;

		if(centerX>=m_rect.right
			|| centerX<m_rect.left
			|| centerY>=m_rect.bottom
			|| centerY<m_rect.top)
			return;
		if(!GetMatrix(centerX,centerY))
			return;
		//寻找下一级相邻的节点
		//此时myRect为上面相邻的节点的RECT
		RECT childRc[4];
		GetChildrenRect(myRect,childRc);

		FixCrackLeftR(childRc[2],fan,false);//下面一个
		fan.push_back(GetVertIndex(myRect.right,centerY));
		FixCrackLeftR(childRc[1],fan,false);//上面一个
	}

	void TQuadTreeTriangulation::FixCrackRightR(RECT myRect,vector<WORD>& fan,bool first)
	{
		if(first)
		{
			//向右偏移
			int offset=myRect.right-myRect.left;
			myRect.left+=offset;
			myRect.right+=offset;
		}
		int centerX=(myRect.left+myRect.right)/2;
		int centerY=(myRect.top+myRect.bottom)/2;

		if(centerX>=m_rect.right
			|| centerX<m_rect.left
			|| centerY>=m_rect.bottom
			|| centerY<m_rect.top)
			return;
		if(!GetMatrix(centerX,centerY))
			return;
		//寻找下一级相邻的节点
		//此时myRect为上面相邻的节点的RECT
		RECT childRc[4];
		GetChildrenRect(myRect,childRc);

		FixCrackRightR(childRc[0],fan,false);//上面一个
		fan.push_back(GetVertIndex(myRect.left,centerY));
		FixCrackRightR(childRc[3],fan,false);//下面一个
	}

	void TQuadTreeTriangulation::FixCrackBottomR(RECT myRect,vector<WORD>& fan,bool first)
	{
		if(first)
		{
			//向下偏移
			int offset=myRect.bottom-myRect.top;
			myRect.top+=offset;
			myRect.bottom+=offset;
		}
		int centerX=(myRect.left+myRect.right)/2;
		int centerY=(myRect.top+myRect.bottom)/2;

		if(centerX>=m_rect.right
			|| centerX<m_rect.left
			|| centerY>=m_rect.bottom
			|| centerY<m_rect.top)
			return;
		if(!GetMatrix(centerX,centerY))
			return;
		//寻找下一级相邻的节点
		//此时myRect为上面相邻的节点的RECT
		RECT childRc[4];
		GetChildrenRect(myRect,childRc);

		FixCrackBottomR(childRc[1],fan,false);//右侧
		fan.push_back(GetVertIndex(centerX,myRect.top));
		FixCrackBottomR(childRc[0],fan,false);//左侧一个
	}

	WORD TQuadTreeTriangulation::GetVertIndex(UINT x,UINT y)
	{
		if((int)x>=m_rect.right)
			x=m_rect.right-1;
		if((int)y>=m_rect.bottom)
			y=m_rect.bottom-1;
		return WORD(m_matW*(y-m_rect.top)+(x-m_rect.left));
	}

	void TQuadTreeTriangulation::AppendTriFan(vector<WORD>& fan)
	{
		size_t num=fan.size();
		ASSERT(num > 0);
		WORD share=fan[0];
		for(size_t i=2;i<num;i++)
		{
			m_indices.push_back(share);
			m_indices.push_back(fan[i-1]);
			m_indices.push_back(fan[i]);
		}
	}

	//对于给定的最精细的分辨率,这个Rect是否需要继续细分
	bool TQuadTreeTriangulation::IfNeedSplit(Heightmap *pMap,RECT rect)
	{
		const int MIN_NODE_SIZE=5;
		//--如果已经达到了最小的节点大小,则不再细分
		if(rect.right-rect.left<=MIN_NODE_SIZE)
			return false;
		if(rect.bottom-rect.top<=MIN_NODE_SIZE)
			return false;

		if(m_bForceTri)//强制切分
			return true;
		
		float res=m_desireResolution;
		//--如果是矩形边缘的节点则一定细分到更精细的LOD等级,以便于不同Lod之间的结合
		if(rect.left==m_rect.left
			|| rect.right==m_rect.right
			|| rect.top==m_rect.top
			|| rect.bottom==m_rect.bottom)
		{
			//res=m_higherRes;
			return true;
		}

		//--计算基本的split分辨率
		float edgeLen=float(max(rect.right-rect.left,rect.bottom-rect.top));
		float baseR=1.0f/edgeLen;
		//--根据rect所包括的height map的粗糙度计算出最终的分辨率
		float f=1.0f;
		float dh=GetMaxDeltaH(pMap,rect);
		if(dh > 1.0f)
		{
			f=pMap->GetXScale()/dh;	//宽高比率

			f=min(f,1);//只允许f缩小当前的分辨率指数
			return baseR*f < res/m_minResolution;//还没有达到最精细的分辨率
		}
		else
			return false;
	}
	float TQuadTreeTriangulation::GetMaxDeltaH(Heightmap *pMap,RECT rect)
	{
		float maxH,minH;
		pMap->GetHeightRange(minH,maxH,rect);
		return (maxH-minH);
	}

	void TQuadTreeTriangulation::SetMatrix(UINT x,UINT y,BYTE val)
	{
		x-=m_rect.left;
		y-=m_rect.top;
		ASSERT(x < m_matW);
		ASSERT(y < m_matH);
		m_quadMatrix[y*m_matW+x]=val;
	}

	BYTE TQuadTreeTriangulation::GetMatrix(UINT x,UINT y)
	{
		x-=m_rect.left;
		y-=m_rect.top;
		ASSERT(x < m_matW);
		ASSERT(y < m_matH);
		return m_quadMatrix[y*m_matW+x];
	}
}//namespace Cool3D