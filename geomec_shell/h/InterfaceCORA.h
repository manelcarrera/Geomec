#ifndef _INTERFACE_CORA_H_
#define _INTERFACE_CORA_H_

#include "InterfaceBase.h"

namespace gm_shell {

class CChannelCORA : public IChannel {

public:
  CChannelCORA(IInterfaceBase *master);

  virtual void AddFailureMechanism(std::string name, int objectType, int failureType, int lsfType,
                                   std::vector<double> values) = 0;
};

class CInterfaceCORA : public CInterfaceBase {
public:
  typedef enum {
    CORA_GETMODELINFO = IInterfaceBase::FOR_ME + 1,
    CORA_RUNMODEL,
    CORA_GET_FAILURE_MODES,
    CORA_EVALUATE_LSF
  } TCoraCommand;

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser &argParser, bool bForced = false);

  virtual void PrintUsage() const;

  virtual void Startup();

  virtual void HandleLastCommand();

private:
  int GetFailureModes();
};

} // namespace gm_shell

#endif