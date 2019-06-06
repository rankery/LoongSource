#include "..\Cool3D.h"
#pragma once

namespace Cool3D
{
#ifdef _DEBUG
	#define RENDER_STATUS
#endif
	
	class IFont;
	struct Cool3D_API RenderStatus  
	{
		int FPS;
		int numTexSwitch;
		int numVBSwitch;
		int numPrim;
		int numDrawCall;
		int numEffectCall;
		UINT texSize;
		UINT renderTexSize;
		UINT staticVBSize;
		UINT dynamicVBSize;
		UINT staticIBSize;
		UINT dynamicIBSize;

		IFont *pFont;

		RenderStatus();
		~RenderStatus();
	public:
		void CalFPS();
		void Clear();
		void Draw();

		int GetNumPrim(){ return numPrim;}
		int GetNumTexSwitch(){ return numTexSwitch;}
		int GetNumDrawCall(){ return numDrawCall;}
		int GetNumVBSwitch(){ return numVBSwitch;}
		int GetFPS()		{ return FPS;}
	};
}//namespace Cool3D