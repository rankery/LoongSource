#pragma once

struct tagGuildMemberClient;
struct tagGuildMemInfo;
struct tagGuildMemInfoEx;

class GuildMembers
{
	friend class GuildMgr;

public:
	typedef vector<tagGuildMemberClient*> MembersArray;

	enum EListColType
	{
		ELCT_Name = 0,		// 玩家姓名
		ELCT_Pos,			// 玩家职位
		ELCT_Level,			// 玩家等级
		ELCT_Sex,			// 玩家性别
		ELCT_Class,			// 玩家职业
		ELCT_Cont,			// 玩家贡献
		ELCT_Online,		// 玩家是否在线

		ELCT_End,
	};

public:
	GuildMembers(void);
	~GuildMembers(void);
	
	/** \根据不同列类型排序
		\param eType为列类型
	*/
	VOID SortMember(EListColType eType);
	/** \增加一个成员
		\param sInfo为该成员的基本信息
	*/
	VOID AddMember(const tagGuildMemInfo& sInfo);
	/** \增加一个成员
		\param sInfo为该成员的基本信息
		\param sInfoEx为该成员的扩展信息
	*/
	VOID AddMember(const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx);
	/** \修改一个成员
		\param dwRoleID为该成员ID
		\param sInfoEx为该成员的扩展信息
	*/
	VOID ModifyMember(DWORD dwRoleID, const tagGuildMemInfoEx& sInfoEx);
	/** \修改一个成员
		\param dwRoleID为该成员ID
		\param sInfoEx为该成员的扩展信息
		\param nIndex为该成员的位置索引（方便查找该成员）
	*/
	VOID ModifyMember(DWORD dwRoleID, const tagGuildMemInfoEx& sInfoEx, INT nIndex);
	/** \修改一个成员
		\param sInfo为该成员的基本信息
		\param sInfoEx为该成员的扩展信息
	*/
	VOID ModifyMember(const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx);
	/** \修改一个成员
		\param sInfo为该成员的基本信息
		\param sInfoEx为该成员的扩展信息
		\param nIndex为该成员的位置索引（方便查找该成员）
	*/
	VOID ModifyMember(const tagGuildMemInfo& sInfo, const tagGuildMemInfoEx& sInfoEx, INT nIndex);
	/** \删除一个成员
		\param dwRoleID为该成员的ID
	*/
	VOID RemoveMember(DWORD dwRoleID);
	/** \获取成员列表
		\param members为输出的成员列表
	*/
	VOID GetMembersList(const MembersArray** members) const;

private:
	/** \根据ID查找一个成员
		\param dwRoleID为该成员的ID
	*/
	tagGuildMemberClient* FindMember(DWORD dwRoleID);
	/** \根据ID查找索引
		\param dwRoleID为该成员的ID
	*/
	INT FindIndex(DWORD dwRoleID);
	/** \清除成员列表
	*/
	VOID FreeArray();

private:
	MembersArray	m_Members;		// 成员列表
	
};
