#pragma once

#include <tbb/tbb.h>

#include "IT_Command.h"

class GmInterfaceGui;

class GuiCommandTask : public tbb::task 
{
  HWND hWnd;
  Command cmd;
  GmInterfaceGui* gm_iface;

  tbb::task* execute();

public:
  GuiCommandTask( GmInterfaceGui* gm_iface, Command _cmd, HWND hWnd_ = nullptr );
};

void Launch_GuiCommandTask(  GmInterfaceGui* gm_iface, Command _cmd, HWND _hWnd = nullptr   );
