#include "stdafx.h"
#include "dsealect.h"

#include "DensityDataReader.h"
#include "CDataModel.h"

CDensityDataReader::CDensityDataReader(CDataModel& model, CValueProfile& profile)
: ITVDDataReader(model),
  m_profile(profile)
{
  AddTag(TAG_DATA, "density");

  AddUnit(UN_KGM3,  "kg/m3");
  AddUnit(UN_KGM3,  "kgm-3");
  AddUnit(UN_KGM3,  "kg.m-3");
  AddUnit(UN_GM3,   "g/m3");
  AddUnit(UN_GM3,   "gm-3");
  AddUnit(UN_GM3,   "g.m-3");
  AddUnit(UN_KGCM3, "kg/cm3");
  AddUnit(UN_KGCM3, "kgcm-3");
  AddUnit(UN_KGCM3, "kg.cm-3");
  AddUnit(UN_GCM3,  "g/cm3");
  AddUnit(UN_GCM3,  "gcm-3");
  AddUnit(UN_GCM3,  "g.cm-3");
  AddUnit(UN_LBGAL, "lb/gal");
  AddUnit(UN_LBGAL, "lbgal-1");
  AddUnit(UN_LBGAL, "lb.gal-1");
  AddUnit(UN_LBFT3, "lb/ft3");
  AddUnit(UN_LBFT3, "lbft-3");
  AddUnit(UN_LBFT3, "lb.ft-3");
}

CString CDensityDataReader::DialogCaption() const
{
  return "Import Density";
}

CString CDensityDataReader::DataColumnCaption() const
{
  return "Density";
}

std::vector<std::pair<int, CString> > CDensityDataReader::DataUnits() const
{
  std::vector<std::pair<int, CString> > vcUnits;
  vcUnits.push_back(std::make_pair(UN_KGM3,  "kg/m3"));
  vcUnits.push_back(std::make_pair(UN_GM3,   "g/m3"));
  vcUnits.push_back(std::make_pair(UN_KGCM3, "kg/cm3"));
  vcUnits.push_back(std::make_pair(UN_GCM3,  "g/cm3"));
  vcUnits.push_back(std::make_pair(UN_LBGAL, "lb/gal"));
  vcUnits.push_back(std::make_pair(UN_LBFT3, "lb/ft3"));

  return vcUnits;
}

bool CDensityDataReader::OnImportData()
{
  return ImportValueProfile(m_profile);
}

double CDensityDataReader::FromUserUnit(double dValue, CImportInfo::TUnitID unit) const
{
  switch(unit)
  {
  case UN_KGM3:
    return dValue;
  case UN_GM3:
    return dValue * 1e-3;
  case UN_KGCM3:
    return dValue * 1e3;
  case UN_GCM3:
    return dValue;
  case UN_LBGAL:
    return dValue / 6.2428e-2;
  case UN_LBFT3:
    return dValue / 8.3454e-3;
  }

  return ITVDDataReader::FromUserUnit(dValue, unit);
}
