/********************************************************************
	created:	2009/12/18
	created:	18:12:2009   11:40
	filename: 	e:\loong\trunk\LoongWorld\external_links.h
	file path:	e:\loong\trunk\LoongWorld
	file base:	external_links
	file ext:	h
	author:		Jason
	
	purpose:	处理简单的外部链接，
*********************************************************************/


#ifndef external_links_h__
#define external_links_h__

#include "../WorldDefine/msg_external_links.h"
#include "../ServerDefine/base_define.h"
#include "mini_tool.h"


class Role;

struct tagSExternalLink
{
	tstring LinkName;
	tstring LinkURL;
	tagSExternalLink(){}
	tagSExternalLink(int){}
	tagSExternalLink(const tagSExternalLink & t)
		:LinkURL(t.LinkURL),LinkName(t.LinkName)
	{
	}
	tagSExternalLink & operator = (const tagSExternalLink & t)
	{
		LinkName	= t.LinkName;
		LinkURL		= t.LinkURL;
		return *this;
	}
};


/**
 * \ingroup LoongWorld
 *
 *	每次需要NS_GetExternalLinks的时候，可以用这个玩意，自己回收内存，使用全局内存池
 *	这个对象，是暂态对象，发完消息，就不要继续使用了
 * 
 * \version 1.0
 * first version
 *
 * \date 2009-12-18
 *
 * \author Jason
 *
 */
class ExternalLinks
{
	jm_tool::CSCreatedLock m_lckTotal;
public:
	typedef tagNS_GetExternalLinks * PTagNS_GetExternalLinks;

private:
	operator PTagNS_GetExternalLinks()
	{
		return m_pNS_GetExternalLinks;
	}

public:

	ExternalLinks()
		:m_pNS_GetExternalLinks(NULL)
	{
	}
	~ExternalLinks()
	{
		jm_tool::MutexGuard<jm_tool::CSCreatedLock> guard(m_lckTotal);
		Free();
	}

	
	void FillExternalLinks(TList<tagSExternalLink> & ExtLinkLst)
	{
		jm_tool::MutexGuard<jm_tool::CSCreatedLock> guard(m_lckTotal);
		
		Free();

		int size = ExtLinkLst.Size  ();
		if( size > 0 )
		{
			DWORD totalsize = sizeof(tagNS_GetExternalLinks) + (size - 1) * sizeof(tagExternalLink);
			m_pNS_GetExternalLinks = (PTagNS_GetExternalLinks)g_MemPoolSafe.Alloc  (totalsize);
			memset(m_pNS_GetExternalLinks,0,totalsize);
			tagNS_GetExternalLinks tem;
			memcpy(m_pNS_GetExternalLinks,&tem,sizeof(tagNS_GetExternalLinks));
			tagExternalLink * p = m_pNS_GetExternalLinks->links;
			int i = 0;
			int l = 0;
			m_pNS_GetExternalLinks->dwSize = totalsize;
			m_pNS_GetExternalLinks->dwElementNum = size > 6 ? 6 : size;
			while( ExtLinkLst.Size  () > 0 )
			{
				tagSExternalLink t = ExtLinkLst.PopFront();
				if( t.LinkName.length  () > 8 - 1 )
					l = 8 - 1;
				else
					l = t.LinkName.length  ();

				_tcsncpy( p[i].linkName,t.LinkName.c_str  (),l );
				if( t.LinkURL.length  () > X_LONG_NAME - 1 )
					l = X_LONG_NAME - 1;
				else
					l = t.LinkURL.length  ();

				_tcsncpy( p[i].linkValue,t.LinkURL.c_str  (),l );

				if ( ++i > 6 )
					break;
			}
		}
	}

	VOID SendMessageToPlayer(Role * pPly);

private:
	PTagNS_GetExternalLinks m_pNS_GetExternalLinks;
	// not tread safety
	void Free()
	{
		PTagNS_GetExternalLinks p = m_pNS_GetExternalLinks;
		m_pNS_GetExternalLinks = NULL;
		if( P_VALID(p) )
			g_MemPoolSafe.Free  (p);
	}
};

#endif // external_links_h__

