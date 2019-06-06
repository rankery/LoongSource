#include "StdAfx.h"
#include ".\resmgr.h"
#include "..\Util\DebugMisc.h"
#include ".\ResPool.h"
#include ".\ResTexture.h"
#include ".\ResStaticMesh.h"
#include ".\ResSceneMesh.h"
#include ".\ResKeyFrame.h"
#include ".\ResSkin.h"
#include ".\ResSkeleton.h"
#include ".\ResSkeletonAction.h"
#include ".\ResSpecialEffect.h"
#include "..\Terrain\ResGrass.h"
#include "..\EditorSupport\ResDynaAlphaMap.h"
#include ".\ResVertShader.h"
#include ".\ResPixelShader.h"
#include "UserResMgr.h"
#include "AsyncResIO.h"
#include "..\Terrain\TerrainRes.h"
#include "..\Terrain\TerrainPatchRes.h"
#include "..\Terrain\TerrainPatchRenderGroupsRes.h"
#include "..\Device\DrawX.h"
#include "..\Image\Image.h"
#include "..\Device\IDeviceTex.h"
#include "..\Kernel.h"
#include "..\Terrain\ResRiver.h"
#include "..\Terrain\ResRiverPatch.h"
#include "..\Cool3DOption.h"
#include "..\Util\CSVFile.h"
#include "ResCloth.h"

#ifdef USE_SPEEDTREE
#include "..\SpeedTreeRT\ResSpeedTree.h"
#include "..\SpeedTreeRT\SpeedTreeInstPool.h"
#endif

namespace Cool3D
{
	IDeviceTex *ResMgr::s_pTexFallback=NULL;

	/**	根据文件名后缀来new不同的ResRenderable派生类对象
	*/
	template <class ResType>
	class ResRenderableFactory
	{
	public:
		ResType* NewRes(const TCHAR* szName)
		{
			TCHAR szExt[15];
			_tsplitpath_s(szName,NULL,0,NULL,0,NULL,0,szExt,15);
			_tcslwr_s(szExt,15);

			if(_tcscmp(szExt,_T(".fsm")) == 0)//simple mesh
				return new ResStaticMesh(szName);
			if(_tcscmp(szExt,_T(".fscn")) == 0)//scene mesh
				return new ResSceneMesh(szName);
			if(_tcscmp(szExt,_T(".fak")) == 0)//key frame
				return new ResKeyFrame(szName);
			if(_tcscmp(szExt,_T(".trn"))==0)//terrain
				return new TerrainRes(szName);
			if(_tcscmp(szExt,_T(".patch"))==0)//terrain patch
				return new TerrainPatchRes(szName);
			if(_tcscmp(szExt,_T(".rendergroups"))==0)//terrain patch render groups
				return new TerrainPatchRenderGroupsRes(szName);
			if(_tcscmp(szExt,_T(".fskin"))==0)//skin
				return new ResSkin(szName);
			if(_tcscmp(szExt,_T(".fskel"))==0)//skeleton
				return new ResSkeleton(szName);
			if(_tcscmp(szExt,_T(".deco"))==0)
				return new ResGrass(szName);
			if(_tcscmp(szExt,_T(".river"))==0)	//river
				return new ResRiver(szName);
			if(_tcscmp(szExt,_T(".rpatch"))==0)	//river patch
				return new ResRiverPatch(szName);
			if(_tcscmp(szExt,_T(".fcloth"))==0) //cloth 
				return new ResCloth(szName);

			return NULL;
		}
	};

	typedef ResPool<ResRenderable,ResRenderableFactory<ResRenderable> > ResRenderablePool;
	struct ResMgr::Member
	{
		IFS							*m_pFS[EFST_Num];
		ResPool<ResTexture>			*m_pTexPool;
		ResRenderablePool			*m_pResRenderPool;
		ResPool<ResSkeletonAction>	*m_pSkeletonPool;
		ResPool<ResSpecialEffect>	*m_pSpecialEffectPool;
		ResPool<ResDynaAlphaMap>    *m_pDyncAlphaMapPool;
		ResPool<ResVertShader>		*m_pVSPool;
		ResPool<ResPixelShader>		*m_pPSPool;

