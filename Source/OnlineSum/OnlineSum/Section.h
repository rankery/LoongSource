#pragma once
#include "stdafx.h"

struct GameWorld
{
public:
	void				PrintInfo(FLOAT fRate, LPCTSTR szSectionName);
	
	void				SetWorldID(DWORD dwID)			{ m_dwWorldID = dwID; }
	void				SetWorldName(LPCTSTR szName)	{ _tcscpy_s(m_szWorldName, X_SHORT_STRING, szName); }
	void				SetOnline(INT nNum)				{ m_nOnline = nNum; }
	void				SetStatus(BOOL bActive)			{ m_bActive = bActive; }

private:
	DWORD				m_dwWorldID;
	TCHAR				m_szWorldName[X_SHORT_STRING];
	INT					m_nOnline;

	BOOL				m_bActive;
};



class Section
{
public:
	void							PrintInfo(FLOAT fRate);

	void							SetSectionID(DWORD dwID) { m_dwSectionID = dwID; }
	void							SetSectionName(LPCTSTR szName) { _tcscpy_s(m_szSectionName, X_SHORT_STRING, szName); }	
	
	void							AddWorld(INT n, GameWorld* pWorld)	{ m_mapWorld.Add(n, pWorld); }
	LPCTSTR							GetSectionName()	{ return (LPCTSTR)m_szSectionName; }
	TMap<INT, GameWorld*>			GetWorldMap()		{ return m_mapWorld; }
	
private:
	DWORD							m_dwSectionID;						// 大区ID
	TCHAR							m_szSectionName[X_SHORT_STRING];		// 大区名称
	TMap<INT, GameWorld*>			m_mapWorld;	
};