//-----------------------------------------------------------------------------
// File: crypt.h
// Desc: 提供线程安全的加密解密
// Auth: Lyp
// Date: 2005-11-17
// Last: 2005-11-17
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
class SectCrypt
{
public:

	UINT Encrypt(tagUnitData* pUnit);
	UINT Decrypt(tagUnitData* pUnit);

	SectCrypt(); 
	~SectCrypt();

private:
	TSFPTrunk<SectCrypt>	m_Trunk;
	TObjRef<Util>			m_pUtil;
	
	PBYTE					m_pSect;

	VOID CreateSect();
};


}