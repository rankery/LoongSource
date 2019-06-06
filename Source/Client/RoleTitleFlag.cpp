#include "stdafx.h"
#include "RoleTitleFlag.h"

RoleTitleFlag::RoleTitleFlag( void )
{

}

RoleTitleFlag::~RoleTitleFlag( void )
{
	Destroy();
}

void RoleTitleFlag::Destroy()
{
	map<tstring, ResTexture*>::iterator it = m_mapRes.begin();
	while(it!=m_mapRes.end())
	{	
		IDraw2D::Inst()->ReleaseTexture(it->second);
		it = m_mapRes.erase(it);
	}
	m_mapRes.clear();

	map<tstring, ResTexture*>::iterator it1 = m_mapResTeam.begin();
	while(it1!=m_mapResTeam.end())
	{
		IDraw2D::Inst()->ReleaseTexture(it1->second);
		it1 = m_mapResTeam.erase(it1);
	}
	m_mapResTeam.clear();
}

void RoleTitleFlag::DrawRoleTitleFlag( RECT rc, float z, const TCHAR* szTitle )
{
	if( szTitle==NULL || _tcslen(szTitle)==0 )
		return;

	tstring strRes = szTitle;
	map<tstring, ResTexture*>::iterator it = m_mapRes.find(strRes);
	if( it!=m_mapRes.end() )
	{
		DrawRoleTitleFlag(rc, z, it->second);
	}
	else 
	{
		TCHAR szFlag[_MAX_PATH]={0};
		IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
		_stprintf(szFlag, _T("data\\ui\\mark\\title\\%s.bmp"), szTitle);
		ResTexture *newTex = IDraw2D::Inst()->NewTexture(szFlag, pIFS);
		m_mapRes.insert( make_pair(strRes, newTex) );
		DrawRoleTitleFlag(rc, z, newTex);
	}
}

void RoleTitleFlag::DrawRoleTitleFlag( RECT rc, float z, ResTexture* pRes )
{
	if( P_VALID(pRes) )
	{
		RECT srcRC;

		srcRC.top=srcRC.left=0;
		srcRC.bottom=srcRC.top+14;
		srcRC.right=srcRC.left+16;

		IDraw2D::Inst()->Draw(&rc, &srcRC, pRes, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, z);
	}
}

void RoleTitleFlag::DrawTeamFlag( RECT rc, float z, const TCHAR* szTeam )
{
	if( szTeam==NULL || _tcslen(szTeam)==0 )
		return;

	tstring strRes = szTeam;
	map<tstring, ResTexture*>::iterator it = m_mapResTeam.find(strRes);
	if( it!=m_mapResTeam.end() )
	{
		DrawRoleTitleFlag(rc, z, it->second);
	}
	else 
	{
		IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
		ResTexture *newTex = IDraw2D::Inst()->NewTexture(szTeam, pIFS);
		m_mapResTeam.insert( make_pair(strRes, newTex) );
		DrawTeamFlag(rc, z, newTex);
	}
}

void RoleTitleFlag::DrawTeamFlag( RECT rc, float z, ResTexture* pRes )
{
	if( P_VALID(pRes) )
	{
		RECT srcRC;

		srcRC.top=srcRC.left=0;
		srcRC.bottom=srcRC.top+16;
		srcRC.right=srcRC.left+15;

		IDraw2D::Inst()->Draw(&rc, &srcRC, pRes, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, z);
	}
}