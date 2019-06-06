#pragma once

#include "..\WorldDefine\AvatarAtt.h"
#include "..\WorldDefine\container_define.h"
#include "..\WorldDefine\ItemDefine.h"

class AvatarEquipNode;
struct tagItemDisplayInfo;
struct tagEquipProto; 

/** \class AvatarEquipElement
    \brief 单个装备，装备中的SceneNode的生命周期由this管理
	\remarks 外部需要在删除AvatarNode之前删除它
*/
class AvatarEquipElement
{
public:
	/** \装备的展开状态
	*/
	enum EOpenState
	{
		EOS_Closed,
		EOS_Opening,
		EOS_Opened,
		EOS_Closing,
		EOS_Num
	};
	/** \装备单个部分类型
	*/
	enum EPartType
	{
		EPT_Main,					// 主模型
		EPT_Pertain,				// 附属模型
		EPT_Attach,					// 挂载变形模型
		EPT_BladeFlare,             // 刀光
		EPT_Glow,					// 光晕
		EPT_LongHunEffect,			// 龙魂特效
		EPT_Mend,                   // 补充部分（时装模式装备拢手需要加一节胳膊）
		EPT_Num
	};

	AvatarEquipElement( AvatarEquipNode* pFather, EAvatarElement eElement );
	~AvatarEquipElement();

	/** \更新(主要用于变形动画控制)
	*/
	void Updata();
	/** \更新(主要用于加载资源)
	*/
	bool ForceUpdate(float deltaTime,SceneGraph *pSG,const tagAvatarAtt& att);
	/** \释放现有模型并加载人物外观属性
	*/
	void ReloadAtt( const tagAvatarAtt& att, const bool bFashion );
	/** \重新加载装备
	*/
	void ReloadEquip( const BYTE bySex, const tagEquipDisplay& display, const bool bFashion );
	/** \获得外观数据
	*/
	const tagEquipDisplay& GetDisplay() const { return m_equipDisplay; }
	/** \获得主模型
	*/
	const SceneNode* GetMainMdl() const;
	/** \拿出/收回(主要用于武器)
	*/
	void HandUp( const bool bHandUp );
	/** \展开/收回(播放变形动画)
	*/
	void Open( const bool bOpen );
	/** \获得展开状态
	*/
	EOpenState GetOpenState() const { return m_eOpenState; }
	/** \隐藏/显示
	    \bRightMdlOnly 是否只作用右边模型(如：左手，右手）
	    \eType         作用类型 eType == EPT_Num 为全部
	*/
	void SetInvisible( const bool bInvisible, const bool bRightMdlOnly = false, const EPartType eType = EPT_Num );
	/** \播放/停止刀光
	*/
	void PlayBladeFlare( const bool bPlay );
	/** \更新龙魂特效
	*/
	void UpdataLongHunEffect( const BYTE byEquipEffect, const bool bAttachToSG );
	/** \更换贴图
	*/
	void ChangeTexture( SceneNode* pNode, const int nMtlID, const TCHAR* szNewTexPath );
	/** \设置发色
	*/
	void SetHairColorTex(BYTE bySex, WORD hairID, WORD colorID);
	/** \设置面部细节
	*/
	void SetFaceDetailTex(BYTE bySex, WORD id);
	/** \所有资源是否都加载完成
	*/
	bool IsResourceReady() const;
	/** \是否有数据
	*/
	bool IsEmpty() const { return m_vecPartData.empty(); }
	/** \连接到场景
	*/
	void AttachToSG();
	void AttachPartNode( SceneNode* pMainNode, SceneNode* pPartNode );
	void Destroy();
private:
	/** \装备单个部分数据
	*/
	struct tagPartData
	{
		EPartType  eType;			// 类型
		bool       bRight;          // 是否为右边模型
		SceneNode* pSGNode;         // scene node
		tagPartData( EPartType type, bool bR ) : eType( type ), bRight( bR ), pSGNode( NULL ) {}
	};
private:
	void		  LoadOnePart( EPartType ePartType, const bool bRight, const tstring& strFileName, const EEquipMdlType eMdlType, const tstring& strTagNodeName );
	void		  LoadOneSkin( EPartType ePartType, const bool bRight, const tstring& strFileName );
	void          LoadOneSFX(  EPartType ePartType, const bool bRight, const tstring& strFileName, const tstring& strTagNodeName, const bool bAttchToSG );
	bool          LoadAttachEffect( const EPartType eType, const tstring& strFileName, const bool bAttchToSG );
	bool		  HaveKeyFrame();
	void          PlayKeyFrameTrack( const tstring& strTrackName, bool bLoop );
	bool          IsKeyFrameTrackEnd( const tstring& strTrackName );
	bool		  SetColor( const int nColor );
	const tstring GetMdlExt( const EEquipMdlType eType ) const;
	const tstring GetEquipFileName( const BYTE bySex, const TCHAR* szMdlName, const EEquipMdlType eMdlType ) const;
	const tstring GetAttFileName( const tagAvatarAtt& att, bool bFashion, bool bRight = true ) const;
	const tstring GetGlowName( const TCHAR* szBaseName, const BYTE byVal ) const;
	const tstring GetAttatchEffectBindBoneName( const bool bRight ) const;
private:
	const EAvatarElement   m_eElement;                // AvatarNode中的部位
	AvatarEquipNode*	   m_pFather;                 // 父AvatarNode
	tagEquipDisplay		   m_equipDisplay;            // 可视外观结构
	vector<tagPartData>    m_vecPartData;             // 数据列表
	EOpenState             m_eOpenState;			  // 打开状态
	tstring				   m_strGlowName;			  // 光晕的特效编号
	tstring				   m_strLongHunEffName;		  // 龙魂特效的特效编号
	bool                   m_bTexChanged;             // 材质修改完毕
};
