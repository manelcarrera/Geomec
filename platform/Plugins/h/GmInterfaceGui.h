#pragma once

#include "IGmInterfaceGui.h"

//#include "Command_.h"
//#include "Command.h"

class Command;

template< class T >
class SafeQueue;

#include "PyTypes.h"


class GmInterfaceGui : public IGmInterface // TODO: Find a way to differenciate it from 'IGmInterface'
{
  static SafeQueue< Command > m_requests_queue_gui;
  static SafeQueue< Command > m_results_queue_gui;

  static PyMethodDef methods_[];

public:
  virtual const char* module();
  virtual PyMethodDef* methods();

  virtual SafeQueue< Command >& requests_queue();
  virtual SafeQueue< Command >& results_queue();

  static PyObject* result( PyObject *self, PyObject *args );
  static PyObject* cmd(	PyObject *self, PyObject *args );


public:
  GmInterfaceGui(){};
  ~GmInterfaceGui(){};
};