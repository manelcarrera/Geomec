/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51OpenGLFrame_DLL)
#if defined (_WIN32) 
  #if defined(lbfw51OpenGLFrame_EXPORTS)
    #define  OPENGLFRAME_EXPORT __declspec(dllexport)
    #define  OPENGLFRAME_EXPORT_TEMPLATE
  #else
    #define  OPENGLFRAME_EXPORT __declspec(dllimport)
    #define  OPENGLFRAME_EXPORT_TEMPLATE extern
  #endif
#else
  #define OPENGLFRAME_EXPORT
  #define OPENGLFRAME_EXPORT_TEMPLATE
#endif
#else
  #define OPENGLFRAME_EXPORT
  #define OPENGLFRAME_EXPORT_TEMPLATE
#endif

