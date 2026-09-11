#ifndef _INTERFACE_GM_H_
#define _INTERFACE_GM_H_

#include "InterfaceBase.h"
#include <vector>

class CModelDiagnostics;

namespace gm_shell
{

class CInterfaceGM : public CInterfaceBase
{
  QString m_model;
  QString m_type;
  QString m_import;
  CInterfaceModelContext *m_pModelContext;
  std::vector<int>        m_indices;

public:
  typedef enum
  {
    GM_LIST = IInterfaceBase::FOR_ME + 1,
    GM_RUN,
    GM_SAVE,
    GM_IMPORT
  } TGMCommand;

  virtual bool Expose() const;

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser& argParser, bool bForced = false);

  virtual void PrintUsage() const;

  virtual void Startup();

  virtual void HandleLastCommand();

  virtual CInterfaceModelContext *GetModelContext();
};


}


#endif