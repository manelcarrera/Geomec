#include "stdafx.h"
#include "dsealect.h"

#include "TemperatureDataReader.h"
#include "CDataModel.h"

CTemperatureDataReader::CTemperatureDataReader(CDataModel& model, CValueProfile& profile)
: ITemperatureDataReader(model),
  m_profile(profile)
{
}

bool CTemperatureDataReader::OnImportData()
{
  return ImportValueProfile(m_profile);
}
