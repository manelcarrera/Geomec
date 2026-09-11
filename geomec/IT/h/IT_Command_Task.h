#ifndef _IT_Command
#define _IT_Command

#include "IT_Command.h"
#include <tbb/tbb.h>

class GeomecPythonInterface;

void Launch_IT_Command_Task(GeomecPythonInterface *gm_iface, Command _cmd, HWND _hWnd = nullptr);

class IT_Command_Task : public tbb::task {
  Command cmd;
  GeomecPythonInterface *gm_iface;
  HWND hWnd;

  tbb::task *execute();

public:
  IT_Command_Task(GeomecPythonInterface *gm_iface_, Command _cmd, HWND hWnd_ = nullptr);
};

#endif
