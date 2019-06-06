//-----------------------------------------------------------------------------
// File: Util.h
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// π§æﬂ¿‡
//-----------------------------------------------------------------------------
#define XZERO_TOLERANCE  (0.005f)
#define XFLOAT_EQUAL( x, y )  ( ffastfabs((x)-(y)) < XZERO_TOLERANCE )
#define ffastfabs(f)		((float)fabs((double)(f)))


struct xVec3D
{
	float x,y,z;

	xVec3D (float x1=0.0f, float y1=0.0f, float z1=0.0f):x(x1),y(y1),z(z1) {}

	void	operator=( const xVec3D &v) { x=v.x , y=v.y , z=v.z; }
	void	operator+=( const xVec3D &v0 ){ x += v0.x; y += v0.y; z += v0.z;}
	void	operator-=( const xVec3D &v0 ){ x -= v0.x; y -= v0.y; z -= v0.z;}
    void    operator*=( const float scale ){ x*=scale; y*=scale; z*=scale; }
    void    operator/=( const float scale ){ x/=scale; y/=scale; z/=scale; }
	bool	operator==( const xVec3D &v0 )
	{ return XFLOAT_EQUAL(x,v0.x)&&XFLOAT_EQUAL(y,v0.y)&&XFLOAT_EQUAL(z,v0.z); }
    xVec3D  operator-() const{ return xVec3D(-x, -y, -z); }
	float   LengthSquare(void) const { return x*x + y*y + z*z ; }
	float   Length(void) const;

	float	DotProduct( const xVec3D *in ) const { return (x*in->x + y*in->y + z*in->z); }
	
	void    CrossProduct( const xVec3D *in , xVec3D *out ) const;
    void    Normalize( void );
	int		IsNormal(void){ return XFLOAT_EQUAL(LengthSquare(),1.0f); }
	bool    IsValid(void) const;
	void	Inverse(void){ x=-x , y=-y, z=-z; }
    void    Zero(void){ x = y = z = 0; }
};













} // namespace vEngine {
