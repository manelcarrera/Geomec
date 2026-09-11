#include "stdafx.h"
#include "dsealect.h"

#include "ITemperatureDataReader.h"

ITemperatureDataReader::ITemperatureDataReader(CDataModel& model)
: ITVDDataReader(model)
{
  AddTag(TAG_DATA, "temp");

  AddUnit(UN_C, "c");
  AddUnit(UN_C, "deg");
  AddUnit(UN_K, "k");
  AddUnit(UN_F, "f");
}

CString ITemperatureDataReader::DialogCaption() const
{
  return "Import Temperature";
}

CString ITemperatureDataReader::DataColumnCaption() const
{
  return "Temperature";
}

std::vector<std::pair<int, CString> > ITemperatureDataReader::DataUnits() const
{
  std::vector<std::pair<int, CString> > vcUnits;
  vcUnits.push_back(std::make_pair(UN_C, "C"));
  vcUnits.push_back(std::make_pair(UN_K, "K"));
  vcUnits.push_back(std::make_pair(UN_F, "F"));

  return vcUnits;
}

double ITemperatureDataReader::FromUserUnit(double dValue, CImportInfo::TUnitID unit) const
{
  switch(unit)
  {
  case UN_C:
    return dValue;
  case UN_K:
    return dValue - 273.15;
  case UN_F:
    return (dValue - 32.) * 5. / 9.;
  }

  return ITVDDataReader::FromUserUnit(dValue, unit);
}
