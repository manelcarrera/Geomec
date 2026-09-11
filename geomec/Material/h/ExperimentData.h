// ExperimentData.h: interface for the CExperimentData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPERIMENTDATA_H__7E632A46_E50D_11D3_B3AD_00A0C941C60B__INCLUDED_)
#define AFX_EXPERIMENTDATA_H__7E632A46_E50D_11D3_B3AD_00A0C941C60B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StressStrain.h"
#include <afxtempl.h>
class _Worksheet;
class Range;

class CExperimentData {
public:
  typedef enum { ELASTIC, PLASTIC } FitType;

  CExperimentData();
  CExperimentData(const CExperimentData &ED);
  virtual ~CExperimentData();
  CExperimentData &operator=(const CExperimentData &ED);
  BOOL ReadFromFile(_Worksheet &ws, CString strLabel, CWnd *pParentWnd);
  const CString &GetFileName() const { return m_strFileName; }
  const CString &GetLabel() const { return m_strLabel; }
  const double &getLowerLimitFraction() const {
    return m_eFitType == ELASTIC ? m_dLowerLimitFractionElastic : m_dLowerLimitFractionPlastic;
  }
  const double &getUpperLimitFraction() const {
    return m_eFitType == ELASTIC ? m_dUpperLimitFractionElastic : m_dUpperLimitFractionPlastic;
  }

  const double &getAxialStrainOffset() const { return getAxialStrainOffset(getLowerLimitFraction()); }

  const double &getAxialStrainOffset(double lowerLimitFraction) const {
    int unsigned index = (int)(lowerLimitFraction * (m_vcStressStrainStep.size()));
    return m_vcStressStrainStep[index].m_dAxialStrain;
  }

  const double &getRadialStrainOffset() const {
    int unsigned index = (int)(getLowerLimitFraction() * (m_vcStressStrainStep.size()));
    return m_vcStressStrainStep[index].m_dRadialStrain;
  }

  void setLowerLimitFraction(const double &v) {
    if (m_eFitType == ELASTIC)
      m_dLowerLimitFractionElastic = v;
    else
      m_dLowerLimitFractionPlastic = v;
  }
  void setUpperLimitFraction(const double &v) {
    if (m_eFitType == ELASTIC)
      m_dUpperLimitFractionElastic = v;
    else
      m_dUpperLimitFractionPlastic = v;
  }

  const CStressStrainArray &GetStressStrainSteps() const;
  double WeightFactor() const;

  void Selected(bool bSelected);
  bool Selected() const;

  void SetFitType(FitType eFitType);

  typedef enum { Time, AxialStress, RadialStress, AxialStrain, RadialStrain, PorePressure } ExperimentDataType;

  static const CString &getColumnHeader(const ExperimentDataType &experimentDataType);

private:
  BOOL GetParameterFromString(const char *str, double &Param);
  BOOL GetUnitString(Range &cells, int iRow, int iCol, CString &strUnit);
  BOOL IsUnit(CString strCheck, CString strUnit);
  BOOL GetStressUnitFactor(CString strUnit, int iCol, double &dUnitFac);
  int GetValueFromCell(Range &cells, int iCol, int iRow, double &dValue);
  BOOL IsCellEmpty(Range &cells, int iCol, int iRow);

private:
  double m_dWeightFactor;
  double m_dLowerLimitFractionElastic;
  double m_dUpperLimitFractionElastic;
  double m_dLowerLimitFractionPlastic;
  double m_dUpperLimitFractionPlastic;
  FitType m_eFitType;
  bool m_bSelected;

  CStressStrainArray m_vcStressStrainStep;

  CString m_strFileName;
  CString m_strLabel;
};

typedef std::vector<CExperimentData> CExperimentArray;

#endif // !defined(AFX_EXPERIMENTDATA_H__7E632A46_E50D_11D3_B3AD_00A0C941C60B__INCLUDED_)
