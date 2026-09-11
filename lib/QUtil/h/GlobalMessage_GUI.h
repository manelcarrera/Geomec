#pragma once

#include <vector>

#include <QString>

#include "IGlobalMessage.h" // MB_OK

class GlobalMessage_GUI : public IGlobalMessage {
public:
  struct GMData {
    const std::string message;
    unsigned int style;
    unsigned int contextID;
  };

  GlobalMessage_GUI() { m_type = GUI; };

  virtual int msg(const char *message, unsigned int style = MB_OK, unsigned int contextID = 0);

  virtual int msg(unsigned int message, unsigned int style = MB_OK, unsigned int contextID = 0);

  void status(const QString &message);
};