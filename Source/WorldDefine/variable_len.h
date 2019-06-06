//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: variable_len.h
// author: 
// actor:
// data: 2009-04-10
// last:
// brief: 不同语言版本字符(TCHAR)个数结构定义
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
struct tagVariableLen
{
	// 角色名字
	INT		nRoleNameMax;
	INT		nRoleNameMin;
	
	// 帮派相关
	INT		nGuildNameMax;
	INT		nGuildNameMin;
	INT		nGuildTenet;

	// 摆摊
	INT		nStallTitleMax;

	tagVariableLen() { ZeroMemory(this, sizeof(*this)); }

	BOOL CheckValid() const
	{
		return (IsRoleNameValid()
				&& IsGuildNameValid()
				&& IsStallTitleValid());
	}

private:
	BOOL IsRoleNameValid() const
	{
		return !(nRoleNameMin < 0 || nRoleNameMax < nRoleNameMin || X_SHORT_NAME < nRoleNameMax);
	}

	BOOL IsGuildNameValid() const
	{
		return !(nGuildNameMin < 0 || nGuildNameMin > nGuildNameMax || nGuildTenet < 0);
	}

	BOOL IsStallTitleValid() const
	{
		return nStallTitleMax > 0;
	}
};