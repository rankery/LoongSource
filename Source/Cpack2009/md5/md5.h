//-------------------------------------------------------------------
#pragma		once

//-------------------------------------------------------------------
typedef struct 
{
	UINT4	state[4];                                   /* state (ABCD) */
	UINT4	count[2];        /* number of bits, modulo 2^64 (lsb first) */
	BYTE	buffer[64];                         /* input buffer */
} MD5_CTX;
//-------------------------------------------------------------------
class CMD5
{
public:
	static BOOL	MD5ForBuffer(BYTE *pBuffer, UINT uBufferLen, BYTE digest[16]);
	static BOOL	MD5ForString(char *szString, BYTE digest[16]);
	static BOOL	MD5ForFile(char *szFileName, BYTE digest[16]);
protected:
	static void	MD5Init();
	static void	MD5Update(BYTE *pInput, UINT uInputLen);
	static void	MD5Final(BYTE digest[16]);
private:
	static void	MD5Transform (UINT4 [4], unsigned char [64]);
	//从UINT4按字节转换为BYTE
	static void	Encode(BYTE *pOutput, UINT4 *pInput, unsigned int);
	//从BYTE按字节转换为UINT4
	static void	Decode(UINT4 *pOutput, BYTE *pInput, unsigned int);
private:
	static MD5_CTX		m_context;
};




