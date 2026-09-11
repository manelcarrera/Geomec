#include "IGlobalMessage.h"


IGlobalMessage::IGlobalMessageObserver::IGlobalMessageObserver()
{
}

IGlobalMessage::IGlobalMessageObserver::~IGlobalMessageObserver()
{
}

void IGlobalMessage::IGlobalMessageObserver::notifyWarning(const std::string& message)
{
}

void IGlobalMessage::IGlobalMessageObserver::notifyError(const std::string& message)
{
}


void IGlobalMessage::notifyWarning(const std::string& message)
{
  for (std::vector<IGlobalMessageObserver *>::iterator it = m_observers.begin(); it != m_observers.end(); ++it)
  (*it)->notifyWarning(message);
}

void IGlobalMessage::notifyError(const std::string& message)
{
  for (std::vector<IGlobalMessageObserver *>::iterator it = m_observers.begin(); it != m_observers.end(); ++it)
  (*it)->notifyError(message);
}

void IGlobalMessage::AttachObserver(IGlobalMessageObserver *observer)
{
  m_observers.push_back(observer);
}

void IGlobalMessage::DetachObserver(IGlobalMessageObserver *observer)
{
  std::vector<IGlobalMessageObserver *>::iterator it = std::find(m_observers.begin(), m_observers.end(), observer);
  if (it != m_observers.end())
  m_observers.erase(it);
}


int IGlobalMessage::msg(const QString& message, unsigned int style, unsigned int contextID)
{
  return msg(message.toStdString().c_str(), style, contextID);
}

int IGlobalMessage::msg(unsigned int message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);
  return msg(newMessage.c_str(), style, contextID);
}

int IGlobalMessage::msg(const std::vector <std::string>& message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);

  return msg(newMessage.c_str(), style, contextID);
}


int IGlobalMessage::warn(const QString& message, unsigned int style, unsigned int contextID)
{
  notifyWarning(message.toStdString());
  return msg(message, style, contextID);
}

int IGlobalMessage::warn(const char* message, unsigned int style, unsigned int contextID)
{
  notifyWarning(message);
  return msg(message, style, contextID);
}

int IGlobalMessage::warn(unsigned int message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);
  notifyWarning(newMessage);
  return msg(newMessage.c_str(), style, contextID);
}

int IGlobalMessage::warn(const std::vector <std::string>& message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);
  notifyWarning(newMessage);
  return msg(newMessage.c_str(), style, contextID);
}

int IGlobalMessage::error(const QString& message, unsigned int style, unsigned int contextID)
{
  notifyError(message.toStdString());
  return msg(message, style, contextID);
}

int IGlobalMessage::error(const char* message, unsigned int style, unsigned int contextID)
{
  notifyError(message);
  return msg(message, style, contextID);
}

int IGlobalMessage::error(unsigned int message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);
  notifyError(newMessage);
  return msg(message, style, contextID);
}

int IGlobalMessage::error(const std::vector <std::string>& message, unsigned int style, unsigned int contextID)
{
  std::string newMessage = constructMessage(message);
  notifyError(newMessage);
  return msg(newMessage.c_str(), style, contextID);
}

std::string IGlobalMessage::constructMessage(
  const std::vector <std::string>& message)
{
  std::string newMessage;

  for (std::vector <std::string>::const_iterator begin = message.begin(),
  end = message.end();
  begin != end;)
  {
  newMessage += *begin;
  ++begin;

  if (begin != end)
      newMessage += '\n';
  }

  return newMessage;
}

std::string IGlobalMessage::constructMessage(unsigned int message)
{
  return std::string();
}
