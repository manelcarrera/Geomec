#include "InterfaceBase.h"
#include "geomec_shell.h"



namespace gm_shell
{


IChannel::IChannel(IInterfaceBase *master, std::string slaveName)
  : m_master(master)
  , m_slave(0)
  , m_slaveName(slaveName)
{
}

IChannel::~IChannel()
{
}

IInterfaceBase *IChannel::Master()
{
  return m_master;
}

IInterfaceBase *IChannel::Slave()
{
  return m_slave;
}

std::string IChannel::SlaveName() const
{
  return m_slaveName;
}

void IChannel::RegisterSlave(IInterfaceBase *slave)
{
  m_slave = slave;
}

std::vector<int>& IChannel::Commands()
{
  return m_commands;
}



CInterfaceBase::CInterfaceBase()
  : m_argParser(0)
  , m_error(0)
  , m_initialized(false)
  , m_channel(0)
{
}

const QString CInterfaceBase::AppVersion() const
{
  return APPLICATION + " " + VERSION;
}

bool CInterfaceBase::IsMaster() const
{
  return m_channel && m_channel->Master() == this;
}

bool CInterfaceBase::IsSlave() const
{
  return m_channel && m_channel->Slave() == this;
}

IChannel *CInterfaceBase::GetMasterChannel()
{
  return m_channel && m_channel->Master() == this ? m_channel : 0;
}

void CInterfaceBase::RegisterSlaveChannel(IChannel *channel)
{
  if (channel)
  {
    m_channel = channel;
    m_channel->RegisterSlave(this);

    for (size_t i = 0; i < m_channel->Commands().size(); ++i)
      m_commands.push(m_channel->Commands()[i]);

    m_commands.push(QUIT);
  }
  else if (IsSlave())
  {
    m_channel = 0;
  }
}

bool CInterfaceBase::Expose() const
{
  return true;
}

const char *CInterfaceBase::ShortDescription() const
{
  return "";
}

bool CInterfaceBase::AcceptParameters(CArgumentParser& /*argParser*/,
  bool bForced)
{
  return bForced;
}

void CInterfaceBase::PrintUsage() const
{
}

int  CInterfaceBase::GetErrorCode()
{
  return m_error;
}

void CInterfaceBase::Startup()
{
  if (IsSlave())
  {
    for (size_t i = 0; i < m_channel->Commands().size(); ++i)
      m_commands.push(m_channel->Commands()[i]);
  }
}

void CInterfaceBase::Shutdown()
{
}

int CInterfaceBase::GetCommand()
{
  int command = IInterfaceBase::QUIT;

  if (!m_commands.empty())
  {
    int tryCommand = m_commands.front();

    if (tryCommand < IInterfaceBase::FOR_ME)
    {
      m_commands.pop();
      command = tryCommand;
    }
    else
    {
      command = IInterfaceBase::FOR_ME;
    }

  }

  return command;
}

void CInterfaceBase::HandleLastCommand()
{
}


CInterfaceModelContext *CInterfaceBase::GetModelContext()
{
  return 0;
}


}
