#include "math.h"

namespace Cool3D
{
	void MatrixPerspectiveFovLH_INF(Matrix4 *pOut,float fov,float aspect, float znear)
	{
		memset(pOut,0,sizeof(Matrix4));
		float h=1.0f/tanf(fov*0.5f);
		float w=h*aspect;

		pOut->_11=w;
		pOut->_22=h;
		pOut->_33=1.0f;
		pOut->_34=1.0f;
		pOut->_43=-znear;
	}
}//namespace Cool3D