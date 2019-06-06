#include "StdAfx.h"
#include "SceneEffectData.h"

SceneEffectData::SceneEffectData(void)
{
}

SceneEffectData::~SceneEffectData(void)
{
}

SceneEffectData scnEffctData;
SceneEffectData* SceneEffectData::Inst()
{
    return &scnEffctData;
}

void SceneEffectData::LoadFromFile()
{
    tstring strPath = g_strLocalPath + _T("\\attdata\\scn_sfx_name.xml");

    list<tstring>       SfxProtoList;

    TObjRef<VarContainer> VarPet = CreateObj("SfxProto", "VarContainer");
    if (!VarPet->Load("VFS_System", strPath.c_str(), "id", &SfxProtoList))
        IMSG(_T("Load file scn_sfx_name.xml failed\r\n"));

    for (list<tstring>::iterator it = SfxProtoList.begin();
        it != SfxProtoList.end(); ++it)
    {
        tagScnEffectData sfxData;
        DWORD sfxID         =   VarPet->GetDword(_T("id"), (*it).c_str());
        sfxData.strPath     =   _T("data\\system\\Effect\\Scn\\");
        sfxData.strPath     +=  VarPet->GetString(_T("sfx_name"), (*it).c_str());
        sfxData.strPath     +=  _T(".sfx");

        m_mapScnSfx.insert(make_pair(sfxID, sfxData));
    }

    KillObj("SfxProto");
}

tstring SceneEffectData::FindSfxPath( DWORD sfxID )
{
    _id2sfxData::const_iterator it = m_mapScnSfx.find(sfxID);
    if (it == m_mapScnSfx.end())
        return _T("");
    else
    {
        return it->second.strPath;
    }
}