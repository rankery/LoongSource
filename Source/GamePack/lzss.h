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


//压缩
BOOL Lzss_Compress(BYTE *pInBuf, DWORD dwInBufSize, BYTE *pOutBuf, DWORD & dwOutSize);
//解压缩
BOOL Lzss_DeCompress(BYTE *pInBuf, DWORD dwInBufSize, BYTE *pOutBuf, DWORD & dwOutSize);
//初始化二叉树
void InitTree(void);
//插入节点
void InsertNode(int r);
//删除节点
void DeleteNode(int p);



