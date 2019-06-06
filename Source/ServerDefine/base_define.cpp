#pragma warning(disable:4355)	// Level 3: "this": 用于基成员初始值设定项列表
#pragma warning(disable:4251)	// Level 3: need to have dll-interface
#pragma warning(disable:4996)
#pragma warning(disable:4311)
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4245)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4127)
#pragma warning(disable:4312)

#include "../vEngine/vEngine.h"
vEngine::SafeMemPool s_MemPoolSafe;

#include "base_define.h"
ServerMemPool g_MemPoolSafe;

LPVOID ServerMemPool::Alloc( DWORD dwSize )
{
	return s_MemPoolSafe.Alloc(dwSize);
}

VOID ServerMemPool::Free( LPVOID pMem )
{
	s_MemPoolSafe.Free(pMem);
}