		UserResMgr					*m_pUserMgr;
		bool						m_bAsync;

		vector<pair<tstring,int>>	m_vecHighTexDetail; // 高精度贴图文件名（可以是目录名）列表
														// 表中的纹理将始终使用高精度贴图

		Member()
		{
			memset(m_pFS,0,EFST_Num*sizeof(IFS*));
			m_pResRenderPool=NULL;
			m_pTexPool=NULL;
			m_pSkeletonPool=NULL;
			m_pSpecialEffectPool=NULL;
			m_pDyncAlphaMapPool=NULL;
			m_pVSPool=NULL;
			m_pPSPool=NULL;

			m_pUserMgr=NULL;
			m_bAsync=false;
		}
		~Member()
		{
			SAFE_DELETE(m_pResRenderPool);
			SAFE_DELETE(m_pTexPool);//必须最后释放Texture pool,因为ResRenderable里面都用到了texture
			SAFE_DELETE(m_pSkeletonPool);
			SAFE_DELETE(m_pSpecialEffectPool);
			SAFE_DELETE(m_pDyncAlphaMapPool);
			SAFE_DELETE(m_pVSPool);
			SAFE_DELETE(m_pPSPool);
		}
		void LoadHighTexDetailList(IFS *pFS,const TCHAR* szFileName)
		{
			CSVFile csvFile;
			bool ret = csvFile.OpenFile( pFS, szFileName );
			if(!ret)
			{
				csvFile.CloseFile();
				return;
			}
			while(true)
			{
				vector<tstring> vec = csvFile.GetNextLine();
				if(vec.empty())
					break;
				m_vecHighTexDetail.push_back( make_pair( vec[0], _tstoi(vec[1].c_str()) ) );
			}
			csvFile.CloseFile();
		}
		bool IsInHighTexDetailList(const TCHAR* szTexName,int& level)
		{
			tstring strTexName = szTexName;
			for( size_t i = 0; i < m_vecHighTexDetail.size(); i++ )
			{
				pair<tstring,int>& texDetail = m_vecHighTexDetail[i];
				if( tstring::npos != strTexName.find( texDetail.first ) )
				{
					level = texDetail.second;
					return true;
				}
			}
			return false;
		}
	};

	ResMgr::ResMgr(void)
	{
		m_p=new Member;
	}

	ResMgr::~ResMgr(void)
	{
		Destroy();
	}

	static ResMgr g_resMgr;
	ResMgr *ResMgr::Inst()
	{
		return &g_resMgr;
	}

	void ResMgr::Destroy()
	{
		AsyncResIO::Inst()->Destroy();
		SAFE_DELETE(s_pTexFallback);
#ifdef USE_SPEEDTREE
		SpeedTreeInstPool::Inst()->Clear();
#endif
		SAFE_DELETE(m_p);
	}

	void ResMgr::Init(UINT texPoolSize,UINT renderableSize,UINT skeletonSize,UINT specialEffectSize,UINT vsNum,UINT psNum)
	{
		m_p->m_pTexPool			  =	new ResPool<ResTexture>(texPoolSize);
		m_p->m_pResRenderPool	  =	new ResRenderablePool(renderableSize);	
		m_p->m_pSkeletonPool	  = new ResPool<ResSkeletonAction>(skeletonSize);
		m_p->m_pSpecialEffectPool = new ResPool<ResSpecialEffect>(specialEffectSize);
		m_p->m_pDyncAlphaMapPool  = new ResPool<ResDynaAlphaMap>(1024*1024*8);
		m_p->m_pVSPool			  = new ResPool<ResVertShader>(vsNum);
		m_p->m_pPSPool			  = new ResPool<ResPixelShader>(psNum);

		//--
		AsyncResIO::Inst()->Create();

		//--build fall back tex
		Image img;
		int size=128;
		img.Create(size,size,EPF_A8R8G8B8);
		Color4ub *pPixel=(Color4ub *)img.GetRawBuffer();
		Color4ub fallColor[2];
		fallColor[0]=Color4ub(255,5,250,250);
		fallColor[1]=Color4ub(255,5,0,5);
		for(int y=0;y<size;y++)
		{
			for(int x=0;x<size;x++)
			{
				int sel=(x/16)%2+(y/16+1)%2;
				pPixel[y*size+x]=fallColor[sel%2];
			}
		}

		s_pTexFallback=Device()->NewTexture();
		s_pTexFallback->CreateFromImage(&img,1);

		// 读取高精度贴图列表
		m_p->LoadHighTexDetailList( Kernel::Inst()->DefaultFS(), _T("config\\HighTexDetailList.csv") );
	}

