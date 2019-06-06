#pragma once
#include "basedata.h"

struct tagScnEffectData
{
    tstring     strPath;
};

//! \class  SceneEffectData
//! \breif  场影特效定义文件
//! \base   BaseData
//! \author hyu
//! \date   2009-7-23
//! \last   2009-7-23
class SceneEffectData :
    public BaseData
{
public:
    typedef map<DWORD, tagScnEffectData> _id2sfxData;

    SceneEffectData(void);
    ~SceneEffectData(void);

    static SceneEffectData* Inst();

    //! 获得宠物原型数据文件
    virtual void LoadFromFile();

    //! 获得特效路径
    tstring FindSfxPath(DWORD sfxID);

private:
    _id2sfxData     m_mapScnSfx;
};
