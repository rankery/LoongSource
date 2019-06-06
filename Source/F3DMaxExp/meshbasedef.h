#pragma once
#include <assert.h>

namespace Cool3D
{
	struct Polygon
	{
		unsigned short numVert;
		unsigned short *indices;
		//--
		Polygon(int numvert)
		{
			assert(numvert > 2);
			numVert=numvert;
			indices=new unsigned short[numVert];
		}
		void SetIndex(int index,unsigned short val)
		{
			assert(index < numVert);
			assert(indices != NULL);
			indices[index]=val;
		}
		Polygon()
		{
			numVert=0;
			indices=NULL;
		}
		~Polygon()
		{
			Destroy();
		}
		void Destroy()
		{
			numVert=0;
			if(indices != NULL)
			{
				delete[] indices;
				indices=NULL;
			}
		}
		void Clone(const Polygon& other)
		{
			numVert=other.numVert;
			assert(numVert > 0);
			int msize=sizeof(unsigned short)*numVert;
			indices=new unsigned short[msize];
			memcpy(indices,other.indices,msize);
		}
		Polygon(const Polygon& other)
		{
			Clone(other);
		}
		const Polygon operator = (const Polygon& other)
		{
			if(this == &other)
				return *this;
			Destroy();
			Clone(other);
			return *this;
		}
	};
}//namespace Cool3D