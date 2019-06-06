#include "StdAfx.h"
#include "pet_id_generator.h"

#include "world.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IDGen
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IDGen g_petIDGen;

VOID IDGen::InitLastValidID(DWORD dwLastValidID)
{
	m_Lock.Acquire();
	m_LastValidID = IS_PET(dwLastValidID) ? dwLastValidID : GT_INVALID;
	m_Lock.Release();
	if (!GT_VALID(m_LastValidID))
	{
		ASSERT(0);
		PetIDErr();
	}
}

DWORD IDGen::GenValidID()
{
	DWORD dwRtID = GT_INVALID;
	if (!GT_VALID(m_LastValidID))
	{
		ASSERT(0);
		PetIDErr();
		return dwRtID;
	}

	m_Lock.Acquire();
	dwRtID = m_LastValidID++;
	m_Lock.Release();

	if (!IS_PET(dwRtID))
	{
		m_LastValidID = GT_INVALID;
		PetIDErr();
	}
	return dwRtID;
}

VOID IDGen::PetIDErr()
{
	IMSG(_T("Pet's ID is full please check out if there is any bug!"));
	ILOG->Write(_T("Pet's ID is full please check out if there is any bug!"));
}
