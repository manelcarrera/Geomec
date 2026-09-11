#ifndef _DENSITYDATAREADER_H_
#define _DENSITYDATAREADER_H_

#include "ITVDDataReader.h"

class CDensityDataReader : public ITVDDataReader
{
public:
  static const CImportInfo::TUnitID UN_KGM3  = 2; // kg/m3
  static const CImportInfo::TUnitID UN_GM3   = 3; // g/m3
  static const CImportInfo::TUnitID UN_KGCM3 = 4; // kg/cm3
  static const CImportInfo::TUnitID UN_GCM3  = 5; // g/cm3
  static const CImportInfo::TUnitID UN_LBGAL = 6; // lb/gal
  static const CImportInfo::TUnitID UN_LBFT3 = 7; // lb/ft3

public:
  CDensityDataReader(CDataModel& mode, CValueProfile& profile);
  virtual CString DialogCaption() const;
  virtual CString DataColumnCaption() const;
  virtual std::vector<std::pair<int, CString> > DataUnits() const;

protected:
  virtual bool OnImportData();
  virtual double FromUserUnit(double dValue, CImportInfo::TUnitID unit) const;

private:
  CValueProfile& m_profile;
};

#endif // _DENSITYDATAREADER_H_
