#include "StdAfx.h"
#include "GuildMembers.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\guild_define.h"

namespace
{
	/** \名字排序规则
	*/
	class MembersNameSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			LPCTSTR szName1 = PlayerNameTab::Inst()->FindNameByID(pMember1->sInfo.dwRoleID);
			LPCTSTR	szName2 = PlayerNameTab::Inst()->FindNameByID(pMember2->sInfo.dwRoleID);

			return _tcscmp(szName1,szName2) < 0;
		}
	};
	/** \等级排序规则
	*/
	class MembersLevelSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			return pMember1->sInfo.byLevel > pMember2->sInfo.byLevel;
		}
	};
	/** \职位排序规则
	*/
	class MembersPosSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			return pMember1->sInfo.n8GuildPos < pMember2->sInfo.n8GuildPos;
		}
	};
	/** \性别排序规则
	*/
	class MembersSexSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			return pMember1->sInfo.bySex > pMember2->sInfo.bySex;
		}
	};
	/** \职业排序规则
	*/
	class MembersClassSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			return FALSE;
		}
	};
	/** \是否在线排序规则
	*/
	class MembersOnlineSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			if(pMember1->sInfo.bOnline == FALSE && pMember2->sInfo.bOnline == TRUE)
				return FALSE;

			if(pMember1->sInfo.bOnline == TRUE && pMember2->sInfo.bOnline == FALSE)
				return TRUE;
			
			if(pMember1->sInfo.bOnline == TRUE)
				return FALSE;
			
			return pMember1->sInfo.dwTimeLastLogout > pMember2->sInfo.dwTimeLastLogout;
		}
	};
	/** \贡献排序规则
	*/
	class MembersContributionSort
	{
	public:
		bool operator()(const tagGuildMemberClient* pMember1,const tagGuildMemberClient* pMember2)
		{
			return pMember1->sInfo.nCurContrib > pMember2->sInfo.nCurContrib;
		}
	};
}

GuildMembers::GuildMembers(void)
{
}

GuildMembers::~GuildMembers(void)
{
	FreeArray();
}

VOID GuildMembers::SortMember( EListColType eType )
{
	// 根据该列类型排序
	switch(eType)
	{
	case ELCT_Name:
		{
			MembersNameSort sortObj;
			sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Pos:
		{
			//MembersPosSort sortObj;
			//sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Level:
		{
			MembersLevelSort sortObj;
			sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Sex:
		{
			MembersSexSort sortObj;
			sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Class:
		{
			//MembersClassSort sortObj;
			//sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Cont:
		{
			MembersContributionSort sortObj;
			sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	case ELCT_Online:
		{
			MembersOnlineSort sortObj;
			sort(m_Members.begin(), m_Members.end(), sortObj);
		}
		break;
	}
}

VOID GuildMembers::AddMember( const tagGuildMemInfo& sInfo )
{
	// 创建一个新的成员
	tagGuildMemberClient* pMember = new tagGuildMemberClient(sInfo);
	m_Members.push_back(pMember);
}

VOID GuildMembers::AddMember( const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx )
{
	// 创建一个新的成员
	tagGuildMemberClient* pMember = new tagGuildMemberClient(sInfo);
	// 赋予扩展信息
	pMember->Modify(sInfoEx);
	// 插入列表	
	m_Members.push_back(pMember);
}

VOID GuildMembers::ModifyMember( DWORD dwRoleID, const tagGuildMemInfoEx& sInfoEx )
{
	// 根据ID查找成员，修改该成员的扩展信息
	tagGuildMemberClient* pMember = FindMember(dwRoleID);
	if(P_VALID(pMember))
	{
		pMember->Modify(sInfoEx);
	}
}

VOID GuildMembers::ModifyMember( DWORD dwRoleID, const tagGuildMemInfoEx& sInfoEx, INT nIndex )
{
	// 根据索引查找成员
	tagGuildMemberClient* pMember = m_Members[nIndex];
	// 校验两个成员是否相同
	if(P_VALID(pMember) && pMember->sInfo.dwRoleID == dwRoleID)
	{
		// 相同则修改成员的扩展信息
		pMember->Modify(sInfoEx);
	}
	else
	{	
		// 不同则根据ID修改成员
		ModifyMember(dwRoleID, sInfoEx);
	}
}

VOID GuildMembers::ModifyMember( const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx )
{
	// 根据ID查找成员，修改成员的普通信息和扩展信息
	tagGuildMemberClient* pMember = FindMember(sInfo.dwRoleID);
	if(P_VALID(pMember))
	{
		pMember->Modify(sInfo, sInfoEx);
	}
	else
	{
		// 如果没找到该成员，则新增该成员
		AddMember(sInfo);
		ModifyMember(sInfo.dwRoleID, sInfoEx);
	}
}

VOID GuildMembers::ModifyMember( const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx, INT nIndex )
{
	// 根据索引查找该成员
	tagGuildMemberClient* pMember = m_Members[nIndex];
	// 校验成员是否是同一个
	if(P_VALID(pMember) && pMember->sInfo.dwRoleID == sInfo.dwRoleID)
	{
		// 相同则修改该成员的普通信息和扩展信息
		pMember->Modify(sInfo, sInfoEx);
	}
	else
	{
		// 不同则根据ID修改成员
		ModifyMember(sInfo, sInfoEx);
	}
}

VOID GuildMembers::RemoveMember( DWORD dwRoleID )
{
	for(MembersArray::iterator it = m_Members.begin(); 
		it != m_Members.end(); ++it)
	{
		// 根据ID寻找成员并删除该成员
		tagGuildMemberClient* pMember = *it;
		if(P_VALID(pMember) && pMember->sInfo.dwRoleID == dwRoleID)
		{
			m_Members.erase(it);
			SAFE_DEL(pMember);
			break;
		}
	}
}

tagGuildMemberClient* GuildMembers::FindMember( DWORD dwRoleID )
{
	// 循环检索成员列表，寻找ID相同的成员
	for(MembersArray::iterator it = m_Members.begin(); 
		it != m_Members.end(); ++it)
	{
		tagGuildMemberClient* pMember = *it;
		if(P_VALID(pMember) && pMember->sInfo.dwRoleID == dwRoleID)
			return pMember;
	}
	return NULL;
}

VOID GuildMembers::GetMembersList( const MembersArray** members ) const
{
	// 获取成员列表
	*members = &m_Members;
}

VOID GuildMembers::FreeArray()
{
	// 清除成员列表
	for(MembersArray::iterator it = m_Members.begin(); 
		it != m_Members.end(); ++it)
	{
		tagGuildMemberClient* pMember = *it;
		SAFE_DEL(pMember);
	}
	m_Members.clear();
}

INT GuildMembers::FindIndex( DWORD dwRoleID )
{
	INT nIndex = 0;
	// 循环检索成员列表，寻找ID相同的成员
	for(MembersArray::iterator it = m_Members.begin(); 
		it != m_Members.end(); ++it)
	{
		tagGuildMemberClient* pMember = *it;
		if(P_VALID(pMember) && pMember->sInfo.dwRoleID == dwRoleID)
			return nIndex;
		// 累加计算索引
		++nIndex;
	}
	// 没有则返回-1
	return GT_INVALID;
}