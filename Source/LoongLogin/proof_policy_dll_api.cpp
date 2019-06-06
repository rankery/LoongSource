#include "stdafx.h"
#include "proof_policy_dll_api.h"

PROOF_XUNLEI_AESENCRYPT        PROOF_XUNLEI_AesEncrypt;

PROOF_XUNLEI_AESDECRYPT        PROOF_XUNLEI_AesDecrypt;

//初始化验证接口
void InitProofXunleiInterface(HINSTANCE hInst)
{
	PROOF_XUNLEI_AesEncrypt	   = (PROOF_XUNLEI_AESENCRYPT)GetProcAddress(hInst,"AesEncrypt");

	PROOF_XUNLEI_AesDecrypt	   = (PROOF_XUNLEI_AESDECRYPT)GetProcAddress(hInst, "AesDecrypt");	
}