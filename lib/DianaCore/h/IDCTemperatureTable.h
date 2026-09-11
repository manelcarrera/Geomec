// TemperatureTable.h: interface for the CTemperatureTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPERATURETABLE_H__A78BB4B2_B51B_4245_B48B_61B357E4123B__INCLUDED_)
#define AFX_TEMPERATURETABLE_H__A78BB4B2_B51B_4245_B48B_61B357E4123B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCValueTable.h"

namespace dia {

class ITemperatureTable : public IValueTable
{
public:
  ITemperatureTable(const IDianaRunner &runner);

protected:
  virtual std::string TableName() const;
  virtual std::string TimeTableName() const;
};

} // namespace dia

#endif // !defined(AFX_TEMPERATURETABLE_H__A78BB4B2_B51B_4245_B48B_61B357E4123B__INCLUDED_)
