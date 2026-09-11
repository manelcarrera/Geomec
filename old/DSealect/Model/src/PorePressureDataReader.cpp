#include "stdafx.h"
#include "dsealect.h"

#include "PorePressureDataReader.h"
#include "CDataModel.h"

CPorePressureDataReader::CPorePressureDataReader(CDataModel& model, CValueProfile& profile)
: IPressureDataReader(model),
  m_profile(profile)
{
  AddTag(TAG_DATA, "pore");
}

bool CPorePressureDataReader::OnImportData()
{
  return ImportValueProfile(m_profile);
}
