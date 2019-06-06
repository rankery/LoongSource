#pragma once

#ifdef F3DMAXEXP_EXPORTS
#define F3D_MAX_API __declspec(dllexport)
#else
#define F3D_MAX_API __declspec(dllimport)
#endif