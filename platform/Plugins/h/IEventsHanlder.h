#pragma once

#include "GmInterfaceGui.h"

class IEventsHandler
{
protected:
  GmInterfaceGui* gm_iface_gui;

public:
  IEventsHandler( GmInterfaceGui* gm_iface_gui_ ) : gm_iface_gui( gm_iface_gui_ ){};
  virtual void run()=0; //events loop
};

