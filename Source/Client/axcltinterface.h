
typedef long ( *_FUNC_C_SEND ) ( const char * pBuffer, int nLen );
typedef long ( *_FUNC_C_REC ) ( const char * pBuffer, int nLen );

extern "C" int ax_CHCStart( _FUNC_C_SEND pfSend, _FUNC_C_REC & pfRec );
extern "C" void ax_CHCEnd();
