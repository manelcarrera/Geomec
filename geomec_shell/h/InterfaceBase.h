#ifndef _INTERFACE_BASE_H_
#define _INTERFACE_BASE_H_

#include <queue>
#include <QString>

namespace
{
  const char* cmd_cp[]={"NONE","USAGE","ABORT","QUIT","CALCULATE","END","CALCULATE_STEP","SWITCH_INTERFACE","FOR_ME"};
}

namespace gm_shell
{
  

class CArgumentParser;
class CInterfaceModelContext;
class IInterfaceBase;


class IChannel
{
  IInterfaceBase *m_master;
  IInterfaceBase *m_slave;
  
  std::string m_slaveName;
  std::vector<int> m_commands;

public:
  IChannel(IInterfaceBase *master, std::string slaveName);
  virtual ~IChannel();

  IInterfaceBase *Master();
  IInterfaceBase *Slave();

  std::string SlaveName() const;
  void RegisterSlave(IInterfaceBase *slave);

  std::vector<int>& Commands();
};


class IInterfaceBase
{
protected:
  virtual const QString AppVersion() const = 0;

public:
  typedef enum
  {
  NONE = 0,
  USAGE,
  ABORT,
  QUIT,
  CALCULATE,
  END,
  CALCULATE_STEP,
  SWITCH_INTERFACE,
  FOR_ME
  } TCommand;

  virtual ~IInterfaceBase() {}

  virtual IChannel *GetMasterChannel() = 0;
  virtual void RegisterSlaveChannel(IChannel *channel) = 0;

  virtual bool Expose() const = 0;
  virtual const char *ShortDescription() const = 0;

  virtual bool AcceptParameters(CArgumentParser& argParser, bool bForced = false) = 0;

  virtual void PrintUsage() const = 0;
  virtual int  GetErrorCode() = 0;
  
  virtual void Startup() = 0;
  virtual void Shutdown() = 0;

  virtual int  GetCommand() = 0;
  virtual void HandleLastCommand() = 0;

  virtual CInterfaceModelContext *GetModelContext() = 0;
};



class CInterfaceBase : public IInterfaceBase
{

protected:
  CArgumentParser *m_argParser;
  
  std::queue<int>  m_commands;
  int              m_error;

  bool             m_initialized;

  virtual const QString AppVersion() const;

  IChannel *m_channel;

  virtual bool IsMaster() const;
  virtual bool IsSlave() const;

public:

  CInterfaceBase();

  virtual IChannel *GetMasterChannel();
  virtual void RegisterSlaveChannel(IChannel *channel);

  virtual bool Expose() const;

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser& argParser, bool bForced = false);

  virtual void PrintUsage() const;
  virtual int  GetErrorCode();
  
  virtual void Startup();
  virtual void Shutdown();

  virtual int  GetCommand();
  virtual void HandleLastCommand();

  virtual CInterfaceModelContext *GetModelContext();

};


}


#endif