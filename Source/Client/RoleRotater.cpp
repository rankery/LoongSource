#include "StdAfx.h"
#include "RoleRotater.h"
#include "Role.h"

RoleRotater::RoleRotater() :
m_bSetTime(false),
m_param(1.0f),
m_type(ET_None),
m_pRole(NULL),
m_bStarted(false)
{
}

RoleRotater::~RoleRotater(void)
{
}

void RoleRotater::BindRole(Role* pRole,EType type,bool setTime,float param)
{ 
	m_pRole=pRole; 
	m_type=type; 
	m_bSetTime=setTime;
	ASSERT(param>0);
	m_param=param;
}

void RoleRotater::Start( float destAngle )
{
	m_bStarted=false;

	switch(m_type)
	{
		case ET_Yaw:		m_srcAngle=m_pRole->GetYaw();	break;
		case ET_Pitch:		m_srcAngle=m_pRole->GetPitch();	break;
		default: return;
	}
	m_dstAngle=destAngle;

	YawNormalize(m_srcAngle);
	YawNormalize(m_dstAngle);
	if(fabsf(m_srcAngle-m_dstAngle)>FLOAT_PI)
	{
		if(m_dstAngle<m_srcAngle)
			m_dstAngle+=FLOAT_PI*2.0f;
		else
			m_srcAngle+=FLOAT_PI*2.0f;
	}
	if(fabsf(m_srcAngle-m_dstAngle)<FLOAT_EPSILON)
		return;

	if( m_bSetTime )
		m_totalTime=m_param;
	else
		m_totalTime=fabsf(m_srcAngle-m_dstAngle)/m_param;

	m_startTime=Kernel::Inst()->GetAccumTime();
	m_bStarted=true;
}

void RoleRotater::Update()
{
	if(!m_bStarted)
		return;

	float runTime=Kernel::Inst()->GetAccumTime()-m_startTime;
	float r=runTime/m_totalTime;
	if(r>1)r=1;

	float nowAngle=LinearLerp(m_srcAngle,m_dstAngle,r);
	YawNormalize(nowAngle);

	switch(m_type)
	{
		case ET_Yaw:		m_pRole->SetYaw(nowAngle,false);	break;
		case ET_Pitch:		m_pRole->SetPitch(nowAngle,false);	break;
	}
	
	if(r>=1)
		m_bStarted=false;
}

bool RoleRotater::IsRotating()
{
	return m_bStarted;
}

float RoleRotater::GetDestAngle()
{
	return m_dstAngle;
}