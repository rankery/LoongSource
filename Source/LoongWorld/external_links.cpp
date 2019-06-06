#include "stdafx.h"

#include "Role.h"

#include "external_links.h"


VOID ExternalLinks::SendMessageToPlayer(Role * pPly)
{
	if( P_VALID(pPly) )
	{
		jm_tool::MutexGuard<jm_tool::CSCreatedLock> guard(m_lckTotal);
		PTagNS_GetExternalLinks p = *this;
		if( P_VALID(p) )
			pPly->SendMessage  (p,p->dwSize);
	}
}

