#include "ExpLight.h"
#include "math\Max2D3D.h"

namespace Cool3D
{
	ExpLight::ExpLight(INode *pNode,TimeValue time)
	{
		ObjectState os=pNode->EvalWorldState(time);
		GenLight *pMaxLight=static_cast<GenLight*>(os.obj);
		LightState lgtState;

		Interval valid = FOREVER;
		pMaxLight->EvalLightState(time, valid, &lgtState);

		Max2LightDesc(lgtState, pMaxLight);

		Matrix3 tm=pNode->GetObjectTM(time);
		Point3 position(0,0,0);
		tm.TransformPoints(&position,1);
		Point3ToVector3_D3D(position, m_desc.m_Position);

		Point3 dir=tm.GetRow(2);//direction
		Point3ToVector3_D3D(dir, m_desc.m_Direction);
	}
	
	ExpLight::~ExpLight()
	{
		
	}
	
	LIGHT_TYPE MaxLightTypeConv(LightType type)
	{
		LIGHT_TYPE ret=LGT_OMNI;
		switch( type )
		{
		case OMNI_LGT:
			ret = LGT_OMNI;
			break;
		case SPOT_LGT:
			ret = LGT_SPOT;
			break;
		case DIRECT_LGT:
			ret = LGT_DIRECT;
			break;
		case AMBIENT_LGT:
			ret = LGT_DIRECT;
		}

		return ret;
	}

	void ExpLight::Max2LightDesc(const LightState& ls, GenLight* pMaxLight)
	{
		memset(&m_desc,0,sizeof(m_desc));
		m_desc.m_LightType=MaxLightTypeConv(ls.type);
		
		if(m_desc.m_LightType == AMBIENT_LGT
			|| ls.ambientOnly)
		{
			//todo 增加都某个灯光的Ambient中
		}
		else
		{
			Point3ToVector3(ls.color*ls.intens,m_desc.m_Diffuse);
			Point3ToVector3(ls.color*ls.intens,m_desc.m_Specular);
		}

		m_desc.m_bShadow=(ls.shadow!=0);
		if(!ls.affectDiffuse)
			m_desc.m_Diffuse=Vector3(0,0,0);
		if(!ls.affectSpecular)
			m_desc.m_Specular=Vector3(0,0,0);

		float max_light_range = (float)sqrt( FLT_MAX );
		m_desc.m_NearStart = ls.useNearAtten ? ls.nearAttenStart : 0;
		m_desc.m_NearEnd = ls.useNearAtten ? ls.nearAttenEnd : 0;
		m_desc.m_FarStart = ls.useAtten ? ls.attenStart : max_light_range;
		m_desc.m_FarEnd = ls.useAtten ? ls.attenEnd : max_light_range;

		m_desc.m_DecayType=DK_DECAY_DIST;
		m_desc.m_DecayRadius = pMaxLight->GetDecayRadius(0);

		if( ls.shape == RECT_LIGHT ) 
			m_desc.m_LightShape = SHAPE_SQUARE;
		else
			m_desc.m_LightShape = SHAPE_CONE;

		m_desc.m_HotSize = ls.hotsize;
		m_desc.m_Falloff = ls.fallsize;
		m_desc.m_Aspect = ls.aspect;
	}
	
	
	LightDesc* ExpLight::GetLightDesc()
	{
		return &m_desc;	
	}
}//namespace Cool3D