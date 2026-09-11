#ifndef EXPORTIDMSTABOR_H
#define EXPORTIDMSTABOR_H
#include <string>
#include <vector>

#include "AnalysisType.h"

class CModelBase;
class CDepletionStage;
class CNewGeoWellPoint;
class CNewWellPoint;

class CExportIDMStabor {
private:
  static std::vector<std::string> m_fieldNames;
  static std::vector<std::string> m_units;
  static bool m_classInited;
  const CModelBase &m_model;
  const CDepletionStage &m_stage;
  const CAnalysisType &m_antype;
  const std::list<CNewWellPoint> *m_pNewPointList;

public:
  CExportIDMStabor(const CModelBase &model, const CDepletionStage &stage, const CAnalysisType &antype,
                   const std::list<CNewWellPoint> *pNewPointList);
  bool eksport(const std::string &filename);
};

class CExportIDMStaborRow {
private:
  const CModelBase &m_model;
  const CDepletionStage &m_stage;
  const CAnalysisType &m_antype;
  const CNewGeoWellPoint *m_pNewWellPoint;

  bool m_validRow;
  int m_rowNr;
  double m_verticalDepth;
  double m_boreHoleRadius;
  double m_inclinationAngle;
  double m_azimuthAngle;
  bool m_stressDefinitionGeneral;
  double m_porePressure;
  double m_totalNorthDirecStress;
  double m_totalEastDirecStress;
  double m_totalDepthDirecStress;
  double m_northEastShearStress;
  double m_eastDepthShearStress;
  double m_northDepthShearStress;
  bool m_validPorePressure;
  bool m_validTotalNorthDirecStress;
  bool m_validTotalEastDirecStress;
  bool m_validTotalDepthDirecStress;
  bool m_validNorthEastShearStress;
  bool m_validEastDepthShearStress;
  bool m_validNorthDepthShearStress;
  // double m_porePressure_kPa_m;
  // double m_vertTSG;
  // double m_maxHorTSG;
  // double m_minHorTSG;
  // double m_azimuthMaxHor;
  bool m_undrained;
  double m_biotAlpha;
  double m_skemptonCoefficient;
  bool m_useCorrelationFunction;
  double m_surfaceArea;
  double m_sonicTransitTime;
  bool m_plasticity;
  double m_cohesionStrength;
  double m_frictionAngle;
  double m_dilatationAngle;
  bool m_isotropy;
  double m_youngsModulus;
  // double m_inPlaneYoungsModulus;
  // double m_YoungsModulusPerpendicularToBeddingPlane;
  // double m_dipAzimuth;
  // double m_dipAngle;
  double m_poissonRatio;
  // double m_hardeningType;
  // double m_hardeningGradient;
  // double m_fractureEnergy;
public:
  CExportIDMStaborRow(const CModelBase &pModel, const CDepletionStage &pStage, const CAnalysisType &antype,
                      const CNewGeoWellPoint *pNewPoint, int rowNr);
  void eksport(std::ostream &stream);
};

class CStartIDMStaborDlg : public CDialog {
private:
public:
  CStartIDMStaborDlg(std::list<CNewWellPoint> *pNewPointList, CModelBase &model,
                     CWnd *pParent = NULL); // standard constructor

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);

  DECLARE_MESSAGE_MAP()

private:
  afx_msg void OnExport();
  afx_msg void OnBrowse();
  afx_msg void UpdateExportButton();

  CAnalysisType::TAnalysisType AnalysisType() const;

private:
  CModelBase &m_model;
  std::list<CNewWellPoint> *m_pNewPointList;
  int m_nAnalysisType;
  int m_nStage;
  CString m_strFileName;
  std::vector<const CDepletionStage *> m_vcStages;
};

#endif // EXPORTIDMSTABOR_H
