#pragma once

class IGmInterface;

#include "IPyInterface.h"

class PyInterface : public IPyInterface
{
public:

  PyInterface(){};
  ~PyInterface(){}
  virtual int launch( std::string name, IGmInterface* gm_iface );
  virtual void finalize();
};