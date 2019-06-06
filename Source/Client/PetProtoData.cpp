#include "StdAfx.h"
#include "PetProtoData.h"
#include "..\WorldDefine\move_define.h"

PetProtoData::PetProtoData(void)
{
}

PetProtoData::~PetProtoData(void)
{
}

PetProtoData g_PetProtoData;
PetProtoData* PetProtoData::Inst()
{
    return &g_PetProtoData;
}

void PetProtoData::LoadFromFile()
{
    // 宠物原型
    tstring strProtoPath = _T("data\\system\\attdata\\pet_proto.xml");
    tstring strPath = g_strLocalPath + _T("\\attdata\\pet_name.xml");
    
    list<tstring>       PetProtoList;
    list<tstring>       PetProtoNameList;

    TObjRef<VarContainer> VarPetProto = CreateObj("PetProto", "VarContainer");
    if (!VarPetProto->Load("VFS_System", strProtoPath.c_str(), "id", &PetProtoList))
        IMSG(_T("Load file pet_proto.xml failed\r\n"));

    TObjRef<VarContainer> VarPet = CreateObj("PetProtoName", "VarContainer");
    if (!VarPet->Load("VFS_System", strPath.c_str(), "id", &PetProtoNameList))
        IMSG(_T("Load file pet_name.xml failed\r\n"));

    for (list<tstring>::iterator it = PetProtoNameList.begin();
        it != PetProtoNameList.end(); ++it)
    {
        tagPetProto     petData;

        petData.dwID                        =       VarPet->GetDword(_T("id"), (*it).c_str());
        petData.strModelName[EPMT_Follow]   =       VarPet->GetString(_T("summon_model"), (*it).c_str(), _T(""));
        petData.strModelName[EPMT_Mount]    =       VarPet->GetString(_T("mount_model"), (*it).c_str(), _T(""));
        petData.eAniType[EPMT_Follow]       =       static_cast<EAniType>(VarPet->GetInt(_T("anitype1"), (*it).c_str()), 2);
        petData.eAniType[EPMT_Mount]        =       static_cast<EAniType>(VarPet->GetInt(_T("anitype2"), (*it).c_str()), 2);
        petData.strAct1                     =       VarPet->GetString(_T("mount_act1"), (*it).c_str(), _T(""));
        petData.strAct2                     =       VarPet->GetString(_T("mount_act2"), (*it).c_str(), _T(""));
        GeneratePath(&petData);

        m_mapPetProto.insert(make_pair(petData.dwID, petData));
    }

    for (list<tstring>::iterator it = PetProtoList.begin();
        it != PetProtoList.end(); ++it)
    {
        DWORD dwID = VarPetProto->GetDword(_T("id"), (*it).c_str());
        _id2pet::iterator itPet = m_mapPetProto.find(dwID);
        if (itPet != m_mapPetProto.end())
        {
            itPet->second.boundingBox.x     =       VarPetProto->GetFloat(_T("box_x"), (*it).c_str(), X_DEF_CREATURE_SIZE_X / 2);
            itPet->second.boundingBox.y     =       VarPetProto->GetFloat(_T("box_y"), (*it).c_str(), X_DEF_CREATURE_SIZE_Y / 2);
            itPet->second.boundingBox.z     =       VarPetProto->GetFloat(_T("box_z"), (*it).c_str(), X_DEF_CREATURE_SIZE_Z / 2);
            itPet->second.scalePercent      =       VarPetProto->GetFloat(_T("scale"), (*it).c_str(), PET_SCALE_PERCENT_BASE) / PET_SCALE_PERCENT_BASE;  // 以PET_SCALE_PERCENT_BASE为底的百分比
            itPet->second.carryLevel        =       VarPetProto->GetInt(_T("rolelvl"), (*it).c_str(), 0);
            itPet->second.bBinding          =       VarPetProto->GetInt(_T("bind"), (*it).c_str(), FALSE);
        }
    }

    KillObj("PetProto");
    KillObj("PetProtoName");

    // 宠物技能原型
    strPath = _T("data\\system\\attdata\\pet_skill_proto.xml");
    tstring strSkillName = g_strLocalPath + _T("\\attdata\\pet_skill_name.xml");
    list<tstring>       PetSkillList;
    list<tstring>       PetSkillNameList;

    TObjRef<VarContainer> VarPetSkill = CreateObj("PetSkillProto", "VarContainer");
    if (!VarPetSkill->Load("VFS_System", strPath.c_str(), "id", &PetSkillList))
        IMSG(_T("Load file pet_skill_proto.xml failed\r\n"));
    for (list<tstring>::iterator it = PetSkillList.begin();
            it != PetSkillList.end(); ++it)
    {
        tagPetSkillData skillData;
        skillData.dwPetSkillID      =   VarPetSkill->GetDword(_T("id"), it->c_str(), GT_INVALID);
        skillData.dwPetSkillTypeID  =   MTransPetSkillID(skillData.dwPetSkillID);
        skillData.nLevel            =   MTransPetSkillLevel(skillData.dwPetSkillID);
        skillData.eType             =   (EPetskillType)VarPetSkill->GetInt(_T("type1"), it->c_str(), 0);
        skillData.eType2            =   (EPetskillType2)VarPetSkill->GetInt(_T("type2"), it->c_str(), 0);
        skillData.eType3            =   (EPetskillType3)VarPetSkill->GetInt(_T("type3"), it->c_str(), 0);
        skillData.eCastType         =   (EPetskillCastType)VarPetSkill->GetInt(_T("cast_type"), it->c_str(), 0);
        skillData.eCastCondition    =   (EPetskillCastCondition)VarPetSkill->GetInt(_T("cast_condition"), it->c_str(), 0);
        skillData.dwCDTime          =   VarPetSkill->GetDword(_T("cooldown_time"), it->c_str(), 0);
        skillData.cast_condition    =   VarPetSkill->GetDword(_T("cast_condition"), it->c_str(), 0);
        skillData.wuxing_cost       =   VarPetSkill->GetInt(_T("wuxing_cost"), it->c_str(), 0);
        skillData.spirit_cost       =   VarPetSkill->GetInt(_T("spirit_cost"), it->c_str(), 0);

        m_mapPetSkillProto.insert(make_pair(skillData.dwPetSkillID, skillData));
    }

    KillObj("PetSkillProto");

    TObjRef<VarContainer> VarPetSkillName = CreateObj("PetSkillName", "VarContainer");
    if (!VarPetSkillName->Load("VFS_System", strSkillName.c_str(), "id", &PetSkillNameList))
        IMSG(_T("Load file pet_skill_name.xml failed\r\n"));

    for (list<tstring>::iterator it = PetSkillNameList.begin();
            it != PetSkillNameList.end(); ++it)
    {
        DWORD skillID = VarPetSkillName->GetDword(_T("id"), it->c_str(), GT_INVALID);
        _id2petskill::iterator itr = m_mapPetSkillProto.find(skillID);
        if (itr != m_mapPetSkillProto.end())
        {
            itr->second.strType =   VarPetSkillName->GetString(_T("display_type"), it->c_str(), _T(""));
            itr->second.strName =   VarPetSkillName->GetString(_T("name"), it->c_str(), _T(""));
            itr->second.strDesc =   VarPetSkillName->GetString(_T("desc"), it->c_str(), _T(""));
            tstringstream sstream;
            sstream << _T("data\\ui\\Icon\\")
                << VarPetSkillName->GetString(_T("icon"), it->c_str(), _T(""))
                << _T(".tga");
            itr->second.strIconPath = sstream.str();
        }
    }

    KillObj("PetSkillName");
}

