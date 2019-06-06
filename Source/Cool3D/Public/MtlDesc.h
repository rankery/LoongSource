#pragma once

#include "MathBaseDefine.h"
namespace Cool3D
{
	enum EMapChannel
	{
		EMap_Diffuse=0,
		EMap_Lighting,
		EMap_Specular,
		EMap_Bump,
		EMap_Sphere,
		EMap_Max
	};
	struct tagMtlDesc
	{
		int		m_id;
		Vector4 m_diffuse;
		Vector4 m_ambient;
		Vector4 m_specular;
		Vector4 m_emissive;
		float	m_power;

		bool	m_bTwoSide;
		bool	m_bOpcityMap;
		BYTE	m_opcityVal;

		TCHAR	m_map[EMap_Max][256];

		bool	m_bMultiPass;
		BYTE	reserve[31];
	};
	
}//endof namespace Cool3D