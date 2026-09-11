#include "GmInterfaceGui.h"

#include "SafeQueue.h"

#include "IT_Command.h"

#include "PyInc.h"

SafeQueue<Command> GmInterfaceGui::m_requests_queue_gui;
SafeQueue<Command> GmInterfaceGui::m_results_queue_gui;

const char *GmInterfaceGui::module() { return "module"; };
PyMethodDef *GmInterfaceGui::methods() { return methods_; };

SafeQueue<Command> &GmInterfaceGui::requests_queue() { return m_requests_queue_gui; };
SafeQueue<Command> &GmInterfaceGui::results_queue() { return m_results_queue_gui; };

PyObject *GmInterfaceGui::result(PyObject *self, PyObject *args) { Py_RETURN_NONE; }
PyObject *GmInterfaceGui::cmd(PyObject *self, PyObject *args) { Py_RETURN_NONE; }

PyMethodDef GmInterfaceGui::methods_[] = {{"cmd", GmInterfaceGui::cmd, METH_VARARGS, "Pass a request to Geomec"},
                                          {"result", GmInterfaceGui::result, METH_VARARGS, "Returns the cmd result."},
                                          {NULL, NULL, 0, NULL}};