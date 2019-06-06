#pragma once
#include "..\Cool3D\Cool3DEngine.h"
#include "..\Cool3D\ResSys\AABBTreeCollider.h"
#include "AABBTreeBuilder.h"
using namespace Cool3D;


class Cool3DModels
{
public:
	Cool3DModels(void);
	~Cool3DModels(void);

	static Cool3DModels* Inst();
	void InitEngine(HWND hDrawWnd);
	void Destroy();

	void LoadModel(const TCHAR* szPath);
	void Render();
	void SetBkColor(DWORD color);
	void ShowSkelDlg();
	void AttachMdl(const TCHAR* szMdlPath,const TCHAR* szTagNodeName);
	void ShowNormal();
	void ShowTangent();
	void SetShowSkel(bool bShow);
	void EnableSnapshoter( bool bEnable );
	bool SavePhoto( const TCHAR* szFilename );
	void SplitSkel();
	CString GetPhotoFileExt();
	CString GetCurMdlPath();

	// 升级AABBTree格式
	void UpdateAABBTree(LPCTSTR szPath);
	// 修正Skin的材质系数
	void ModifySkin(LPCTSTR szPath);
	// 检查骨骼
	void CheckSkel(void);

	ArcBall* GetArcBall();
	CameraEuler* GetCamera();
	SceneGraph* GetSceneGraph();
	SceneNode* GetSceneNode();
	SceneNode* GetAvatarNode();
	AnimationCtrl* GetSceneNodeAni();
	CString	GetSceneNodeResName();

	void SetCameraFov(float rad);

	// 加载骑乘人物模型
	void LoadHorsemanModel(const TCHAR* szPath);
	// 加载骑乘坐骑模型
	void LoadHorseModel(const TCHAR* szPath);
	// 把骑乘人物绑定到骑乘坐骑上
	void BindToHorse();
	// 获得骑乘人物的臀部高度
	float GetBunsHeight();

	// 更新上半身的骨骼动画
	void UpdateUpperSkeAni(const TCHAR* szTrackName);
	// 更新下半身的骨骼动画
	void UpdateLowerSkeAni(const TCHAR* szTrackName);
	// 更新全身的骨骼动画
	void UpdateAllSkeAni(const TCHAR* szTrackName);
	// 转动上身骨骼
	void RotateUpperSkeAni(float yaw, float pitch, float roll);
	// 转动头部骨骼
	void RotateHeadSkeAni(float yaw, float pitch, float roll);
	// 混合上半身的骨骼动画
	void BlendUpperSkeAni(const TCHAR *szTrackName,float weight=1.0f);
	// 混合下半身的骨骼动画
	void BlendLowerSkeAni(const TCHAR *szTrackName,float weight=1.0f);
	// 混合全身的骨骼动画
	void BlendAllSkeAni(const TCHAR *szTrackName,float weight=1.0f);

	// 修改lightmap的大小
	void ModifyLightmapSize(int size);
	int GetLightmapSize() const;

	typedef enum _skintype
	{
		ST_INVALID		= -1,
		ST_BOY			= 0,		//男装
		ST_GIRL						//女装
	} SkinType;

	//载入系列蒙皮
	//serialIndex——0-based
	bool LoadSkin(SkinType st, unsigned char serialIndex=0);

	//增加球形环境映射
	void ChangeSEMMtl(const TCHAR* szEnvMap);

private:
	BOOL ReadAABBTree(IFS *pFS,LPCTSTR szFile);
	void WriteAABBTree(LPCTSTR szFile);
	
	AABBTreeBuilder		m_builder;
	Cool3D::EVertType	m_vertType;
	AABBTreeCollider	*m_pCDTree;
	BYTE *m_pVB;

private:
	class Member;
	Member *m_p;
};
