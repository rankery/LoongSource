#pragma once
#include "basedata.h"
#include "PetDef.h"
#include "../WorldDefine/pet_skill_define.h"

//! \class  PetProtoData
//! \breif  宠物原始属性
//! \base   BaseData
//! \author hyu
//! \date   2009-4-2
//! \last   2009-4-2
//! \sa     Pet,PetManager
class PetProtoData :
    public BaseData
{
public:
    PetProtoData(void);
    ~PetProtoData(void);

    typedef map<DWORD, tagPetProto> _id2pet;
    typedef map<DWORD, tagPetSkillData> _id2petskill;

    static PetProtoData* Inst();

    //! 获得宠物原型数据文件
    virtual void LoadFromFile();

    //! 获得宠物全部原型数据
    const _id2pet& GetMapId2Pet() { return m_mapPetProto; }

    //! 获得宠物原型数据
    const tagPetProto* FindPetProto(DWORD typeId) { return FindInMap(m_mapPetProto, typeId); }

    //! 获得宠物技能数据
    const tagPetSkillData* FindPetSkill(DWORD skillId) { return FindInMap(m_mapPetSkillProto, skillId); }

    //! 获得技能冷却时间
    int GetPetSkillMaxCd(DWORD skillId);

    //! 获取宠物骑乘动作
    tstring GetAction1(DWORD typeId);
    tstring GetAction2(DWORD typeId);
private:
    //! 生成宠物模型相关跟径
    void GeneratePath(tagPetProto* pPetData);

    _id2pet             m_mapPetProto;
    _id2petskill        m_mapPetSkillProto;
};