	UINT ResMgr::GetNumLoading()
	{
		return AsyncResIO::Inst()->GetQueueSize();
	}

	void ResMgr::EnableIOAsync(bool bAsync)
	{
		m_p->m_bAsync=bAsync;
	}

	bool ResMgr::IsIOAsync() const
	{
		return m_p->m_bAsync;
	}

	void ResMgr::AttachUserResMgr(UserResMgr *pMgr)
	{
		m_p->m_pUserMgr=pMgr;
	}

	UserResMgr* ResMgr::GetUserResMgr() const
	{
		return m_p->m_pUserMgr;
	}

	void ResMgr::SetFS(EFSType type,IFS* pFS)
	{
		ASSERT(type>=0&&type<EFST_Num);
		m_p->m_pFS[type]=pFS;
	}

	IFS* ResMgr::GetFS(EFSType type)
	{
		ASSERT(type>=0&&type<EFST_Num);
		return m_p->m_pFS[type];
	}

	IFS* ResMgr::GetFS(const TCHAR* szResName)
	{
		if(_tcsnicmp(szResName,_T("data\\system"),11)==0)
			return m_p->m_pFS[EFST_System];
		if(_tcsnicmp(szResName,_T("data\\sfx"),8)==0)
			return m_p->m_pFS[EFST_Effect];
		if(_tcsnicmp(szResName,_T("data\\animation"),14)==0)
			return m_p->m_pFS[EFST_Animation];
		if(_tcsnicmp(szResName,_T("data\\model"),10)==0)
			return m_p->m_pFS[EFST_Model];
		if(_tcsnicmp(szResName,_T("data\\map"),8)==0)
			return m_p->m_pFS[EFST_Map];
		return NULL;
	}

	IFS* ResMgr::SafeGetFS(const TCHAR* szResName)
	{
		IFS* pFS=GetFS(szResName);
		if(pFS==NULL)
			pFS=Kernel::Inst()->DefaultFS();
		return pFS;
	}

	IFS* ResMgr::SafeGetFS(EFSType type)
	{
		ASSERT(type>=0&&type<EFST_Num);
		if(m_p->m_pFS[type]==NULL)
			return Kernel::Inst()->DefaultFS();
		return m_p->m_pFS[type];
	}

	void ResMgr::OnFSChanged(EFSType type)
	{
		AsyncResIO::Inst()->CancleByFS(m_p->m_pFS[type]);
	}

	ResTexture*	ResMgr::NewTexture(const TCHAR* szName,UINT numLevels)
	{
		IFS *pFS=SafeGetFS(szName);

		ETexDetail texDetail = ETexDetail_High;
		int level;
		int sysTexDetail = Cool3DOption::Inst()->GetOption(Cool3DOption::EO_TexDetail);

		if( !m_p->IsInHighTexDetailList( szName, level ) )
		{
			switch( sysTexDetail )
			{
			case Cool3DOption::ETexDetail_High:	texDetail=ETexDetail_High;	break;
			case Cool3DOption::ETexDetail_Mid:	texDetail=ETexDetail_Mid;	break;
			case Cool3DOption::ETexDetail_Low:	texDetail=ETexDetail_Low;	break;
			}
		}
		else
		{
			if( level > sysTexDetail )
				texDetail = (ETexDetail)sysTexDetail;
			else
				texDetail = (ETexDetail)level;
		}

		DWORD dwParam=MAKELONG(numLevels,texDetail);
		return m_p->m_pTexPool->NewRes(szName,pFS,dwParam,m_p->m_bAsync);
	}

