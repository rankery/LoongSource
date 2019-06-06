#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

class EditorCamera :
	public CameraEuler
{
public:
	EditorCamera(void);
	virtual ~EditorCamera(void);
};