void PetProtoData::GeneratePath( tagPetProto* pPetData )
{
    tstringstream strPath;

    for (int i = 0; i != EPMT_NUM; ++i)
    {
        // 读取路径
        strPath.str(_T(""));
        strPath.clear();

        strPath << _T("data\\animation\\npc\\") << pPetData->strModelName[i] << _T("\\");
        switch (pPetData->eAniType[i])
        {
        case EAT_None:
			strPath << pPetData->strModelName[i];
            strPath << _T(".fsm");
            break;

		case EAT_KeyFrame:
			strPath << pPetData->strModelName[i];
            strPath << _T(".fak");
            break;

		case EAT_Skeleton:
            strPath << _T("std.fskel");
            break;
        }

        pPetData->strModelPath[i]   =   strPath.str();

        // 读取蒙皮
        if (EAT_Skeleton == pPetData->eAniType[i])
        {
            pPetData->lstSkinPath[i].clear();
            
            for(int index = 1;;++index)
            {
                strPath.str(_T(""));
                strPath.clear();

                strPath << _T("data\\animation\\npc\\") << pPetData->strModelName[i] << _T("\\") << pPetData->strModelName[i]
                    << _T("_obj") << index << _T(".fskin");

                IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
                DWORD fileSize = pIFS->GetSize(strPath.str().c_str());
                if (0 == fileSize || GT_INVALID == fileSize)
                    break;

                pPetData->lstSkinPath[i].push_back(strPath.str());
            }
        }
    }
}

tstring PetProtoData::GetAction1( DWORD typeId )
{
    const tagPetProto* pData = FindPetProto(typeId);
    if (P_VALID(pData))
        return pData->strAct1;

    return _T("");
}

tstring PetProtoData::GetAction2( DWORD typeId )
{
    const tagPetProto* pData = FindPetProto(typeId);
    if (P_VALID(pData))
        return pData->strAct2;

    return _T("");
}

int PetProtoData::GetPetSkillMaxCd( DWORD skillId )
{
    const tagPetSkillData* pSkillData = FindInMap(m_mapPetSkillProto, skillId);
    if (P_VALID(pSkillData))
        return pSkillData->dwCDTime;
    else
        return GT_INVALID;
}