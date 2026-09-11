#ifndef _INTERFACE_RGI_H_
#define _INTERFACE_RGI_H_

#include "InterfaceBase.h"
#include "IGlobalMessage.h"

#include <QString>


namespace GeomecRGI
{
class CRockMechProcessor;
class CBuildTensor;
}
class RGSync;
class RGInterface;
class GMCommand;


namespace gm_shell
{

class CInterfaceModelContext;

class CRGIGlobalMessageObserver : public IGlobalMessage::IGlobalMessageObserver
{
  GeomecRGI::CRockMechProcessor *m_pRMP;
  RGInterface        *m_pRgi;
public:
  CRGIGlobalMessageObserver(GeomecRGI::CRockMechProcessor *RMP);
  virtual ~CRGIGlobalMessageObserver();
  void setRGInterface(RGInterface *pRgi);
  virtual void notifyWarning(const std::string& message);
  virtual void notifyError(const std::string& message);
};

class CInterfaceRGI : public CInterfaceBase
{

  struct ParsedData
  {
  QString sModelFile;
  QString sRockMechFile;
  QString sOutputDir;
  QString sLog;
  QString sTempDir;
  } parsedData;

  CInterfaceModelContext *m_pModelContext;

  GeomecRGI::CRockMechProcessor *m_pRockMechProcessor;
  RGSync *m_pSync;
  RGInterface *m_pRgi;
  GeomecRGI::CBuildTensor *m_pBuildTensor;

  bool m_bSkipShutdown;
  bool m_bSaveModel;
  bool m_bQuit;
  bool m_bHibernate;
  bool m_bHibernating;
  int  m_nCommands;
  int  m_nCurrentCommand;
  bool m_bHaveCalculation;

  CRGIGlobalMessageObserver *m_pGlobalMessageObserver;

public:
  typedef enum
  {
  RGI_SETUP = IInterfaceBase::FOR_ME + 1,
  RGI_SHUTDOWN,
  /* We add RGI_LOAD_GRID to GMCommand::GetType() so keep these in consecutive order */
  RGI_LOAD_GRID,
  RGI_SAVE_GRID,
  RGI_LOAD_PROPERTY,
  RGI_SAVE_PROPERTY,
  RGI_CALCULATE,
  RGI_DEFINE_INITIAL_DATE,
  RGI_QUIT,
  RGI_HIBERNATE,
  RGI_LIST_AVAILABLE_FAILURE_MECHANISMS,
  RGI_EVALUATE_LIMIT_STATE_FUNCTIONS,
  RGI_LIST_MONITORABLE_PROPERTIES,
  RGI_LOAD_MONITOR_POINTSETS,
  RGI_EVALUATE_MONITOR_VALUES,
  /* From here we can define our own again in any order you see fit */
  RGI_LIST_AVAILABLE_FAILURE_MECHANISMS_RETURN,
  RGI_EVALUATE_LIMIT_STATE_FUNCTIONS_RETURN,
  RGI_CALCULATE_END
  } TRgiCommand;

  CInterfaceRGI();

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser& argParser, bool bForced = false);

  virtual void PrintUsage() const;
  
  virtual void Startup();

  virtual void Shutdown();

  virtual void HandleLastCommand();

  virtual CInterfaceModelContext *GetModelContext();

private:

  void RGISetup();
  void RGIShutdown();

  void LogCommand(GMCommand& cmd, int command);

};


}


#endif