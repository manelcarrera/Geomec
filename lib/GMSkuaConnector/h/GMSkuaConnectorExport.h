#pragma once

#ifdef WIN32
#ifdef GMSKUACONNECTOR_EXPORT
#define INTERFACE_GMSKUACONNECTOR __declspec(dllexport)
#else
#define INTERFACE_GMSKUACONNECTOR __declspec(dllimport)
#endif
#else
#define INTERFACE_GMSKUACONNECTOR
#endif
