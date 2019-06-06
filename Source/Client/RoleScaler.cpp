#include "StdAfx.h"
#include "RoleScaler.h"
#include "Role.h"
#include "LocalPlayer.h"
#include "WorldFrame.h"
#include "ClientCamera.h"

const float g_scalerSpeed=1.0f;//每秒放大多少倍

RoleScaler::RoleScaler() :
m_pRole( NULL ),
m_srcScale( 1.0f, 1.0f, 1.0f ),
m_dstScale( 1.0f, 1.0f, 1.0f ),
m_startTime( 0.0f ),
m_totalTime( 0.0f ),
m_bStarted( false )
{
}

RoleScaler::~RoleScaler(void)
{
}

void RoleScaler::Start( const Vector3& destScale )
{
	m_srcScale=m_pRole->GetObjectSpace().GetScale();
	m_dstScale=destScale;

	Vector3 scaleDelta = m_dstScale-m_srcScale;
	float maxScale = fabsf( max( max( scaleDelta.x, scaleDelta.y ), scaleDelta.z ) );
	if( maxScale < FLOAT_EPSILON )
	{
		m_bStarted=false;
		return;
	}

	m_startTime=Kernel::Inst()->GetAccumTime();
	
	m_totalTime=maxScale/g_scalerSpeed;
	m_bStarted=true;
}

void RoleScaler::Update()
{
	if(!m_bStarted)
		return;

	float runTime=Kernel::Inst()->GetAccumTime()-m_startTime;
	float r=runTime/m_totalTime;
	if(r>1)r=1;

	Vector3 curScale( 1.0f, 1.0f, 1.0f );
	D3DXVec3Lerp( &curScale, &m_srcScale, &m_dstScale, r );
	m_pRole->SetScale( curScale, false );
	
    // 如果是LocalPlayer在缩放，一并更改摄像机高度
    if (m_pRole->IS_KIND_OF(LocalPlayer))
    {
        WorldFrame* pWfm = GetWorldFrm();
        if (P_VALID(pWfm))
        {
            ClientCamera* pCamera = pWfm->GetCamera();
            if (P_VALID(pCamera))
            {
                pCamera->SetHeightRate(curScale.y);
            }
        }
    }

	if(r>=1)
		m_bStarted=false;
}

bool RoleScaler::IsScaling() const
{
	return m_bStarted;
}

const Vector3& RoleScaler::GetDestScale() const
{
	return m_dstScale;
}