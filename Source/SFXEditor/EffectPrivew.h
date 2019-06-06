#pragma once
#include "..\Cool3D\Cool3DEngine.h"
#include "..\Cool3D\SpecialEffect\EffectAll.h"
using namespace Cool3D;

enum EEPAvatarElement		
{
	EEPAE_Head,		// 头
	EEPAE_Face,     // 脸
	EEPAE_Upper,	// 上身
	EEPAE_Lower,	// 下身
	EEPAE_Glove,	// 手
	EEPAE_Boot,		// 脚
	EEPAE_Num,
};

class EffectPrivew : public SimpleView
{
public:
	EffectPrivew(void);
	virtual ~EffectPrivew(void);

	void Init(EffectBase *pEft);
	void LoadDefaultAvatar(BYTE bySex=0);
	void UpdateActList(CComboBox *pCmb);
	void PlayAvatarAction(const TCHAR* szActName);
	void UpdateEffect()
	{
		if(m_pEftNode)
			m_pEftNode->RebuildEffectRenderNodes();
	}
	SGEffectNode *GetEffectNode()	{	return m_pEftNode;}
	void SetTimeScale(float s);
	void SetShowMdl(bool bShow);
	void EnableDrawBox(bool bDraw) { m_bDrawBox = bDraw; }
protected:
	virtual void RenderView(bool bAdaptive);
	SGEffectNode	*m_pEftNode;
	AvatarNode		*m_pAvatarNode;
	Timer			m_timer;
	bool            m_bDrawBox;
};

AvatarNode *_LoadDefaultAvatar(BYTE bySex);
AvatarNode *_LoadNPC(const TCHAR* szPath);
void _LoadAvatarActionList(AvatarNode* pNode,CComboBox *pCmb);
void _PlayAvatarAction(AvatarNode* pNode,const TCHAR* szActName,bool bLoop=true);
CString _GetAvatarElementName( const int nEle );
void _LoadAvatarElementList(CComboBox *pCmb);
void _LoadBoneTagNodeList(CComboBox* pCmb);
bool _LoadEquipTagNodeList(SGAvatarAttachment* pNode, CComboBox* pCmb);
bool _ChangeSkin(AvatarNode* pNode,CString strEleName,CString strFileName);