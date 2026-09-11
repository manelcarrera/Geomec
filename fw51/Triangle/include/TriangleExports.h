/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51Triangle_DLL)
#if defined(_WIN32)
#if defined(lbfw51Triangle_EXPORTS)
#define TRIANGLE_EXPORT __declspec(dllexport)
#define TRIANGLE_EXPORT_TEMPLATE
#else
#define TRIANGLE_EXPORT __declspec(dllimport)
#define TRIANGLE_EXPORT_TEMPLATE extern
#endif
#else
#define TRIANGLE_EXPORT
#define TRIANGLE_EXPORT_TEMPLATE
#endif
#else
#define TRIANGLE_EXPORT
#define TRIANGLE_EXPORT_TEMPLATE
#endif
