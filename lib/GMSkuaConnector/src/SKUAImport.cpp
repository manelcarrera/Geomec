#include "SKUAImport.h"
#include "SKUAIProgressHandler.h"
#include "SKUAParser.h"

namespace gm_skua
{

SKUAImport::SKUAImport(FILE *fp, IProgressHandler& progress)
  : m_parser(new internal::SKUAParser(fp, progress))
{
}

SKUAImport::~SKUAImport()
{
  delete m_parser;
}

int SKUAImport::LineNr() const
{
  return m_parser->LineNr();
}

int SKUAImport::ErrorNr() const
{
  return m_parser->ErrorNr();
}

const char *SKUAImport::Error(int index) const
{
  return m_parser->Error(index);
}

int SKUAImport::ObjectNr() const
{
  return m_parser->ObjectNr();
}

const SKUAParseData *SKUAImport::Object(int index) const
{
  return m_parser->Object(index);
}

bool SKUAImport::Import()
{
  return m_parser->Parse();
}

}
