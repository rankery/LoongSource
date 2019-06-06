#include "StdAfx.h"
#include "NavMapEditorUtil.h"

namespace WorldBase
{
	void NavMapEditorUtil::MergeCangoMap( BitMap* pSrc,BitMap* pTarget )
	{
		ASSERT(pSrc->Width()==pTarget->Width());
		ASSERT(pSrc->Height()==pTarget->Height());

		for(UINT z=0;z<pSrc->Height();z++)
		{
			for(UINT x=0;x<pSrc->Width();x++)
			{
				if(!pSrc->GetValue(x,z))
					pTarget->SetValue(x,z,false);
			}
		}
	}
}//namespace WorldBase