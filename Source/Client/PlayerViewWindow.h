#pragma once

#include "..\WorldDefine\ItemDefine.h"

class Player;
class AvatarEquipNode;

/** \class   PlayerViewWindow
    \brief   显示一个角色avatar，并可实时换装
    \remarks 如果有2个名字为roletl和roletr的Button子控件,则内部会给它们添加旋转角色功能
*/
class PlayerViewWindow : public GUIStatic
{
public:
	PlayerViewWindow();
	virtual ~PlayerViewWindow();

	/** \GUIStatic
	*/
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();
	virtual VOID RefreshViewCoord();
	virtual VOID AddChild(GUIWnd* pChild);
	virtual VOID SetInvisible(BOOL b);

	/** \渲染3D角色，需要外部在GameFrame的Render3D中调用
	*/
	VOID Render3D();
	/** \旋转角色
	*/
	VOID RotateYaw( const FLOAT fAngle );
	/** \使用pPlayer的角色可视属性和装备信息更新avtatar
	*/
	VOID UpdataAvatar( Player* pPlayer );
	/** \换单个装备
	*/
	void SetEquip( const EAvatarElement eAvaEnt, const tagEquipDisplay& ed );
	/** \更新显示设置
	*/
	VOID UpdataDisplaySet( const tagDisplaySet& ds );
protected:
	VOID UpdataCamera();
protected:
	tagGUIImage*			m_pNewCanvas;     // RenderTarget
	AvatarEquipNode*		m_pAvatarNode;    // SceneNode
	NullSceneGraph*			m_pSG;            // 用于保存摄像机、光照等信息,不用于渲染
	CameraBase				m_Camera;
	FLOAT                   m_fAvatarYaw;

	GUIButton*              m_pBnRotaLeft;
	GUIButton*              m_pBnRotaRight;
};
