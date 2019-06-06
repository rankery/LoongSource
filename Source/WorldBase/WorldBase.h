#pragma once

#ifdef WORLD_BASE_EXPORTS
#define WORLD_BASE_API __declspec(dllexport)
#else
#define WORLD_BASE_API __declspec(dllimport)
#endif
