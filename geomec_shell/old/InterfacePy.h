#ifndef _InterfacePy_H_
#define _InterfacePy_H_

#include "InterfaceBase.h"


namespace gm_shell
{


class CInterfacePy : public CInterfaceBase
{
public:
  typedef enum
  {
    PY_SCRIPT = IInterfaceBase::FOR_ME + 1,
  } TPyCommand;

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser& argParser, bool bForced = false);

  virtual void PrintUsage() const;
  
  virtual void Startup();

  virtual void HandleLastCommand();

};


}


#endif