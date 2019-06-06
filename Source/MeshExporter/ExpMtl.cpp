#include <windows.h>
#include "ExpMtl.h"
#include <assert.h>
#include "stdmat.h"//max stdmat
#include "MsgBox.h"

namespace Cool3D
{
	ExpMtl::ExpMtl(Mtl* pMtl,int subID,int myID,Mtl *pBakedMtl):F3DExpMtl(pMtl,subID,myID,pBakedMtl)
	{}

	ExpMtl::~ExpMtl()
	{}
}