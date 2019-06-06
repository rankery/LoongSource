#include "stdafx.h"
#include ".\TRiverEditor.h"
#include ".\TRiverPatchProxy.h"
#include "..\Util\FMemory.h"
#include "..\Terrain\HeightMap.h"
#include "..\Math\Frustum.h"
#include "..\Device\IDevice.h"
#include "..\SceneGraph\SceneGraph.h"
#include ".\IUICallBack.h"
#include "..\Device\StateBlock.h"
#include "..\Device\DrawX.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "..\Terrain\ResRiver.h"
#include "..\Terrain\BitMap.h"

namespace Cool3D
{
	const TCHAR* const RiverEditorBasePath=_T(".\\Data\\RiverEditor");
	UINT TRiverEditor::OP_Draw_Dist=800;

	class TRiverEditor::Member
	{
	public:
		vector<tagRiver>				m_rivers;
		tstring							m_szName;		//地图名称，用于生成路径
		MConstColor						m_wireColor;
		set<int>						m_freeIDList;
		
	public:
		Member()
		{
			m_wireColor.m_bUsingVertColor=true;

			//--初始化空闲河流ID列表
			for(int i=0;i<MAX_RIVER;i++)
				m_freeIDList.insert(i);
		}

		~Member()
		{
			FreeAllRivers();
		}

		int GetFreeRiverID()
		{
			if(m_freeIDList.empty())
				return -1;

			int id=*m_freeIDList.begin();
			m_freeIDList.erase(m_freeIDList.begin());
			return id;
		}

		int GetRiverIndex(UINT id)
		{
			for(UINT i=0;i<m_rivers.size();i++)
			{
				if(m_rivers[i].id==id)
					return i;
			}

			return -1;
		}

		void CreateRiverPatchs(UINT riverIndex)
		{
			UINT width=m_rivers[riverIndex].pHMap->Width();
			UINT height=m_rivers[riverIndex].pHMap->Height();

			UINT index=0;
			for(UINT y=0;y<height;y+=EPatchSize)
			{
				for(UINT x=0;x<width;x+=EPatchSize)
				{
					RECT rect;
					rect.left=x;
					rect.right=min(x+EPatchSize+1,width);
					rect.top=y;
					rect.bottom=min(y+EPatchSize+1,height);

					TRiverPatchProxy *pPatch=new TRiverPatchProxy(m_rivers[riverIndex].height);
					pPatch->Create(rect,m_rivers[riverIndex].id,index);
					m_rivers[riverIndex].patchs.push_back(pPatch);

					index++;
				}
			}
		}

		void UpdateRiver(UINT riverIndex,CameraBase *pCamera,TRiverEditor* pEditor)
		{
			ASSERT(riverIndex<m_rivers.size());
			ASSERT(m_rivers[riverIndex].pHMap!=NULL);

			//判断河流的每个Patch是否在编辑范围内
			Heightmap *pHMap=m_rivers[riverIndex].pHMap;

			POINT pt=pHMap->World2Tile(pCamera->GetLookat());
			if(pt.x<EPatchSize/2)
				pt.x=EPatchSize/2;
			if(pt.y<EPatchSize/2)
				pt.y=EPatchSize/2;
			if(pt.x>(int)pHMap->Width()-EPatchSize/2)
				pt.x=pHMap->Width()-EPatchSize/2;
			if(pt.y>(int)pHMap->Height()-EPatchSize/2)
				pt.y=pHMap->Height()-EPatchSize/2;

			RECT drawRect;
			drawRect.left=pt.x-OP_Draw_Dist;
			drawRect.right=pt.x+OP_Draw_Dist;
			drawRect.top=pt.y-OP_Draw_Dist;
			drawRect.bottom=pt.y+OP_Draw_Dist;

			Frustum frustum;
			frustum.Extract(pCamera->GetView(),pCamera->GetPrj());

			for(UINT i=0;i<m_rivers[riverIndex].patchs.size();i++)
			{
				TRiverPatchProxy *pPatch=m_rivers[riverIndex].patchs[i];
				pPatch->Update(pEditor,pHMap,&drawRect);
				pPatch->Draw(frustum);
			}
		}

