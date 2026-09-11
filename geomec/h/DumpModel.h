#ifndef _DumpModel_h_
#define _DumpModel_h_

class CModelBase;
class CMaterialServer;
class CDepletionStage;
class CFormationBase;
class CTetraSubHorizon;

class CDumpModel {
public:
  enum TLineType { text, newLine, boldText, boldUnderlinedText, redText };

  CDumpModel(const CModelBase &model);

  void dump(const QString &modelName, const QString &fileLocation);

  void addLine(TLineType lineType, const QString &line);

  std::vector<TLineType> getLineTypes() const;
  std::vector<QString> getLines() const;

private:
  CDumpModel(const CDumpModel &rhs);
  CDumpModel &operator=(const CDumpModel &rhs);

  void generalModelInfo(const QString &modelName, const QString &fileLocation);
  void nonLinCalcSettingsInfo();
  void analysisOptionsInfo();
  void globalDataInfo();
  void depletionStageInfo();
  void pressureDataInfo();
  void faultPressureInfo();
  void temperatureDataInfo();
  void strainDataInfo();
  void attachedMaterialsInfo();
  void FaultsInfo();

  void materialInfo(const CMaterialServer *pMatServer);
  void boundaryInfo();
  void upscalingInfo();
  void zoomInInfo();

  const CModelBase &m_model;
  std::vector<TLineType> m_lineTypes;
  std::vector<QString> m_lines;
  std::vector<const CDepletionStage *> m_vcStages;
  std::vector<const CFormationBase *> m_vcFormations;
  std::vector<const CTetraSubHorizon *> m_vcFaults;
};

#endif // _DumpModel_h_
