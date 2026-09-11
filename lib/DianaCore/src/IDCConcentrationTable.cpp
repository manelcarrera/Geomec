// TemperatureTable.cpp: implementation of the CTemperatureTable class.
//
//////////////////////////////////////////////////////////////////////
#include "IDCConcentrationTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

IConcentrationTable::IConcentrationTable(const IDianaRunner &runner) : IValueTable(runner) {}

std::string IConcentrationTable::TableName() const { return "CONCEN"; }

std::string IConcentrationTable::TimeTableName() const { return "CTIMPT"; }

} // namespace dia
