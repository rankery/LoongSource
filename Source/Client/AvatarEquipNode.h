#pragma once

#include "..\Cool3D\SceneGraph\AvatarNode.h"
#include "..\WorldDefine\AvatarAtt.h"
#include "..\WorldDefine\ItemDefine.h"

class AvatarEquipElement;

/** \class AvatarEquipNode，继承AvatarNode
    \brief 提供了角色可视属性及装备的管理功能
*/
class AvatarEquipNode : public AvatarNode
{
public:
	AvatarEquipNode(void);
	virtual ~AvatarEquipNode(void);

	/** \AvatarNode
	*/
	virtual bool IsInFrustum(const Frustum *pFrumstum) const;
	virtual void Update(float deltaTime,SceneGraph *pSG);
	virtual void ForceUpdate(float deltaTime,SceneGraph *pSG);

	/** \加载默认Avatar
	*/
	void LoadDefault();
	/** \加载指定的Avatar
	*/
	void LoadAvatar( const tagAvatarAtt& att, const tagAvatarEquip& ep );
	/**	\更新全部Att，没有修改的部分则不更新
	*/
	void UpdateAllAtt( const tagAvatarAtt& att );
	/**	\更新全部Equip，没有修改的部分则不更新
	*/
	void UpdateAllEquip( const tagAvatarEquip& ep );
	/** \设定单个装备
	*/
	void SetEquip( const EAvatarElement eAvaEnt, const tagEquipDisplay& ed );
	/** \获得单个装备数据
	*/
	const tagEquipDisplay& GetEquip( const EAvatarElement eAvaEnt ) const;
	/** \获得单个装备的主模型指针
	*/
	const SceneNode* GetElementMainMdl( const EAvatarElement eAvaEnt ) const;
	/** \拿出/收回武器
	*/
	void HandUpWeapon( const bool bHandUp );
	/** \隐藏/显示武器
	*/
	void HideWeapon( const bool bHide );

	/** \隐藏/显示武器附属模型
	*/
	void HideWeaponPertain(bool bHide);


	/** \获取主手武器类型
	*/
	EItemTypeEx GetRWeaponType() const;
	/** \获取附手武器类型
	*/
	EItemTypeEx GetLWeaponType() const;
	/** \展开/关闭所有装备(播放变形动画)
	*/
	void OpenAllEquip( const bool bOpen );
	/** \所有装备是否已打开
	*/
	bool IsAllEquipOpened();
	/** \所有装备是否已关闭
	*/
	bool IsAllEquipClosed();
	/** \播放/停止刀光（主手武器）
	*/
	void PlayBladeFlare( const bool bPlay );
	
	/** \播放/停止套装特效
	*/
	void PlaySuitEffect(bool bPlay);


	/** \更新龙魂特效
	*/
	void UpdataLongHunEffect( const EAvatarElement eAvaEnt, const BYTE byEquipEffect );
	/** \更新套装特效
	*/
	void UpdataSuitEffect( const DWORD dwSuitID );
	/** \获得角色外观属性
	*/
	const tagAvatarAtt& GetAtt() const { return m_Att; }
	/** \获得套装ID
	*/
	const DWORD GetSuitID() const { return m_dwSuitID; }
	/** \所有资源是否都加载完成
	*/
	bool IsAllResourceLoaded() const;
	/** \显示设置
	*/
	const tagDisplaySet& GetDisplaySet() const { return m_displaySet; }
	void SetDisplaySet( const tagDisplaySet& ds, const bool bHead = true );
private:
	/** \检查重叠
	*/
	void CheckOverlap();
	/** \设置头是否隐藏
	*/
	void ReloadHead( const bool bHide, const bool bFashion );
private:
	tagAvatarAtt			m_Att;										// 角色创建的时候指定的外观属性数据
	AvatarEquipElement*		m_pEquipElement[X_AVATAR_ELEMENT_NUM];		// 角色装备
	AvatarEquipElement*		m_pLoadingElement[X_AVATAR_ELEMENT_NUM];	// 正在加载的装备
	DWORD					m_dwSuitID;									// 套装ID
	bool					m_bWeaponHandUp;							// 武器是否已拿出
	bool                    m_bWeaponHide;								// 武器是否隐藏
	bool					m_bWeaPonPertainHide;						// 武器附属模型是否隐藏
	bool					m_bPlaySuitEffect;							// 是否播放套装特效
	tagDisplaySet			m_displaySet;								// 外观显示设置
	tagEquipDisplay			m_edHead;									// 会隐藏，所以备份一下
	SGAttachableEffectNode* m_pSuitEffect;                              // 套装特效
	bool                    m_bNeedCull;
	DECL_RTTI(AvatarEquipNode);
};