		void UpdateAllRivers(CameraBase *pCamera,TRiverEditor* pEditor)
		{
			for(UINT i=0;i<m_rivers.size();i++)
				UpdateRiver(i,pCamera,pEditor);
		}

		void FreeRiver(UINT riverIndex,bool bDeleteFile=false)
		{
			if(riverIndex>=m_rivers.size())
				return;

			for(UINT i=0;i<m_rivers[riverIndex].patchs.size();i++)
			{
				TRiverPatchProxy *pPatch=m_rivers[riverIndex].patchs[i];
				if(bDeleteFile)
					pPatch->DeleteCache();
				SAFE_DELETE(pPatch);
			}
			m_rivers[riverIndex].patchs.clear();

			//--回收RiverID
			m_freeIDList.insert(m_rivers[riverIndex].id);

			m_rivers.erase(m_rivers.begin()+riverIndex);
		}

		void FreeAllRivers()
		{
			while (m_rivers.size()>0)
				FreeRiver(0);

			//--初始化空闲河流ID列表
			m_freeIDList.clear();
			for(int i=0;i<MAX_RIVER;i++)
				m_freeIDList.insert(i);
		}
	};

	TRiverEditor::TRiverEditor()
	{
		m_p=new Member();
	}

	TRiverEditor::~TRiverEditor()
	{
		SAFE_DELETE(m_p);
	}

	void TRiverEditor::Draw(SceneGraph *pSG)
	{
		StateBlock sb;
		IDevice *pDev=Device();
		sb.SaveState(ERS_DepthWriteEnable);		pDev->SetRenderState(ERS_DepthWriteEnable,false);

		CameraBase *pCamera=pSG->GetCamera();
		for(UINT i=0;i<m_p->m_rivers.size();i++)
			m_p->UpdateRiver(i,pCamera,this);

		sb.Apply();
	}

