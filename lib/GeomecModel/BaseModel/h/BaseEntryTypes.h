#if !defined(__SUPPORTED_BASE_MODEL_ENTRY_TYPE_)
#define __SUPPORTED_BASE_MODEL_ENTRY_TYPE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Base model space 10000 - 20000

// Geology
#define MD_BASE_HORIZON 12000
#define MD_BASE_FORMATION 12100
#define MD_BASE_DEPLETION_STAGE 12200
#define MD_BASE_POINTSET 12400 // Supported by DataSource view
#define MD_BASE_SURFACE 12500  // Supported by Model view
#define MD_BASE_ANALYSIS_POINT 12600
#define MB_GEO_MESH 2103 // Supported by Model view

#define MD_ROCK_MATERIAL 2106 // Supported by Model view
#define MD_WELLCASING_STEELMATERIAL_ENTRY 2109
#define MD_BASE_INTERFACEMATERIAL 2110

#define MD_BASE_XSECTION 2111
#define MD_BASE_WELLPATH 2112
#define MD_NEW_WELLPATH 2113 // wjrx mantis 3185

// #define MD_BASE_FILE_RESULT		2113
// #define MD_BASE_REGISTRY_RESULT 2114

#define MD_BASE_MESH_RESULT 2115
#define MD_BASE_MATERIAL_RESULT 2116

#define MD_BASE_HOMO_BOX 2120

#define MD_BASE_UPSCALING 2140
#define MD_BASE_ZOOMIN_MODEL 2150

#define MD_BASE_EXPORT_MACROS 2160

#define MD_BASE_NONMESHEDSURFACE 2170

#define MD_BASE_GROUPNODE 2180

#endif // !defined(__SUPPORTED_BASE_MODEL_ENTRY_TYPE_)
