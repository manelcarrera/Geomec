#include "stdafx.h"
#include "dsealect.h"
#include "SurveyReader.h"
#include "ImportSurveyDlg.h"
#include "CDrillingScheme.h"
#include "CDataModel.h"

///// CSurveyReader

CSurveyReader::CSurveyReader(CDataModel& model)
: m_model(model)
{
  // it is assumed the tags start with these string (case insensitive)
  AddTag(TAG_MD, "md");
  AddTag(TAG_MD, "tmd");

  AddTag(TAG_INC, "inc");
  AddTag(TAG_INC, "dip");

  AddTag(TAG_AZI, "azi");

  AddUnit(UN_M, "m");
  AddUnit(UN_FT, "f");
}

bool CSurveyReader::OnImportData()
{
  // remove existing sections
  m_model.DrillingScheme()->RemoveAllDrillingDirectionSections();

  double dPrevMD = 0;

  for(int i = ImportInfo().DataStart(); i <= ImportInfo().DataEnd(); ++i)
  {
    const std::vector<CString>& vcColumns = ImportInfo().LineVec(i);
    CString strMD = vcColumns[ImportInfo().GetColumnNumber(TAG_MD)];
    double dMD = QString((LPCTSTR)strMD).toDouble();

    if(i > ImportInfo().DataStart() || dMD > 0)
    {
      m_model.DrillingScheme()->AddDrillingDirectionSection();
      CDrillingDirectionSection* pSection = m_model.DrillingScheme()->GetDrillingDirectionSection(m_model.DrillingScheme()->NrOfDrillingDirections() - 1);

      CString strAzi = vcColumns[ImportInfo().GetColumnNumber(TAG_AZI)];
      double dAzi = QString((LPCTSTR)strAzi).toDouble();
      CString strInc = vcColumns[ImportInfo().GetColumnNumber(TAG_INC)];
      double dInc = QString((LPCTSTR)strInc).toDouble();

      CDoubleQuantity::UNIT unit = (ImportInfo().GetUnit(TAG_MD) == UN_M ? CDoubleQuantity::SI_UNIT : CDoubleQuantity::FIELD_UNIT);

      pSection->DrillingSectionTopTMD()->Value(dPrevMD, unit);
      pSection->DrillingSectionBottomTMD()->Value(dMD, unit);

      // CAngleQuantity assumes degrees, not radians
      pSection->AzimuthWithNorth()->SetValue(dAzi);
      pSection->WellInclination()->SetValue(dInc);
    }

    dPrevMD = dMD;
  }

  return true;
}
bool CSurveyReader::OnPresentImportDialog(const CString& strFilePath)
{
  CImportSurveyDlg dlg(ImportInfo(), strFilePath);
  return dlg.DoModal() == IDOK;
}
