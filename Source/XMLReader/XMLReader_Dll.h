#pragma once

#ifdef XMLREADER_EXPORTS
#define XMLREADER_API __declspec(dllexport)
#else
#define XMLREADER_API __declspec(dllimport)
#endif
