#include "StdAfx.h"
#include "shadowpool.h"
#include "SGDynamicProjector.h"
#include "RoleMgr.h"
#include "Role.h"
#include "NPC.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "MapMgr.h"

int ShadowPool::OP_MAX_SHADOW = 10;
int ShadowPool::OP_SHADOW_SIZE = 128;
float ShadowPool::OP_MAX_DIST = 64.0f * TILE_SCALE;

ShadowPool::ShadowPool(void)
{
}

ShadowPool::~ShadowPool(void)
{
	Destroy();
}

void ShadowPool::CreateData()
{
	// 已经完全分配了所有资源
	if(m_shadows.size()==OP_MAX_SHADOW)
		return;

	// 分配剩余的(处理第一次或者第二此进入游戏的情况)
	for(int i=m_shadows.size();i<OP_MAX_SHADOW;i++)
	{
		SGDynamicProjector *pShadow=new SGDynamicProjector;
		pShadow->CreateData(OP_SHADOW_SIZE,OP_SHADOW_SIZE);
		m_shadows.push_back(pShadow);
	}
}

void ShadowPool::Destroy()
{
	for(vector<SGDynamicProjector*>::iterator iter=m_shadows.begin();
		iter!=m_shadows.end();iter++)
	{
		SGDynamicProjector *pShadow=(*iter);
		delete pShadow;
	}
	m_shadows.clear();
}


class CmpRoles
{
public:
	Vector3 refPos;
	bool operator()( Role *pRole1,Role *pRole2 )
	{
		const float dist1 = Vec3DistSq( refPos, pRole1->GetPos() );
		const float dist2 = Vec3DistSq( refPos, pRole2->GetPos() );
		return dist1 < dist2;
	}
};

void ShadowPool::AssignToRoles()
{
	if( 0 == m_shadows.size() )
		return;

	// 得到视锥
	CameraBase* pCam = MapMgr::Inst()->GetSceneGraph()->GetCamera();
	if( NULL == pCam )
		return;
	Frustum frustum;
	frustum.Extract( pCam->GetView(), pCam->GetPrj() );

	Role* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID( pLocalPlayer ) || !P_VALID( pLocalPlayer->GetSceneNode() ) )
		return;

	// 把可以绑定阴影的角色放入列表
	list<Role*> listRoles;
	pLocalPlayer->DetachShadow();
	if( CanAttach( pLocalPlayer, frustum ) )
		listRoles.push_back( pLocalPlayer );

	const map<DWORD,Role*>& mapRoles = RoleMgr::Inst()->GetRoleMap();
	for( map<DWORD,Role*>::const_iterator cItr = mapRoles.begin(); cItr != mapRoles.end(); cItr++ )
	{
		Role* pRole = cItr->second;
		if( !P_VALID( pRole ) )
			continue;

		// 取消绑定
		pRole->DetachShadow();

		if( !CanAttach( pRole, frustum ) )
			continue;

		listRoles.push_back( cItr->second );
	}

	// 排序
	CmpRoles CmpFuc;
	CmpFuc.refPos = pLocalPlayer->GetPos();
	listRoles.sort( CmpFuc );

	// 给角色绑定阴影
	int c = 0;
	for( list<Role*>::iterator iter = listRoles.begin(); iter != listRoles.end(); iter++ )
	{
		if( c >= OP_MAX_SHADOW )
			break;
		Role *pRole=(*iter);
		pRole->AttachShadow( m_shadows[c++] );
	}
}

bool ShadowPool::CanAttach( Role* pRole, const Frustum& frustum )
{
	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	NavCollider* pCollider = pNav->GetCollider();
	if( NULL == pNav || NULL == pCollider )
		return false;

	// 角色被隐藏
	if( pRole->IsHide() )
		return false;

	// 角色半透明
	if( pRole->GetTransparent() < 1.0f )
		return false;

	// 角色在水里，不绑定阴影（会有很多问题）
	float fWaterHeight = 0.0f;
	if( pCollider->GetWaterHeight( pRole->GetPos(), fWaterHeight ) && pRole->GetPos().y <= fWaterHeight )
		return false;

	// 资源没有加载完成，不绑定阴影
	SceneNode* pSGNode = pRole->GetSceneNode();
	if( !P_VALID( pSGNode ) || !pSGNode->IsLoaded() )
		return false;

	// 玩家有骑乘，不绑定阴影
	if( pRole->IS_KIND_OF(Player) )
	{
		Player* pPlayer = (Player*)pRole;
		if( pPlayer->GetMount() != NULL )
			return false;
	}

	// 视锥之外
	if( !pSGNode->IsInFrustum( &frustum ) )
		return false;

	// 超过了最大距离
	const float fDistToLPSq = Vec3DistSq( RoleMgr::Inst()->GetLocalPlayer()->GetPos(), pRole->GetPos() );
	if( fDistToLPSq > OP_MAX_DIST * OP_MAX_DIST )
		return false;

	return true;
}