	void TRiverEditor::ApplyBrush2River( UINT riverIndex,const tagTEditBrush& brush )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		for(UINT i=0;i<m_p->m_rivers[riverIndex].patchs.size();i++)
		{
			TRiverPatchProxy *pPatch=m_p->m_rivers[riverIndex].patchs[i];
			if( pPatch->GetRealPatch()!=NULL && CheckRivers(riverIndex,i) )
			{
				pPatch->BrushMtl(brush);
			}
		}
	}

	void TRiverEditor::SmoothRiver( UINT riverIndex,const tagTEditBrush& brush )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		for(UINT i=0;i<m_p->m_rivers[riverIndex].patchs.size();i++)
		{
			TRiverPatchProxy *pPatch=m_p->m_rivers[riverIndex].patchs[i];
			pPatch->SmoothMtl(brush);
		}
	}

	void TRiverEditor::NoiseRiver( UINT riverIndex,const tagTEditBrush& brush,float noiseFreq )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		for(UINT i=0;i<m_p->m_rivers[riverIndex].patchs.size();i++)
		{
			TRiverPatchProxy *pPatch=m_p->m_rivers[riverIndex].patchs[i];
			pPatch->NoiseMtl(brush,noiseFreq);
		}
	}

	bool TRiverEditor::AddRiver( int riverHeight,UINT sizeW,UINT sizeH,
		float xScale,float zScale,float yScale )
	{
		//增加一条河流
		tagRiver river;

		river.id=m_p->GetFreeRiverID();
		river.height=riverHeight;
		river.pHMap=new Heightmap;
		river.pHMap->Create(riverHeight,sizeW,sizeH,xScale,zScale,yScale,Heightmap::EF_16Bit);
		m_p->m_rivers.push_back(river);
		m_p->CreateRiverPatchs(m_p->m_rivers.size()-1);

		return true;
	}

	bool TRiverEditor::RemoveRiver( UINT riverIndex )
	{
		//删除一条河流
		m_p->FreeRiver(riverIndex,true);

		return true;
	}

	void TRiverEditor::ClearRivers()
	{
		m_p->FreeAllRivers();
	}

	tagRiverMtlDesc * TRiverEditor::GetRiverMtlDescByID( UINT riverID )
	{
		int riverIndex=m_p->GetRiverIndex(riverID);
		ASSERT(riverIndex>=0 && riverIndex<(int)m_p->m_rivers.size());

		return &m_p->m_rivers[riverIndex].mtlDesc;
	}

	tagRiverMtlDesc * TRiverEditor::GetRiverMtlDescByIndex( UINT riverIndex )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		return &m_p->m_rivers[riverIndex].mtlDesc;
	}

	int TRiverEditor::Build( const TCHAR* szBasePath,Heightmap* pTerrainHmap,IUICallBack* pUI )
	{
		int patchsNum=0;
		int buildedNum=0;
		for(UINT i=0;i<m_p->m_rivers.size();i++)
			patchsNum+=m_p->m_rivers[i].patchs.size();

		pUI->SetProgress(0,patchsNum);
		for(UINT i=0;i<m_p->m_rivers.size();i++)
		{
			for(UINT j=0;j<m_p->m_rivers[i].patchs.size();j++)
			{
				TRiverPatchProxy *pPatch=m_p->m_rivers[i].patchs[j];
				if(!pPatch->Build(this,m_p->m_rivers[i].pHMap,szBasePath,pTerrainHmap,pUI))
					return false;

				pUI->SetProgress(++buildedNum,patchsNum);
			}

			ResRiver::EditorSave(this,m_p->m_rivers[i].id,szBasePath);
		}

		return true;
	}

	UINT TRiverEditor::GetRiverNum()
	{
		return m_p->m_rivers.size();
	}

	const TRiverEditor::tagRiver& TRiverEditor::GetRiverByIndex( UINT riverIndex )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		return m_p->m_rivers[riverIndex];
	}

	int TRiverEditor::GetRiverHeightByIndex( UINT riverIndex )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		return m_p->m_rivers[riverIndex].height;
	}

	int TRiverEditor::GetRiverHeightByID( UINT riverID )
	{
		int riverIndex=m_p->GetRiverIndex(riverID);
		ASSERT(riverIndex>=0 && riverIndex<(int)m_p->m_rivers.size());

		return m_p->m_rivers[riverIndex].height;
	}

	Heightmap * TRiverEditor::GetRiverHeightMapByIndex( UINT riverIndex )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		return m_p->m_rivers[riverIndex].pHMap;
	}

	Heightmap * TRiverEditor::GetRiverHeightMapByID( UINT riverID )
	{
		int riverIndex=m_p->GetRiverIndex(riverID);
		ASSERT(riverIndex>=0 && riverIndex<(int)m_p->m_rivers.size());

		return m_p->m_rivers[riverIndex].pHMap;
	}

	bool TRiverEditor::IsRiverOverLayGround( const Vector3& vecGround )
	{
		//todo leili 判断某点地表是否被河流覆盖

		return false;
	}

	void TRiverEditor::ApplyHeight2River( UINT riverIndex,int height )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		m_p->m_rivers[riverIndex].height=height;

		for(UINT i=0;i<m_p->m_rivers[riverIndex].patchs.size();i++)
		{
			TRiverPatchProxy *pPatch=m_p->m_rivers[riverIndex].patchs[i];
			pPatch->SaveCache();
			pPatch->ClearCache();
		}
		Heightmap *pHMap=m_p->m_rivers[riverIndex].pHMap;
		UINT sizeW=pHMap->Width();
		UINT sizeH=pHMap->Height();
		float xScale=pHMap->GetXScale();
		float yScale=pHMap->GetYScale();
		float zScale=pHMap->GetZScale();
		Heightmap::EFormat format=pHMap->GetFormat();
		SAFE_DELETE(pHMap);
		m_p->m_rivers[riverIndex].pHMap=new Heightmap;
		m_p->m_rivers[riverIndex].pHMap->Create(
			height,sizeW,sizeH,xScale,zScale,yScale,format);
	}

	void TRiverEditor::ApplyMtl2River( UINT riverIndex )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		for(UINT i=0;i<m_p->m_rivers[riverIndex].patchs.size();i++)
		{
			TRiverPatchProxy *pPatch=m_p->m_rivers[riverIndex].patchs[i];
			pPatch->ApplyMtl(this);
		}
	}

	namespace Detail
	{
		struct tagREFileHeader 
		{
			TCHAR				magic[4];			//"RE"
			BYTE				ver;				//1表示8位高度图；2表示16位高度图
			UINT				riverNum;
			DWORD				riverOffset;

			BYTE				reserve[64-1];		//保留字段
		};

		struct tagRERiverInfo
		{
			UINT				id;					//河流ID
			int					height;				//河流的高度
			tagRiverMtlDesc		mtlDesc;			//材质描述信息
			UINT				hmapWidth;
			UINT				hmapHeight;
			DWORD				hmapOffset;

			BYTE				reserve[64];		//保留字段
		};
	}//namespace Detail

	using namespace Detail;

	bool TRiverEditor::Save() const
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s\\RiverEditor.re"),RiverEditorBasePath,m_p->m_szName.c_str());
		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp==0)
		{
			Kernel::Inst()->SetLastError(_T("RiverEditor:河流存盘文件打开失败:%s."),szPath);
			return false;
		}

		//--write header
		tagREFileHeader header;
		FZeroMem(&header,sizeof(header));
		_tcscpy((TCHAR*)header.magic,_T("RE"));
		header.ver=2;
		header.riverNum=m_p->m_rivers.size();
		fwrite(&header,sizeof(header),1,fp);
		header.riverOffset=ftell(fp);

		//--write riverInfo
		tagRERiverInfo riverInfo;
		for( UINT i=0;i<header.riverNum;i++ )
		{
			int offset=ftell(fp);

			riverInfo.id=m_p->m_rivers[i].id;
			riverInfo.height=m_p->m_rivers[i].height;
			riverInfo.mtlDesc=m_p->m_rivers[i].mtlDesc;
			riverInfo.hmapWidth=m_p->m_rivers[i].pHMap->Width();
			riverInfo.hmapHeight=m_p->m_rivers[i].pHMap->Height();
			fwrite(&riverInfo,sizeof(riverInfo),1,fp);

			riverInfo.hmapOffset=ftell(fp);
			m_p->m_rivers[i].pHMap->WriteToFile(fp);
			int endOffset=ftell(fp);

			//--write back riverInfo
			fseek(fp,offset,SEEK_SET);
			fwrite(&riverInfo,sizeof(riverInfo),1,fp);

			fseek(fp,endOffset,SEEK_SET);
		}

		//--write back header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		fclose(fp);

		//--save patch cache
		for(UINT i=0;i<m_p->m_rivers.size();i++)
		{
			for(UINT j=0;j<m_p->m_rivers[i].patchs.size();j++)
			{
				TRiverPatchProxy *pPatch=m_p->m_rivers[i].patchs[j];
				pPatch->SaveCache();
			}
		}

		return true;
	}

	bool TRiverEditor::Load()
	{
		::SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		TCHAR szPath[512];
		_stprintf(szPath,_T("%s\\%s\\RiverEditor.re"),RiverEditorBasePath,m_p->m_szName.c_str());

		IFS *pFS=Kernel::Inst()->DefaultFS();
		DWORD hFile=pFS->Open(szPath);
		if(hFile==NULL)
		{
			Kernel::Inst()->SetLastError(_T("RiverEditor:河流存盘文件打开失败:%s."),szPath);
			return false;
		}

		//--clear rivers
		m_p->FreeAllRivers();

		//--load header
		tagREFileHeader header;
		pFS->Read(&header,sizeof(header),hFile);
		for(UINT i=0;i<header.riverNum;i++)
		{
			//--load riverInfo
			tagRERiverInfo riverInfo;
			pFS->Read(&riverInfo,sizeof(riverInfo),hFile);
			tagRiver river;
			river.id=riverInfo.id;
			river.height=riverInfo.height;
			river.mtlDesc=riverInfo.mtlDesc;
			river.pHMap=new Heightmap;
			pFS->Seek(hFile,riverInfo.hmapOffset,FILE_BEGIN);
			river.pHMap->ReadFromFile(pFS,hFile);
			river.pHMap->SetYScale(1.0f);//todo leili 处理old代码

			m_p->m_freeIDList.erase(river.id);

			m_p->m_rivers.push_back(river);
		}

		for(UINT i=0;i<m_p->m_rivers.size();i++)
			m_p->CreateRiverPatchs(i);

		return true;
	}

	void TRiverEditor::SetName( const TCHAR *szMapName )
	{
		ASSERT(szMapName!=NULL);

		m_p->m_szName=szMapName;

		TCHAR szSavePath[512]={0};
		_stprintf_s(szSavePath,512,_T("%s\\%s"),RiverEditorBasePath,szMapName);
		::CreateDirectory(RiverEditorBasePath,NULL);
		::CreateDirectory(szSavePath,NULL);
		TRiverPatchProxy::SetCachePath(szSavePath);
	}

	void TRiverEditor::DrawLineStrip( vector<Vert_PD>& vertBuf )
	{
		IDevice *pDev=Device();
		StateBlock sb;
		sb.SaveState(ERS_FLightEnable);
		pDev->FEnableLight(false);

		Kernel::Inst()->GetRenderSys()->PrepMtl(&m_p->m_wireColor);//设置材质状态为使用顶点颜色
		MtlRenderCache *pMtlCache=m_p->m_wireColor.GetRenderCache();
		pMtlCache->Begin();
		pMtlCache->BeginPass(0);
		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PD));
		pDev->DrawMem(EDraw_LineStrip,vertBuf.size()-1,&(*vertBuf.begin()),sizeof(Vert_PD));
		pMtlCache->EndPass();
		pMtlCache->End();
		sb.Apply();
	}

	void TRiverEditor::DrawGroundCircle( UINT riverIndex,float centreX,float centreZ,UINT r )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		Heightmap *pHMap=m_p->m_rivers[riverIndex].pHMap;
		if(pHMap==NULL)
			return;
		DWORD color=0xFFFFFF00;
		float radius=r*pHMap->GetXScale();
		vector<Vert_PD> vertBuf;

		Vert_PD v;
		v.diffuse=color;
		POINT tilePt;

		float offset=8.0f;
		Vert_PD startPt;
		startPt.diffuse=color;
		startPt.pos.x=centreX+radius;
		startPt.pos.z=centreZ;
		tilePt=pHMap->World2Tile(startPt.pos);
		startPt.pos.y=pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
		vertBuf.push_back(startPt);

		int numSplit=max(36,r*r);
		for(int i=1;i<numSplit;i++)
		{
			float a=(FLOAT_PI*2/numSplit)*i;
			v.pos.x=centreX+cosf(a)*radius;
			v.pos.z=centreZ+sinf(a)*radius;
			tilePt=pHMap->World2Tile(v.pos);
			v.pos.y=pHMap->GetSafeHeight(tilePt.x,tilePt.y)+offset;
			vertBuf.push_back(v);
		}
		vertBuf.push_back(startPt);

		DrawLineStrip(vertBuf);
	}

	void TRiverEditor::DrawGroundQuad( UINT riverIndex,int left,int right,int top,int bottom,DWORD color )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		Heightmap *pHMap=m_p->m_rivers[riverIndex].pHMap;

		if(pHMap==NULL)
			return;

		if(left<0)
			left=0;
		if(top<0)
			top=0;
		if(right>(int)pHMap->Width()-1)
			right=pHMap->Width()-1;
		if(bottom>(int)pHMap->Height()-1)
			bottom=pHMap->Height()-1;

		vector<Vert_PD> vertBuf;
		Vert_PD v;
		v.diffuse=color;

		UINT x=left;
		UINT y;
		//--left
		float offset=8.0f;
		for(y=top;y<(UINT)bottom;y++)
		{
			v.pos=pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}

		//--bottom
		y=bottom;
		for(x=left;x<(UINT)right;x++)
		{
			v.pos=pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}
		//--right
		x=right;
		for(y=bottom;y>(UINT)top;y--)
		{
			v.pos=pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}

		//--top
		y=top;
		for(x=right;x>(UINT)left;x--)
		{
			v.pos=pHMap->Tile2World(x,y);
			v.pos.y+=offset;
			vertBuf.push_back(v);
		}

		DrawLineStrip(vertBuf);
	}

	bool TRiverEditor::MousePick( UINT riverIndex,UINT cursorX,UINT cursorY,CameraBase *pCamera,Vector3& out )
	{
		ASSERT(riverIndex<m_p->m_rivers.size());

		if(m_p->m_rivers[riverIndex].pHMap==NULL)
			return false;
		Ray mRay;
		pCamera->GetMouseRay(cursorX,cursorY,mRay);
		return m_p->m_rivers[riverIndex].pHMap->RayCollision(mRay,out);
	}

	bool TRiverEditor::BuildWaterBitmap( BitMap& bitmap,Heightmap* pTrnHMap,IUICallBack* pUI )
	{
		bitmap.Create(pTrnHMap->Width(),pTrnHMap->Height(),false);

		for(size_t i=0;i<m_p->m_rivers.size();++i)
		{
			tagRiver& rvr=m_p->m_rivers[i];

			for(size_t j=0;j<rvr.patchs.size();++j)			
			{
				TRiverPatchProxy* pPatch=rvr.patchs[j];
				if(!pPatch->BuildWaterBitmap(bitmap,this,rvr.pHMap,pUI))
					return false;
			}
		}

		return true;
	}

	bool TRiverEditor::BuildWaterHMap( Heightmap& hmap,Heightmap* pTrnHMap,IUICallBack* pUI )
	{
		UINT w=pTrnHMap->Width()/EPatchSize;
		UINT h=pTrnHMap->Height()/EPatchSize;
		if(pTrnHMap->Width()%EPatchSize!=0)
			++w;
		if(pTrnHMap->Height()%EPatchSize!=0)
			++h;
			
		hmap.Create(0,w,h,1.0f,1.0f,1.0f,Heightmap::EF_16Bit);

		for(size_t i=0;i<m_p->m_rivers.size();++i)
		{
			tagRiver& rvr=m_p->m_rivers[i];
			for(size_t j=0;j<rvr.patchs.size();++j)			
			{
				TRiverPatchProxy* pPatch=rvr.patchs[j];
				if(pPatch->HaveWater(this,rvr.pHMap))
				{
					int x=pPatch->GetRect().left/EPatchSize;
					int z=pPatch->GetRect().top/EPatchSize;
					hmap.SafeSetVal(x,z,rvr.height);
				}
			}
		}

		return true;
	}

	bool TRiverEditor::CheckRivers( UINT curRiverIndex,UINT curPatchIndex )
	{
		//检查是否还有同一地块上是否还存在别的河流
		bool bExist=false;
		for(size_t riverindex=0; riverindex<m_p->m_rivers.size(); riverindex++)
		{
			if( riverindex == curRiverIndex )
				continue;

			tagRiver& river = m_p->m_rivers[riverindex];
			Heightmap *pHMap=m_p->m_rivers[riverindex].pHMap;
			TRiverPatchProxy *pPatch = river.patchs[curPatchIndex];

			if( pPatch->HaveWater(this,pHMap,false) )
			{
				bExist = true;
				break;
			}
		}

		return !bExist;
	}
}//namespace Cool3D
