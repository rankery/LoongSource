//-----------------------------------------------------------------------------
// File: fmod_support
// Desc: interface for fmod
// Auth: Lyp
// Date: 2004/02/20
//-----------------------------------------------------------------------------
#include "..\stdafx.h"

#ifdef _USE_FMOD_
#include "audio_port.h"
#include "fmod_support.h"

// fmod
#include "fmod\api\inc\fmod.h"
#include "fmod\api\inc\fmod_errors.h"
#pragma comment(lib,"audio\\fmod\\api\\lib\\fmodvc.lib")

namespace vEngine{
//-----------------------------------------------------------------------------
// init the fmod sound/music effect system
//-----------------------------------------------------------------------------
BOOL Fmod::Init()
{
	// 检测FMOD的版本
    if( FSOUND_GetVersion() < FMOD_VERSION )
    {
		INT nVerMajor = (INT)(FMOD_VERSION);	// Msg 不支持 "%f"
		INT nVerMinor = (INT)((FMOD_VERSION - nVerMajor)*100);
        ERR(_T("FMOD: Wrong DLL version!  Version%d.%d needed"), 
			nVerMajor, nVerMinor);
        return FALSE;
    }

	// 初始化FMOD
	return FSOUND_Init(44100, 32, 0);	// 32 channels
}

//-----------------------------------------------------------------------------
// destroy the fmod sound/music effect system
//-----------------------------------------------------------------------------
VOID Fmod::Destroy()
{
	// This also closes down the sample management system, 
	// freeing all MANAGED samples loaded (unless they were 
	// allocated with the FSOUND_UNMANAGED flag).
	// Streams are not freed. You must close them yourself.
	// 另外注意，FMOD的FSOUND_Close不能通过静态对象在程序退出的时候析构来
	// 调用,应该提前手动关闭
	FSOUND_Close();
}


//-----------------------------------------------------------------------------
// 设置我们自己的文件操作函数, !!注意!!必须先初始化(FSOUND_Init)，
// 再设置Callback，否则FMOD会用我们的文件操作函数去搜索d3d9.dll! hehe
//-----------------------------------------------------------------------------
VOID Fmod::SetVFSCallBack(FP_OPENCALLBACK fp1, FP_CLOSECALLBACK fp2, 
		   FP_READCALLBACK fp3, FP_SEEKCALLBACK fp4, FP_TELLCALLBACK fp5)
{
	FSOUND_File_SetCallbacks(fp1, fp2, fp3, fp4, fp5);
}


//-----------------------------------------------------------------------------
// 取得和设置总体音量
//-----------------------------------------------------------------------------
INT Fmod::GetMasterVolume()
{
	return FSOUND_GetSFXMasterVolume();
}

VOID Fmod::SetMasterVolume(INT nVolume)
{
	FSOUND_SetSFXMasterVolume(nVolume);
}



//-----------------------------------------------------------------------------
// 取得和设置通道音量
//-----------------------------------------------------------------------------
INT Fmod::GetChannelVolume(INT nChannel)
{
	return FSOUND_GetVolume(nChannel);
}


INT Fmod::SetChannelVolume(INT nChannel, INT nVolume)
{
	return FSOUND_SetVolume(nChannel, nVolume);
}



//-----------------------------------------------------------------------------
// 对于music(mod/s3m/xm/it/mid/rmi/sgt...)的操作
//-----------------------------------------------------------------------------
DWORD Fmod::OpenMusic(LPCTSTR szName)
{
	return (DWORD)FMUSIC_LoadSong((const char *)szName);
}


BOOL Fmod::CloseMusic(DWORD dwMusicHandle)
{
	return FMUSIC_FreeSong((FMUSIC_MODULE*)dwMusicHandle);
}


INT Fmod::PlayMusic(DWORD dwMusicHandle, BOOL bLoop)
{

	BOOL bResult = FMUSIC_PlaySong((FMUSIC_MODULE*)dwMusicHandle);
	if( !bResult )
		return GT_INVALID;
	return 1;
}

BOOL Fmod::PauseMusic(DWORD dwMusicHandle, BOOL bPause)
{
	return FMUSIC_SetPaused((FMUSIC_MODULE*)dwMusicHandle, (signed char)bPause);
}

BOOL Fmod::StopMusic(DWORD dwMusicHandle)
{
	return FMUSIC_StopSong((FMUSIC_MODULE*)dwMusicHandle);
}


}

#endif // _USE_FMOD_