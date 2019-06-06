//-----------------------------------------------------------------------------
// File: fmod_support
// Desc: interface for fmod
// Auth: Lyp
// Date: 2004/02/20
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{
//-----------------------------------------------------------------------------
// fmod interface
//-----------------------------------------------------------------------------
class Fmod : public AudioPort
{
public:
	virtual BOOL Init();
	virtual VOID Destroy();
	virtual VOID SetVFSCallBack(FP_OPENCALLBACK, FP_CLOSECALLBACK,
		FP_READCALLBACK, FP_SEEKCALLBACK, FP_TELLCALLBACK);

	// 取得和设置总体音量
	virtual INT  GetMasterVolume();
	virtual VOID SetMasterVolume(INT nVolume);

	// 取得和设置通道音量
	virtual INT GetChannelVolume(INT nChannel);
	virtual INT SetChannelVolume(INT nChannel, INT nVolume);
	
	// 对于music(mod/s3m/xm/it/mid/rmi/sgt...)的操作
	virtual DWORD	OpenMusic(LPCTSTR szName);
	virtual BOOL	CloseMusic(DWORD dwMusicHandle);

	virtual INT		PlayMusic(DWORD dwMusicHandle, BOOL bLoop);
	virtual BOOL	PauseMusic(DWORD dwMusicHandle, BOOL bPause);
	virtual BOOL	StopMusic(DWORD dwMusicHandle);


};


}