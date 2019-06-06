#pragma once

#include "..\WorldDefine\pet_define.h"

//! \class  PetShowWindow
//! \brief  宠物模型展台
//! \author hyu
//! \date   2009-6-17
//! \last   2009-6-17
class PetShowWindow :
    public GUIStatic
{
public:
    PetShowWindow(void);
    virtual ~PetShowWindow(void);

    // 重写基类
    virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID RefreshViewCoord();
    virtual VOID SetInvisible(BOOL b);

    //! \brief  渲染宠物模型
    void Render3D();

    //! \brief  旋转,累积型变量
    void RotateYaw(FLOAT fAngle);

    //! \func   SetPetShow
    //! \param  dwPetTypeID     宠物原型id
    //! \param  eModelType      宠物模型类弄
    //! \brief  设置显示宠物
    void SetPetShow(DWORD dwPetTypeID, EPetModelType eModelType = EPMT_Follow);

protected:

    //! \brief  更新摄像机
    void UpdateCamera();

    //! \brief  加载模型
    EntityNode* LoadEntity(LPCTSTR szFileName);
    void LoadSkin(LPCTSTR szFileName, EntityNode* pEntityNode);

private:
    tagGUIImage*			m_pNewCanvas;       // RenderTarget
    EntityNode*		        m_pPetNode;         // SceneNode
    NullSceneGraph*			m_pSG;              // 用于保存摄像机、光照等信息,不用于渲染
    CameraBase				m_Camera;
    FLOAT                   m_fModelYaw;

};
