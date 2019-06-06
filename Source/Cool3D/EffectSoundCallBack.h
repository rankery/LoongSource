#pragma once

#include "Cool3D.h"
#include "math\math.h"

using namespace Cool3D;

class Cool3D_API EffectSoundCallBack
{
public:
	EffectSoundCallBack(void)	{}
	virtual ~EffectSoundCallBack(void)	{}

	virtual int  Create3DEventSound(const TCHAR* szFileName,bool bLoop,float minDist,float maxDist) =0;
	virtual void Destroy3DEventSound(int id) =0;
	virtual void Play3DEventSound(int id) =0;
	virtual void Stop3DEventSound(int id) =0;
	virtual void Set3DEventSoundAtt(int id,Vector3 pos,Vector3 vel) =0;
};
