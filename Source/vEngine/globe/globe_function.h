//-----------------------------------------------------------------------------
// File: globe_function
// Auth: Lyp
// Date: 2004/3/14
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine
{
//-----------------------------------------------------------------------------
// 基本操作
//-----------------------------------------------------------------------------
VENGINE_API BOOL InitEngine();
VENGINE_API VOID DestroyEngine();
VENGINE_API VOID SetDefaultDir();	// 设置默认路径
VENGINE_API BOOL InitNetwork();		// 初始化网络层
VENGINE_API VOID DestroyNetwork();	

//-----------------------------------------------------------------------------
// 对象操作
//-----------------------------------------------------------------------------
//! 得到向指定对象的指针
VENGINE_API LPVOID GetObj(LPCSTR szName);
//! 根据注册过的对象创建对象
VENGINE_API LPCSTR CreateObj(LPCSTR szDestObjName, LPCSTR szSrcObjName);
//! 尝试析构对象
VENGINE_API BOOL KillObj(LPCSTR szObjName);


//-----------------------------------------------------------------------------
// 异常处理
//-----------------------------------------------------------------------------
// 结构化异常转换
VENGINE_API VOID SEH_Translation(UINT unCode, EXCEPTION_POINTERS* pExp);
// 配合::SetUnhandledExceptionFilter使用的异常处理函数
// 当无法使用try...catch块时可以使用
VENGINE_API LONG WINAPI SEH_ExceptionFilter(EXCEPTION_POINTERS* pExp);
// 设置邮寄打包mini_dump
VENGINE_API VOID InitDump(LPCTSTR szIP, LPCTSTR szUser, LPCTSTR szPassword, LPCTSTR szVersion=NULL);

//-----------------------------------------------------------------------------
// 注册新的对象
//-----------------------------------------------------------------------------
template<typename class_type>
VOID Register(vEngine::TWrap<class_type> wrap, LPCSTR szName)
{ vEngine::g_pInterfaceMgr->Register(wrap, szName); }


//-----------------------------------------------------------------------------
//	客户端->服务器数据包头扰码处理	by: Jampe
//-----------------------------------------------------------------------------
#define XCS_KEY						0x12345678

#define XCS_FLAG					0xC0000000

#define XCS_MAX_SIZE				0x00FFFFFF

#define XCS_SWAP_WORD(n)			((((n) & 0xFF00) >> 8) | (((n) & 0xFF) << 8))

#define XCS_SWAP_DWORD(n)			((XCS_SWAP_WORD(((n) & 0xFFFF0000) >> 16)) | (XCS_SWAP_WORD((n) & 0xFFFF) << 16))

//	解密包头长度信息
#define XCS_FIX_SIZE(n)				do{													\
										(n) ^= XCS_KEY;									\
										(n) = XCS_SWAP_DWORD(n);						\
										if(XCS_FLAG == ((n) & XCS_FLAG))				\
											(n) = XCS_MAX_SIZE - (XCS_MAX_SIZE & (n));	\
										else											\
											(n) = 0x7FFFFFFF;							\
									}while(0)

//	加密包头长度信息
#define XCS_SET_SIZE(n)				do{													\
										INT r = m_pUtil->RandomInRange(0, 0x0F);		\
										r <<= 24;										\
										(n) = XCS_FLAG | r | (XCS_MAX_SIZE - (n));		\
										(n) = XCS_SWAP_DWORD(n);						\
										(n) ^= XCS_KEY;									\
									}while(0)


} // namespace vEngine {



