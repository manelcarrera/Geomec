#ifndef _Py_Tasks
#define _Py_Tasks

#include <tbb/tbb.h>

class IGmInterface;
class IPyInterface;

class PyTask : public tbb::task {
protected:
  HWND hWnd;
  std::string name;
  IGmInterface *gm_iface;
  IPyInterface *py_iface;
  virtual tbb::task *execute();

public:
  PyTask(std::string name_, IGmInterface *gm_iface_, IPyInterface *py_iface_, HWND hWnd_ = nullptr)
      : name(name_), gm_iface(gm_iface_), py_iface(py_iface_), hWnd(hWnd_) {}
};

void Launch_Py_Task(std::string name, IGmInterface *gm_iface, IPyInterface *py_iface, HWND hWnd = nullptr);

#endif
