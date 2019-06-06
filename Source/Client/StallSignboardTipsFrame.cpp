#include "StdAfx.h"
#include "StallSignboardTipsFrame.h"
#include "StallMgr.h"
#include "Role.h"
#include "Player.h"
#include "RoleMgr.h"
#include "MapMgr.h"

class StallBoardSortFunc
{
public:
	bool operator()(const tagStall *p1, const tagStall *p2)
	{
		if (!P_VALID(p1) || !P_VALID(p2))
			return false;

		Role* pRole1 = RoleMgr::Inst()->FindRole(p1->dwRoleID);
		Role* pRole2 = RoleMgr::Inst()->FindRole(p2->dwRoleID);
		
		if (!P_VALID(pRole1) || !P_VALID(pRole2))
			return false;

		CameraBase* pCam = MapMgr::Inst()->GetSceneGraph()->GetCamera();
		if (!P_VALID(pCam))
			return false;

		Vector3 vTipPos1;
		pRole1->GetHeadPos(vTipPos1);		
		Vector3 vScreenPos1 = pCam->WorldToScreenF( vTipPos1 );
		float z1 = vScreenPos1.z * 0.5f;

		Vector3 vTipPos2;
		pRole2->GetHeadPos(vTipPos2);		
		Vector3 vScreenPos2 = pCam->WorldToScreenF( vTipPos2);
		float z2 = vScreenPos2.z * 0.5f;
		return z1 > z2;
	};
};






StallSignboardTipsFrame::StallSignboardTipsFrame(void):
m_pFont(NULL)
{
}

StallSignboardTipsFrame::~StallSignboardTipsFrame(void)
{
}

BOOL StallSignboardTipsFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr,pFather,dwParam) )
		return FALSE;
	
	// 创建招牌字体
	m_pFont = Device()->NewDepthFont();
	m_pFont->Create( 7, 20, 0, FALSE, g_StrTable[_T("Font_MSYH")]);
	
	// 创建招牌图片
	//IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	//tstring strFlag;
	//strFlag = _T("data\\model\\BT\\ZP\\");
	//strFlag += _T("ZP1\\zhaopai2");
	//strFlag += _T(".dds");
	//m_pTex = IDraw2D::Inst()->NewTexture(strFlag.c_str(),pIFS);

	return TRUE;
}

BOOL StallSignboardTipsFrame::Destroy()
{
	SAFE_DELETE(m_pFont);

	std::map<BYTE, ResTexture*>::iterator end = m_mapTexs.end();

	for (std::map<BYTE, ResTexture*>::iterator iter = m_mapTexs.begin(); iter != end; ++iter)
	{
		IDraw2D::Inst()->ReleaseTexture(iter->second);
	}
	m_mapTexs.clear();

	return GameFrame::Destroy();
}




VOID StallSignboardTipsFrame::Render3D()
{
	m_vecSortStall.clear();
	TMap<DWORD,tagStall*>& stallMap = StallMgr::Inst()->GetStallMap();
	stallMap.ResetIterator();
	tagStall* pStall = NULL;
	while(stallMap.PeekNext(pStall))
	{
		m_vecSortStall.push_back(pStall);
	}
	// 排序
	std::sort(m_vecSortStall.begin(), m_vecSortStall.end(), StallBoardSortFunc());
	
	std::vector<tagStall*>::iterator end = m_vecSortStall.end();
	for (std::vector<tagStall*>::iterator iter = m_vecSortStall.begin(); iter != end; ++iter)
	{
		DrawItemTip(*iter);
	}

}

