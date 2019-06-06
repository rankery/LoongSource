#pragma	   once

//---------------------------------------------------------------------
/**************************************************************
LZSS.C -- A Data Compression Program
(tab = 4 spaces)
***************************************************************
4/6/1989 Haruhiko Okumura
Use, distribute, and modify this program freely.
Please send me your improved versions.
PC-VAN		SCIENCE
NIFTY-Serve	PAF01022
CompuServe	74050,1022
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */


//—πÀı
BOOL Lzss_Compress(BYTE *pInBuf, DWORD dwInBufSize, BYTE *pOutBuf, DWORD & dwOutSize);
//Ω‚—πÀı
BOOL Lzss_DeCompress(BYTE *pInBuf, DWORD dwInBufSize, BYTE *pOutBuf, DWORD & dwOutSize);

void InitTree(void);

void InsertNode(int r);

void DeleteNode(int p);



