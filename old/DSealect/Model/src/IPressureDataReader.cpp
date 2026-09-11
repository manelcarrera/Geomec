#include "stdafx.h"
#include "dsealect.h"

#include "IPressureDataReader.h"

IPressureDataReader::IPressureDataReader(CDataModel& model)
: ITVDDataReader(model)
{
  AddTag(TAG_DATA, "pressure");

  AddUnit(UN_MPA, "mpa");
  AddUnit(UN_KPA, "kpa");
  AddUnit(UN_PA,  "pa");
  AddUnit(UN_PSI, "psi");
  AddUnit(UN_BAR, "bar");
}

CString IPressureDataReader::DialogCaption() const
{
  return "Import Pressure";
}

CString IPressureDataReader::DataColumnCaption() const
{
  return "Pressure";
}

std::vector<std::pair<int, CString> > IPressureDataReader::DataUnits() const
{
  std::vector<std::pair<int, CString> > vcUnits;
  vcUnits.push_back(std::make_pair(UN_MPA, "MPa"));
  vcUnits.push_back(std::make_pair(UN_KPA, "kPa"));
  vcUnits.push_back(std::make_pair(UN_PA,  "Pa"));
  vcUnits.push_back(std::make_pair(UN_PSI, "psi"));
  vcUnits.push_back(std::make_pair(UN_BAR, "bar"));

  return vcUnits;
}

double IPressureDataReader::FromUserUnit(double dValue, CImportInfo::TUnitID unit) const
{
  switch(unit)
  {
  case UN_MPA:
    return dValue;
  case UN_KPA:
    return dValue * 1e-3;
  case UN_PA:
    return dValue * 1e-6;
  case UN_PSI:
    return dValue / 145.038;
  case UN_BAR:
    return dValue * 1e-1;
  }

  return ITVDDataReader::FromUserUnit(dValue, unit);
}
