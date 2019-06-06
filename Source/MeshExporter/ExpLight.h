#include "Max.h"
#include "MeshExportBaseDef.h"
#pragma once

namespace Cool3D
{
	//--Light-----------------------------------
	enum LIGHT_TYPE
	{
		LGT_OMNI,
		LGT_SPOT,
		LGT_DIRECT
	};
	enum DECAY_TYPE
	{
		DK_DECAY_NONE,
		DK_DECAY_DIST,
		DK_DECAY_SQUARE
	};
	enum LIGHT_SHAPE
	{
		SHAPE_CONE,
		SHAPE_SQUARE
	};
	
	struct LightDesc
	{
		LIGHT_TYPE m_LightType;
		bool  m_bShadow;  //whether to project shadow
		Vector3 m_Diffuse;
		Vector3 m_Specular;
		Vector3 m_Ambient;
		float m_NearStart;
		float m_NearEnd;
		float m_FarStart;
		float m_FarEnd;
		
		DECAY_TYPE  m_DecayType;
		float m_DecayRadius;
		
		LIGHT_SHAPE m_LightShape;
		float m_HotSize;	//in degrees with spot light,  in units with direct light
		float m_Falloff;	//in degrees,  in units with direct light
		float m_Aspect;		//only useful when lightshape is square
		
		Vector3	m_Position;//for Omni and spot
		Vector3 m_Direction;//for Direct and spot
	};
	
	/**	\class ExpLight
		\brief 灯光节点数据
	*/
	class ExpLight  
	{
	public:
		LightDesc* GetLightDesc();
		ExpLight(INode *pNode,TimeValue time);
		~ExpLight();

	private:
		void Max2LightDesc(const LightState& ls ,GenLight* pMaxLight);
		Cool3D::LightDesc m_desc;
	};
}