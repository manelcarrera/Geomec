#include "stdafx.h"
#include "dsealect.h"
#include "ITVDDataReader.h"
#include "ValueProfile.h"
#include "TVDDataImportDlg.h"

ITVDDataReader::ITVDDataReader(CDataModel& model)
: m_model(model)
{
  AddTag(TAG_TVD, "tvd");
  AddTag(TAG_TVD, "depth");

  AddUnit(UN_M, "m");
  AddUnit(UN_FT, "ft");
  AddUnit(UN_FT, "feet");
}

bool ITVDDataReader::OnPresentImportDialog(const CString& strFilePath)
{
  CTVDDataImportDlg dlg(*this, strFilePath);
  return dlg.DoModal() == IDOK;
}

CDataModel& ITVDDataReader::Model() const
{
  return m_model;
}

double ITVDDataReader::ReadValue(int iLine, CImportInfo::TTagID tag) const
{
  const std::vector<CString>& vcColumns = ImportInfo().LineVec(iLine);
  CString strValue = vcColumns[ImportInfo().GetColumnNumber(tag)];

  bool bValid;
  double dRet = FromUserUnit(QString((LPCTSTR)strValue).toDouble(&bValid), ImportInfo().GetUnit(tag));
  if(!bValid)
  {
    CString msg;
    msg.Format("Invalid value '%s' at line %d, column %d", strValue, iLine, ImportInfo().GetColumnNumber(tag));
    AfxMessageBox(msg);
    throw std::exception();
  }

  return dRet;
}

bool ITVDDataReader::ImportValueProfile(CValueProfile& profile)
{
  for(int i = ImportInfo().DataStart(); i <= ImportInfo().DataEnd(); ++i)
  {
    try
    {
      double dTVD = ReadValue(i, TAG_TVD);
      double dData = ReadValue(i, TAG_DATA);
      profile.AddValues(dTVD, dData);
    }
    catch(std::exception&)
    {
      return false;
    }
  }

  return true;
}

double ITVDDataReader::FromUserUnit(double dValue, CImportInfo::TUnitID unit) const
{
  switch(unit)
  {
  case UN_M:
    break;
  case UN_FT:
    dValue /= 3.2808399;
    break;
  default:
    assert(false); // override FromUserUnit for user-defined units
  }

  return dValue;
}
