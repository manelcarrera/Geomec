#pragma once

#include <vector>

#include <QString>

#include "IGlobalMessage.h" // MB_OK

class GlobalMessage_GUI_MFC : virtual public IGlobalMessage
{
public:

  GlobalMessage_GUI_MFC(){ m_type = GUI_MFC; };

  virtual int msg(	
    const QString& message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual int msg(	
    const char* message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual int msg(	
    unsigned int message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual int msg(	
    const std::vector <std::string>& message,
    unsigned int style = MB_OK, 
    unsigned int contextID = 0);

  void status( const QString& message );
};