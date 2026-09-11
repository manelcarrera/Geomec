#pragma once

#include "Printer.h"

#include <mutex>

// single-process
class Printer_SP : public Printer {

public:
  Printer_SP();
  ~Printer_SP() {};

  void debug(const char *fmt, ...);
  void error(const char *fmt, ...);
  void info(const char *fmt, ...);

  mutable std::mutex m;
};
