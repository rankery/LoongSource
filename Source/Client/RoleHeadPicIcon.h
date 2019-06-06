#pragma once

#include "ActiveStatic.h"

class Role;
class AvatarEquipNode;

/** \class		RoleHeadPicIcon
    \brief		显示一个角色头像
	\remarks	玩家动态拍照，NPC从磁盘加载(由Cool3DView手动生成)，背景色为m_dwPicColor
*/
class RoleHeadPicIcon : public ActiveStatic
{
public:
	RoleHeadPicIcon();
	virtual ~RoleHeadPicIcon();

	// ActiveStatic
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual VOID SetInvisible(BOOL b);

	/** \更新头像,注意，不能在控件的Render()中调用此函数
	*/
	VOID UpdataHeadPic( Role* pRole );
	VOID UpdataAvatarHeadPic( const tagAvatarAtt& att, const tagAvatarEquip& ep, const tagDisplaySet* const pDisplaySet );
	VOID UpdataAvatarHeadPic( AvatarEquipNode* pAvatarNode );
	VOID UpdataDisplaySet( const tagDisplaySet& ds );
	/** \需要外部调用
	*/
	VOID Render3D();
	/** \设备丢失处理
	*/
	VOID OnDeviceResetEvent(tagGameEvent* );
private:
	BOOL UpdataCamera();
	VOID FreeDynamicData();							// 释放所有动态创建的资源
	VOID NewPlayerData();							// 创建玩家头像资源
	VOID NewNPCData();								// 创建NPC头像资源

	tstring FileNameMdlToHeadPic( tstring strMdlFileName, BOOL bFSKEL = FALSE );
private:
	tagGUIImage*            m_pHeadImg;				 // 头像图片(玩家:RenderTarget,Npc:Restexture)
	NullSceneGraph*			m_pSG;					 // 用于保存摄像机、光照等信息,不用于渲染
	CameraBase*				m_pCamera;				 // 用于给玩家拍照
	AvatarEquipNode*		m_pAvatarNode;			 // 用于给玩家拍照
	tstring                 m_strNpcHeadPicFileName; // NPC头像的文件名
	BOOL                    m_bRefershAvatar;		 // 是否需要刷新玩家头像(角色变了,或是玩家换装)
	BOOL                    m_bRefershNPC;			 // 是否需要刷新NPC头像(角色变了)
	Vector3                 m_vCamDir;				 // 给玩家拍照的照相机方向
	Vector3                 m_vCamLooatOffset;		 // 给玩家拍照的视点偏移系数(相对头的Box)
	float                   m_fCamDist;				 // 给玩家拍照的视距系数(相对头的Box对角长度)
	float                   m_fRenderTargetZoom;	 // 给玩家拍照时使用RenderTarget相对控件大小的放大系数(用于减少锯齿)
	DWORD                   m_dwBackColor;           // 背景颜色

	TSFPTrunk<RoleHeadPicIcon> m_Trunk;
};
