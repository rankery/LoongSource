//-----------------------------------------------------------------------------
// File: vector.cpp
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "vector.h"

namespace vEngine {

float xVec3D::Length(void) const
{
	ASSERT( this->IsValid() );
    return sqrt( x*x + y*y + z*z );
}

bool xVec3D::IsValid(void) const
{
	if(x*x < 0.0f) return false;
	if(y*y < 0.0f) return false;
	if(z*z < 0.0f) return false;
	return true;
}

//this * in = out
void xVec3D::CrossProduct( const xVec3D *in , xVec3D *out ) const
{
	ASSERT( this->IsValid() );
	ASSERT( in->IsValid() );

	out->x = y*in->z - z*in->y;
	out->y = -x*in->z + z*in->x;
	out->z = x*in->y - y*in->x;
}

void xVec3D::Normalize(void)
{
	float length = this->Length();
	if( XFLOAT_EQUAL(length,0) ) 
    {
        x = z = 0;  y = 1;
        return;
    }
	x /= length;
	y /= length;
	z /= length;
}

}