#pragma once


// FilterSelect 对话框

class FilterSelect : public CDialog
{
	DECLARE_DYNAMIC(FilterSelect)

public:
	FilterSelect(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FilterSelect();

// 对话框数据
	enum { IDD = IDD_FilterSelect };
	virtual BOOL OnInitDialog();
	bool GetIsShowNpc()const{return m_IsNpc;}
	bool GetIsShowDynamicBlock()const{return m_IsDynaBlock;}
	bool GetIsShowStaticObj()const{return m_IsStaticObj;}
	bool GetIsShowQuad()const{return m_IsQuad;}
	bool GetIsShowLock()const{return m_IsLock;}
	bool GetIsShowFlag()const{return m_IsFlag;}
	bool GetIsShowMapRect()const {return m_IsMapRect;}
	bool GetIsShowTrigger()const {return m_IsTrigger;}
	bool GetIsShowWayPoint()const{return m_IsWayPoint;}
	bool GetIsShowMapDoor()const{return m_IsMapDoor;}
	bool GetIsShowMapTriggerEffect()const { return m_IsMapTriggerEffect; }
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckNpc();
	afx_msg void OnBnClickedOk();

private:
	bool m_IsNpc;
	bool m_IsDynaBlock;
	bool m_IsStaticObj;
	bool m_IsQuad;
	bool m_IsLock;
	bool m_IsFlag;
	bool m_IsMapRect;
	bool m_IsTrigger;
	bool m_IsWayPoint;
	bool m_IsMapDoor;
	bool m_IsMapTriggerEffect;
};
