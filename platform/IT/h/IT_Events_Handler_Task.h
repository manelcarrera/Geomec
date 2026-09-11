#pragma once

#include <tbb/tbb.h>

class GeomecPythonInterface;

class IT_Events_Handler_Task : public tbb::task {
  HWND hWnd;
  GeomecPythonInterface *gm_iface;
  tbb::task *execute();

public:
  IT_Events_Handler_Task(GeomecPythonInterface *gm_iface_, HWND hWnd_ = nullptr);
};

void Launch_IT_Events_Handler_Task(GeomecPythonInterface *gm_iface, HWND hWnd = nullptr);