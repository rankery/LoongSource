//-----------------------------------------------------------------------------
// File: audio
// Desc: interface for audio
// Auth: Lyp
// Date: 2004/02/09
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "audio.h"

// 使用其它部件
#include "..\vfs\vfs.h"
#include "audio_port.h"

#ifdef _USE_FMOD_
#include "fmod_support.h"
#endif

namespace vEngine{
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
Audio::Audio()//:m_Task(this)
{ 
	m_bFadein = FALSE;
	m_bFadeout = TRUE;
	m_nCurrentVirtualChannel = GT_INVALID;


	m_vecMusicHandle.clear();
	for(INT n=0; n<GT_MAX_STREAM_CHANNEL; n++)
		m_vecMusicHandle.push_back(0);

	m_fDeltaTime = 0;
	m_fCurTime = 0;
}

Audio::~Audio()
{
	Destroy();
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL Audio::Init(LPCTSTR szEngineName)
{
#ifdef _USE_FMOD_
	m_pAudio = new Fmod;
	if( NULL == m_pAudio )
		return FALSE;
#else 
	return FALSE;
#endif

	// 初始化引擎
	BOOL bResult = m_pAudio->Init();

	// 设置文件操作回调函数
	if( bResult )
		m_pAudio->SetVFSCallBack(myopen, myclose, myread, myseek, mytell);

	return bResult;
}


VOID Audio::Destroy()
{
	for(INT nVol = m_pAudio->GetMasterVolume(); nVol>5; nVol-=5 )
	{
		m_pAudio->SetMasterVolume(nVol);
		Sleep(20);
	}

	m_pAudio->Destroy();
	SAFE_DEL(m_pAudio);
}




//-----------------------------------------------------------------------------
// 对于music(mod/s3m/xm/it/mid/rmi/sgt...)的操作
//-----------------------------------------------------------------------------
BOOL Audio::PlayMusic(INT nChannel, LPCTSTR szName, BOOL bLoop)
{
	ASSERT(szName);

	// 打开Music
	DWORD dwHandle = m_pAudio->OpenMusic(szName);
	if( NULL == dwHandle )
		return FALSE;

	// 播放Music
	INT nReturn = m_pAudio->PlayMusic(dwHandle, bLoop);
	if( GT_INVALID == nReturn )
	{
		m_pAudio->CloseMusic(dwHandle);
		return FALSE;
	}
	
	m_vecMusicHandle[nChannel] = dwHandle;
	return TRUE;
}

BOOL Audio::StopMusic(INT nChannel)
{
	return m_pAudio->StopMusic(m_vecMusicHandle[nChannel]);
}

BOOL Audio::PauseMusic(INT nChannel, BOOL bPause)
{
	return m_pAudio->PauseMusic(m_vecMusicHandle[nChannel], bPause);
}





//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID Audio::Update(FLOAT fDeltaTime)
{
/*
	DWORD dw = 0;
	int n = m_Task.AddTask(0, &dw, true);
	if( n != GT_INVALID )
	{
		m_Task.AddSubTask(n, ProcCommonTask, NULL, 0, 2);
		m_Task.StartTask(n);
	}

	m_fDeltaTime = fDeltaTime;
	m_fCurTime += fDeltaTime;

	// 调用任务系统的内置时间控制
	m_Task.Update(m_fCurTime);

	m_Task.DoTask(0);*/
}

/*
//-----------------------------------------------------------------------------
// 完成普通型任务
//-----------------------------------------------------------------------------
DWORD Audio::ProcCommonTask(DWORD dwParam)
{
	if( !m_p )
		return 0;

		// for short
	INT nCurTask = GetCurTask();
	INT nCurSubTask = GetCurSubTask();
	tagTask<DWORD, Audio>* pTask = GetTask(nCurTask);
	tagSubTask<Audio>* pSub = &(pTask->pSubTask[nCurSubTask]);

	// 进行帧时间校正
	float fDeltaTime = m_fDeltaTime;
	if( m_fCurTime - pSub->fStartTime > pSub->fExpectTime )
		fDeltaTime -= m_fCurTime - pSub->fStartTime - pSub->fExpectTime;

	// 处理第一次运行
	if( FALSE == pSub->bExecuted )
		pSub->bExecuted = TRUE;


	if( TRUE == pSub->bRequireToEnd 
		|| m_fCurTime - pSub->fStartTime >= pSub->fExpectTime)
	{
		pSub->eStats = ETS_Finished;
	}

	return 0;
}
*/


//-----------------------------------------------------------------------------
// 提供给各音乐引擎的文件操作callback函数
//-----------------------------------------------------------------------------
unsigned int Audio::myopen(const char *name)
{
//	return (unsigned int)m_pVFS->Open((LPCTSTR)name);
	return 0;
}

void Audio::myclose(unsigned int handle)
{
    // m_pVFS->Close((DWORD)handle);
}

int Audio::myread(void *buffer, int size, unsigned int handle)
{
//	return (int)m_pVFS->Read(buffer, (DWORD)size, (DWORD)handle);
	return 0;
}

int Audio::myseek(unsigned int handle, int pos, signed char mode)
{
//	return (int)m_pVFS->Seek((DWORD)handle, pos, (DWORD)mode);
	return 0;
}

int Audio::mytell(unsigned int handle)
{
//	return (int)m_pVFS->Tell((DWORD)handle);
	return 0;
}

}