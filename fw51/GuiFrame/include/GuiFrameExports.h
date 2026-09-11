/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51GuiFrame_DLL)
#if defined (_WIN32) 
  #if defined(lbfw51GuiFrame_EXPORTS)
    #define  GUIFRAME_EXPORT __declspec(dllexport)
    #define  GUIFRAME_EXPORT_TEMPLATE
  #else
    #define  GUIFRAME_EXPORT __declspec(dllimport)
    #define  GUIFRAME_EXPORT_TEMPLATE extern
  #endif
#else
  #define GUIFRAME_EXPORT
  #define GUIFRAME_EXPORT_TEMPLATE
#endif
#else
  #define GUIFRAME_EXPORT
  #define GUIFRAME_EXPORT_TEMPLATE
#endif

