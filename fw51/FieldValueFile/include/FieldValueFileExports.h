/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51FieldValueFile_DLL)
#if defined (_WIN32) 
  #if defined(lbfw51FieldValueFile_EXPORTS)
  #define  FIELDVALUEFILE_EXPORT __declspec(dllexport)
  #define  FIELDVALUEFILE_EXPORT_TEMPLATE
  #else
  #define  FIELDVALUEFILE_EXPORT __declspec(dllimport)
  #define  FIELDVALUEFILE_EXPORT_TEMPLATE extern
  #endif
#else
  #define FIELDVALUEFILE_EXPORT
  #define FIELDVALUEFILE_EXPORT_TEMPLATE
#endif
#else
  #define FIELDVALUEFILE_EXPORT
  #define FIELDVALUEFILE_EXPORT_TEMPLATE
#endif

