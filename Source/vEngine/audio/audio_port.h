//-----------------------------------------------------------------------------
// File: audio_port
// Desc: interface for all audio system
// Auth: Lyp
// Date: 2004/02/20
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{
//-----------------------------------------------------------------------------
// 提供各种音效系统(如FMOD/MILES/等)的统一接口
// 请尽量保证接口的最小化,以便今后对接各种Audio引擎
// 复杂的播放逻辑可以放到上层CAudio(audio.cpp)
//-----------------------------------------------------------------------------
class AudioPort
{
public:
	// 初始化操作
	virtual BOOL Init(){return FALSE;}
	virtual VOID Destroy(){}
	virtual VOID SetVFSCallBack(FP_OPENCALLBACK, FP_CLOSECALLBACK,
		FP_READCALLBACK, FP_SEEKCALLBACK, FP_TELLCALLBACK){}
	
	// 取得和设置总体音量
	virtual INT  GetMasterVolume(){return 0;}
	virtual VOID SetMasterVolume(INT nVolume){}

	// 取得和设置通道音量
	virtual INT GetChannelVolume(INT nChannel){return 0;}
	virtual INT SetChannelVolume(INT nChannel, INT nVolume){return 0;}
	
	// 对于music(mod/s3m/xm/it/mid/rmi/sgt...)的操作
	virtual DWORD	OpenMusic(LPCTSTR szName){return GT_INVALID;}
	virtual BOOL	CloseMusic(DWORD dwMusicHandle){return FALSE;}
	virtual INT		PlayMusic(DWORD dwMusicHandle, BOOL bLoop){return 0;}
	virtual BOOL	PauseMusic(DWORD dwMusicHandle, BOOL bPause){return FALSE;}
	virtual BOOL	StopMusic(DWORD dwMusicHandle){return FALSE;}


	// 对于3d/2d sound effect的操作
};




}