	void ResMgr::ReleaseTexture(ResTexture *pTex)
	{
		if(pTex==NULL)
			return;
		m_p->m_pTexPool->DelRes(pTex);
	}

	IDeviceTex *ResMgr::GetTexFallBack()
	{
		return s_pTexFallback;
	}

	ResRenderable* ResMgr::NewRenderable(const TCHAR* szName,DWORD param)
	{
		IFS *pFS=SafeGetFS(szName);
		//ASSERT(m_p!=NULL && m_p->m_pResRenderPool!=NULL);
		return m_p->m_pResRenderPool->NewRes(szName,pFS,param,m_p->m_bAsync);
	}

	void ResMgr::ReleaseRenderable(ResRenderable *pMesh)
	{
		//ASSERT(m_p!=NULL && m_p->m_pResRenderPool!=NULL);
		m_p->m_pResRenderPool->DelRes(pMesh);
	}

	ResSkeletonAction* ResMgr::NewSkeleton(const TCHAR* szName,DWORD param,bool bAsync)
	{
		IFS *pFS=m_p->m_pFS[EFST_Animation];
		return m_p->m_pSkeletonPool->NewRes(szName,pFS,param,bAsync);
	}

	void ResMgr::ReleaseSkeleton(ResSkeletonAction *pSkeleton)
	{
		if(pSkeleton==NULL)
			return;
		if(m_p==NULL)
			return;
		m_p->m_pSkeletonPool->DelRes(pSkeleton);
	}

	ResSpecialEffect* ResMgr::NewSpecialEffect(const TCHAR* szName,DWORD param)
	{
		IFS *pFS=m_p->m_pFS[EFST_Effect];
		return m_p->m_pSpecialEffectPool->NewRes(szName,pFS,param,m_p->m_bAsync);
	}

	void ResMgr::ReleaseSpecialEffect(ResSpecialEffect *pSpecialEffect)
	{
		if(pSpecialEffect==NULL)
			return;
		m_p->m_pSpecialEffectPool->DelRes(pSpecialEffect);
	}

	ResDynaAlphaMap* ResMgr::NewDyncAlphaMap(const TCHAR* szName,int initVal,UINT size)
	{
		IFS *pFS=m_p->m_pFS[EFST_Map];
		return m_p->m_pDyncAlphaMapPool->NewRes(szName,pFS,MAKELONG(size,initVal),m_p->m_bAsync);
	}

	void ResMgr::ReleaseDyncAlphaMap(ResDynaAlphaMap *pAlphaMap)
	{
		if(pAlphaMap==NULL)
			return;
		m_p->m_pDyncAlphaMapPool->DelRes(pAlphaMap);
	}

	ResVertShader* ResMgr::NewVS(const TCHAR* szName,DWORD ver)
	{
		IFS *pFS=m_p->m_pFS[EFST_System];
		return m_p->m_pVSPool->NewRes(szName,pFS,ver,m_p->m_bAsync);
	}

	void ResMgr::ReleaseVS(ResVertShader* pVS)
	{
		if(pVS==NULL)
			return;
		m_p->m_pVSPool->DelRes(pVS);
	}

	ResPixelShader* ResMgr::NewPS(const TCHAR* szName,DWORD ver)
	{
		IFS *pFS=m_p->m_pFS[EFST_System];
		return m_p->m_pPSPool->NewRes(szName,pFS,ver,m_p->m_bAsync);
	}

	void ResMgr::ReleasePS(ResPixelShader* pPS)
	{
		if(pPS==NULL)
			return;
		m_p->m_pPSPool->DelRes(pPS);
	}

	void ResMgr::DrawInfo(int x,int y)
	{
		TCHAR szInfo[512];
		_stprintf_s(szInfo,512,_T("Texture pool %dK,Renderable pool %dK,Skeleton pool %dK,SpecialEffect pool %dK,VertShader pool %d,PixelShader pool %dAsyncQueue %d."),
			m_p->m_pTexPool->GetCurSize()/1024,
			m_p->m_pResRenderPool->GetCurSize()/1024,
			m_p->m_pSkeletonPool->GetCurSize()/1024,
			m_p->m_pSpecialEffectPool->GetCurSize()/1024,
			m_p->m_pVSPool->GetCurSize(),
			m_p->m_pPSPool->GetCurSize(),
			AsyncResIO::Inst()->GetQueueSize());
		DrawX::DrawDebugString(szInfo,x,y);
	}

