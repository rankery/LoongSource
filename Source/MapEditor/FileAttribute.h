#pragma once
#include "afxwin.h"
#include ".\ColorButton.h"

// FileAttribute 对话框

class FileAttribute : public CDialog
{
	DECLARE_DYNAMIC(FileAttribute)

public:
	FileAttribute(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FileAttribute();

// 对话框数据
	enum { IDD = IDD_MapProperty };
	virtual BOOL OnInitDialog();

	Vector3 GetSunPos()const{return m_vecSunPos;}
	Color4f GetSunDiffuse()const{return m_SunCol;}
	Color4f GetAmbient()const{return m_AmbientCol;}
	Color4f GetSunSpecular()const{return m_Specular;}
	void GetSkyBox(CString& szTopTex, CString& szFrontRightTex, CString& szBackLeftTex, 
		Vector3& drawSize, Vector3& offSize)const;
	FLOAT GetSunModulus()const { return m_fSunModulus;}
	FLOAT GetSkyYaw()const { return m_fSkyYaw;}
	VOID GetFogAttribute(FLOAT& start, FLOAT& end, FLOAT& den){start=m_fstart; end=m_fend; den=m_fdensity;}
	Color4f GetFogColor()const { return m_FogColor; }
	Color4f GetSkyCol()const { return m_SkyCol; }
	DWORD GetSkyShadeCol()const { return m_dwSkyShadeCol; }
	bool IsRenderSkyShade()const { return TRUE == m_bRenderSkyShade; }
	DWORD GetDynamicDiffCol()const { return m_dwDynamicDiffCol; }
	DWORD GetDynamicAmbCol()const { return m_dwDynamicAmbCol; }
	DWORD GetDynamicSpecCol()const { return m_dwDynamicSpecCol; }

	void SetOperationType(bool bVal){m_bSave=bVal;}
	void SetSunPos(const Vector3& sunPos){m_vecSunPos = sunPos;}
	void SetSunDiffuse(const Color4f& sunCol){m_SunCol = sunCol;}
	void SetAmbient(const Color4f& echoCol){m_AmbientCol = echoCol;}
	void SetSunSpecular(const Color4f& specular){m_Specular = specular;}
	void SetSkyBox(const CString& szTopTex, const CString& szFrontRightTex, const CString& szBackLeftTex, 
		const Vector3& drawSize, const Vector3& offSize);
	void SetSunModulus(FLOAT fVal) { m_fSunModulus = fVal; }
	void SetSkyYaw(FLOAT fVal) { m_fSkyYaw = fVal; }
	void SetFogAttribute(FLOAT start, FLOAT end, FLOAT den) {m_fstart=start; m_fend=end; m_fdensity=den;}
	void SetFogColor(const Color4f& col) { m_FogColor = col; }
	void SetSkyCol(const Color4f& skyCol) { m_SkyCol = skyCol;}
	void SetSkyShadeCol(const DWORD dwSkyShadeCol) { m_dwSkyShadeCol = dwSkyShadeCol;}
	void SetRenderSkyShade(const bool bRenderSkyShade) { m_bRenderSkyShade = bRenderSkyShade; }
	void SetDynamicDiffCol(const DWORD dwCol) { m_dwDynamicDiffCol = dwCol; }
	void SetDynamicAmbCol(const DWORD dwCol) { m_dwDynamicAmbCol = dwCol; }
	void SetDynamicSpecCol(const DWORD dwCol) { m_dwDynamicSpecCol = dwCol; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HBRUSH m_brMine;
private:
	bool m_bSave;							// 是保存还是读盘
	Vector3 m_vecSunPos;					// 太阳相对位置
	Color4f m_SunCol;						// 太阳光颜色
	Color4f m_AmbientCol;					// 天空散射光颜色
	Color4f m_Specular;						// 高光
	DWORD   m_dwDynamicDiffCol;             // 动态平行光diffuse
	DWORD   m_dwDynamicAmbCol;				// 动态平行光Ambient
	DWORD   m_dwDynamicSpecCol;             // 动态平行光Specular
	Color4f m_SkyCol;						// 天光
	DWORD   m_dwSkyShadeCol;				// 天空遮罩颜色
	BOOL    m_bRenderSkyShade;              // 是否渲染天空遮罩
	CString m_szSkyBoxTopTex;				//天空盒上纹理
	CString m_szSkyBoxFrontRightTex;		//天空盒前右纹理
	CString m_szSkyBoxBackLeftTex;			//天空盒后左纹理
	Vector3 m_vecSkyBoxSize;				//天空盒大小
	Vector3 m_vecSkyBoxOffSize;				//天空盒偏移
	FLOAT	m_fSunModulus;					//太阳光增强系数
	FLOAT	m_fSkyYaw;						//天空盒旋转角度
	Color4f m_FogColor;						//雾颜色
	FLOAT	m_fstart;						//雾start					
	FLOAT	m_fend;							//雾end	
	FLOAT	m_fdensity;						//雾浓度
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeMapid();
	CEdit m_EditSunPosX;					// 太阳相对位置X轴
	CEdit m_EditSunPosY;					// 太阳相对位置Y轴
	CEdit m_EditSunPosZ;					// 太阳相对位置Z轴
	CColorButton m_btnSunCol;
	CColorButton m_btnAmbientCol;
	CColorButton m_btnSpecular;
	CColorButton m_btnSkyCol;
	CColorButton m_btnSkyShadeCol;
	CColorButton m_btnDynamicDiffCol;
	CColorButton m_btnDynamicAmbCol;
	CColorButton m_btnDynamicSpecCol;
	afx_msg void OnBnClickedButtonSuncol();	//太阳光颜色按钮事件
	afx_msg void OnBnClickedButtonEchocol();//天空散射光颜色按钮事件
	afx_msg void OnBnClickedButtonSpecular();//镜面光按钮事件
	CEdit m_editSkyBoxX;		//天空合X轴size
	CEdit m_editSkyBoxY;		//天空合Y轴size
	CEdit m_editSkyBoxZ;		//天空合Z轴size
	afx_msg void OnBnClickedBtnTop();
	afx_msg void OnBnClickedBtnFntrgt();
	afx_msg void OnBnClickedBtnBckleft();
	CEdit m_EditSkyBoxOffX;
	CEdit m_EditSkyBoxOffY;
	CEdit m_EditSkyBoxOffZ;
	CEdit m_editSunModulus;		//太阳光增强系数
	CEdit m_editSkyYaw;			//天空盒旋转角度
	CColorButton m_btnFogColor;	
	CEdit m_editFogDen;
	CEdit m_editFogStart;
	CEdit m_editFogEnd;
	afx_msg void OnBnClickedButtonFogcolor();
	int m_dist;
	afx_msg void OnEnChangeEditDist();
	afx_msg void OnBnClickedButtonSkycol();//天光
	afx_msg void OnBnClickedSkyshadecol();
	afx_msg void OnBnClickedButtonDynamicdiff();
	afx_msg void OnBnClickedButtonDynamicamb();
	afx_msg void OnBnClickedButtonDynamicspec();
};
