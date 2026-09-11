#ifndef _ITEMPERATUREDATAREADER_H_
#define _ITEMPERATUREDATAREADER_H_

#include "ITVDDataReader.h"

class ITemperatureDataReader : public ITVDDataReader
{
public:
  static const CImportInfo::TUnitID UN_C = 2;
  static const CImportInfo::TUnitID UN_K = 3;
  static const CImportInfo::TUnitID UN_F = 4;

public:
  ITemperatureDataReader(CDataModel& model);
  virtual CString DialogCaption() const;
  virtual CString DataColumnCaption() const;
  virtual std::vector<std::pair<int, CString> > DataUnits() const;

protected:
  virtual double FromUserUnit(double dValue, CImportInfo::TUnitID unit) const;
};

#endif // _ITEMPERATUREDATAREADER_H_
