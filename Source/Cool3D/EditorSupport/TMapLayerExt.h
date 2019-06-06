#pragma once

#include "..\Image\Image.h"
#include "..\Util\Filename.h"

namespace Cool3D
{
	struct tagTMapLayerExt : public tagTMapLayer
	{
		tagTMapLayerExt()
		{
			Zero();
		}
		void Zero()
		{
			memset(this,0,sizeof(tagTMapLayer));
		}
		bool CreateBaseImage(float uScale,float vScale)
		{
			return true;
		}
	};
}//namespace Cool3D