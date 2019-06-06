#include "StdAfx.h"
#include "AvatarCfg.h"

AvatarCfg::AvatarCfg(void)
{
}

AvatarCfg::~AvatarCfg(void)
{
}

static AvatarCfg g_AvatarCfg;
AvatarCfg* AvatarCfg::Inst()
{
	return &g_AvatarCfg;
}

const TCHAR* AvatarCfg::GetHairTypeMdlPath(BYTE bySex, WORD id)
{ 
	if(id<0 || id>=AVATAR_HAIR_TYPE)
		return NULL;
	TCHAR path[512] = {0};
	if(bySex)
	{
		_stprintf(path, _T("data\\model\\avatarboy\\hair\\Mhair%02d_1_hair.fskin"), id+1);
		m_strPath = path;
	}
	else
	{
		_stprintf(path, _T("data\\model\\avatargirl\\hair\\Fhair%02d_1_hair.fskin"), id+1);
		m_strPath = path;
	}
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetHairColorTexPath(BYTE bySex, WORD hairID, WORD colorID)
{
	if(hairID<0 || hairID>=AVATAR_HAIR_TYPE)
		return NULL;
	if(colorID<0 || colorID>=AVATAR_HAIR_COLOR)
		return NULL;
	TCHAR path[512] = {0};
	if(bySex)
	{
		_stprintf(path, _T("data\\model\\avatarboy\\hair\\Mhair%02d_%d.tga"), hairID+1, colorID+1);
	}
	else
	{
		_stprintf(path, _T("data\\model\\avatargirl\\hair\\Fhair%02d_%d.tga"), hairID+1, colorID+1);
	}
	m_strPath = path;
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetFaceMdlPath(BYTE bySex, WORD id)
{
	if(id<0 || id>=AVATAR_FACE)
		return NULL;
	TCHAR path[512] = {0};
	if(bySex)
	{
		_stprintf(path, _T("data\\model\\avatarboy\\face\\Mface%02d_face.fskin"), id+1);
	}
	else
	{
		_stprintf(path, _T("data\\model\\avatargirl\\face\\Fface%02d_face.fskin"), id+1);	
	}
	m_strPath = path;
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetFaceDtlTexPath(BYTE bySex, WORD id)
{
	if(id<0 || id>=AVATAR_FACE_DETAIL)
		return NULL;

	// 默认没有细节贴图
	if(0==id)
		return _T("");

	TCHAR path[512] = {0};
	if(bySex)
	{
		_stprintf(path, _T("data\\model\\avatarboy\\face\\MfaceDetail%02d.tga"), id);
	}
	else
	{
		_stprintf(path, _T("data\\model\\avatargirl\\face\\FfaceDetail%02d.tga"), id);	
	}
	m_strPath = path;
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetUpperUWMdlPath(BYTE bySex,BOOL bFashion)
{
	if(bySex)
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatarboy\\upper\\suw_upper.fskin");
		else
			m_strPath = _T("data\\model\\avatarboy\\upper\\uw_upper.fskin");
	}
	else
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatargirl\\upper\\suw_upper.fskin");
		else
			m_strPath = _T("data\\model\\avatargirl\\upper\\uw_upper.fskin");
	}
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetLowerUWMdlPath(BYTE bySex,BOOL bFashion)
{
	if(bySex)
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatarboy\\lower\\suw_lower.fskin");
		else
			m_strPath = _T("data\\model\\avatarboy\\lower\\uw_lower.fskin");
	}
	else
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatargirl\\lower\\suw_lower.fskin");
		else
			m_strPath = _T("data\\model\\avatargirl\\lower\\uw_lower.fskin");
	}
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetBootUWMdlPath(BYTE bySex,BOOL bFashion)
{
	if(bySex)
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatarboy\\boot\\suw_boot.fskin");
		else
			m_strPath = _T("data\\model\\avatarboy\\boot\\uw_boot.fskin");
	}
	else
	{
		if( bFashion )
			m_strPath = _T("data\\model\\avatargirl\\boot\\suw_boot.fskin");
		else
			m_strPath = _T("data\\model\\avatargirl\\boot\\uw_boot.fskin");
	}
	return m_strPath.c_str();
}

const TCHAR* AvatarCfg::GetGloveUWMdlPath(BYTE bySex, BOOL bRight,BOOL bFashion)
{
	if(bySex)
	{
		if( bFashion )
		{
			if( bRight )
				m_strPath = _T("data\\model\\avatarboy\\glove\\suw_r_glove.fskin");
			else
				m_strPath = _T("data\\model\\avatarboy\\glove\\suw_l_glove.fskin");
		}
		else
		{
			if( bRight )
				m_strPath = _T("data\\model\\avatarboy\\glove\\uw_r_glove.fskin");
			else
				m_strPath = _T("data\\model\\avatarboy\\glove\\uw_l_glove.fskin");
		}
	}
	else
	{
		if( bFashion )
		{
			if( bRight )
				m_strPath = _T("data\\model\\avatargirl\\glove\\suw_r_glove.fskin");
			else
				m_strPath = _T("data\\model\\avatargirl\\glove\\suw_l_glove.fskin");
		}
		else
		{
			if( bRight )
				m_strPath = _T("data\\model\\avatargirl\\glove\\uw_r_glove.fskin");
			else
				m_strPath = _T("data\\model\\avatargirl\\glove\\uw_l_glove.fskin");
		}
	}
	return m_strPath.c_str();
}

void AvatarCfg::MakeNewPlayerFahion( const BYTE bySex, const int n, tagAvatarEquip& out ) const
{
	ASSERT( n >= 0 && n < AVATAR_DRESS );
	ZeroMemory( &out, sizeof(tagAvatarEquip) );
	const int nPartSex = 10 * ( 1 - bySex );
	const int nPartIndex = 100 * n;
	DWORD dwStartTypeID = 9000000 + nPartSex + nPartIndex;
	out.Set( EAE_Upper, dwStartTypeID + 1, 0, 0 );
	out.Set( EAE_Lower, dwStartTypeID + 2, 0, 0 );
	out.Set( EAE_Glove, dwStartTypeID + 3, 0, 0 );
	out.Set( EAE_Boot,  dwStartTypeID + 4, 0, 0 );
}