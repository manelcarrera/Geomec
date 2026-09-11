#ifndef _POREPRESSUREDATAREADER_H_
#define _POREPRESSUREDATAREADER_H_

#include "IPressureDataReader.h"

class CPorePressureDataReader : public IPressureDataReader
{
public:
  CPorePressureDataReader(CDataModel& model, CValueProfile& profile);

protected:
  virtual bool OnImportData();

private:
  CValueProfile& m_profile;
};

#endif // _POREPRESSUREDATAREADER_H_
