#pragma once

#include "Printer.h"

class QSystemSemaphore;

// multi-process
class Printer_MP : public Printer
{
  QSystemSemaphore* m_sem;

public:
  
  Printer_MP();
  ~Printer_MP();

  void debug( const char* fmt, ... );
  void error( const char* fmt, ... );
  void info( const char* fmt, ... );
};

