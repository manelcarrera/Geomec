/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51TetMesh_DLL)
#if defined (_WIN32) 
  #if defined(lbfw51TetMesh_EXPORTS)
  #define  TETMESH_EXPORT __declspec(dllexport)
  #define  TETMESH_EXPORT_TEMPLATE
  #else
  #define  TETMESH_EXPORT __declspec(dllimport)
  #define  TETMESH_EXPORT_TEMPLATE extern
  #endif
#else
  #define TETMESH_EXPORT
  #define TETMESH_EXPORT_TEMPLATE
#endif
#else
  #define TETMESH_EXPORT
  #define TETMESH_EXPORT_TEMPLATE
#endif

