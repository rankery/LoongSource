//-----------------------------------------------------------------------------
//!\file ini_obj.h
//!\brief ini文件对象
//!
//!\date 2004-04-03
//! last 2004-04-03
//!\author Lyp
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {


//------------------------------------------------------------------------------
//! \brief ini文件对象.
//!
//! - 设计观念
//!  -#	ini文件是一个矩阵的形式，我们这里把第一列定为ID(把其的CRC32视为ID)
//!  -#	根据每一键的ID访问此键
//!  -#	在ini文件load时候就获得所有section,key,value的偏移量
//------------------------------------------------------------------------------
class INIObj
{
public:
	BOOL Load(LPCSTR szVFSName, LPCTSTR szFileName, const INT nMethod = GT_INVALID);	//!< 装入ini文件
	VOID Unload();	//!< 卸出ini文件

	//! 得到指定键的内容
	PBYTE Read(DWORD& dwSize, LPCTSTR szKeyName, LPCTSTR szSectionName);
	//! 得到总共行数
	INT GetRowNum() { return m_mapOffset.size(); }

	//! 将数据传到指定container
	BOOL PutToContainer(VarContainer* pVarContainer);

	INIObj();~INIObj();

private:
	TObjRef<VirtualFileSys>			m_pVFS;
	TObjRef<Util>					m_pUtil;

	typedef std::vector<DWORD>	VEC_DWORD;
	typedef	std::map<DWORD, VEC_DWORD>	MAP_VEC_DWORD;

	std::map<DWORD, MAP_VEC_DWORD>	m_mapOffset;
	std::map<DWORD, tstring>		m_mapSectionName;
	PBYTE							m_pData; //!< ini 数据块
	DWORD							m_dwDataSize; //!< ini 数据块大小

};







}	// namespace vEngine