	void ResMgr::Update()
	{
		m_p->m_pTexPool->UpdateLoadingList();
		m_p->m_pResRenderPool->UpdateLoadingList();
		m_p->m_pSkeletonPool->UpdateLoadingList();
		m_p->m_pSpecialEffectPool->UpdateLoadingList();
		m_p->m_pDyncAlphaMapPool->UpdateLoadingList();
		m_p->m_pVSPool->UpdateLoadingList();
		m_p->m_pPSPool->UpdateLoadingList();
	}

	bool CompResInfo(const pair<tstring,int>& info1,const pair<tstring,int>& info2)
	{
		return info1.second > info2.second;
	}
	void ResMgr::Report()
	{
		vector<pair<tstring,int> > texInfoArray;
		m_p->m_pTexPool->ExportInfo(texInfoArray);

		std::sort(texInfoArray.begin(),texInfoArray.end(),CompResInfo);
		LOG(_T("--Texture pool report------------------------------------------------------\r\n"));
		int sum=0;
		for(size_t i=0;i<texInfoArray.size();i++)
		{
			sum+=texInfoArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,texInfoArray[i].second/1024,texInfoArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);

		vector<pair<tstring,int> > renderInfoArray;
		m_p->m_pResRenderPool->ExportInfo(renderInfoArray);

		std::sort(renderInfoArray.begin(),renderInfoArray.end(),CompResInfo);
		LOG(_T("--Renderable pool report------------------------------------------------------\r\n"));
		sum=0;
		for(size_t i=0;i<renderInfoArray.size();i++)
		{
			sum+=renderInfoArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,renderInfoArray[i].second/1024,renderInfoArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);

		vector<pair<tstring,int> > skeletonInfoArray;
		m_p->m_pSkeletonPool->ExportInfo(skeletonInfoArray);

		std::sort(skeletonInfoArray.begin(),skeletonInfoArray.end(),CompResInfo);
		LOG(_T("--Skeleton pool report------------------------------------------------------\r\n"));
		sum=0;
		for(size_t i=0;i<skeletonInfoArray.size();i++)
		{
			sum+=skeletonInfoArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,skeletonInfoArray[i].second/1024,skeletonInfoArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);

		vector<pair<tstring,int> > specialEffectInfoArray;
		m_p->m_pSpecialEffectPool->ExportInfo(specialEffectInfoArray);

		std::sort(specialEffectInfoArray.begin(),specialEffectInfoArray.end(),CompResInfo);
		LOG(_T("--SpecialEffect pool report------------------------------------------------------\r\n"));
		sum=0;
		for(size_t i=0;i<specialEffectInfoArray.size();i++)
		{
			sum+=specialEffectInfoArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,specialEffectInfoArray[i].second/1024,specialEffectInfoArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);

	}

	void ResMgr::DoGC()
	{
		m_p->m_pTexPool->DoGC();
		m_p->m_pResRenderPool->DoGC();
		m_p->m_pSkeletonPool->DoGC();
		m_p->m_pSpecialEffectPool->DoGC();
		m_p->m_pDyncAlphaMapPool->DoGC();
		m_p->m_pVSPool->DoGC();
		m_p->m_pPSPool->DoGC();
	}

	int ResMgr::GetAsyncQueueSize()
	{
		return AsyncResIO::Inst()->GetQueueSize();
	}

	void ResMgr::GetLoadCount( UINT& finishedCount,UINT& reqCount )
	{
		finishedCount=AsyncResIO::Inst()->GetQueuePopCount();
		reqCount=AsyncResIO::Inst()->GetQueuePushCount();
	}

	void ResMgr::ZeroLoadCount()
	{
		AsyncResIO::Inst()->ZeroQueuePushPopCount();
	}

}//namespace Cool3D