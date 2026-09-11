#ifndef _ROCKMECHPROCESSOR_H_
#define _ROCKMECHPROCESSOR_H_

class CModelBase;
class IDianaXWrapper;
class ISaveModel;
class IRetrieveDianaFileNames;
class RGInterface;
class CDianaRunController;

namespace geo {
  class IElement;
}

class RGProperty;
class RGDate;
class CAnalysisLogger;
class CMaterialEntry;
class IMaterial;
class CFormationBase;
class CPointSet;
class IValueComponentBase;

#include <QString>

#include "RGElement.h"
#include "RGGeneralProperty.h"

namespace gm_shell
{
class CInterfaceRGI;
}

namespace test_model_operations
{
class ModelOperationsRGI;
}

namespace GeomecRGI
{

class CRockMechProcessor
{
  friend class ::gm_shell::CInterfaceRGI;
  friend class ::test_model_operations::ModelOperationsRGI;
public:
  CRockMechProcessor(const QString& strFileName,  const QString& strOutputDir, QString strTempDir = QString());

  void SetModel(CModelBase& model, bool isExistingModel);

  void executeRockMechCommands_DEPRECATED(int& nStage, const std::string& title,
  IDianaXWrapper* dianaXWrapper, const QString& getPathName, bool& bRet,
  ISaveModel& saveModel, IRetrieveDianaFileNames& retrieveDianaFileNames,
  bool& bSaveModel, bool validate = true);
  bool QuitCommandReceived() const;
  bool isExistingModel() const;

  static CMaterialEntry* createMaterial(CModelBase& modelBase,
  const QString& materialName, int materialModel);
  static void connectMaterials2Formations(CModelBase& modelBase,
  const std::vector <IMaterial*> materials);
  static void connectMaterial2Formation(CModelBase& modelBase,
  IMaterial* material, CFormationBase* formationBase);

private:
  bool LoadGrid(RGInterface& rgi);
  bool SaveGrid(RGInterface& rgi) const;
  RGElementType::Type GetRGElementType(const geo::IElement& elm, RGInterface& rgi) const;
  bool LoadProperty(RGInterface& rgi, const RGProperty& rgProperty);
  bool SaveProperty(RGInterface& rgi, const RGProperty& rgProperty);
  bool CalculateFirst(const QString& getPathName, IDianaXWrapper* dianaXWrapper,
  const std::string &title, RGInterface& rgi, ISaveModel& saveModel,
  IRetrieveDianaFileNames& retrieveDianaFileNames);
  bool CalculateNext(const QString& getPathName, IDianaXWrapper* dianaXWrapper,
  const std::string &title, RGInterface& rgi, ISaveModel& saveModel,
  IRetrieveDianaFileNames& retrieveDianaFileNames);
  bool Calculate(const QString& getPathName, IDianaXWrapper* dianaXWrapper,
  const std::string &title, RGInterface& rgi, ISaveModel& saveModel,
  IRetrieveDianaFileNames& retrieveDianaFileNames);
  bool DefineInitialDate(RGInterface& rgi, const RGDate& date);
  bool EnsureDepletionStageAvailable(RGInterface& rgi);

  void ListMonitorableProperties(RGInterface& rgi);
  bool LoadMonitorPointSets(RGInterface& rgi);
  bool EvaluateMonitorValues(RGInterface& rgi);

  void CollectMonitorablePropertiesFromModel(std::map<std::string, const IValueComponentBase *>& properties, bool withDepletionStages) const;
  void TransformMonitorableProperties(const std::map<std::string, const IValueComponentBase *>& properties, std::vector<RGGeneralProperty>& rgProps) const;

  // logging
  CAnalysisLogger& Logger() const;
public:
  void AddLogLine(const QString& str, RGInterface* pRGI = 0, bool bVerbose = false, bool bFatal = false) const;
private:
  friend class CSavePropertyPermMultFrac;
  friend class CLoadPropertyBase;
  friend class CCalculateShearModulus;
  friend class CLoadPropertyMaterialType;
  QString PropertyName(const RGProperty& property) const;

public:
  QString getSaveModelName(const QString& pathName, int nStage, bool bSuccess, bool bAppendStageNumber);
private:
  bool CheckModel(RGInterface& rgi) const;

  bool HandleFatalErrors(RGInterface* pRGI);

  void SetupQuantities();

private:
  CModelBase *m_model;
  QString m_strFileName;
  QString m_strOutputDir;
  bool m_bQuit;

  QString m_strTempPath;
  CDianaRunController *m_drc;
  bool m_isExistingModel;

  mutable std::vector<QString> m_fatalErrors;

  // monitoring pointsets
  struct Quantity
  {
  std::string name;
  bool doConvert;
  double conversionFactor;

  Quantity() : doConvert(false), conversionFactor(1) {}
  Quantity(const std::string& name, bool doConvert, double conversionFactor) : name(name), doConvert(doConvert), conversionFactor(conversionFactor) {}

  double Convert(double value) { return doConvert ? value * conversionFactor : value; }
  };
  std::map<std::string, Quantity> m_quantities;

  Quantity GetQuantity(const std::string& unit) const;

  std::vector<CPointSet *> m_monitorPointSets;
  std::vector< std::vector<std::string> > m_monitorPointSetProps;
};

} // namespace GeomecRGI

#endif // _ROCKMECHPROCESSOR_H_
