#ifndef _ITVDDATAREADER_H_
#define _ITVDDATAREADER_H_

#include "IDataReader.h"

class CDataModel;
class CValueProfile;

class ITVDDataReader : public IDataReader
{
public:
  static const CImportInfo::TTagID TAG_TVD  = 0;
  static const CImportInfo::TTagID TAG_DATA = 1;

  static const CImportInfo::TUnitID UN_M  = 0;
  static const CImportInfo::TUnitID UN_FT = 1;

public:
  ITVDDataReader(CDataModel& model);
  virtual CString DialogCaption() const = 0;
  virtual CString DataColumnCaption() const = 0;
  virtual std::vector<std::pair<int, CString> > DataUnits() const = 0;

protected:
  virtual bool OnPresentImportDialog(const CString& strFilePath);
  CDataModel& Model() const;

  virtual bool OnImportData() = 0;

  bool ImportValueProfile(CValueProfile& profile);
  virtual double FromUserUnit(double dValue, CImportInfo::TUnitID unit) const;

private:
  double ReadValue(int iLine, CImportInfo::TTagID tag) const;

private:
  CDataModel& m_model;
};

#endif // _ITVDDATAREADER_H_
