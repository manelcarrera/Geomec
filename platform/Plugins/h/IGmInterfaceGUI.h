#pragma once

#include "IGmInterface.h"
#include "PyTypes.h"

template <class T> class SafeQueue;

class ICommand_;

class IGmInterfaceGui : public IGmInterface // TODO: Find a way to differenciate it from 'IGmInterface'
{
public:
  virtual const char *module() = 0;
  virtual PyMethodDef *methods() = 0;

  // virtual SafeQueue< ICommand_ >& requests_queue()=0;
  // virtual SafeQueue< ICommand_ >& results_queue()=0;

  // virtual PyObject* get_result( PyObject *self, PyObject *args )=0; -> STATIC
  // virtual PyObject* send_cmd(	PyObject *self, PyObject *args )=0; -> STATIC

public:
  IGmInterfaceGui() {};
  ~IGmInterfaceGui() {};
};
