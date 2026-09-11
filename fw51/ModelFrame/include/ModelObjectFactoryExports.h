 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined (_WIN32)
  #if defined(lbfw51ModelObjectFactory_EXPORTS)
  #define  MODELOBJFACTORY_EXPORT __declspec(dllexport)
  #else
  #define  MODELOBJFACTORY_EXPORT //__declspec(dllimport)
  #endif
#else
  #define MODELOBJFACTORY_EXPORT
#endif
