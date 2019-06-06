//-----------------------------------------------------------------------------
//!\file var_mgr.h
//!\brief var变量管理器
//!
//!\date 2007-04-10
//! last 2007-04-10
//!\author Lyp
//!
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum EXDataType
{
	EXDT_Int=0,
	EXDT_Dword,
	EXDT_Float,
	EXDT_String,
	EXDT_Enum,
	EXDT_End
};

struct tagVarDataItem
{
	tstring					strName;	// 名称
	EXDataType				eType;		// 数据类型
	std::vector<tstring>*	pVecEnum;	// 枚举名字列表
	LPVOID					pVar;		// 内存位置

	tagVarDataItem():eType(EXDT_Int),pVecEnum(0),pVar(0) {}
};

//------------------------------------------------------------------------------
//! \brief var变量管理器
//!
//! - 设计概念
//!  -#	记录对象里的指定数据的指针以及相关信息
//!  -#	根据数据的名字ID访问此变量的信息
//!  -#	存盘的时候存储数据的实际值
//------------------------------------------------------------------------------
class VENGINE_API VarMgr
{
public:
	BOOL Register(LPCTSTR szName, INT* pVar, LPCTSTR szEnmuText=NULL); // INT或者枚举
	BOOL Register(LPCTSTR szName, DWORD* pVar);	// DWORD
	BOOL Register(LPCTSTR szName, FLOAT* pVar);	// Float
	BOOL Register(LPCTSTR szName, tagPoint* pVar);	// tagPoint
	BOOL Register(LPCTSTR szName, tagRect* pVar);	// tagRect
	BOOL Register(LPCTSTR szName, tstring* pVar); // std::string
	
	std::vector<tstring>* GetEnumText(LPCTSTR szName)
	{ return  (m_mapData.Peek(m_pUtil->Crc32(szName)))->pVecEnum; }

	VarMgr();~VarMgr();
	//------------------------------------------------------------------------------
	// 相对于区块的操作
	//------------------------------------------------------------------------------
	VOID Save(XmlElement* pXmlElement, BOOL bIgnoreNull=FALSE); //!< 存储
	BOOL Load(XmlElement* pXmlElement); //!< 存读取

private:
	TObjRef<VirtualFileSys>			m_pVFS;
	TObjRef<Util>					m_pUtil;

	TMap<DWORD, tagVarDataItem*>	m_mapData;
	TList<tagVarDataItem*>			m_listData;	// 用来保证顺序的链表
};





}	// namespace vEngine





