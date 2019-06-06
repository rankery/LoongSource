#include "stdafx.h"
#include "HeadRotater.h"
#include "Player.h"
#include "NPC.h"

const float g_headRotateSpeed=FLOAT_PI;//每秒旋转多少弧度

HeadRotater::HeadRotater(Role* pRole)
: m_pRole(pRole)
, m_pAniCtrl(0)
, m_srcYaw(0.0f)
, m_dstYaw(0.0f)
, m_srcPitch(0.0f)
, m_dstPitch(0.0f)
, m_yawStartTime(0.0f)
, m_yawTotalTime(0.0f)
, m_bYawStarted(false)
, m_bPitchStarted(false)
, m_pitchStartTime(0.0f)
, m_pitchTotalTime(0.0f)
, m_bInit(false)
{
}

HeadRotater::~HeadRotater(void)
{
	m_pAniCtrl = NULL;
}

void HeadRotater::GetAniCtrl( void )
{
	if( m_bInit || !P_VALID(m_pRole) )
		return;

	if( m_pRole->IS_KIND_OF(Player) )
	{
		Player *p = static_cast<Player*>(m_pRole);
		m_pAniCtrl = p->GetAniCtrl();
	}
	else if( m_pRole->IS_KIND_OF(NPC) )
	{
		NPC *p = static_cast<NPC*>(m_pRole);
		AnimationCtrl *pAniCtrl = p->GetAniCtrl();
		if( P_VALID(pAniCtrl) || pAniCtrl->IS_STATIC_CLASS(NodeAnimationCtrl) )
		{
			m_pAniCtrl = static_cast<NodeAnimationCtrl*>(pAniCtrl);
		}
	}

	m_bInit = true;
}

void HeadRotater::Start( float destYaw, float destPitch )
{
	GetAniCtrl();
	if( !P_VALID(m_pAniCtrl) )
		return;

	m_srcYaw = m_pAniCtrl->GetHeadYaw();
	m_dstYaw = destYaw;
	m_srcPitch = m_pAniCtrl->GetHeadPitch();
	m_dstPitch = destPitch;

	if( fabsf(m_srcYaw-m_dstYaw) > FLOAT_PI )
	{
		if( m_dstYaw < m_srcYaw )
			m_dstYaw += FLOAT_PI*2.0f;
		else
			m_srcYaw += FLOAT_PI*2.0f;
	}
	if( fabsf(m_dstYaw-m_srcYaw) < FLOAT_EPSILON )
		m_bYawStarted = false;
	else
		m_bYawStarted = true;

	if( fabsf(m_srcPitch-m_dstPitch) > FLOAT_PI )
	{
		if( m_dstPitch < m_srcPitch )
			m_dstPitch += FLOAT_PI*2.0f;
		else
			m_srcPitch += FLOAT_PI*2.0f;
	}
	if( fabsf(m_dstPitch-m_srcPitch) < FLOAT_EPSILON )
		m_bPitchStarted = false;
	else
		m_bPitchStarted = true;

	// Yaw
	if( m_bYawStarted )
	{
		m_yawStartTime = Kernel::Inst()->GetAccumTime();
		m_yawTotalTime = fabsf(m_srcYaw-m_dstYaw) / g_headRotateSpeed;
	}

	// Pitch
	if( m_bPitchStarted )
	{
		m_pitchStartTime = Kernel::Inst()->GetAccumTime();
		m_pitchTotalTime = fabsf(m_srcPitch-m_dstPitch) / g_headRotateSpeed;
	}
}

void HeadRotater::Update()
{
	if( m_bYawStarted )
	{
		float runTime = Kernel::Inst()->GetAccumTime() - m_yawStartTime;
		float r = runTime/m_yawTotalTime;
		if( r > 1.0f )
			r = 1.0f;

		float nowYaw = LinearLerp(m_srcYaw,m_dstYaw,r);
		m_pAniCtrl->RotateHeadYaw(nowYaw);

		if( r >= 1.0f )
			m_bYawStarted = false;
	}
	
	if( m_bPitchStarted )
	{
		float runTime = Kernel::Inst()->GetAccumTime() - m_pitchStartTime;
		float r = runTime/m_pitchTotalTime;
		if( r > 1.0f )
			r = 1.0f;

		float nowPitch = LinearLerp(m_srcPitch,m_dstPitch,r);
		m_pAniCtrl->RotateHeadPitch(nowPitch);

		if( r >= 1.0f )
			m_bPitchStarted = false;
	}
}

bool HeadRotater::IsRotating( void ) const
{
	return ( m_bYawStarted || m_bPitchStarted );
}