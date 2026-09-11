/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51Geometry_DLL)
#if defined(_WIN32)
#if defined(lbfw51Geometry_EXPORTS)
#define GEOMETRY_EXPORT __declspec(dllexport)
#define GEOMETRY_EXPORT_TEMPLATE
#else
#define GEOMETRY_EXPORT __declspec(dllimport)
#define GEOMETRY_EXPORT_TEMPLATE extern
#endif
#else
#define GEOMETRY_EXPORT
#define GEOMETRY_EXPORT_TEMPLATE
#endif
#else
#define GEOMETRY_EXPORT
#define GEOMETRY_EXPORT_TEMPLATE
#endif
