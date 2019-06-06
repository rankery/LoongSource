#ifndef _PROOF_Dll_INTERFACE_H_
#define _PROOF_Dll_INTERFACE_H_

/**
/	迅雷验证部分 add by songg 2009.6.22
*/

//迅雷   用于加密数据，deData为待加密串，nBufLen为待加密串的长度，enData为加密后串；
typedef int ( *PROOF_XUNLEI_AESENCRYPT)(const char *deData, char *enData, int nBufLen);
//迅雷   用于解密数据，enData为解密前的串，deData为解密后的串。
typedef void ( *PROOF_XUNLEI_AESDECRYPT)(const char *enData, char *deData);

extern void InitProofXunleiInterface(HINSTANCE);

extern PROOF_XUNLEI_AESENCRYPT        PROOF_XUNLEI_AesEncrypt;
extern PROOF_XUNLEI_AESDECRYPT        PROOF_XUNLEI_AesDecrypt;





#endif