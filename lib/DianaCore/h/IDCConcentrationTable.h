#ifndef _IDCCONCENTRATIONTABLE_H_
#define _IDCCONCENTRATIONTABLE_H_

#include "IDCValueTable.h"

namespace dia {

class IConcentrationTable : public IValueTable
{
public:
  IConcentrationTable(const IDianaRunner &runner);

protected:
  virtual std::string TableName() const;
  virtual std::string TimeTableName() const;
};

} // namespace dia

#endif // _IDCCONCENTRATIONTABLE_H_
