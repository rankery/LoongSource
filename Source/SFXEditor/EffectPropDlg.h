#pragma once
#include "EffectPrivew.h"
#include "..\Cool3D\public\propertylist.h"
#include "afxwin.h"
#include "afxcmn.h"


// CEffectPropDlg 对话框

class CEffectPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CEffectPropDlg)

public:
	CEffectPropDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEffectPropDlg();

// 对话框数据
	enum { IDD = IDD_EffectProp };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	EffectPrivew m_preView;
	
	//quad
	EffectQuad::tagQuadProp			m_quadProp;
	vector<EffectQuad::tagState>	m_quadStates;
	//cylinder
	EffectCylinder::tagCylinderProp	m_cyProp;
	vector<EffectCylinder::tagState> m_cyStates;
	//grid
	EffectGrid::tagGridProp			m_gridProp;
	vector<EffectGrid::tagState>	m_gridStates;
	//beam
	EffectBeam::tagBeamProp			m_beamProp;
	vector<EffectBeam::tagState>	m_beamStates;
	//SceneDecal
	EffectSceneDecal::tagDecalProp		m_scenedecalProp;
	vector<EffectSceneDecal::tagState>	m_scenedecalStates;
	//Motion path
	EffectMotion::tagMotionProp		m_mtProp;
	vector<EffectMotion::tagState>	m_mtStates;
	//Target path
	EffectTarget::tagTargetProp		m_targetProp;
	//Particle Emitter
	EffectParticleEmitter::tagPEMProp		m_parProp;
	vector<EffectParticleEmitter::tagState>	m_parStates;
	//Msg
	EffectMsg::tagProp				m_msgProp;
	vector<EffectMsg::tagState>		m_msgStates;
	//Mesh
	EffectMesh::tagMeshProp			m_meshProp;
	vector<EffectMesh::tagState>	m_meshStates;
	//sound
	EffectSound::tagSoundProp		m_soundProp;
	vector<EffectSound::tagState>	m_soundStates;
	//cross
	EffectCross::tagCrossProp		m_crossProp;
	vector<EffectCross::tagState>	m_crossStates;
	//explosive
	EffectExplosive::tagExpProp		m_expProp;
	//sphere
	EffectSphere::tagSphereProp		m_sphereProp;
	vector<EffectSphere::tagState>	m_sphereStates;
	//blade flare
	EffectBladeFlare::tagBladeFlareProp	m_bladeFlareProp;
	//camera
	EffectCamera::tagCameraProp     m_cameraProp;
	vector<EffectCamera::tagState>  m_cameraStates;

	int	m_numStates;//包含初始话prop,即states.size()+1
	tagMtlEffectDesc	m_mtlDesc;

	void CopyEftData(EffectBase *pEft);
	void UpdateMtlProp(bool bSave);
	void UpdateStateProp(int stateIndex,bool bSave);
		void UpdateQuadState(int stateIndex,bool bSave);
		void UpdateCylinderState(int stateIndex,bool bSave);
		void UpdateGridState(int stateIndex,bool bSave);
		void UpdateBeamState(int stateIndex,bool bSave);
		void UpdateSceneDecalState(int stateIndex,bool bSave);
		void UpdateMotionState(int stateIndex,bool bSave);
		void UpdateTargetState(int stateIndex,bool bSave);
		void UpdateParticleState(int stateIndex,bool bSave);
		void UpdateMsgState(int stateIndex,bool bSave);
		void UpdateMeshState(int stateIndex,bool bSave);
		void UpdateSoundState(int stateIndex,bool bSave);
		void UpdateCrossState(int stateIndex,bool bSave);
		void UpdateExpState(int stateIndex,bool bSave);
		void UpdateBladeFlareState(int stateIndex,bool bSave);
		void UpdateSphereState(int stateIndex,bool bSave);
		void UpdateCameraState(int stateIndex,bool bSave);
			int	UpdateBaseProp(EffectBase::tagProp *pProp,int stateIndex,bool bSave);
public:
	void SetEditEffect(EffectBase* pEftRender,EffectPathCtrl *pPath,bool bEditPath)
	{
		m_pEftRender=pEftRender;
		m_pEftPath=pPath;
		if(bEditPath)
			m_pEditEffect=pPath;
		else
			m_pEditEffect=m_pEftRender;
		if(pPath!=NULL)
			m_szEftPathName=pPath->GetName();
	}
private:
	EffectBase		*m_pEditEffect;	//需要编辑的特效,必须是下面两个中的一个
	EffectBase		*m_pEftRender;	//用来渲染的元素
	EffectPathCtrl	*m_pEftPath;	//用来控制渲染元素运动的路径
	CString			m_szEftPathName;//因为PathName可能改变，所以为了正确从Preview中detach，需要记录初始名称

	bool m_bPrivew;//是否渲染预览

	void DetachEffectFromPreView();
	
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedApplyedit();
	afx_msg void OnEnChangeIndex();
	afx_msg void OnBnClickedIndexinc();
	afx_msg void OnBnClickedIndexdec();
	afx_msg void OnBnClickedDelstate();
	afx_msg void OnBnClickedAddstate();
	afx_msg void OnBnClickedOpenmdl();
	afx_msg void OnBnClickedWireframe();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnCbnSelchangeActlist();
	afx_msg void OnEnChangeTimescale();
	afx_msg void OnBnClickedShowmdl();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBkcolor();
	
	CStatic m_hPreviewWnd;
	CString m_szStateIndex;
	CPropertyList m_mtlProp;
	CPropertyList m_stateProp;
	CEdit m_hShowTime;
	CComboBox m_actListBox;
	CEdit m_edTimeScale;
	CButton m_hDrawBox;
public:
	afx_msg void OnLbnSelchangeState();
	afx_msg void OnLbnSelchangeMtlprop();
	void ShowSelectedPropInfo(CPropertyList* pList);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedDrawbox();
};
