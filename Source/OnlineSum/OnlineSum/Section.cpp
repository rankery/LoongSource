#include "stdafx.h"
#include "Section.h"


void GameWorld::PrintInfo(FLOAT fRate, LPCTSTR szSectionName)
{
	INT nSum = INT(m_nOnline * fRate);

	_tprintf(_T(" | %-20s %-20s %-d \n"), szSectionName, m_szWorldName, nSum);
}

void Section::PrintInfo(FLOAT fRate)
{
	TMap<INT, GameWorld*>::TMapIterator it = m_mapWorld.Begin();
	GameWorld* pWorld = NULL;

	while(m_mapWorld.PeekNext(it, pWorld))
	{
		pWorld->PrintInfo(fRate, m_szSectionName);
	}
}