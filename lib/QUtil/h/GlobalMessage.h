#pragma once

#include "IGlobalMessage.h"

class GlobalMessage
{
  static IGlobalMessage* m_instance;
  static IGlobalMessage* m_instance_mfc;

public:
  static IGlobalMessage* instance();

  static void init_mfc( IGlobalMessage* val );
  static void init( IGlobalMessage* val );
  static void cleanup();

private:
  GlobalMessage();
};

namespace
{
  auto _m = []() { return GlobalMessage::instance(); };
}

class GlobalMessage_Empty : virtual public IGlobalMessage
{
public:

  virtual int msg(	
    const QString& message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0){ return 0; };

  virtual int msg(	
    const char* message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0){ return 0; };

  virtual int msg(	
    unsigned int message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0){ return 0; };

  virtual int msg(	
    const std::vector <std::string>& message,
    unsigned int style = MB_OK, 
    unsigned int contextID = 0){ return 0; };

  virtual void status( const QString& message ){};
};