#pragma once
#include "EffectLib.h"
#include "afxwin.h"
#include "EffectPrivew.h"

class CEffectPropDlg;
// CMainDlg 对话框

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
	enum { IDD = IDD_MainDlg };

	void SetEffectPos(const Vector3& pos,bool bUpdateEdit=true);
	void OnRender();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedLibadd();
	afx_msg void OnBnClickedLibsave();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedLibload();
	afx_msg void OnBnClickedLibdel();
	afx_msg void OnLbnSetfocusLiblist();
	afx_msg void OnLbnSetfocusPathlist();

	EffectBase *NewEftAndEdit(bool bLib);
	void Libedit(int focusList);
	bool SetEditEffect_Lib(CEffectPropDlg* pDlg,EffectBase *pEffect);
	bool SetEditEffect_SFX(CEffectPropDlg* pDlg,EffectBase *pEffect);
	EffectBase *GetSelectedLibEffect(int focusList);

	EffectLib	m_eftLib;
	CListBox	m_hRenderList;
	CListBox	m_hPathList;
	CListBox	m_sgEffectList;
	CListBox	m_sgPathList;
	CComboBox	m_actList;

	SGAttachableEffectNode *m_pEditEffect;
	void NewEditEffectIfNULL();
	void UpdateSGEffectList();
	void UpdateSGEffectPathList();

	AvatarNode	*m_pPlayerMdl;//用来预览动作的主角模型

	void LibCloneEffect(int focusList);

	afx_msg void OnBnClickedEftreset();
	afx_msg void OnBnClickedEftadd();
	afx_msg void OnBnClickedEftdel();
	afx_msg void OnBnClickedEftsave();
	afx_msg void OnBnClickedEftload();
	afx_msg void OnLbnDblclkLiblist();
	afx_msg void OnLbnDblclkPathlist();
	afx_msg void OnBnClickedPathadd();
	afx_msg void OnBnClickedLibdelpath();
	afx_msg void OnBnClickedPathdel();
	afx_msg void OnLbnDblclkSgeffectlist();
	afx_msg void OnLbnDblclkSgpathlist();
	afx_msg void OnLbnSelchangeSgeffectlist();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedEftnew();
	afx_msg void OnBnClickedLibclone();
	afx_msg void OnBnClickedLibclonepath();

	afx_msg void OnBnClickedLibimport();
	afx_msg void OnEnChangePosy();
	afx_msg void OnEnChangePosx();
	afx_msg void OnEnChangePosz();
	afx_msg void OnBnClickedEftclone();
	afx_msg void OnBnClickedEftpathclone();

	afx_msg void OnLbnSelchangeSgpathlist();
	afx_msg void OnBnClickedLoadboy();
	afx_msg void OnBnClickedLoadgirl();
	afx_msg void OnCbnSelchangePlayeraction();

	void UpdatePosFromEdit();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	CEdit m_hTimeScale;
	afx_msg void OnEnChangeTimes();

	CEdit m_hShowPlayTime;

	afx_msg void OnTimer(UINT nIDEvent);

	CButton m_hActLoop;

	void AttachToSG( SceneNode* pFather, CString szTagNodeName );
	void DetachFromSG();
	bool m_bAttachedToSG;

	SGAvatarAttachment* m_pEquipMdl;			// 当前挂载的装备
	CComboBox	m_Comb_AvatarEle;				// 装备部位列表
	CComboBox   m_Comb_EquiptBoneTagNodes;		// 装备骨骼挂载点列表
	CComboBox   m_Comb_EffectBoneTagNodes;		// 特效骨骼挂载点列表
	CComboBox   m_Comb_EffectEquipTagNodes;		// 特效装备挂载点列表
	CComboBox   m_Comb_LOD;						// 特效细节列表

	CStatic     m_hFileName;

	CEdit		m_hMsgPlay;
	CEdit		m_hMsgStop;
	CButton		m_hPlay;
	CButton     m_hPause;
	CButton     m_hStop;
	BOOL        m_bUseActMsg;
	CString     m_strMsgPlay;
	CString     m_strMsgStop;

	CEdit       m_hRoleScale;
public:
	afx_msg void OnBnClickedAttachmdl();
	afx_msg void OnBnClickedBindtobone();
	afx_msg void OnBnClickedChangeavatar();
	afx_msg void OnBnClickedBindtoequip();
	afx_msg void OnCbnSelchangeCombLod();
	afx_msg void OnEnChangeMsgplay();
	afx_msg void OnEnChangeMsgstop();
	afx_msg void OnBnClickedUseactmsg();
	afx_msg void OnBnClickedReloadmsgtab();
	afx_msg void OnBnClickedLoadnpc();
	afx_msg void OnEnChangeRolescale();
};
