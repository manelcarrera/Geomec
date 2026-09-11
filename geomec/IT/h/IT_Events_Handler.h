#ifndef _IT_Events_Handler
#define _IT_Events_Handler

//#include "IEventsHanlder.h"
class GeomecPythonInterface;

class IT_Events_Handler //: public IEventsHandler
{
  GeomecPythonInterface* gm_iface;

public:
  IT_Events_Handler( GeomecPythonInterface* gm_iface_ );
  void run(); //events loop
};


#endif