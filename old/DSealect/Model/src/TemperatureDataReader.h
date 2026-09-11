#ifndef _TEMPERATUREDATAREADER_H_
#define _TEMPERATUREDATAREADER_H_

#include "ITemperatureDataReader.h"

class CTemperatureDataReader : public ITemperatureDataReader
{
public:
  CTemperatureDataReader(CDataModel& model, CValueProfile& profile);

protected:
  virtual bool OnImportData();

private:
  CValueProfile& m_profile;
};

#endif // _TEMPERATUREDATAREADER_H_
