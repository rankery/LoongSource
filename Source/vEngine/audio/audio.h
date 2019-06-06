//-----------------------------------------------------------------------------
// File: audio
// Desc: interface for audio
// Auth: Lyp
// Date: 2004/02/09
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

//-----------------------------------------------------------------------------
/**	Audio引擎接口 设计观念
	堆栈模式的播放: play带push功能(只对Channel不对曲子,具体解释如下)
 1.	当新播放的Channel和当前播放的Channel不同时,停止当前播放的Channel,压栈,
	并播放新的Channel
 2.	当新播放的Channel和当前播放的Channel相同时,直接用新曲取代旧曲,无压栈操作
 3.	PopStream停止播放当前Channel,恢复堆栈中上一Channel的播放

	结构设计思想

	Audio -> audio_port -> fmod_port(or other audi engine)

	将复杂逻辑集中在上层,也就是CAudio,将引擎功能接口audio_port最简化,
	以便于与各种Audio Engine做接口

	engine: "fmod"
			"miles"
			"xaudio"
*/
//-----------------------------------------------------------------------------

CONST INT GT_MAX_STREAM_CHANNEL	= 8;
class  AudioPort;
class VENGINE_API Audio
{
public:
	BOOL Init(LPCTSTR szEngineName);
	VOID Destroy();

	// 对于music(mod/s3m/xm/it/mid/rmi/sgt...)的操作
	BOOL PlayMusic(INT nChannel, LPCTSTR szName, BOOL bLoop=TRUE);
	BOOL StopMusic(INT nChannel);
	BOOL PauseMusic(INT nChannel, BOOL bPause);
	
	VOID Update(FLOAT fDeltaTime);
	
	Audio();
	virtual ~Audio();

private:
	TObjRef<Util>	m_pUtil;
	AudioPort*		m_pAudio;	// mutex factory

	BOOL m_bFadein;
	BOOL m_bFadeout;
	INT	 m_nCurrentVirtualChannel;

	std::vector<DWORD> m_vecMusicHandle;
	
	FLOAT m_fDeltaTime;
	FLOAT m_fCurTime;

	// 对于虚拟文件系统的支持
	TObjRef<VirtualFileSys> m_pVFS;
	static UINT myopen(const char *name);
	static VOID myclose(unsigned int handle);
	static INT  myread(void *buffer, int size, unsigned int handle);
	static INT  myseek(unsigned int handle, int pos, signed char mode);
	static INT  mytell(unsigned int handle);
};


}	// namespace vEngine{