VOID StallSignboardTipsFrame::DrawItemTip( const tagStall* pItem )
{
	if( !P_VALID(pItem) || !P_VALID(pItem->pNode) )
		return;
	// 找到玩家，如果玩家不在视野内，则隐藏
	Role* pRole = RoleMgr::Inst()->FindRole(pItem->dwRoleID);
	if(!P_VALID(pRole))
		return;
	
	pItem->pNode->Hide(!RoleMgr::Inst()->IsInViewDist(pRole));
	
	if(pItem->pNode->IsHide())
		return;

	DWORD dwTextColor = 0xFF290404;
	
	if( 0 == _tcsclen(pItem->szName) )
			return;
	/*switch( pItem->byLevel )
	{
		case EIQ_White:		dwTextColor = 0xFFFFFFFF; break;
		case EIQ_Yellow:	dwTextColor = 0xFFFFFF00; break;
		case EIQ_Green:		dwTextColor = 0xFF00FF00; break;
		case EIQ_Blue:		dwTextColor = 0xFF0000FF; break;
		case EIQ_Orange:	dwTextColor = 0xFFFF8000; break;
	}
	*/
	/*AABBox box;
	pItem->pNode->GetBox( box );
	Vector3 vTipPos = pItem->space.GetPos();
	vTipPos.y += box.max.y - box.min.y;*/
	Vector3 vTipPos;
	pRole->GetHeadPos(vTipPos);
	

	//计算招牌屏幕位置、z值和缩放值
	CameraBase* pCam = MapMgr::Inst()->GetSceneGraph()->GetCamera();
	Vector3 vScreenPos = pCam->WorldToScreenF( vTipPos );
	POINT ptPos = pCam->ScreenFToScreen( vScreenPos );
	float z = vScreenPos.z * 0.5f;
	if(z<0.0f || z>0.5f)			//视锥剪裁
		return;
	const Vector3& lookFrom = pCam->GetPos();
	float scalePerUnit = 2.0e-4f;//每个世界单位产生的缩放比例
	float dist = D3DXVec3Length(&(lookFrom-vTipPos));
	float scale = 1.0f;
	if( dist > 2000.0f )
	{
		scale = 1.0f - (dist-2000.0f)*scalePerUnit;
		if( scale < 0.1f )
			scale = 0.1f;
	}
	//修改高度
	ptPos.y -= (LONG)(84.0f*scale);
	if(IS_PLAYER(pRole->GetID()))
	{
		Player* pPlayer = (Player*)pRole;
		if(pPlayer->IsInGuild())
			ptPos.y -= (LONG)(18.0f*scale);
		
		if(pPlayer->GetTitleName() != _T(""))
			ptPos.y -= (LONG)(18.0f*scale);
	}
	//绘制招牌
	tstring strFlag = StallMgr::Inst()->GetStallSignBoardPath(pItem->byLevel);
	ResTexture *pTex = NULL;
	std::map<BYTE, ResTexture*>::iterator iter = m_mapTexs.find(pItem->byLevel);
	if (iter != m_mapTexs.end())
	{
		pTex = iter->second;
	}
	else
	{
		IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
		pTex = IDraw2D::Inst()->NewTexture(strFlag.c_str(), pIFS);
		m_mapTexs.insert(std::make_pair(pItem->byLevel, pTex));
	}

	// 特殊处理
	if (pItem->byLevel == 10)
		ptPos.y -= (LONG)(18.0f * scale);
	if (pItem->byLevel == 7 || pItem->byLevel == 8 )
		ptPos.y -= (LONG)(16.0F * scale);

	IDraw2D::Inst()->BeginDraw(IDraw2D::EBlend_Alpha, true);
	DrawSignboard(pTex, ptPos, z, scale );
	IDraw2D::Inst()->EndDraw();		
	//绘制字
	if (pItem->byLevel == 10)
		ptPos.y += (LONG)(30.0f * scale);
	if (pItem->byLevel == 7 || pItem->byLevel == 8 )
		ptPos.y -= (LONG)(18.0f * scale);

	m_pFont->Begin();
	

	DrawTip( ptPos, pItem->szName, dwTextColor, z, scale );

	m_pFont->End();
}

VOID StallSignboardTipsFrame::DrawTip( const POINT &ptPos, const TCHAR* szText, const DWORD dwColor, const float z, const float scale )
{
	SIZE sizeText;
	m_pFont->MeasureText( szText, sizeText );

	RECT rcText;
	rcText.left = ptPos.x - sizeText.cx / 2;
	rcText.top = ptPos.y - sizeText.cy / 2;
	rcText.right = rcText.left + sizeText.cx;
	rcText.bottom = rcText.top + sizeText.cy;

	// 增加字体阴影
	/*RECT rcShade;
	rcShade = rcText;
	rcShade.left-=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.right+=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.top-=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);
	rcShade = rcText;
	rcShade.bottom+=2;
	m_pFont->DrawText(szText, (LPRECT)&rcShade, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, 0xFF290404, z);*/
	// 显示摊位名称
	m_pFont->DrawText( szText, &rcText, DT_TOP|DT_CENTER|DT_SINGLELINE|DT_NOCLIP, dwColor, z, scale );

}

VOID StallSignboardTipsFrame::DrawSignboard(ResTexture *pTex,  const POINT &ptPos, const float z, const float scale )
{
	/*
	if(!P_VALID(pTex))
		return;

	RECT destRC,srcRC;
	srcRC.top=srcRC.left=0;
	srcRC.bottom=srcRC.top+128;
	srcRC.right=srcRC.left+256;

	destRC.left=ptPos.x - (LONG)(196*scale) / 2;
	destRC.right=destRC.left + (LONG)(196*scale);
	destRC.top=ptPos.y - (LONG)(30*scale) / 2;
	destRC.bottom=destRC.top + (LONG)(32*scale);
	
	IDraw2D::Inst()->Draw(&destRC, &srcRC, pTex, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, z);
	*/
	
	if (!P_VALID(pTex))
		return;
	IDeviceTex *pRealTex = pTex->GetDeviceTex();
	if (!P_VALID(pRealTex))
		return;

	RECT destRC,srcRC;
	srcRC.top=srcRC.left=0;
	srcRC.bottom=srcRC.top + pRealTex->Height();
	srcRC.right=srcRC.left + pRealTex->Width();

	destRC.left=ptPos.x - (LONG)(pRealTex->Width() * scale) / 2;
	destRC.right=destRC.left + (LONG)(pRealTex->Width() * scale);
	destRC.top=ptPos.y - (LONG)(pRealTex->Height() * scale) / 2;
	destRC.bottom=destRC.top + (LONG)(pRealTex->Height() * scale);

	IDraw2D::Inst()->Draw(&destRC, &srcRC, pTex, 0xFFFFFFFF, ETFilter_POINT, 0, IDraw2D::EAWT_Write, z);

}

void StallSignboardTipsFrame::ReleaseTexture( const tagStall* pItem )
{
	std::map<BYTE, ResTexture*>::iterator iter = m_mapTexs.find(pItem->byLevel);
	if (iter != m_mapTexs.end())
	{
		if (P_VALID(iter->second))
			IDraw2D::Inst()->ReleaseTexture(iter->second);
		m_mapTexs.erase(iter);
	}
}