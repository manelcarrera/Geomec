// TemperatureTable.cpp: implementation of the CTemperatureTable class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCTemperatureTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

ITemperatureTable::ITemperatureTable(const IDianaRunner &runner)
: IValueTable(runner)
{
}

std::string ITemperatureTable::TableName() const
{
  return "TEMPER";
}

std::string ITemperatureTable::TimeTableName() const
{
  return "TTIMPT";
}

} // namespace dia
