#pragma once

#include "PlayerViewWindow.h"
#include "AvatarEquipElement.h"

class Player;
class AvatarEquipNode;
/**	class PlayerTryDressWnd
	brief 试穿窗口
*/
class PlayerTryDressWnd : public PlayerViewWindow
{
public:
	PlayerTryDressWnd(void);
	virtual ~PlayerTryDressWnd(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();

	/** \使用pPlayer的角色avtatar和试穿的装备
	*/
	VOID UpdataAvatar( Player* pPlayer );
	/** \旋转角色
	*/
	VOID RotateYaw( const FLOAT fAngle );
	/** \清空所有本地窗口保存的试穿装备
	*/
	VOID ReLoadTryEquip();

};