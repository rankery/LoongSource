#include "stdafx.h"
#include "UpperRotater.h"
#include "Player.h"
#include "NPC.h"

const float g_upperRotateSpeed=FLOAT_PI;//每秒旋转多少弧度

UpperRotater::UpperRotater(Role* pRole)
: m_pRole(pRole)
, m_pAniCtrl(0)
, m_srcYaw(0.0f)
, m_dstYaw(0.0f)
, m_startTime(0.0f)
, m_totalTime(0.0f)
, m_bStarted(false)
, m_bInit(false)
{
}

UpperRotater::~UpperRotater(void)
{
}

void UpperRotater::GetAniCtrl(void)
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
		if( P_VALID(pAniCtrl) && pAniCtrl->IS_STATIC_CLASS(NodeAnimationCtrl) )
		{
			m_pAniCtrl = static_cast<NodeAnimationCtrl*>(pAniCtrl);
		}
	}

	m_bInit = true;
}

void UpperRotater::Start( float destYaw )
{
	GetAniCtrl();
	if( !P_VALID(m_pAniCtrl) )
		return;

	m_srcYaw = m_pAniCtrl->GetUpperYaw();
	m_dstYaw = destYaw;

	if( fabsf(m_srcYaw-m_dstYaw) > FLOAT_PI )
	{
		if( m_dstYaw < m_srcYaw )
			m_dstYaw += FLOAT_PI*2.0f;
		else
			m_srcYaw += FLOAT_PI*2.0f;
	}
	if( fabsf(m_dstYaw-m_srcYaw) < FLOAT_EPSILON )
	{
		m_bStarted = false;
		return;
	}

	m_startTime = Kernel::Inst()->GetAccumTime();
	m_totalTime = fabsf(m_srcYaw-m_dstYaw) / g_upperRotateSpeed;
	m_bStarted = true;
}

void UpperRotater::Update()
{
	if( !m_bStarted )
		return;

	float runTime = Kernel::Inst()->GetAccumTime() - m_startTime;
	float r = runTime/m_totalTime;
	if( r > 1.0f )
		r = 1.0f;

	float nowYaw = LinearLerp(m_srcYaw,m_dstYaw,r);
	m_pAniCtrl->RotateUpperYaw(nowYaw);

	if( r >= 1.0f )
		m_bStarted = false;
}

bool UpperRotater::IsRotating()
{
	return m_bStarted;
}

float UpperRotater::GetDestYaw()
{
	return m_dstYaw;
}