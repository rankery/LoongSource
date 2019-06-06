#pragma once

const INT X_STRENGTH_RANKINGS_LEN = 500;

struct tagStrength
{
	DWORD			dwValue;			// 实力值
	DWORD			m_dwHeroism;		// 英雄值
	DWORD			m_dwWit;			// 谋略值
	DWORD			m_dwErrantry;		// 侠义值
	DWORD			m_dwValor;			// 征战值

	tagStrength()
	{
		dwValue = m_dwErrantry + m_dwHeroism + m_dwValor + m_dwWit;
	}
};

struct tagStrengthRoleNode
{
	INT		nNum;						// 该角色名次	
	INT		nPreNum;					// 该角色上次名次	
	TCHAR	szRoleName[X_SHORT_NAME];	// 角色名字
	DWORD	dwRoleID;					// 角色ID
	DWORD	dwStrength;					// 该角色的实力值
};
