#include "stdafx.h"
#include ".\frustum.h"
#include "math.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{
	Frustum::Frustum(void)
	{
	}

	Frustum::~Frustum(void)
	{
	}

	void Frustum::Extract(Matrix4 *pView,Matrix4 *pPrj)
	{
		Matrix4 comboMatrix=(*pView)*(*pPrj);
		// Left clipping plane
		m_Planes[0].normal.x = comboMatrix._14 + comboMatrix._11;
		m_Planes[0].normal.y = comboMatrix._24 + comboMatrix._21;
		m_Planes[0].normal.z = comboMatrix._34 + comboMatrix._31;
		m_Planes[0].dist = comboMatrix._44 + comboMatrix._41;
		// Right clipping plane
		m_Planes[1].normal.x = comboMatrix._14 - comboMatrix._11;
		m_Planes[1].normal.y = comboMatrix._24 - comboMatrix._21;
		m_Planes[1].normal.z = comboMatrix._34 - comboMatrix._31;
		m_Planes[1].dist = comboMatrix._44 - comboMatrix._41;
		// Top clipping plane
		m_Planes[2].normal.x = comboMatrix._14 - comboMatrix._12;
		m_Planes[2].normal.y = comboMatrix._24 - comboMatrix._22;
		m_Planes[2].normal.z = comboMatrix._34 - comboMatrix._32;
		m_Planes[2].dist = comboMatrix._44 - comboMatrix._42;
		// Bottom clipping plane
		m_Planes[3].normal.x = comboMatrix._14 + comboMatrix._12;
		m_Planes[3].normal.y = comboMatrix._24 + comboMatrix._22;
		m_Planes[3].normal.z = comboMatrix._34 + comboMatrix._32;
		m_Planes[3].dist = comboMatrix._44 + comboMatrix._42;
		// Near clipping plane
		m_Planes[4].normal.x = comboMatrix._13;
		m_Planes[4].normal.y = comboMatrix._23;
		m_Planes[4].normal.z = comboMatrix._33;
		m_Planes[4].dist = comboMatrix._43;
		// Far clipping plane
		m_Planes[5].normal.x = comboMatrix._14 - comboMatrix._13;
		m_Planes[5].normal.y = comboMatrix._24 - comboMatrix._23;
		m_Planes[5].normal.z = comboMatrix._34 - comboMatrix._33;
		m_Planes[5].dist = comboMatrix._44 - comboMatrix._43;

		for(int i=0;i<6;i++)
			m_Planes[i].Normalize();
	}

	bool Frustum::BoxInFrustum(const OBBox& box) const
	{
		ASSERT(0);
		return true;
	}

	bool Frustum::SphereInFrustum(const Sphere& sphere) const
	{
		// For each plane, see if sphere is on negative side
        // If so, object is not visible
        for (int plane = 0; plane < 6; ++plane)
        {
            // If the distance from sphere center to plane is negative, and 'more negative' 
            // than the radius of the sphere, sphere is outside frustum
            if (m_Planes[plane].DistToPoint(sphere.centre) < -sphere.radius)
            {
                // ALL corners on negative side therefore out of view
                return false;
            }

        }

        return true;
	}

	bool Frustum::BoxInFrustum(const AABBox& box) const
	{
		Vector3 pt[8];
		box.GetCorners(pt);

		// Go through all of the corners of the box and check then again each plane
		// in the frustum.  If all of them are behind one of the planes, then it most
		// like is not in the frustum.
		for(int i=0;i<6;i++)
		{
			if(m_Planes[i].ClassifyPoint(pt[0])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[1])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[2])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[3])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[4])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[5])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[6])==POSITIVE)
				continue;
			if(m_Planes[i].ClassifyPoint(pt[7])==POSITIVE)
				continue;

			// If we get here, it isn't in the frustum
			return false;
		}

		// Return a true for the box being inside of the frustum
		return true;
	}

	bool Frustum::BoxAllInFrustum(const AABBox& box) const
	{
		Vector3 pt[8];
		box.GetCorners(pt);

		for(int i=0;i<6;i++)
		{
			if(m_Planes[i].ClassifyPoint(pt[0])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[1])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[2])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[3])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[4])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[5])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[6])!=POSITIVE)
				return false;
			if(m_Planes[i].ClassifyPoint(pt[7])!=POSITIVE)
				return false;
		}

		return true;
	}
}
