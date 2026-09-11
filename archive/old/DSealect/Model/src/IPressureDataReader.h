#ifndef _IPRESSUREDATAREADER_H_
#define _IPRESSUREDATAREADER_H_

#include "ITVDDataReader.h"

class IPressureDataReader : public ITVDDataReader
{
public:
  static const CImportInfo::TUnitID UN_MPA = 2;
  static const CImportInfo::TUnitID UN_KPA = 3;
  static const CImportInfo::TUnitID UN_PA  = 4;
  static const CImportInfo::TUnitID UN_PSI = 5;
  static const CImportInfo::TUnitID UN_BAR = 6;

public:
  IPressureDataReader(CDataModel& model);
  virtual CString DialogCaption() const;
  virtual CString DataColumnCaption() const;
  virtual std::vector<std::pair<int, CString> > DataUnits() const;

protected:
  virtual double FromUserUnit(double dValue, CImportInfo::TUnitID unit) const;
};

#endif // _IPRESSUREDATAREADER_H_
