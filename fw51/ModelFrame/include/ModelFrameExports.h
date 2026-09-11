/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51ModelFrame_DLL)
#if defined(_WIN32)
#if defined(lbfw51ModelFrame_EXPORTS)
#define MODELFRAME_EXPORT __declspec(dllexport)
#define MODELFRAME_EXPORT_TEMPLATE
#else
#define MODELFRAME_EXPORT __declspec(dllimport)
#define MODELFRAME_EXPORT_TEMPLATE extern
#endif
#else
#define MODELFRAME_EXPORT
#define MODELFRAME_EXPORT_TEMPLATE
#endif
#else
#define MODELFRAME_EXPORT
#define MODELFRAME_EXPORT_TEMPLATE
#endif
