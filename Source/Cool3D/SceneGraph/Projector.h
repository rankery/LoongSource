#pragma once

namespace F3D
{
	class Projector
	{
	public:
		Projector(void);
		virtual ~Projector(void);

	protected:
		bool	m_bProject2Terrain;
		bool	m_bProject2StaticMesh;
	};
}//namespace F3D