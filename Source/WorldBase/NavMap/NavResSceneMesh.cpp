#include "stdafx.h"
#include "NavResSceneMesh.h"
#include "NavOctreeCollider.h"
#include "NavBintreeCollider.h"
#include "..\..\Cool3D\Public\StaticMeshFile.h"

namespace WorldBase
{
	IMP_RTTI(NavResSceneMesh,NavResBase);

	class NavResSceneMesh::Member
	{
	public:
		AABBox				m_box;
		NavBaseCollider		*m_pCollider;

	public:
		Member()
			: m_pCollider(0)
		{
		}

		~Member()
		{
			SAFE_DELETE(m_pCollider);
		}

		void LoadFromFile(IFS *pFS,const TCHAR* szName)
		{
			ASSERT(pFS!=NULL);
			ASSERT(szName!=NULL);
			ASSERT(_tcslen(szName) > 0);

			DWORD hFile=pFS->Open(szName);
			THROW_NULLEX(hFile,_T("NavResSceneMesh open file failed."),szName);

			SceneMeshNavHeader header;
			pFS->Read(&header,sizeof(SceneMeshNavHeader),hFile);
			if( header.ver == 1 )//°Ë²æÊ÷
			{
				m_pCollider = new NavOctreeCollider;
			}
			else if( header.ver == 2 )//¶þ²æÊ÷
			{
				m_pCollider = new NavBintreeCollider;
			}
			THROW_NULLEX(m_pCollider,_T("NavCollider create failed."),szName);

			pFS->Seek(hFile,header.offsetOctree,FILE_BEGIN);
			m_pCollider->ReadFile(pFS,hFile);
			m_box=m_pCollider->GetBigBox();

			pFS->Close(hFile);
		}
	};

	NavResSceneMesh::NavResSceneMesh( const TCHAR* szName )
		: NavResBase(szName)
	{
		m_p=new Member;
	}

	NavResSceneMesh::~NavResSceneMesh()
	{
		SAFE_DELETE(m_p);
	}

	UINT NavResSceneMesh::CreateContent( DWORD param )
	{
		m_p->LoadFromFile(GetSafeFS(),GetName());

		return NavResBase::CreateContent(param);
	}

	bool NavResSceneMesh::AABBoxCollideBoxTopHighest( const AABBox& box,float& intersectY )
	{
		return m_p->m_pCollider->AABBoxCollideBoxTopHighest(box,intersectY);
	}

	bool NavResSceneMesh::AABBoxCollideBox( const AABBox& box )
	{
		return m_p->m_pCollider->AABBoxCollideBox(box);
	}

	const AABBox& NavResSceneMesh::GetBox()
	{
		return m_p->m_box;
	}

	TResult NavResSceneMesh::RayCollideBox( const Ray& ray )
	{
		return m_p->m_pCollider->RayCollideBox(ray);
	}

	bool NavResSceneMesh::RayCollideBoxOnce(const Ray& ray)
	{
		return m_p->m_pCollider->RayCollideBoxOnce(ray);
	}

	void NavResSceneMesh::DrawBox()
	{
		m_p->m_pCollider->DrawBox();
	}
}//namespace WorldBase