#ifndef _SURVEYREADER_H_
#define _SURVEYREADER_H_

#include <vector>
#include <map>

#include "IDataReader.h"

class CDataModel;

class CSurveyReader : public IDataReader
{
public:
  // tag IDS
  static const CImportInfo::TTagID TAG_MD  = 0;
  static const CImportInfo::TTagID TAG_INC = 1;
  static const CImportInfo::TTagID TAG_AZI = 2;

  // unit types
  static const CImportInfo::TUnitID UN_M  = 0;
  static const CImportInfo::TUnitID UN_FT = 1;

public:
  CSurveyReader(CDataModel& model);

protected:
  virtual bool OnPresentImportDialog(const CString& strFilePath);
  virtual bool OnImportData();

private:
  CDataModel& m_model;
};

#endif // _SURVEYREADER_H_
