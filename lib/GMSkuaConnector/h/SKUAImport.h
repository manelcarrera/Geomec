#pragma once

#include "GMSkuaConnectorExport.h"

#include <cstdio>


namespace gm_skua
{

class IProgressHandler;
struct SKUAParseData;

namespace internal
{
class SKUAParser;
}

class INTERFACE_GMSKUACONNECTOR SKUAImport
{
public:
  SKUAImport(FILE *fp, IProgressHandler& progress);
  ~SKUAImport();

  int LineNr() const;

  int ErrorNr() const;
  const char *Error(int index) const;

  int ObjectNr() const;
  const SKUAParseData *Object(int index) const;

  bool Import();

private:
  internal::SKUAParser *m_parser;
};

}

